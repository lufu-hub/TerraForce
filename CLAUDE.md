# TerraForge — Claude giriş noktası
Çalışma sözleşmesi ve sırası: @docs/development/CLAUDE.md
Güncel durum: docs/development/CURRENT_STATE.md · Kararlar: docs/development/DECISIONS.md · Görevler: docs/development/BACKLOG.json

Komutlar (Windows, repo kökünde): `tf build` · `tf test` · `tf smoke` · `tf package` · `tf checkpoint "mesaj"` — ayrıntı: `tf help`.
Unreal olmadan çalışan kontroller: `Tests/SimTests` (CMake), `python Scripts/validate_data.py`, `python -m unittest discover -s Tests/ToolTests`.
Yapı: `Source/TerraForge/Public/Sim` saf C++ simülasyon çekirdeği (Unreal tipi yok) · `Content/Data` JSON katalog ve metinler (varsayılan dil `en`, `tr` yerelleştirme) · `Scripts` otomasyon.

Klon kuralı (D24, D28): bu klasör UE çalışma klonudur. Eski web prototipi klonundan bu depoya commit/push/pull yapma. Karar numaraları ve onaylı kararlar yalnızca `docs/development/DECISIONS.md` içinde; yeni karar bir sonraki boş numarayla yazılır.
