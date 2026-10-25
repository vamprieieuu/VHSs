#!/usr/bin/env python3
"""
tools/test_sdk_discovery.py - Validates Adobe After Effects SDK Discovery & Layout Logic
Simulates various Adobe SDK folder layouts in temporary directories and tests:
  1. Standard flat layout (<Root>/Headers/AE_Effect.h, <Root>/Resources/PiPLtool.exe)
  2. Examples layout (<Root>/Examples/Headers/AE_Effect.h, <Root>/Examples/Resources/PiPLtool.exe)
  3. Nested zip extraction layout (<Root>/Adobe After Effects SDK/Headers/AE_Effect.h)
  4. Incomplete SDK detection (missing required header triggers clean failure)
  5. Missing PiPLtool.exe detection
  6. Completely missing SDK triggers clean failure with diagnostic error
"""

import os
import shutil
import tempfile
import sys

REQUIRED_HEADERS = [
    "AE_Effect.h",
    "AE_EffectCB.h",
    "AE_EffectCBSuites.h",
    "AE_EffectUI.h",
    "AE_Macros.h",
    "Param_Utils.h"
]

def simulate_find_sdk(candidate_path):
    if not candidate_path or not os.path.exists(candidate_path):
        return False, "Candidate path does not exist", []

    # Search for AE_Effect.h
    found_header = None
    direct = os.path.join(candidate_path, "Headers", "AE_Effect.h")
    examples = os.path.join(candidate_path, "Examples", "Headers", "AE_Effect.h")

    if os.path.isfile(direct):
        found_header = direct
    elif os.path.isfile(examples):
        found_header = examples
    else:
        for root, dirs, files in os.walk(candidate_path):
            if "AE_Effect.h" in files:
                found_header = os.path.join(root, "AE_Effect.h")
                break

    if not found_header:
        return False, "AE_Effect.h not found in search tree", []

    headers_dir = os.path.dirname(found_header)
    include_root = os.path.dirname(headers_dir)

    # Check required headers
    missing_headers = [h for h in REQUIRED_HEADERS if not os.path.isfile(os.path.join(headers_dir, h))]

    # Check PiPLtool.exe
    pipl_tool = None
    pipl1 = os.path.join(candidate_path, "Resources", "PiPLtool.exe")
    pipl2 = os.path.join(include_root, "Resources", "PiPLtool.exe")
    if os.path.isfile(pipl1):
        pipl_tool = pipl1
    elif os.path.isfile(pipl2):
        pipl_tool = pipl2
    else:
        for root, dirs, files in os.walk(candidate_path):
            if "PiPLtool.exe" in files:
                pipl_tool = os.path.join(root, "PiPLtool.exe")
                break

    if missing_headers:
        return False, f"Missing required headers: {missing_headers}", missing_headers

    if not pipl_tool:
        return False, "Missing PiPLtool.exe in SDK", []

    return True, "Valid complete SDK", []

def run_tests():
    temp_dir = tempfile.mkdtemp(prefix="ae_sdk_test_")
    try:
        # Test 1: Standard layout
        sdk1 = os.path.join(temp_dir, "sdk_standard")
        os.makedirs(os.path.join(sdk1, "Headers"), exist_ok=True)
        os.makedirs(os.path.join(sdk1, "Resources"), exist_ok=True)
        for h in REQUIRED_HEADERS:
            with open(os.path.join(sdk1, "Headers", h), "w") as f: f.write("// dummy\n")
        with open(os.path.join(sdk1, "Resources", "PiPLtool.exe"), "w") as f: f.write("dummy\n")

        valid1, msg1, _ = simulate_find_sdk(sdk1)
        assert valid1, f"Test 1 failed: {msg1}"
        print("[TEST PASS] Layout 1: Standard flat layout successfully verified.")

        # Test 2: Examples layout
        sdk2 = os.path.join(temp_dir, "sdk_examples")
        os.makedirs(os.path.join(sdk2, "Examples", "Headers"), exist_ok=True)
        os.makedirs(os.path.join(sdk2, "Examples", "Resources"), exist_ok=True)
        for h in REQUIRED_HEADERS:
            with open(os.path.join(sdk2, "Examples", "Headers", h), "w") as f: f.write("// dummy\n")
        with open(os.path.join(sdk2, "Examples", "Resources", "PiPLtool.exe"), "w") as f: f.write("dummy\n")

        valid2, msg2, _ = simulate_find_sdk(sdk2)
        assert valid2, f"Test 2 failed: {msg2}"
        print("[TEST PASS] Layout 2: SDK Examples layout successfully verified.")

        # Test 3: Nested zip extraction layout
        sdk3 = os.path.join(temp_dir, "sdk_nested")
        nested_sub = os.path.join(sdk3, "Adobe After Effects 2023 SDK")
        os.makedirs(os.path.join(nested_sub, "Headers"), exist_ok=True)
        os.makedirs(os.path.join(nested_sub, "Resources"), exist_ok=True)
        for h in REQUIRED_HEADERS:
            with open(os.path.join(nested_sub, "Headers", h), "w") as f: f.write("// dummy\n")
        with open(os.path.join(nested_sub, "Resources", "PiPLtool.exe"), "w") as f: f.write("dummy\n")

        valid3, msg3, _ = simulate_find_sdk(sdk3)
        assert valid3, f"Test 3 failed: {msg3}"
        print("[TEST PASS] Layout 3: Nested archive extraction successfully verified.")

        # Test 4: Incomplete SDK (missing Param_Utils.h)
        sdk4 = os.path.join(temp_dir, "sdk_incomplete")
        os.makedirs(os.path.join(sdk4, "Headers"), exist_ok=True)
        os.makedirs(os.path.join(sdk4, "Resources"), exist_ok=True)
        for h in REQUIRED_HEADERS[:-1]: # exclude Param_Utils.h
            with open(os.path.join(sdk4, "Headers", h), "w") as f: f.write("// dummy\n")
        with open(os.path.join(sdk4, "Resources", "PiPLtool.exe"), "w") as f: f.write("dummy\n")

        valid4, msg4, missing4 = simulate_find_sdk(sdk4)
        assert not valid4 and "Param_Utils.h" in missing4
        print(f"[TEST PASS] Incomplete SDK cleanly rejected: {msg4}")

        # Test 5: Missing PiPLtool.exe
        sdk5 = os.path.join(temp_dir, "sdk_no_pipl")
        os.makedirs(os.path.join(sdk5, "Headers"), exist_ok=True)
        for h in REQUIRED_HEADERS:
            with open(os.path.join(sdk5, "Headers", h), "w") as f: f.write("// dummy\n")

        valid5, msg5, _ = simulate_find_sdk(sdk5)
        assert not valid5 and "PiPLtool.exe" in msg5
        print(f"[TEST PASS] Missing PiPLtool cleanly rejected: {msg5}")

        # Test 6: Missing directory
        valid6, msg6, _ = simulate_find_sdk(os.path.join(temp_dir, "nonexistent"))
        assert not valid6
        print(f"[TEST PASS] Non-existent path cleanly rejected: {msg6}")

        print("\n[ALL SDK DISCOVERY TESTS PASSED (6/6)]")
        return 0
    finally:
        shutil.rmtree(temp_dir, ignore_errors=True)

if __name__ == "__main__":
    sys.exit(run_tests())
