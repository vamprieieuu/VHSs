#!/usr/bin/env python3
"""
validate_project.py - Comprehensive Project Validation and Functional Verification
Verifies:
  1. Static parameter enum & registration consistency.
  2. Required UI controls, selectors, and outflags in source and PiPL.
  3. Version numbers (1.2.0.r1 / 591361) and outflags (0x48000006).
  4. Native C++ pixel-processing test execution (Master=0 identity, camera profiles, CRT TV opt-in, module toggles).
  5. Embedded PiPL outflags in compiled binary artifacts (.aex) if present.
"""

from pathlib import Path
import os
import re
import subprocess
import sys
import shutil

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "src" / "VHS6RIFT.cpp"
PIPL = ROOT / "resources" / "VHS6RIFTPiPL.r"
CMAKE = ROOT / "CMakeLists.txt"


def fail(message: str) -> None:
    print(f"FAIL: {message}")
    sys.exit(1)


for path in (CPP, PIPL, CMAKE, ROOT / ".github/workflows/build.yml", ROOT / "README.md"):
    if not path.exists():
        fail(f"required file missing: {path.relative_to(ROOT)}")

source = CPP.read_text(encoding="utf-8")
pipl = PIPL.read_text(encoding="utf-8")
cmake = CMAKE.read_text(encoding="utf-8")

# 1. Parameter Registration Order Check
enum_match = re.search(r"// Stable parameter IDs.*?enum\s*\{(.*?)\n\};", source, re.S)
if not enum_match:
    fail("parameter enum not found")
enum_ids = [piece.strip().split("=")[0].strip() for piece in enum_match.group(1).split(",") if piece.strip()]
params_match = re.search(r"static PF_Err ParamsSetup\(.*?\n\}\n\nstatic inline float clamp01", source, re.S)
if not params_match:
    fail("ParamsSetup block not found")
params_block = params_match.group(0)
ops = []
for match in re.finditer(r"(PF_ADD_\w+|PF_END_TOPIC)\((.*?)\);", params_block, re.S):
    ids = re.findall(r"P_[A-Z0-9_]+", match.group(2))
    if ids:
        ops.append(ids[-1])
expected = enum_ids[1:-1]  # param 0 is the implicit input; P_COUNT is not a parameter
if ops != expected:
    for i, (a, b) in enumerate(zip(expected, ops)):
        if a != b:
            fail(f"parameter registration order mismatch at slot {i + 1}: enum={a}, setup={b}")
    fail(f"parameter count mismatch: enum={len(expected)}, setup={len(ops)}")

# 2. Required UI Controls Check
required_controls = [
    "CAMERA ENGINE | Profile", "Fxc6 Character", "Gen 7 Character", "JVC GR-C7U Character",
    "VHS-R20 Character", "Fall-78 Character", "ERC-V2S Character", "JVC GR-SXM740U Character",
    "MASTER | Continuous Signal Treatment", "MASTER | Event Glitches", "MASTER | Random Seed", "Colored Grain Amount",
    "Tape Type", "Overlay Layer (optional)", "Other VHS Engine", "VHS Craft — Signal Controls",
    "NTSC-RS — Base Signal Controls", "Signal Reference — Controls", "Audio Glitch Enable",
    "Audio Event Glitches", "Audio Wet / Dry Mix", "PF_Cmd_AUDIO_RENDER",
    "Enable Grain / Noise", "Enable Tracking Damage", "Enable Tape Effects",
    "Enable Overlay Layer", "Enable Other Signal Engines", "Enable CRT TV Engine",
    "Procedural Transparent Tape", "Moving Tape Sweep", "CRT Scanline Strength", "Built-in OSD Style", "OSD Opacity", "OSD Scale"
]
for item in required_controls:
    if item not in source:
        fail(f"expected control/selector not found: {item}")

# All non-topic controls must be referenced outside enum/setup
for param_id in enum_ids:
    if param_id == "P_INPUT" or param_id == "P_COUNT" or "TOPIC" in param_id or param_id.endswith("_END"):
        continue
    if len(re.findall(r"\b" + re.escape(param_id) + r"\b", source)) < 3:
        fail(f"parameter appears unused or unregistered: {param_id}")

# 3. PiPL Identity & Outflags Check
if 'AE_Effect_Match_Name { "com.vhs6rift.effect" }' not in pipl:
    fail("PiPL match name changed")
if 'Name { "VHS6RIFT" }' not in pipl:
    fail("PiPL plugin name changed")
if "PF_OutFlag_AUDIO_EFFECT_TOO | PF_OutFlag_AUDIO_FLOAT_ONLY" not in source:
    fail("audio effect flags missing from GlobalSetup")
if "AE_Effect_Global_OutFlags { 0x48000006 }" not in pipl:
    fail("PiPL global flags do not match the expected 0x48000006 flags")
if "PF_VERSION(1, 2, 0, PF_Stage_RELEASE, 1)" not in source:
    fail("compiled effect version is not the agreed 1.2 version")
if "AE_Effect_Version { 591361 }" not in pipl:
    fail("PiPL version is not the agreed 1.2 version")
if '#include "entry.h"' in source:
    fail("missing entry.h dependency remains; this SDK snapshot does not contain entry.h")
if 'PF_ADD_FLOAT_SLIDERX("MASTER | VHS Strength", 0, 2000, 0, 2000, 350' not in source:
    fail("Master VHS Strength range/default is not configured as expected")
if 'PF_ADD_CHECKBOX("Enable CRT TV Engine", "Enable", FALSE' not in source:
    fail("CRT engine must be optional and disabled by default")
if 'SUFFIX ".aex"' not in cmake or 'OUTPUT_NAME "VHS6RIFT"' not in cmake:
    fail("CMake target is not configured to emit VHS6RIFT.aex")

print(f"PASS: {len(expected)} registered parameters are ordered consistently.")
print("PASS: camera, signal, tape, overlay, Other VHS and audio controls are present.")
if "kCameraProfiles[20]" not in source or "PF_ADD_POPUP(\"CAMERA ENGINE | Profile\", 20, 1" not in source:
    fail("the 20-profile camera engine selector/table is missing")
print("PASS: all 20 camera profiles are registered in the selector and render table.")
print("PASS: PiPL identity, version (591361) and outflags (0x48000006) match the source.")

# Regression guard: Glitch Cam's effective rate must drive both slot timing and gating.
if "const bool chosenEvent = s.events && effectiveEventRate > 0.0f" not in source:
    fail("Glitch Cam event gate is not using effectiveEventRate; its rate override can be ignored.")
print("PASS: Glitch Cam event gating uses the effective event rate override.")
if "assets/overlay-library/manifest.json" not in str([p.relative_to(ROOT).as_posix() for p in (ROOT / "assets" / "overlay-library").rglob("manifest.json")]):
    fail("bundled overlay library manifest is missing")
print("PASS: bundled overlay reference library manifest is present.")
if "if (osdStyle > 0 && osdOpacity > 0.0f)" not in source or "osdGlyphRow(osdText[ci], row)" not in source:
    fail("built-in OSD render path is missing")
print("PASS: built-in OSD has a registered, wired render path.")

# 4. Native C++ Image Processing Test Suite Execution
# This harness is a GCC/Linux-oriented test and includes the Adobe SDK headers.
# On Windows runners, compiling it with g++ against the Windows Adobe SDK is
# unsupported and can fail on SDK configuration headers (e.g. PreConfig.h).
# The authoritative Windows check is the MSVC/CMake plugin build in build.yml.
test_src = ROOT / "tools" / "test_native_render.cpp"
test_bin = ROOT / "test_native_render"
ntsc_lib = ROOT / "extern" / "ntscrs_c" / "target" / "release" / "libntscrs_c.a"
if test_src.exists() and os.name == "nt":
    print("SKIP: GCC native-render harness is not portable to the Windows Adobe SDK; the CI workflow validates the native plugin with MSVC/CMake.")
elif test_src.exists():
    cargo_path = shutil.which("cargo")
    if not ntsc_lib.exists() and not cargo_path:
        print("SKIP: Native C++ render suite requires Cargo to build ntscrs_c; Cargo is unavailable in this environment.")
    else:
        if not ntsc_lib.exists():
            res_cargo = subprocess.run([cargo_path, "build", "--release", "--manifest-path", str(ROOT / "extern" / "ntscrs_c" / "Cargo.toml")], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            if res_cargo.returncode != 0:
                fail(f"Failed to build ntscrs_c library:\n{res_cargo.stderr}")
        cmd_build = [
            "g++", "-O2",
            "-I", str(ROOT / "tools" / "mock_win"),
            "-I", str(ROOT / "include"),
            "-I", str(ROOT / "extern" / "ntscrs_c" / "include"),
            "-I", str(ROOT / "extern" / "AfterEffectsSDK" / "Examples" / "Headers"),
            "-I", str(ROOT / "extern" / "AfterEffectsSDK" / "Examples" / "Headers" / "SP"),
            str(test_src),
            str(ROOT / "extern" / "ntscrs_c" / "target" / "release" / "libntscrs_c.a"),
            "-ldl", "-lpthread", "-lm",
            "-o", str(test_bin)
        ]
        res_compile = subprocess.run(cmd_build, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if res_compile.returncode != 0:
            fail(f"Failed to compile native C++ render test harness:\n{res_compile.stderr}")

        res_run = subprocess.run([str(test_bin)], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        print(res_run.stdout)
        if res_run.returncode != 0 or "11 / 11 native C++ tests PASSED" not in res_run.stdout:
            fail(f"Native C++ pixel processing test suite failed:\n{res_run.stderr}")
        print("PASS: Native C++ pixel processing test suite passed 11/11 test cases.")

# 5. Compiled Binary Artifact PiPL Verification (.aex)
aex_files = list(ROOT.glob("**/*.aex"))
if aex_files:
    for aex_path in aex_files:
        content = aex_path.read_bytes()
        # Search for PiPL 'eGLO' tag followed by outflags 0x48000006
        eglo_idx = content.find(b"eGLO")
        if eglo_idx != -1:
            flags_bytes = content[eglo_idx + 8 : eglo_idx + 12]
            flags_val = int.from_bytes(flags_bytes, byteorder="big")
            if flags_val != 0x48000006:
                fail(f"Embedded PiPL outflags in {aex_path.name} is {hex(flags_val)}, expected 0x48000006")
            print(f"PASS: Verified embedded PiPL outflags (0x48000006) in compiled binary artifact: {aex_path.relative_to(ROOT)}")
else:
    print("NOTE: No compiled .aex binary artifact found locally yet. Verification will run on build artifacts.")

print("\nSTATIC PROJECT VALIDATION PASSED. Runtime/render checks are only passed when their test suite actually runs.")
