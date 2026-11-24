"""Content/Data altindaki oyun verisini dogrular (Unreal gerektirmez).

Kullanim: python validate_data.py [proje_koku]
Cikis kodu: 0 = gecerli, 1 = hata var.
"""
import json
import os
import sys


def load(path, errors):
    try:
        with open(path, "r", encoding="utf-8") as handle:
            return json.load(handle)
    except (OSError, ValueError) as exc:
        errors.append("%s: okunamadi/cozumlenemedi (%s)" % (path, exc))
        return None


def is_number(value):
    return isinstance(value, (int, float)) and not isinstance(value, bool)


def check_color(name, value, errors):
    if not (isinstance(value, list) and len(value) >= 3 and all(is_number(v) and 0 <= v <= 1 for v in value[:3])):
        errors.append("%s: renk 0-1 araliginda en az 3 sayi olmali" % name)


def validate(root):
    errors = []
    data_dir = os.path.join(root, "Content", "Data")

    strings = {}
    tr = load(os.path.join(data_dir, "Localization", "tr.json"), errors)
    if tr is not None:
        strings = tr.get("strings") or {}
        if not isinstance(strings, dict) or not strings:
            errors.append("tr.json: 'strings' nesnesi bos veya yok")
            strings = {}
        for key, value in strings.items():
            if not isinstance(value, str) or not value.strip():
                errors.append("tr.json: '%s' bos metin" % key)

    terrain = load(os.path.join(data_dir, "Terrain.json"), errors)
    if terrain is not None:
        for field in ("sizeXMeters", "sizeYMeters", "cellSizeMeters", "maxBuildHeightDeltaMeters"):
            if not is_number(terrain.get(field)) or terrain[field] <= 0:
                errors.append("Terrain.json: '%s' pozitif sayi olmali" % field)
        if is_number(terrain.get("sizeXMeters")) and is_number(terrain.get("sizeYMeters")):
            donum = terrain["sizeXMeters"] * terrain["sizeYMeters"] / 1000.0
            if not 1 <= donum <= 60:
                errors.append("Terrain.json: alan %.1f donum; vizyon 5-50 donum" % donum)
        if "groundColor" in terrain:
            check_color("Terrain.json groundColor", terrain["groundColor"], errors)

    buildings = load(os.path.join(data_dir, "Buildings.json"), errors)
    if buildings is not None:
        entries = buildings.get("buildings")
        if not isinstance(entries, list) or not entries:
            errors.append("Buildings.json: 'buildings' bos olmayan dizi olmali")
            entries = []
        seen = set()
        for index, entry in enumerate(entries):
            label = "Buildings.json[%d]" % index
            bid = entry.get("id") if isinstance(entry, dict) else None
            if not isinstance(bid, str) or not bid:
                errors.append("%s: 'id' eksik" % label)
                continue
            label = "Buildings.json '%s'" % bid
            if bid in seen:
                errors.append("%s: tekrar eden id" % label)
            seen.add(bid)
            for field in ("footprintX", "footprintY"):
                if not isinstance(entry.get(field), int) or entry[field] < 1:
                    errors.append("%s: '%s' >= 1 tam sayi olmali" % (label, field))
            if not is_number(entry.get("heightMeters")) or entry["heightMeters"] <= 0:
                errors.append("%s: 'heightMeters' pozitif olmali" % label)
            key = entry.get("nameKey")
            if strings and key not in strings:
                errors.append("%s: nameKey '%s' tr.json icinde yok" % (label, key))
            if "color" in entry:
                check_color(label + " color", entry["color"], errors)

    # Kodun kullandigi sabit metin anahtarlari
    required = [
        "hud.title", "hud.help.camera", "hud.help.build", "hud.buildings_count", "hud.mode.build",
        "placement.ok", "placement.placed", "placement.out_of_bounds", "placement.overlap",
        "placement.too_steep", "placement.invalid_size", "placement.not_ready",
    ]
    for key in required:
        if strings and key not in strings:
            errors.append("tr.json: kodun kullandigi '%s' anahtari eksik" % key)
    return errors


def main(argv=None):
    argv = sys.argv[1:] if argv is None else argv
    root = argv[0] if argv else os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    errors = validate(root)
    if errors:
        print("VERI DOGRULAMA BASARISIZ (%d):" % len(errors))
        for error in errors:
            print(" - " + error)
        return 1
    print("Veri dogrulama: OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
