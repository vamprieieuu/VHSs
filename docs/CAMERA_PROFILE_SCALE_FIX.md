> **Historical note (2026-10-20):** This document describes an earlier source revision and is superseded where it mentions the Camera Engine selector, CRT TV system, or external layer-based overlay. See `WORK_COMPLETION_REPORT.md`, `IMPLEMENTED_FEATURES.md`, and `KNOWN_LIMITATIONS.md` for the current repair status.

# Camera profile control scaling fix

## Root cause
The seven camera profile controls are UI sliders in the 0–100 range. The render path previously normalized their values using `0.65`, `0.55`, and `0.35`, although the profile defaults are `65`, `55`, and `35`. At the default values, character/color/signal scaling was therefore inflated by 100x before the master-drive multiplier.

## Fix
Normalize against `65.0`, `55.0`, and `35.0`, respectively. At the intended defaults and default Master VHS Strength (3.5), each scale is 1.0. Users can still increase/decrease each per-camera control relative to its designed default. The master drive still gates the camera transform and reaches zero at Master Strength = 0, preserving the existing bypass.

## Scope and validation
- Changed `src/VHS6RIFT.cpp` only for this scaling correction, plus this explanatory note.
- This is a source-level correction; no claim is made that the Windows/MSVC build or After Effects runtime has been tested in this environment.
