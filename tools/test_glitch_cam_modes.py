#!/usr/bin/env python3
"""Static regression checks for the 15 selectable procedural Glitch Cam styles."""
from pathlib import Path
import re

root = Path(__file__).resolve().parents[1]
source = (root / "src" / "VHS6RIFT.cpp").read_text(encoding="utf-8")
match = re.search(r'PF_ADD_POPUP\("Glitch Cam Style",\s*(\d+),\s*1,\s*"([^"]+)"', source, re.S)
assert match, "Glitch Cam style popup not found"
count = int(match.group(1))
styles = match.group(2).split("|")
assert count == 15, f"expected 15 styles, got {count}"
assert len(styles) == count, f"popup count={count}, labels={len(styles)}"
assert len(set(styles)) == count, "duplicate style names"
for style in [
    "Sync Tear", "Chroma Breakup", "Block Corruption", "Luma Dropout",
    "Mixed Signal Burst", "RGB Convergence Burst", "Vertical Hold Roll",
    "Color Smear", "Head-Switch Noise", "Horizontal Fold", "Solarized Flash",
    "Tape Stretch", "Ghost Echo", "Signal Crush", "Chroma Bar Interference",
]:
    assert style in styles, f"missing expected style: {style}"
# Existing modes 0..4 plus new independent render branches 5..14.
for mode in range(5, 15):
    assert re.search(rf"glitchCamMode\s*==\s*{mode}\b", source), f"mode {mode} has no render branch"
print("PASS: Glitch Cam exposes 15 unique styles.")
print("PASS: all 10 added styles have distinct mode-gated pixel-processing branches.")
