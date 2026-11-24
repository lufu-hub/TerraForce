# Karar kaydı
## Önceden onaylanmış tasarım
- D01: Manor Lords tarzı gerçek 3D nihai hedef; serbest strateji kamerası ve yaya keşif.
- D02: 5–50 dönümlük gerçek/gerçeğe yakın arazi.
- D03: Aile başlangıç ve hamilelik kuralları GAME_VISION.md içinde; çocuklu senaryoda ek hamilelik seçilebilir.
- D04: Master Tree seviyeleri, araştırma/uygulama ayrımı, yatay bağlantılar, bilgi kartı; mevcut düğümler revizyon bitmeden sabitlenmeyecek.
## Öneri / doğrulama bekleyen
- D05: UE5 + C++/Blueprint/Python; PoC ve donanım doğrulaması gerekir.
- D06: Strateji merkezli ana döngü, yaşam/keşif ikinci katman; ürün sahibi onayı bekliyor.
- D07: 10 dönüm sentetik ilk harita, 12 aylık tahmini yol haritası.
- D08: Pro başlangıç, Max ihtiyaç halinde; ücretli API yok.
- D09: Git LFS, çevrimdışı Windows, 1080p performans hedefi.
- D10: Unreal MCP ancak PoC sonrası.
- D11: Geliştirme Master Tree ile oyun içi ilerleme ağacının ilişkisi netleştirilecek; aynı yapı varsayılmayacak.
- D12: Çocuklu başlangıçta ek çocuk üst sınırı açık karar.
Önerileri onaylanmış gibi uygulama; erken PoC geri alınabilir, büyük geçiş için onay al.
## Kullanıcı onayı — 2026-11-24 (TF-000 raporu §5–§7)
- D05 → **Onaylandı (PoC):** UE5 + C++, Python editör otomasyonu. Hedef sürüm 5.7 (`TerraForge.uproject`); kurulumda betikle doğrulanır, farklıysa burada güncellenir.
- D13: Unreal derleme/test Windows'ta yerel Claude Code ile; bulut oturumu yalnızca motordan bağımsız işler.
- D14: Unreal çalışması ayrı yeni klonda; web prototipi ve Master Tree mevcut yerel klasörde dokunulmadan kalır.
- D15: Simülasyon çekirdeği saf C++ (`Source/TerraForge/Public/Sim`), Unreal tipi kullanmaz, Unreal'siz test edilir.
- D16: Yapı kataloğu, arazi ayarları ve Türkçe metinler `Content/Data/*.json` içinde; kodda liste yok.
- D17 (M0 geçici, geri alınabilir): arazi ProceduralMesh (Landscape: TF-050); girdi doğrudan tuş okuma (Enhanced Input varlıkları: TF-008); HUD Canvas metni (UMG: TF-008); harita boş, sahne C++ ile çalışma anında kurulur.
