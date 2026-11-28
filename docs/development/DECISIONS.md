# Karar kaydı — v2.0 (28.11.2026)
Durumlar: APPROVED ürün kararı; PROPOSED teknik öneri; OPEN belirsiz; SUPERSEDED tarihsel karar. Son onaylı karar önceki çelişkili kuralı geçersiz kılar; öneriler uygulama yetkisi değildir.
## APPROVED
- D01 SUPERSEDED: önceki Manor Lords tarzı 3D ve zorunlu yaya keşfi; D13 geçerlidir.
- D02: gerçek/gerçeğe yakın kırsal arazi, 5–50 dönüm önceki tasarım referansı; kesin boyut ve genişleme sınırı açık (D19).
- D03: aile kuralları GAME_VISION; çocuklu ve çocuksuz çiftte hamilelik aç/kapat; D12 açık.
- D04: geliştirme Master Tree seviyeleri, araştırma/uygulama ayrımı, yatay bağlantılar ve bilgi kartı; kullanıcı revizyonu bitmeden mevcut düğümler sabitlenmeyecek.
- D05: UE5 artık onaylı motor (başlangıçtan görsel evrime kadar); C++/Blueprint/Python dağılımı ve sürüm teknik öneri, PoC doğrulaması gerekir.
- D13: Gameplay First; 2.5D sunum (Project Zomboid/AoE II düzeyi referans), temel görsellerle oynanabilir sürüm ve içerik önce; animasyon/ses paralel. İleri grafik ve serbest yaya kamera ilk sürüm şartı değil. İleride aynı UE projesinde görsel evrim.
- D14: oyun dili İngilizce; yerelleştirme altyapısı baştan önerilir.
- D15: World Atlas → ülke → gerçek şehir çevresinden hazırlanmış kırsal bölge → yerel Realtor → Property. Ülke büyüklüğü ve potansiyel oyuncu sayısına göre bölge adedi; ileri/geri arazi kataloğu, boyut, fiyat, koşullar. Tüm dünya dinamik oynanabilir değil.
- D16: ülke etkileri yalnız ekonomik istikrar, sağlık, eğitim politikası, güvenlik, yaşa bağlı ölüm, sosyal güvence. Yerel ücret, alım gücü, elektrik/internet/ticaret altyapısı ve mevzuat ülkeye göre farklılaşmaz. Fiziksel coğrafya/mesafe, kaynaklar ve talep koşulları etkiler.
- D17: ortak ekonomi, sınırlı talep, katma değer, pratik+bilgi+ekipman+zaman+şans; Nursery, Creative Economy, Contract Production dahil; Knowledge Economy, Circular Economy ve Rental Properties hariç. Hasarlı uygun hasat düşük bedelle satılabilir.
- D18: yapılar aynı kimlikte upgrade ile büyür, ayrı seviye binası gerekmez; konut aileleri 1–4. Hospitality yalnız uygun arazide, işletmeci/eş, rapor ve önemli karar; mikro yönetim yok; yüksek seviye yüksek kâr garantisi değil.
## PROPOSED / OPEN
- D06: 3D dünya + 2.5D izometrik strateji kamerası; ortografik/perspektif, pan/zoom/rotate, kontrol ve olası yaya keşfi PoC; serbest kamera ertelendi.
- D07: yaklaşık 10 dönüm sentetik ilk harita ve eski 12 aylık takvim tahminiydi; kabul kapıları esas, tarih garantisi yok.
- D08: Pro başlangıç, Max ölçülen ihtiyaç halinde; ücretli API yok.
- D09: Git LFS, Windows, 1080p performans hedefi; donanımla doğrula.
- D10: Unreal MCP ancak PoC sonrası, zorunlu değil.
- D11: geliştirme Master Tree ve oyun içi skill tree aynı yapı varsayılmayacak.
- D12: çocuklu başlangıçta toplam/yeni çocuk üst sınırı açık; çocuksuz başlangıçta 3 onaylı.
- D19: kesin parsel boyutu/sınırı, hazır bölge içinde serbest çizim, şehir listesi, gerçek veri sadakati; 5x5–30x30 hücre/90 dönüm gibi önceki örnekler bağlayıcı değil.
- D20: ücretsiz starter araziler ve oynayarak/ödeyerek harita açma tasarım yönü; hesap para birimi, ödeme ve ticari model açık, gerçek ödeme uygulaması onaysız yok.
- D21: ekonomik denge laboratuvarı kullanıcı ve arkadaşıyla; kesin fiyat, süre dışındaki üretim/tüketim katsayıları, şans, talep ve ROI açık. Hospitality 5 seviye/çadır veya bungalov ilk seviye aday, kesin değil.
- D22: co-op zaman kontrolü, ortak mülk/retirement detayları, 60 öncesi ölüm sonucu açık.
Kaynak: Terraforge tasarım görüşmeleri, en son 28.11.2026 Gameplay First güncellemesi. Oyun tasarımının ayrıntıları GAME_VISION.md; teknik sözleşmeler ARCHITECTURE.md; uygulama sırası ROADMAP.md/BACKLOG.json.
## APPROVED — operasyonel kararlar (serene dalından taşındı, 28.11.2026)
Kayıt: kullanıcı onayı 24.11.2026 (TF-000 raporu §5–§7). Serene dalında bunlar D13–D17 olarak numaralanmıştı; v2 ürün kararlarıyla çakıştığı için **yeniden numaralandı** (eski D13–D17 atıfları artık D23–D27).
- D23 (eski D13): Unreal derleme/test Windows'ta yerel Claude Code ile; bulut oturumu yalnızca motordan bağımsız işler.
- D24 (eski D14): Unreal çalışması ayrı yeni klonda; eski yerel klasör (web prototipi, Master Tree) dokunulmadan kalır. Uzantı (28.11, kurucu): iki klon aynı uzak depoyu paylaşıyor; eski klon bu depoya commit/push/pull yapmaz ve web prototipi bu depoya girmez (kök dosya adları — CLAUDE.md, README.md, .gitignore — çakışır; depo public).
- D25 (eski D15): simülasyon çekirdeği saf C++ (`Source/TerraForge/Public/Sim`), Unreal tipi kullanmaz, Unreal'siz test edilir.
- D26 (eski D16): yapı kataloğu, arazi ayarları ve metinler `Content/Data/*.json` içinde; kodda liste yok. Güncelleme (28.11): varsayılan metin dili İngilizce (D14, `en.json`); `tr.json` yerelleştirme olarak kalır, anahtar kümesi doğrulayıcıyla eşit tutulur.
- D27 (eski D17, M0 geçici, geri alınabilir): arazi ProceduralMesh (Landscape: TF-050); girdi doğrudan tuş okuma (Enhanced Input: TF-008); HUD Canvas metni (UMG: TF-008); harita boş, sahne C++ ile çalışma anında kurulur.
- Not (D05 ile ilgili, ayrı karar numarası değil): hedef UE sürümü 5.7 (`TerraForge.uproject`); yerelde doğrulanıp sabitlenir, farklıysa burada güncellenir.
## APPROVED — belge düzeni (kurucu, 28.11.2026)
- D28: Karar numaraları ve onaylı kararlar için tek kaynak bu depodaki `DECISIONS.md`. Claude proje belgeleri (vizyon, bu-hafta, çalışma düzeni, uzlaştırma raporu) sohbet/araştırma notudur; çelişirse bu dosya geçerlidir. Yeni karar bu dosyaya, bir sonraki boş numarayla yazılır (sıradaki: D30).
## PROPOSED
- D29: Claude projesinde 22–23.11'de başlatılan React/Vite web prototipi (eski klonda) UE5 yolunu (D05/D13) değiştirmez; yeni özellik eklenmez, korunur. İçerik/katalog araştırması UE Data Asset/JSON kataloğuna akar. Web prototipinin akıbeti (arşiv, ayrı depo) kurucu onayı bekler.
