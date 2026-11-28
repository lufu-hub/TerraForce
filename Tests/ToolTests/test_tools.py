"""Otomasyon araclarinin testleri (Unreal gerektirmez). Calistirma: python -m unittest discover -s Tests/ToolTests"""
import json
import os
import shutil
import sys
import tempfile
import unittest

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "Scripts"))

import summarize_log  # noqa: E402
import validate_data  # noqa: E402

MSVC_FAIL = r"""
[1/12] Compile TFGameMode.cpp
C:\TF\Source\TerraForge\Private\TFGameMode.cpp(42): error C2065: 'Foo': undeclared identifier
C:\TF\Source\TerraForge\Private\TFGameMode.cpp(42): error C2065: 'Foo': undeclared identifier
C:\TF\Source\TerraForge\Private\TFHUD.cpp(10): warning C4996: deprecated
TFGameMode.cpp.obj : error LNK2019: unresolved external symbol
Total time in Parallel executor: 12.00 seconds
"""

SMOKE_PASS = """[2026.11.24-10.00.00:000][  0]LogTerraForge: Display: TF_SMOKE_START
[2026.11.24-10.00.05:000][300]LogTerraForge: Display: TF_SMOKE_FPS 61.2 (300 kare / 4.9 sn)
[2026.11.24-10.00.05:000][300]LogTerraForge: Display: TF_SMOKE_RESULT PASS
[2026.11.24-10.00.05:000][300]LogTerraForge: Warning: ornek uyari
"""

SMOKE_FAIL = "[2026.11.24-10.00.05:000][300]LogTerraForge: Error: TF_SMOKE_RESULT FAIL: arazi yok\n"


class SummarizeTests(unittest.TestCase):
    def test_msvc_errors_deduplicated_and_categorized(self):
        errors, warnings, _ = summarize_log.summarize(MSVC_FAIL)
        self.assertEqual(len(errors), 2)
        self.assertEqual(warnings, 1)
        self.assertIn("Derleyici (MSVC)", errors.values())
        self.assertIn("Baglayici (LNK)", errors.values())

    def test_smoke_pass_has_no_errors(self):
        errors, warnings, successes = summarize_log.summarize(SMOKE_PASS)
        self.assertEqual(errors, {})
        self.assertEqual(warnings, 1)
        self.assertIn("TF_SMOKE_RESULT PASS", successes)

    def test_smoke_fail_detected_and_timestamp_removed(self):
        errors, _, _ = summarize_log.summarize(SMOKE_FAIL)
        self.assertEqual(len(errors), 1)
        line = next(iter(errors))
        self.assertTrue(line.startswith("LogTerraForge"), line)

    def test_cli_exit_codes(self):
        with tempfile.TemporaryDirectory() as tmp:
            bad = os.path.join(tmp, "bad.log")
            good = os.path.join(tmp, "good.log")
            with open(bad, "w", encoding="utf-8") as handle:
                handle.write(MSVC_FAIL)
            with open(good, "w", encoding="utf-8") as handle:
                handle.write(SMOKE_PASS)
            out = os.path.join(tmp, "ozet.md")
            self.assertEqual(summarize_log.main([bad, "--out", out]), 1)
            self.assertTrue(os.path.exists(out))
            self.assertEqual(summarize_log.main([good]), 0)
            self.assertEqual(summarize_log.main([os.path.join(tmp, "yok.log")]), 2)


class ValidateDataTests(unittest.TestCase):
    def test_repository_data_is_valid(self):
        self.assertEqual(validate_data.validate(ROOT), [])

    def test_detects_broken_building(self):
        with tempfile.TemporaryDirectory() as tmp:
            shutil.copytree(os.path.join(ROOT, "Content", "Data"), os.path.join(tmp, "Content", "Data"))
            path = os.path.join(tmp, "Content", "Data", "Buildings.json")
            with open(path, encoding="utf-8") as handle:
                data = json.load(handle)
            data["buildings"].append(dict(data["buildings"][0]))  # tekrar eden id
            data["buildings"].append({"id": "x", "nameKey": "eksik.anahtar", "footprintX": 0, "footprintY": 2, "heightMeters": 3})
            with open(path, "w", encoding="utf-8") as handle:
                json.dump(data, handle)
            errors = validate_data.validate(tmp)
            joined = "\n".join(errors)
            self.assertIn("tekrar eden id", joined)
            self.assertIn("footprintX", joined)
            self.assertIn("eksik.anahtar", joined)

    def test_detects_localization_key_mismatch(self):
        with tempfile.TemporaryDirectory() as tmp:
            shutil.copytree(os.path.join(ROOT, "Content", "Data"), os.path.join(tmp, "Content", "Data"))
            path = os.path.join(tmp, "Content", "Data", "Localization", "tr.json")
            with open(path, encoding="utf-8") as handle:
                data = json.load(handle)
            data["strings"].pop("placement.ok")
            data["strings"]["fazladan.anahtar"] = "x"
            with open(path, "w", encoding="utf-8") as handle:
                json.dump(data, handle)
            joined = "\n".join(validate_data.validate(tmp))
            self.assertIn("placement.ok", joined)
            self.assertIn("fazladan.anahtar", joined)


if __name__ == "__main__":
    unittest.main()
