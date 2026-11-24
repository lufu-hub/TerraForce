"""Unreal Editor Python otomasyonu (TF-002 ornegi): bos M0 haritasini olusturur.

Harita bilerek bostur; isik, gokyuzu ve arazi calisma aninda C++ (ATFGameMode) tarafindan uretilir.
Boylece ikili .umap icerigi minimumda kalir ve sahne metin kaynaklarindan tekrar uretilebilir.

Calistirma: Scripts\\tf.cmd map  (UnrealEditor-Cmd -ExecutePythonScript=...)
Basari isareti: Saved/Automation/map_ok.txt dosyasi ve logda TF_MAP_OK.
"""
import os

import unreal

MAP_PATH = "/Game/Maps/M0_Bootstrap"


def main():
    exists = unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH)
    if not exists:
        level_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
        if not level_subsystem.new_level(MAP_PATH):
            raise RuntimeError("new_level basarisiz: " + MAP_PATH)
        if not level_subsystem.save_current_level():
            raise RuntimeError("save_current_level basarisiz")
    unreal.log("TF_MAP_OK %s (onceden vardi: %s)" % (MAP_PATH, exists))

    marker_dir = os.path.join(unreal.Paths.project_saved_dir(), "Automation")
    os.makedirs(marker_dir, exist_ok=True)
    with open(os.path.join(marker_dir, "map_ok.txt"), "w", encoding="utf-8") as handle:
        handle.write(MAP_PATH + "\n")


try:
    main()
except Exception as exc:  # noqa: BLE001 - hatayi loga yazip editoru yine de kapat
    unreal.log_error("TF_MAP_FAIL %s" % exc)
finally:
    unreal.SystemLibrary.quit_editor()
