#!/usr/bin/env python3
"""Source-level wiring/regression audit for VHS6RIFT; does not render pixels."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "src" / "VHS6RIFT.cpp").read_text(encoding="utf-8")

def require(condition: bool, message: str) -> None:
    if not condition:
        print(f"FAIL: {message}")
        sys.exit(1)
    print(f"PASS: {message}")

m = re.search(r"enum\s*\{(.*?)P_COUNT\s*\};", source, re.S)
require(m is not None, "parameter enum is present")
ids = list(dict.fromkeys(re.findall(r"\bP_[A-Z0-9_]+\b", m.group(1))))
require(len(ids) == 164 and ids[0] == "P_INPUT", f"parameter ID inventory has expected shape ({len(ids)} IDs incl. implicit input)")

render_tail = source[source.index("static inline float clamp01"):]
structural = {x for x in ids if "TOPIC" in x or x.endswith("_END") or x in {"P_INPUT", "P_COUNT"}}
unused = []
for param in ids:
    if param in structural:
        continue
    if len(re.findall(r"\b" + re.escape(param) + r"\b", render_tail)) == 0:
        unused.append(param)
require(not unused, "all non-structural parameter IDs are referenced after registration" if not unused else "unused parameter IDs: " + ", ".join(unused))

require("const int otherEngine = selOtherEngine;" in source, "Other VHS Engine Off is a true neutral selection")
require("const int tapeType = selTapeType;" in source, "Tape Type Off is a true neutral selection")
require("const float otherStrength = otherEnabled ? otherStrengthRaw * drive : 0.0f;" in source, "Other VHS module has explicit enable/strength gate")
require("const float effTapeStrength = tapeEnabled ? tapeStrengthRaw : 0.0f;" in source, "Tape module has explicit enable/strength gate")
require("if (strength <= 0.0001f)" in source, "Master Strength zero bypass remains present")
require("const float crtScanlines = crtEnabled ?" in source and "if (crtEnabled)" in source, "CRT parameters are gated by CRT enable")
require("const bool chosenEvent = s.events && effectiveEventRate > 0.0f" in source, "Glitch Cam event gate uses effective event rate")
require("PF_ADD_POPUP(\"Built-in OSD Style\", 5" in source, "built-in OSD selector is registered")
require("if (osdStyle > 0 && osdOpacity > 0.0f)" in source and "osdGlyphRow(osdText[ci], row)" in source, "built-in OSD has a wired pixel-render path")
require("PF_VERSION(1, 2, 0, PF_Stage_RELEASE, 1)" in source, "plugin version identity unchanged")
require('Name { "VHS6RIFT" }' in (ROOT / "resources" / "VHS6RIFTPiPL.r").read_text(encoding="utf-8"), "PiPL name remains VHS6RIFT")
print("PASS: static parameter wiring audit completed. No pixel-render, Windows build, or AE runtime claim is implied.")
