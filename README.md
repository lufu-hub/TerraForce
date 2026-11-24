# TerraForge

Gerçekçi 3D aile yaşam/strateji simülasyonu (Unreal Engine 5, Windows). Plan ve kurallar: `docs/development/`.

## İlk kurulum (Windows, bir kez)
1. **Epic Games Launcher** → Unreal Engine **5.7** kurun (`TerraForge.uproject` içindeki sürüm).
2. **Visual Studio 2022 Community** → "C++ ile oyun geliştirme" ve "C++ ile masaüstü geliştirme" iş yükleri.
3. **Git for Windows** (Git LFS dahil).
4. Bu repoyu **yeni ve ayrı bir klasöre** klonlayın (eski web prototipi klasörüne değil).

## Günlük komutlar (repo klasöründe komut istemi)
```
tf setup      # bir kez: Git LFS + proje dosyaları
tf build      # derle
tf map        # bir kez: boş M0 haritasını oluştur
tf run        # oyunu aç
tf smoke      # otomatik duman testi
tf package    # Windows oyun paketi (Packaged\Windows\TerraForge.exe)
```

## Oyun içi (Milestone 0)
WASD / kenar kaydırma: kamera · Q/E veya orta tuş: döndür · tekerlek: yakınlaştır · B: inşa modu · sol tık: yerleştir · R: döndür · sağ tık/Esc: iptal · F10: çıkış.
