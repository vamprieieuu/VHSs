> **Superseded status note:** This historical note predates the 2026-10-19 repair pass. Current parameter count and verification status are in `WORK_COMPLETION_REPORT.md` and `FEATURE_VALIDATION.md`.

# VHS6RIFT regression repair — 2026-10-19

## Changes made to the latest supplied repository snapshot

- Decoupled camera color/gamma identity from the global VHS drive. Previously `charScale` and `colorScale` were multiplied by the master drive, so a low VHS Strength (e.g. UI 30) pushed saturation and channel response toward neutral and made the selected camera look nearly monochrome. Camera character/color now retain their selected profile at low strengths; signal damage remains driven by master strength.
- Removed the `MASTER` prefix from the visible VHS Strength, Continuous Signal Treatment, and Random Seed labels.
- Changed the legacy master Event Glitches control label to explicitly mark it disabled/replaced. The dedicated Glitch Cam event scheduler no longer checks that legacy toggle, so Glitch Cam can trigger independently.
- Kept Block Corruption defaulted to zero, preventing block-shaped corruption from appearing on a fresh effect unless the user explicitly raises that control.
- Renamed the footage layer field to make clear that it is only needed for footage overlays, not procedural/built-in effects.
- Updated static validation checks for the revised labels and regression guards.

## Verification

- `python tools/validate_project.py`: PASS for 187 parameter ordering, required controls, 21 camera profiles, PiPL identity, and source regression guards. Native render suite skipped because Cargo is unavailable in this environment.
- `python -m pytest -q tools/test_vhs6rift.py tools/test_glitch_cam_modes.py`: 4 passed.
- Windows/MSVC build and After Effects render were not run here. The existing CMake, PiPL, SDK, Rust bridge, and parameter IDs were otherwise left unchanged.

## Important scope note

This pass addresses the diagnosed low-strength monochrome regression and removes the default source of block-corruption squares. It does not claim that every requested signal, CRT, overlay, grain, or camera-engine control has been visually verified in After Effects; those require runtime frame comparisons.
