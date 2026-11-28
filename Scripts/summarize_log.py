"""Basarisiz build/test/oyun loglarini kisa Markdown ozetine cevirir.

Kullanim: python summarize_log.py <log> [--max 25] [--out ozet.md]
Cikis kodu: 0 = hata yok, 1 = hata bulundu, 2 = log okunamadi.
Yalnizca standart kutuphane kullanir (Unreal'in gomulu Python'u ile de calisir).
"""
import argparse
import re
import sys
from collections import OrderedDict

# (kategori, desen) - sira onemli: ilk eslesen kategori kullanilir.
ERROR_PATTERNS = [
    ("Derleyici (MSVC)", re.compile(r"\berror C\d{4}\b")),
    ("Baglayici (LNK)", re.compile(r"\berror LNK\d{4}\b|\bfatal error LNK\d{4}\b")),
    ("Derleyici (clang/gcc)", re.compile(r":\d+:\d+: (fatal )?error:")),
    ("Fatal", re.compile(r"\bfatal error\b", re.IGNORECASE)),
    ("UnrealBuildTool", re.compile(r"^(?:.*\s)?(?:ERROR|Error):\s", re.MULTILINE)),
    ("Unreal log hatasi", re.compile(r"\bLog\w+:\s*(?:Error|Fatal):")),
    ("UHT", re.compile(r"\bUnrealHeaderTool\b.*\berror\b", re.IGNORECASE)),
    ("Duman testi", re.compile(r"TF_SMOKE_RESULT FAIL")),
    ("Otomasyon testi", re.compile(r"Test Completed\. Result=\{Fail", re.IGNORECASE)),
    ("Komut basarisiz", re.compile(r"AutomationTool exiting with ExitCode=[1-9]|BUILD FAILED", re.IGNORECASE)),
]
WARNING_PATTERN = re.compile(r"\bwarning C\d{4}\b|: warning:|\bLog\w+:\s*Warning:")
SUCCESS_MARKERS = ("TF_SMOKE_RESULT PASS", "BUILD SUCCESSFUL", "Result: Succeeded", "AutomationTool exiting with ExitCode=0")

# Ozetten cikarilacak gurultu: tam yol on ekleri ve zaman damgalari.
TIMESTAMP = re.compile(r"^\[\d{4}\.\d{2}\.\d{2}-[\d.:]+\]\[\s*\d+\]")


def normalize(line):
    line = TIMESTAMP.sub("", line).strip()
    return line[:400]


def summarize(text, max_items=25):
    errors = OrderedDict()
    warnings = 0
    for raw in text.splitlines():
        if WARNING_PATTERN.search(raw):
            warnings += 1
            continue
        for category, pattern in ERROR_PATTERNS:
            if pattern.search(raw):
                key = normalize(raw)
                if key not in errors:
                    errors[key] = category
                break
    successes = [m for m in SUCCESS_MARKERS if m in text]
    return errors, warnings, successes


def render(log_name, errors, warnings, successes, max_items):
    out = ["# Log ozeti: %s" % log_name, ""]
    out.append("- Benzersiz hata: %d" % len(errors))
    out.append("- Uyari satiri: %d" % warnings)
    if successes:
        out.append("- Basari isaretleri: %s" % ", ".join(successes))
    if errors:
        out.append("")
        out.append("## Ilk hatalar")
        by_cat = OrderedDict()
        for line, cat in errors.items():
            by_cat.setdefault(cat, []).append(line)
        shown = 0
        for cat, lines in by_cat.items():
            out.append("")
            out.append("### %s (%d)" % (cat, len(lines)))
            for line in lines:
                if shown >= max_items:
                    break
                out.append("- `%s`" % line.replace("`", "'"))
                shown += 1
        if len(errors) > max_items:
            out.append("")
            out.append("... ve %d hata daha (tam log: %s)" % (len(errors) - max_items, log_name))
    return "\n".join(out) + "\n"


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("log")
    parser.add_argument("--max", type=int, default=25)
    parser.add_argument("--out")
    args = parser.parse_args(argv)
    try:
        with open(args.log, "r", encoding="utf-8", errors="replace") as handle:
            text = handle.read()
    except OSError as exc:
        print("Log okunamadi: %s" % exc, file=sys.stderr)
        return 2
    errors, warnings, successes = summarize(text, args.max)
    report = render(args.log, errors, warnings, successes, args.max)
    if args.out:
        with open(args.out, "w", encoding="utf-8") as handle:
            handle.write(report)
    sys.stdout.write(report)
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())
