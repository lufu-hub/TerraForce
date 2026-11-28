# Güncel durum — 28.11.2026
GitHub dokümantasyonu v2.0 (UE5, 2.5D Gameplay First, İngilizce, hazırlanmış gerçek bölgeler ve Realtor, geniş ekonomi/aile/hospitality kuralları) ile `claude/serene-lamport-3x9203` dalındaki TF-001..004 kodu bu dalda (`integration/v2-reconcile`) birleştirildi. Oyun özelliği tamamlanmış sayılmaz.

## Kanıt durumu
- **Doğrulandı (28.11, bulut, Unreal'siz):** simülasyon çekirdeği 4025/4025 kontrol, araç testleri 7/7, veri doğrulama OK.
- **Doğrulanmadı:** Unreal derlemesi, editör, oyun, Windows paketi, FPS/donanım ölçümü, UE ve Visual Studio kurulumunun tamamlanıp tamamlanmadığı.
- Tamamlanan oyun özelliği: yok. Hiçbir BACKLOG görevi `completed` değil.

## Yerel durum (28.11, kurucunun bilgisayarından salt okunur gözlem; git status çalıştırılamadı)
İki yerel klon var, ikisi de `https://github.com/lufu-hub/TerraForce.git` adresine bağlı (D24):
| Klasör | Aktif dal | İçerik |
|---|---|---|
| `Documents\TerraForge` | `claude/serene-lamport-3x9203` | UE5 projesi (serene kodu). Ekran görüntüsünde `tools/tf000/Inspect-Environment.ps1` tüm satırlarıyla "değişmiş" görünüyor. |
| `Documents\GitHub\TerraForce` | `main` | Commit edilmemiş görünen React/Vite web prototipi (`package.json`, `src/`, `catalog/`, `docs/bu-hafta.md`, kök `CLAUDE.md`/`README.md`). `docs/development/` klasörü çalışma ağacında listede görünmedi (yerelde silinmiş olabilir; doğrulanmadı). |

Bulgular:
- **`Inspect-Environment.ps1` farkı:** dosya yerelde ve depoda bayt bayt aynı (CRLF). `.gitattributes` `*.ps1 text eol=crlf` diyor, ama blob dizinde CRLF olarak kayıtlı (TF-000 commit'i `.gitattributes`'tan önce). Git bu yüzden "tüm satırlar değişti" gösteriyor. Bu dalda `git add --renormalize` ile düzeltildi; içerik değişmedi.
- **Çakışma riski:** iki klon aynı uzak depoyu paylaşıyor ve kök dosya adları çakışıyor (`CLAUDE.md`, `README.md`, `.gitignore`). Web klonundan commit/push/pull yapılırsa UE işiyle karışır veya yerel işi ezer.
- Yerel web prototipi ve Master Tree'ye dokunulmadı.

## Sonraki iş
1. Kurucu onayıyla bu dalın incelenip `main`'e alınması (PR). Otomatik merge/reset/force push yok.
2. TF-000'ı kapatmak: `tools/tf000/TF000_Incele.cmd` çıktısı (donanım, UE/VS kurulumu, yerel git durumu).
3. Ardından TF-001'i yeniden üretmeden doğrulamak (`tf build`). Yeni geliştirme yok.
4. Master Tree'ye dokunma. Her pakette tarih, commit, sonuç, risk, sonraki görev, gerekli karar ekle.

## Günlük
### 2026-11-28 — v2 ve serene birleştirme
- Yapıldı: serene commit'leri v2 belgelerinin üstüne uygulandı; karar numaraları D23–D27'ye taşındı, D28–D29 eklendi; `en.json` varsayılan metin dili yapıldı (`tr.json` yerelleştirme olarak kalır, doğrulayıcı anahtar eşitliğini denetler); BACKLOG durum notları işlendi; `.ps1`/`.cmd` satır sonu normalleştirildi.
- Test: yukarıdaki Unreal'siz kontroller geçti. C++ değişikliği tek satır (`LoadStrings("en")`); derlenmedi.
- Risk: Unreal API ayrıntılarında ilk `tf build`'de derleme hatası çıkabilir; iki yerel klonun aynı uzak depoyu paylaşması.
- Gerekli karar: D29 (web prototipinin akıbeti); PR'ın `main`'e alınması.

### 2026-11-24 — TF-000 (kısmi)
- Sonuç: uzak repo ve Claude bulut ortamı incelendi; yerel PC/repo için salt okunur betik eklendi (`tools/tf000/`). Ayrıntı: `reports/TF-000.md`.
- Test: betik bu ortamda çalıştırılamadı (PowerShell/Windows yok); elle gözden geçirildi.

### 2026-11-24 — TF-001…TF-004 kodu hazır, Windows'ta doğrulanmadı
- Onay: D05 ve operasyonel kararlar (şimdi D23–D27).
- Yapıldı: UE5 C++ projesi (`TerraForge.uproject`, modül `TerraForge`), Git LFS, `tf` komutları (build/test/map/smoke/package/checkpoint/rollback/log özeti), Python editör örneği, 10 dönüm sentetik arazi, strateji kamerası, tek yapı yerleştirme (eğim/çakışma/sınır), HUD, otomatik duman testi.
- Risk: arazi üçgen yönü yanlışsa `Terrain.json` → `flipWinding: true`.
- Sonraki (Windows'ta): `tf000` raporu → kurulum → `tf setup`, `tf build`, `tf map`, `tf test`, `tf smoke`, `tf package` → `Content/Maps/M0_Bootstrap.umap` commit.
