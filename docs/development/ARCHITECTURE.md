# Mimari öneri
Sunum (kamera, UI, aktörler) ile simülasyon çekirdeği ayrılır. Game State + Event System ortak veri sözleşmesi; Data Layer sürümlü Save/Load, kataloglar, dengeleme. Görsel aktörler simülasyon durumunu temsil eder; her kare kaynak hesabı yapma. Zaman ve rastlantısal olaylar test edilebilir; kayıtların sürüm geçişini planla.
P0 TF-Core (saat/olay/durum), TF-World (arazi/eğim/su), TF-Camera (strateji/keşif), TF-Build (yerleşim).
P1 TF-Resources (envanter/üretim/tüketim), TF-Environment (hava/mevsim/bitki), TF-Family (yaş/üyeler), TF-Characters (hareket/görev/animasyon).
P2 TF-Survival (ihtiyaçlar), TF-Events (yaşam olayları), TF-Economy (maliyet/sürdürülebilirlik), TF-Progression (araştırma/uygulama).
Veri odaklı kataloglar, modül sözleşmeleri, tekrarlanabilir build. Source, Content, Config, Scripts, Tests ayrımı. .uasset/.umap ikili dosyaları elle birleştirme; Git LFS ve uygun kilitleme, gerektiğinde One File Per Actor. Erken performans ölçümü: instancing, LOD, seyrek simülasyon tick, grafik profilleri. Gerçek arazi hattı: konum/parsel → DEM/iklim → QGIS → Landscape importer → çevre PCG → simülasyon. İlk arazi sentetik.
