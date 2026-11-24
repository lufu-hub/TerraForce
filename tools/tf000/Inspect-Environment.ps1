# TerraForge TF-000 - SALT OKUNUR ortam ve repo incelemesi
# Bu betik HICBIR dosyayi degistirmez, silmez, tasimaz; git fetch/pull/commit YAPMAZ.
# Tek yazdigi dosya: Masaustu\TerraForge_TF000_Rapor.md
# Kullanim: TF000_Incele.cmd dosyasina cift tiklayin (veya: powershell -ExecutionPolicy Bypass -File Inspect-Environment.ps1 [-RepoPath "C:\...\TerraForce"])

param(
    [string]$RepoPath = ""
)

$ErrorActionPreference = "Continue"
$Lines = New-Object System.Collections.Generic.List[string]
function Add([string]$Text = "") { $Lines.Add($Text) }
function Section([string]$Title) { Add ""; Add "## $Title"; Add "" }

function Try-Cmd([string]$Exe, [string[]]$CmdArgs) {
    $cmd = Get-Command $Exe -ErrorAction SilentlyContinue
    if (-not $cmd) { return $null }
    try { return ((& $cmd.Source @CmdArgs 2>&1) | Out-String).Trim() } catch { return "HATA: $($_.Exception.Message)" }
}

function GB([double]$Bytes) { return [math]::Round($Bytes / 1GB, 1) }

Add "# TerraForge TF-000 Ortam Raporu"
Add ""
Add "Tarih: $(Get-Date -Format 'yyyy-MM-dd HH:mm')  |  Betik: tools/tf000/Inspect-Environment.ps1 (salt okunur)"

# ---------------------------------------------------------------- Donanim
Section "Donanim ve isletim sistemi"
try {
    $os = Get-CimInstance Win32_OperatingSystem
    Add "- Windows: $($os.Caption) $($os.Version) (build $($os.BuildNumber)), $($os.OSArchitecture)"
    $cpu = Get-CimInstance Win32_Processor | Select-Object -First 1
    Add "- CPU: $($cpu.Name.Trim()) | $($cpu.NumberOfCores) cekirdek / $($cpu.NumberOfLogicalProcessors) is parcacigi"
    $ram = (Get-CimInstance Win32_ComputerSystem).TotalPhysicalMemory
    Add "- RAM: $(GB $ram) GB"
    foreach ($g in Get-CimInstance Win32_VideoController) {
        Add "- GPU: $($g.Name) | surucu $($g.DriverVersion)"
    }
    $smi = Try-Cmd "nvidia-smi" @("--query-gpu=name,memory.total,driver_version", "--format=csv,noheader")
    if ($smi) { Add "- nvidia-smi: $smi" }
} catch { Add "- Donanim okunamadi: $($_.Exception.Message)" }

Section "Diskler"
try {
    $phys = @{}
    try { foreach ($p in Get-PhysicalDisk) { $phys[$p.DeviceId] = "$($p.MediaType) / $($p.BusType)" } } catch {}
    foreach ($v in Get-CimInstance Win32_LogicalDisk -Filter "DriveType=3") {
        Add "- $($v.DeviceID) bos $(GB $v.FreeSpace) GB / toplam $(GB $v.Size) GB"
    }
    foreach ($k in $phys.Keys) { Add "- Fiziksel disk ${k}: $($phys[$k])" }
} catch { Add "- Disk bilgisi okunamadi" }

# ---------------------------------------------------------------- Araclar
Section "Araclar"
function Tool([string]$Name, [string]$Exe, [string[]]$CmdArgs) {
    $r = Try-Cmd $Exe $CmdArgs
    if ($r) { Add "- ${Name}: $(($r -split "`n")[0])" } else { Add "- ${Name}: BULUNAMADI" }
}
Tool "Git" "git" @("--version")
Tool "Git LFS" "git" @("lfs", "version")
Tool "Python" "python" @("--version")
Tool "Node.js" "node" @("--version")
Tool "npm" "npm" @("--version")
Tool "Claude Code" "claude" @("--version")
Tool "CMake" "cmake" @("--version")

$ghd = Join-Path $env:LOCALAPPDATA "GitHubDesktop"
Add "- GitHub Desktop: $(if (Test-Path $ghd) { 'kurulu' } else { 'bulunamadi' })"

# API anahtari: degeri ASLA yazilmaz, yalnizca tanimli olup olmadigi.
$apiKey = [Environment]::GetEnvironmentVariable("ANTHROPIC_API_KEY", "User")
if (-not $apiKey) { $apiKey = [Environment]::GetEnvironmentVariable("ANTHROPIC_API_KEY", "Machine") }
if (-not $apiKey) { $apiKey = $env:ANTHROPIC_API_KEY }
Add "- ANTHROPIC_API_KEY ortam degiskeni: $(if ($apiKey) { 'TANIMLI (ucretli API faturalandirmasi riski - kontrol edin)' } else { 'tanimli degil (iyi: Pro oturumu kullanilir)' })"

foreach ($b in @("C:\Program Files\Blender Foundation")) {
    if (Test-Path $b) { Add "- Blender: $((Get-ChildItem $b -Directory | ForEach-Object Name) -join ', ')" } else { Add "- Blender: bulunamadi (bu fazda gerekli degil)" }
}
$qgis = Get-ChildItem "C:\Program Files" -Directory -Filter "QGIS*" -ErrorAction SilentlyContinue
Add "- QGIS: $(if ($qgis) { ($qgis | ForEach-Object Name) -join ', ' } else { 'bulunamadi (bu fazda gerekli degil)' })"

Section "Unreal Engine ve Epic Launcher"
$launcherDat = "C:\ProgramData\Epic\UnrealEngineLauncher\LauncherInstalled.dat"
Add "- Epic Games Launcher: $(if (Test-Path 'C:\Program Files (x86)\Epic Games\Launcher') { 'kurulu' } else { 'bulunamadi' })"
$found = $false
if (Test-Path $launcherDat) {
    try {
        $dat = Get-Content $launcherDat -Raw | ConvertFrom-Json
        foreach ($i in $dat.InstallationList) {
            if ($i.AppName -like "UE_*") { Add "- $($i.AppName): $($i.InstallLocation) (surum $($i.AppVersion))"; $found = $true }
        }
    } catch {}
}
$regRoot = "HKLM:\SOFTWARE\EpicGames\Unreal Engine"
if (Test-Path $regRoot) {
    foreach ($k in Get-ChildItem $regRoot) {
        $dir = (Get-ItemProperty $k.PSPath -ErrorAction SilentlyContinue).InstalledDirectory
        Add "- Kayit defteri UE $($k.PSChildName): $dir"; $found = $true
    }
}
if (-not $found) { Add "- Unreal Engine: KURULU DEGIL" }

Section "Visual Studio (C++ arac zinciri)"
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vswhere) {
    try {
        $vs = & $vswhere -all -products * -format json | ConvertFrom-Json
        if (-not $vs) { Add "- Visual Studio: KURULU DEGIL" }
        foreach ($v in $vs) {
            Add "- $($v.displayName) $($v.installationVersion) : $($v.installationPath)"
            $game = & $vswhere -path $v.installationPath -requires Microsoft.VisualStudio.Workload.NativeGame -property installationPath
            $desk = & $vswhere -path $v.installationPath -requires Microsoft.VisualStudio.Workload.NativeDesktop -property installationPath
            Add "  - 'C++ ile oyun gelistirme' is yuku: $(if ($game) { 'VAR' } else { 'YOK' }) | 'C++ ile masaustu' is yuku: $(if ($desk) { 'VAR' } else { 'YOK' })"
        }
    } catch { Add "- vswhere okunamadi" }
} else { Add "- Visual Studio: KURULU DEGIL (vswhere yok)" }
$sdk = Get-ChildItem "${env:ProgramFiles(x86)}\Windows Kits\10\Include" -Directory -ErrorAction SilentlyContinue | ForEach-Object Name
Add "- Windows SDK: $(if ($sdk) { $sdk -join ', ' } else { 'bulunamadi' })"

# ---------------------------------------------------------------- Repo
Section "Yerel TerraForge repo (salt okunur)"
$candidates = @()
if ($RepoPath) { $candidates += $RepoPath }
foreach ($base in @("$env:USERPROFILE\Documents\GitHub", "$env:USERPROFILE\source\repos", "$env:USERPROFILE\Desktop", "$env:USERPROFILE\Documents")) {
    if (Test-Path $base) {
        $candidates += Get-ChildItem $base -Directory -ErrorAction SilentlyContinue |
            Where-Object { $_.Name -match "terra" -and (Test-Path (Join-Path $_.FullName ".git")) } |
            ForEach-Object FullName
    }
}
$candidates = $candidates | Select-Object -Unique
if (-not $candidates) {
    Add "- Repo bulunamadi. Betigi -RepoPath ""C:\yol\TerraForce"" ile tekrar calistirin."
}

foreach ($repo in $candidates) {
    Add "### $repo"
    Add ""
    if (-not (Test-Path (Join-Path $repo ".git"))) { Add "- Git deposu degil"; continue }
    # --no-optional-locks: git status bile index dosyasina yazmasin.
    function G([string[]]$GArgs) { return ((& git --no-optional-locks -C $repo @GArgs 2>&1) | Out-String).Trim() }
    Add "- Aktif dal: $(G @('rev-parse','--abbrev-ref','HEAD'))"
    Add "- HEAD: $(G @('log','-1','--format=%h %ad %s','--date=short'))"
    Add "- Uzak depolar:"; Add '```'; Add (G @('remote','-v')); Add '```'
    Add "- Yerel dallar:"; Add '```'; Add (G @('branch','-vv')); Add '```'
    $originMain = G @('rev-parse','--verify','--quiet','refs/remotes/origin/main')
    if ($originMain -and $originMain -notmatch "fatal") {
        Add "- HEAD ile (son bilinen) origin/main farki [yerel-ileri  uzak-ileri]: $(G @('rev-list','--left-right','--count','HEAD...origin/main'))"
        Add "- Ortak ata: $(G @('merge-base','HEAD','origin/main'))"
    } else { Add "- origin/main yerelde bilinmiyor (fetch yapilmadi; bilerek)" }
    Add "- Son 10 commit:"; Add '```'; Add (G @('log','-10','--format=%h %ad %an %s','--date=short')); Add '```'
    $porc = (& git --no-optional-locks -C $repo status --porcelain=v1 --untracked-files=all 2>&1)
    $porc = @($porc | Where-Object { $_ })
    Add "- Commit edilmemis degisiklik sayisi: $($porc.Count)"
    if ($porc.Count -gt 0) {
        $mod = @($porc | Where-Object { $_ -match '^( M|M |MM|AM)' }).Count
        $new = @($porc | Where-Object { $_ -match '^\?\?' }).Count
        $del = @($porc | Where-Object { $_ -match '^( D|D )' }).Count
        Add "  - degistirilmis: $mod | yeni (izlenmeyen): $new | silinmis: $del"
        Add '```'; $porc | Select-Object -First 200 | ForEach-Object { Add $_ }; if ($porc.Count -gt 200) { Add "... (+$($porc.Count - 200) satir)" }; Add '```'
    }
    $stash = G @('stash','list')
    Add "- Stash: $(if ($stash) { $stash } else { 'yok' })"
    Add "- .gitattributes: $(if (Test-Path (Join-Path $repo '.gitattributes')) { 'var' } else { 'yok' }) | .gitignore: $(if (Test-Path (Join-Path $repo '.gitignore')) { 'var' } else { 'yok' })"
    Add "- docs/development yerelde: $(if (Test-Path (Join-Path $repo 'docs\development')) { 'var' } else { 'yok' })"

    Add "- Ust dizin icerigi:"; Add '```'
    Get-ChildItem $repo -Force -ErrorAction SilentlyContinue | Where-Object { $_.Name -ne ".git" } |
        ForEach-Object { Add ("{0}{1}" -f $_.Name, $(if ($_.PSIsContainer) { "/" } else { "  ($([math]::Round($_.Length/1KB,1)) KB)" })) }
    Add '```'

    $all = Get-ChildItem $repo -Recurse -File -Force -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -notmatch '\\(\.git|node_modules|dist|build|\.vite)\\' }
    $size = ($all | Measure-Object Length -Sum).Sum
    Add "- Dosya sayisi (node_modules/.git haric): $($all.Count), toplam $([math]::Round($size/1MB,1)) MB"
    $ext = $all | Group-Object Extension | Sort-Object Count -Descending | Select-Object -First 12 | ForEach-Object { "$($_.Name):$($_.Count)" }
    Add "- Uzanti dagilimi: $($ext -join ', ')"

    $web = $all | Where-Object { $_.Name -in @('package.json','index.html','vite.config.js','vite.config.ts','tsconfig.json') }
    Add "- Web prototipi isaretleri:"; Add '```'; $web | ForEach-Object { Add $_.FullName.Substring($repo.Length) }; Add '```'
    $pkg = $web | Where-Object Name -eq 'package.json' | Select-Object -First 1
    if ($pkg) {
        try { $j = Get-Content $pkg.FullName -Raw | ConvertFrom-Json; Add "- package.json: ad=$($j.name), scripts=$(($j.scripts.PSObject.Properties.Name) -join ',')" } catch {}
    }
    $tree = $all | Where-Object { $_.Name -match '(?i)master|tree|agac|node|dugum' }
    Add "- Master Tree aday dosyalari (ad eslesmesi):"; Add '```'; $tree | Select-Object -First 50 | ForEach-Object { Add "$($_.FullName.Substring($repo.Length))  ($([math]::Round($_.Length/1KB,1)) KB, $($_.LastWriteTime.ToString('yyyy-MM-dd')))" }; Add '```'
    $big = $all | Where-Object { $_.Length -gt 10MB }
    Add "- 10 MB ustu dosyalar (LFS adayi): $(if ($big) { ($big | ForEach-Object { $_.FullName.Substring($repo.Length) }) -join ', ' } else { 'yok' })"
    Add ""
}

# ---------------------------------------------------------------- Yaz
$out = Join-Path ([Environment]::GetFolderPath("Desktop")) "TerraForge_TF000_Rapor.md"
$Lines | Out-File -FilePath $out -Encoding utf8
Write-Host ""
Write-Host "Rapor yazildi: $out" -ForegroundColor Green
Write-Host "Bu dosyanin icerigini Claude oturumuna yapistirin. Hicbir proje dosyasi degistirilmedi."
