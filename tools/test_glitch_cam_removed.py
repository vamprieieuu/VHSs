"""Regression: the ineffective Glitch Cam module is intentionally removed."""
from pathlib import Path
source = (Path(__file__).resolve().parents[1] / "src" / "VHS6RIFT.cpp").read_text(encoding="utf-8")
assert "Glitch Cam" not in source
assert "GLITCH_CAM" not in source
print("PASS: Glitch Cam UI, parameters, and render stage are removed.")
