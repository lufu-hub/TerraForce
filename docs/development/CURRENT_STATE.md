# Güncel durum
Durum: plan paketi, henüz Unreal kurulum/derleme/test kanıtı yok. Yerel Windows repo dosyaları görülmedi. Ekran görüntüsünde GitHub Desktop'ta yerel web projesine ait olabilecek uncommitted değişiklikler bulunuyor. Bunları silme, taşıma veya üzerine yazma. GitHub uzak repo başlangıçta boştu; dokümantasyon ilk içerik olarak ekleniyor.
Sonraki iş: TF-000 ortam ve mevcut repo incelemesi. Önce yerel Git durumu ve remote geçmişi karşılaştır, gerekiyorsa güvenli yedek ve ayrı çalışma alanı öner. Master Tree'ye dokunma. Donanım, disk, Unreal/C++ kurulumları ve Claude yetkileri doğrulanacak.
Tamamlanan oyun özelliği: doğrulanmadı. Kabul testi: çalıştırılmadı. Her paket sonunda bu dosyaya tarih, commit, sonuç, risk, sonraki görev ve gerekli kullanıcı kararı ekle.

## 2026-11-24 — TF-000 (kısmi)
- Sonuç: uzak repo ve Claude bulut ortamı incelendi; yerel PC/repo için salt okunur betik eklendi (`tools/tf000/`). Ayrıntı: `reports/TF-000.md`.
- Test: betik bu ortamda çalıştırılamadı (PowerShell/Windows yok); elle gözden geçirildi.
- Risk: bulut oturumu UE derleyemez; yerel prototip uzak repoda yok, yerel değişiklikler yedeksiz.
- Sonraki: kullanıcı `TF000_Incele.cmd` çalıştırıp raporu paylaşır → TF-000 kapanır.
- Gerekli karar: K1–K4 (`reports/TF-000.md` §7).

## 2026-11-24 — TF-001…TF-004 kodu hazır, Windows'ta doğrulanmadı
- Onay: D05, D13–D17 (DECISIONS.md).
- Yapıldı: UE5 C++ projesi (`TerraForge.uproject`, modül `TerraForge`), Git LFS (`.gitattributes`), `tf` komutları (build/test/map/smoke/package/checkpoint/rollback/log özeti), Python editör örneği, 10 dönüm sentetik arazi, strateji kamerası, tek yapı yerleştirme (eğim/çakışma/sınır), Türkçe HUD, otomatik duman testi.
- Test kanıtı (bulut): simülasyon çekirdeği 4025/4025, araç testleri 6/6, veri doğrulama OK. **Unreal derlemesi, editör, oyun ve paket hiç çalıştırılmadı.**
- Risk: Unreal API ayrıntılarında derleme hatası çıkabilir (ilk `tf build`'de düzeltilecek); arazi üçgen yönü yanlışsa `Terrain.json` → `flipWinding: true`.
- Sonraki (Windows'ta): `tf000` raporu → kurulum → `tf setup`, `tf build`, `tf map`, `tf test`, `tf smoke`, `tf package` → `Content/Maps/M0_Bootstrap.umap` commit.
- Gerekli karar: yok.
