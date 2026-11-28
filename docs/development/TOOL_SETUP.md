# Kurulum ve doğrulama sırası v2.0
1. Yerel git status, remote, HEAD, mevcut prototip ve TF-000 raporu; temiz olmayan ağacı otomatik değiştirme. Git/LFS kontrol.
2. Windows CPU/RAM/disk/GPU, UE/VS/SDK kurulumunun tamamlanıp tamamlanmadığı; RTX 4060 hedef, gerçek ölçüm gerekir.
3. Epic Launcher kararlı UE5, uyumlu Visual Studio C++/SDK; güvenli alanda boş proje build ve basit 2.5D kamera testi.
4. Claude Code Pro oturum, ücretli API yok; docs/development/CLAUDE.md açıkça okut, minimum izin. Kurulum (Windows, PowerShell, yönetici gerekmez): `irm https://claude.ai/install.ps1 | iex` → yeni pencere → `claude --version` → repo kökünde `claude` ve tarayıcıdan giriş (`ANTHROPIC_API_KEY` tanımlanmaz). Klasöre güven sorusuna evet; izinler `.claude/settings.json`'da (tf ve okuma/commit serbest; push, pull, dal değiştirme, winget ve .uasset/.umap sorulur; silme, reset, clean, stash, force push ve web klonu yasak). İsteğe bağlı Notion: `claude mcp add --transport http --scope local notion https://mcp.notion.com/mcp`, sonra `/mcp` → notion → Authenticate.
5. Python/editor utilities küçük örnek; MCP gerekirse ayrı sandbox/uyumluluk.
6. Placeholder UE/Blender/Fab lisanslı varlık; asset kayıt. Animasyon ve ses paralel, ileri grafik beklenmez.
7. QGIS/DEM ve opsiyonel Cesium gerçek hazırlanmış bölgeler aşamasında; Audacity, Mixamo/Tripo uygun işlerde.
8. Sürüm sabitleme, tekrar kurulabilir build ve risk bazlı test. Resmî kaynaklar: https://dev.epicgames.com/documentation/ ; https://code.claude.com/docs/ ; https://git-lfs.com/ ; https://www.blender.org/ ; https://qgis.org/ ; https://www.fab.com/ .
