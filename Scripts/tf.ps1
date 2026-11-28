# TerraForge gelistirme komutlari (Windows). Kok klasordeki tf.cmd uzerinden calistirin:
#   tf help | doctor | setup | build | map | test | run | smoke | package | checkpoint "mesaj" | rollback <ref> | summarize <log>
# Ortam degiskeni TF_UE_ROOT ile motor klasoru elle verilebilir (or. C:\Program Files\Epic Games\UE_5.7).

param(
    [Parameter(Position = 0)][string]$Command = "help",
    [Parameter(Position = 1, ValueFromRemainingArguments = $true)][string[]]$Rest
)

# Continue: Unreal stderr satirlari PS 5.1'de istisna atmasin; basari cikis kodlariyla denetlenir.
$ErrorActionPreference = "Continue"
$Root = Split-Path -Parent $PSScriptRoot
$Project = Join-Path $Root "TerraForge.uproject"
$LogDir = Join-Path $Root "Saved\Logs\tf"
New-Item -ItemType Directory -Force -Path $LogDir | Out-Null
$Stamp = Get-Date -Format "yyyyMMdd-HHmmss"

function Info([string]$m) { Write-Host "[tf] $m" -ForegroundColor Cyan }
function Ok([string]$m) { Write-Host "[tf] OK  $m" -ForegroundColor Green }
function Fail([string]$m) { Write-Host "[tf] HATA $m" -ForegroundColor Red; exit 1 }

function Get-EngineVersion {
    return (Get-Content $Project -Raw | ConvertFrom-Json).EngineAssociation
}

function Get-UERoot {
    if ($env:TF_UE_ROOT -and (Test-Path $env:TF_UE_ROOT)) { return $env:TF_UE_ROOT }
    $ver = Get-EngineVersion
    $key = "HKLM:\SOFTWARE\EpicGames\Unreal Engine\$ver"
    if (Test-Path $key) {
        $dir = (Get-ItemProperty $key).InstalledDirectory
        if ($dir -and (Test-Path $dir)) { return $dir }
    }
    $dat = "C:\ProgramData\Epic\UnrealEngineLauncher\LauncherInstalled.dat"
    if (Test-Path $dat) {
        foreach ($i in (Get-Content $dat -Raw | ConvertFrom-Json).InstallationList) {
            if ($i.AppName -eq "UE_$ver" -and (Test-Path $i.InstallLocation)) { return $i.InstallLocation }
        }
    }
    $default = "C:\Program Files\Epic Games\UE_$ver"
    if (Test-Path $default) { return $default }
    Fail "Unreal Engine $ver bulunamadi. Epic Games Launcher'dan $ver kurun veya TF_UE_ROOT ayarlayin."
}

function Get-Python {
    $py = Get-Command python -ErrorAction SilentlyContinue
    if ($py -and $py.Source -notmatch "WindowsApps") { return $py.Source }
    $ue = Get-UERoot
    $bundled = Join-Path $ue "Engine\Binaries\ThirdParty\Python3\Win64\python.exe"
    if (Test-Path $bundled) { return $bundled }
    Fail "Python bulunamadi."
}

function Editor-Exe([switch]$Cmd) {
    $name = if ($Cmd) { "UnrealEditor-Cmd.exe" } else { "UnrealEditor.exe" }
    return Join-Path (Get-UERoot) "Engine\Binaries\Win64\$name"
}

function Summarize([string]$Log) {
    $out = "$Log.ozet.md"
    & (Get-Python) (Join-Path $PSScriptRoot "summarize_log.py") $Log --max 25 --out $out | Out-Host
    Info "Ozet: $out"
}

# Uzun suren bir islemi loga yazarak calistirir; cikis kodunu dondurur.
function Run-Logged([string]$Name, [string]$Exe, [string[]]$ArgList) {
    $log = Join-Path $LogDir "$Name-$Stamp.log"
    Info "$Name basliyor -> $log"
    & $Exe @ArgList *>&1 | Tee-Object -FilePath $log | Out-Host
    $code = $LASTEXITCODE
    return @{ Code = $code; Log = $log }
}

function Cmd-Doctor {
    & (Join-Path $Root "tools\tf000\Inspect-Environment.ps1") -RepoPath $Root
    Info "Hedef motor surumu (.uproject): $(Get-EngineVersion)"
    Info "Motor klasoru: $(Get-UERoot)"
}

function Cmd-Setup {
    git -C $Root lfs install --local | Out-Host
    Ok "Git LFS bu repo icin etkin"
    Cmd-Validate
    $r = Run-Logged "projectfiles" (Join-Path (Get-UERoot) "Engine\Build\BatchFiles\Build.bat") @("-projectfiles", "-project=$Project", "-game", "-engine", "-progress")
    if ($r.Code -ne 0) { Summarize $r.Log; Fail "Visual Studio proje dosyalari uretilemedi" }
    Ok "TerraForge.sln uretildi"
}

function Cmd-Validate {
    & (Get-Python) (Join-Path $PSScriptRoot "validate_data.py") $Root | Out-Host
    if ($LASTEXITCODE -ne 0) { Fail "Veri dogrulama basarisiz" }
}

function Cmd-Build {
    $r = Run-Logged "build" (Join-Path (Get-UERoot) "Engine\Build\BatchFiles\Build.bat") @("TerraForgeEditor", "Win64", "Development", "-Project=$Project", "-WaitMutex")
    if ($r.Code -ne 0) { Summarize $r.Log; Fail "Derleme basarisiz (ozet yukarida)" }
    Ok "Editor derlemesi basarili"
}

function Cmd-Map {
    $marker = Join-Path $Root "Saved\Automation\map_ok.txt"
    if (Test-Path $marker) { Remove-Item $marker }
    $script = Join-Path $PSScriptRoot "Editor\create_bootstrap_map.py"
    $r = Run-Logged "map" (Editor-Exe -Cmd) @($Project, "-ExecutePythonScript=$script", "-unattended", "-nosplash", "-nopause", "-stdout", "-FullStdOutLogOutput")
    if (-not (Test-Path $marker)) { Summarize $r.Log; Fail "Harita olusturulamadi (Python editor otomasyonu)" }
    Ok "Content/Maps/M0_Bootstrap hazir (Python editor otomasyonu dogrulandi)"
}

function Cmd-Test {
    Cmd-Validate
    if (Get-Command cmake -ErrorAction SilentlyContinue) {
        $b = Join-Path $Root "Intermediate\SimTests"
        cmake -S (Join-Path $Root "Tests\SimTests") -B $b | Out-Host
        cmake --build $b --config Release | Out-Host
        & (Join-Path $b "Release\tf_sim_tests.exe")
        if ($LASTEXITCODE -ne 0) { Fail "Simulasyon cekirdegi testleri basarisiz" }
        Ok "Simulasyon cekirdegi testleri"
    } else { Info "cmake yok; saf C++ testleri atlandi (Unreal testleri ayni kurallari kapsar)" }

    $r = Run-Logged "autotest" (Editor-Exe -Cmd) @($Project, "-ExecCmds=Automation RunTests TerraForge", "-TestExit=Automation Test Queue Empty", "-unattended", "-nopause", "-nosplash", "-NullRHI", "-stdout", "-FullStdOutLogOutput")
    $text = Get-Content $r.Log -Raw
    if ($text -match "Result=\{Fail" -or $text -notmatch "Result=\{Success") { Summarize $r.Log; Fail "Unreal otomasyon testleri basarisiz veya calismadi" }
    Ok "Unreal otomasyon testleri"
}

function Cmd-Run {
    Start-Process (Editor-Exe) -ArgumentList @("`"$Project`"", "-game", "-windowed", "-ResX=1600", "-ResY=900")
    Ok "Oyun baslatildi (editor -game). Cikis: F10"
}

function Invoke-Smoke([string]$Exe, [string[]]$Prefix, [string]$Name) {
    $log = Join-Path $LogDir "$Name-$Stamp.log"
    $argList = $Prefix + @("-tfsmoke", "-windowed", "-ResX=1280", "-ResY=720", "-unattended", "-nosplash", "-abslog=`"$log`"")
    Info "${Name}: oyun ~10 sn acilip kendiliginden kapanacak..."
    $p = Start-Process $Exe -ArgumentList $argList -PassThru
    if (-not $p.WaitForExit(180000)) { $p.Kill(); Fail "$Name zaman asimi (3 dk)" }
    if (-not (Test-Path $log)) { Fail "$Name log dosyasi olusmadi: $log" }
    $text = Get-Content $log -Raw
    $fps = ([regex]::Match($text, "TF_SMOKE_FPS [^\r\n]*")).Value
    if ($text -match "TF_SMOKE_RESULT PASS") { Ok "$Name GECTI  $fps"; return }
    Summarize $log
    Fail "$Name BASARISIZ"
}

function Cmd-Smoke {
    $packaged = Join-Path $Root "Packaged\Windows\TerraForge.exe"
    if ($Rest -contains "-packaged") {
        if (-not (Test-Path $packaged)) { Fail "Paket yok; once: tf package" }
        Invoke-Smoke $packaged @() "smoke-packaged"
    } else {
        Invoke-Smoke (Editor-Exe) @("`"$Project`"", "-game") "smoke-editor"
    }
}

function Cmd-Package {
    $out = Join-Path $Root "Packaged"
    $r = Run-Logged "package" (Join-Path (Get-UERoot) "Engine\Build\BatchFiles\RunUAT.bat") @(
        "BuildCookRun", "-project=$Project", "-platform=Win64", "-clientconfig=Development",
        "-build", "-cook", "-stage", "-pak", "-iostore", "-archive", "-archivedirectory=$out",
        "-unattended", "-utf8output", "-nop4", "-nocompileeditor")
    if ($r.Code -ne 0) { Summarize $r.Log; Fail "Paketleme basarisiz" }
    Ok "Windows paketi: $out\Windows\TerraForge.exe"
    $script:Rest = @("-packaged")
    Cmd-Smoke
}

function Cmd-Checkpoint {
    $msg = ($Rest -join " ").Trim()
    if (-not $msg) { Fail "Kullanim: tf checkpoint ""ne yapildi""" }
    Cmd-Validate
    git -C $Root status --short | Out-Host
    git -C $Root add -A
    git -C $Root commit -m "checkpoint: $msg"
    if ($LASTEXITCODE -ne 0) { Fail "Commit olusturulamadi (degisiklik yok olabilir)" }
    $tag = "cp-$Stamp"
    git -C $Root tag $tag
    Ok "Checkpoint: $tag  (geri donus: tf rollback $tag)"
}

function Cmd-Rollback {
    $ref = $Rest | Select-Object -First 1
    if (-not $ref) { git -C $Root tag --list "cp-*" --sort=-creatordate | Select-Object -First 10 | Out-Host; Fail "Kullanim: tf rollback <cp-etiketi>" }
    git -C $Root rev-parse --verify --quiet "$ref^{commit}" | Out-Null
    if ($LASTEXITCODE -ne 0) { Fail "Bilinmeyen ref: $ref" }
    # Yikici degil: kaydedilmemis isler stash'e, yeni dal olusturulur; hicbir commit silinmez.
    if (git -C $Root status --porcelain) {
        git -C $Root stash push -u -m "tf-rollback-yedek-$Stamp" | Out-Host
        Info "Kaydedilmemis degisiklikler stash'e alindi: tf-rollback-yedek-$Stamp (geri almak: git stash pop)"
    }
    git -C $Root switch -c "rollback/$Stamp" $ref
    if ($LASTEXITCODE -ne 0) { Fail "Dal olusturulamadi" }
    Ok "Su an $ref durumunda, yeni dal: rollback/$Stamp. Eski dal aynen duruyor."
}

switch ($Command.ToLower()) {
    "doctor" { Cmd-Doctor }
    "setup" { Cmd-Setup }
    "validate" { Cmd-Validate }
    "build" { Cmd-Build }
    "map" { Cmd-Map }
    "test" { Cmd-Test }
    "run" { Cmd-Run }
    "smoke" { Cmd-Smoke }
    "package" { Cmd-Package }
    "checkpoint" { Cmd-Checkpoint }
    "rollback" { Cmd-Rollback }
    "summarize" { if (-not $Rest) { Fail "Kullanim: tf summarize <log>" }; Summarize $Rest[0] }
    default {
        Write-Host @"
TerraForge komutlari:
  tf doctor             Ortam/donanim raporu (salt okunur)
  tf setup              Git LFS + veri dogrulama + Visual Studio proje dosyalari
  tf build              Editor derlemesi (hata olursa log ozeti)
  tf map                Bos M0 haritasini Python editor otomasyonuyla olustur
  tf test               Veri + simulasyon + Unreal otomasyon testleri
  tf run                Oyunu pencerede calistir
  tf smoke [-packaged]  Otomatik duman testi (yerlesim, carpisma, FPS)
  tf package            Windows paketi + paket uzerinde duman testi
  tf checkpoint "mesaj" Commit + cp-etiketi
  tf rollback <etiket>  Yikici olmayan geri donus (yeni dal)
  tf summarize <log>    Log ozeti
"@
    }
}
