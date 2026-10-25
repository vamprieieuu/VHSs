#!/usr/bin/env python3
"""Source-level contract checks for effect paths that model tests cannot prove."""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "src" / "VHS6RIFT.cpp").read_text(encoding="utf-8")
bridge = (ROOT / "extern" / "ntscrs_c" / "src" / "lib.rs").read_text(encoding="utf-8")
header = (ROOT / "extern" / "ntscrs_c" / "include" / "ntscrs_c.h").read_text(encoding="utf-8")
failures = []

# Each numbered Glitch Cam preset must have its own processing branch.
branches = {int(n) for n in re.findall(r"case\s+(\d+)\s*:\s*\{\s*//", source[source.find("// Fifteen separately tuned"):source.find("// Procedural camcorder OSD")])}
if branches != set(range(1, 16)):
    failures.append(f"Glitch Cam branches must cover 1..15 independently; found {sorted(branches)}")

# All 21 native overlay effects need dedicated switch branches and registered UI choices.
overlay_start = source.find("// Built-in VHS overlay effects.")
overlay_end = source.find("// Damage Overlay Layer", overlay_start)
overlay_block = source[overlay_start:overlay_end] if overlay_start >= 0 and overlay_end > overlay_start else ""
overlay_branches = {int(n) for n in re.findall(r"case\s+(\d+)\s*:\s*\{\s*//", overlay_block)}
if overlay_branches != set(range(1, 22)):
    failures.append(f"native overlay branches must cover 1..21 independently; found {sorted(overlay_branches)}")
if 'PF_ADD_POPUP("Native Overlay Preset", 22, 1,' not in source:
    failures.append("native overlay selector must expose Off plus 21 effects")

checks = {
    "native OSD renders date even when PLAY/REC/timecode are off": "if (osdCharCount > 0 && osdOpacity > 0.001f",
    "audio lo-fi quantization preserves signed waveform": "wet = std::round(normalized * levels) / levels;",
    "master drive reaches full level at 1000 UI units": "const float drive = clamp01(strength / 10.0f)",
    "secondary engine obeys explicit strength instead of forced fallback": "otherStrengthRaw * drive",
    "grain module is opt-in by default": 'PF_ADD_CHECKBOX("Enable Grain / Noise", "Enable", FALSE',
    "real ntsc-rs C ABI is called from the effect": "ntscrs_process_argb8(&np",
    "NTSC UI percentage controls are scaled to bridge percentage units": "np.chroma_delay = ntscChroma * 100.0f;",
    "native overlay blend-in affects continuous and event looks": "builtinOverlayIsEvent ? (builtinOverlayEvent ? builtinOverlayFade : 0.0f) : builtinOverlayFade",
    "NTSC-RS implementation calls upstream effect engine": "effect.apply_effect_to_yiq",
    "NTSC-RS preserves alpha channel": "original_alphas[y * w + x]",
    "CRT curvature masks pixels outside the warped aperture": "crtFrameMask = smooth(-0.002f, 0.018f + crtCurveAmount * 0.006f, aperture);",
    "CRT glow samples adjacent phosphor energy": "const float bloomEnergy = std::max(0.0f, std::max(l, neighborLuma * 0.78f) - 0.58f);",
    "CRT phosphor triad is implemented": "const int triad = x % 3;",
}
for description, token in checks.items():
    haystack = bridge if description.startswith("NTSC-RS implementation") or description.startswith("NTSC-RS preserves") else source
    if token not in haystack:
        failures.append(description)

if "PF_Cmd_AUDIO_RENDER" not in source or "AudioRender(in_data, out_data, params)" not in source:
    failures.append("native audio render callback is not routed from EffectMain")
test_harness = (ROOT / "tools" / "test_native_render.cpp").read_text(encoding="utf-8")
if "Test 15: Native audio callback preserves exact dry audio" not in test_harness:
    failures.append("native C++ harness must exercise audio pass-through and sample-domain DSP")
if "P_OVERLAY_LAYER" not in source or "PF_CHECKOUT_PARAM(in_data, P_OVERLAY_LAYER" not in source:
    failures.append("native layer overlay checkout is missing")

# Ensure the included media set corresponds to all fifteen presets and the five supplied tape models.
for i in range(1, 16):
    found = list((ROOT / "assets" / "overlays" / "glitch-cam").glob(f"glitch-cam-{i:02d}.*"))
    if not found:
        failures.append(f"missing user-provided Glitch Cam reference asset {i:02d}")
for i in range(1, 6):
    found = list((ROOT / "assets" / "overlays" / "tape-models").glob(f"tape-signal-model-{i:02d}.*"))
    if not found:
        failures.append(f"missing user-provided tape/signal reference asset {i:02d}")

if failures:
    print("SOURCE CONTRACT FAILED:")
    for failure in failures:
        print(f" - {failure}")
    sys.exit(1)

print("PASS: all 15 distinct procedural Glitch Cam branches and all 21 native overlay branches are present.")
print("PASS: OSD/date, signed audio DSP, full-range master drive, NTSC-RS bridge, layer overlays, native audio tests, and CRT aperture/bloom/phosphor stages are wired in source.")
print("PASS: all 15 Glitch Cam and 5 tape-model reference assets are included.")
print("SOURCE CONTRACT PASSED; this is source inspection, not an AE runtime test.")
