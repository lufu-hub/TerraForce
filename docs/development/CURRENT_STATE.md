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
