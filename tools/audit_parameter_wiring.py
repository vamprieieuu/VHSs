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
require(len(ids) == 136 and ids[0] == "P_INPUT", f"parameter ID inventory has expected shape ({len(ids)} IDs incl. implicit input)")

render_tail = source[source.index("static inline float clamp01"):]
structural = {x for x in ids if "TOPIC" in x or x.endswith("_END") or x in {"P_INPUT", "P_COUNT"}}
unused = []
for param in ids:
    if param in structural:
        continue
    if len(re.findall(r"\b" + re.escape(param) + r"\b", render_tail)) == 0:
        unused.append(param)
require(not unused, "all non-structural parameter IDs are referenced after registration" if not unused else "unused parameter IDs: " + ", ".join(unused))

require("const bool craftEngineEnabled = getCheck(params, P_CRAFT_ENABLE) && otherEnabled;" in source, "VHS Craft has its own enable gate")
require("const bool legendEngineEnabled = getCheck(params, P_LEGEND_ENABLE) && otherEnabled;" in source, "VHS Legend has its own enable gate")
require("const bool signalReferenceEnabled = getCheck(params, P_SIGNAL_ENABLE) && otherEnabled;" in source, "Signal Reference has its own enable gate")
require("const int tapeType = selTapeType;" in source, "Tape Type Off is a true neutral selection")
require("const float otherStrength = clamp01(otherStrengthRaw) * drive;" in source, "independent signal engines share a master strength level")
require("const float effTapeStrength = tapeEnabled ? tapeStrengthRaw : 0.0f;" in source, "Tape module has explicit enable/strength gate")
require("PF_ADD_CHECKBOX(\"Enable VHS Overlay on Source Footage\"" in source and "PF_ADD_LAYER" not in source, "overlay is a yes/no source-footage toggle, not a layer selector")
require("CAMERA ENGINE" not in source and "kCameraProfiles" not in source, "selectable camera/color engine removed")
require("if (strength <= 0.0001f)" in source, "Master Strength zero bypass remains present")
require("P_CRT_ENABLE" not in source and "CRT TV Engine" not in source, "requested CRT TV system has been removed")
require("const bool chosenEvent = effectiveEventRate > 0.0f" in source and "chosenEvent && glitchCamEnabled" in source, "Glitch Cam uses effective event rate and explicit enable gate")
require("PF_VERSION(1, 2, 0, PF_Stage_RELEASE, 1)" in source, "plugin version identity unchanged")
require('Name { "VHS6RIFT" }' in (ROOT / "resources" / "VHS6RIFTPiPL.r").read_text(encoding="utf-8"), "PiPL name remains VHS6RIFT")
print("PASS: static parameter wiring audit completed. No pixel-render, Windows build, or AE runtime claim is implied.")
