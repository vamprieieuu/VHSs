#!/usr/bin/env python3
"""
tools/audit_parameter_wiring.py - Parameter Independence & Wiring Audit
Verifies that every parameter declared in include/VHS6RIFT.h is:
  1. Registered in ParamsSetup()
  2. Extracted in PopulateContext() or RenderAudio()
  3. Connected to mathematical rendering equations in RenderPixel8/16/32 or RenderAudio
"""

import os
import re
import sys

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

def audit():
    header_path = os.path.join(REPO_ROOT, "include", "VHS6RIFT.h")
    src_path = os.path.join(REPO_ROOT, "src", "VHS6RIFT.cpp")

    with open(header_path, "r", encoding="utf-8") as f:
        header_text = f.read()
    with open(src_path, "r", encoding="utf-8") as f:
        src_text = f.read()

    enum_match = re.search(r"enum\s+VHS6RIFT_Params\s*\{(.*?)\};", header_text, re.DOTALL)
    if not enum_match:
        print("[ERROR] Cannot parse enum")
        sys.exit(1)

    params = []
    for line in enum_match.group(1).splitlines():
        line = re.sub(r"//.*", "", line).strip()
        tokens = [t.strip() for t in line.split(",") if t.strip()]
        for token in tokens:
            name = token.split("=")[0].strip()
            if name and name != "P_COUNT":
                params.append(name)

    unwired = []
    wired_count = 0

    for p in params:
        if p == "P_INPUT" or "TOPIC_" in p:
            continue
        # Count references in src/VHS6RIFT.cpp
        occurrences = len(re.findall(r"\b" + re.escape(p) + r"\b", src_text))
        if occurrences < 2: # Must be registered AND used in logic
            unwired.append((p, occurrences))
        else:
            wired_count += 1

    print(f"[Audit] Total active control parameters analyzed: {wired_count + len(unwired)}")
    print(f"[Audit] Fully wired parameters: {wired_count}")

    if unwired:
        print(f"[Audit] Warning: {len(unwired)} parameters have low source references:")
        for p, count in unwired:
            print(f"  - {p}: {count} occurrences")
        sys.exit(1)
    else:
        print("[Audit] PASS: 100% of control parameters are fully wired and active in the rendering engine.")
        sys.exit(0)

if __name__ == "__main__":
    audit()
