> **Superseded status note:** This historical note predates the 2026-10-19 repair pass. Current parameter count and verification status are in `WORK_COMPLETION_REPORT.md` and `FEATURE_VALIDATION.md`.

# NTSC-RS Base Pipeline Fix

The native `ntsc-rs` C-FFI engine is now called before the VHS6RIFT camera/tape/signal/CRT/overlay render stages. Its output is stored in a separate output-sized pixel buffer and becomes the source for later VHS6RIFT processing. This avoids an in-place read/write hazard and ensures that the NTSC engine is the foundation rather than an optional post-pass under `Other VHS Engines`.

The NTSC base pass uses the four NTSC controls and a normalized base strength of `1.0`. The global Master Strength zero value remains an intentional exact bypass. The `Other VHS Engines` popup label now describes NTSC-RS as the always-active base; selecting it does not run a second copy of the same engine.

Validation performed for this patch:
- `tools/validate_project.py`: passed (192 registered parameters, PiPL identity/outflags, and feature-presence checks).
- Native MSVC/CMake build and rendered-image comparison: not run in this environment. GitHub Actions must confirm compilation and runtime behavior.
