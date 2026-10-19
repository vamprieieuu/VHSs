# VHS6RIFT — Repair Pass Report (2026-10-19)

## Changes made in this repair pass

- Added the 21st camera profile, **B&W Surveillance VHS**, without applying monochrome to other camera profiles.
- Added granular grain controls: 4mm–64mm size choices, real softness blending, luma/chroma/full-chroma structure, and camera/monochrome/manual RGB color modes.
- Fixed percentage normalization paths that made Glitch Cam, tape strength/size/speed, footage overlay opacity/speed, procedural tape, and several advanced controls much weaker than their UI values implied.
- Corrected NTSC-RS parameter units to match the C wrapper's 0–100 contract, and bypassed the base NTSC stage when its controls are all zero.
- Added lowpass filter type, wider-range composite sharpening, chroma demodulation choice, vertical composite noise, dot noise, and adjustable head-switch position/height/motion/random bursts.
- Added built-in OSD modes and a 21-style procedural overlay library with opacity, speed, blend, and an enable toggle; neither requires a separate footage layer.
- Added a dedicated Glitch Cam enable toggle and changed the old video event toggle label to identify it as legacy/unused without renumbering existing parameters.
- Updated source validators and the static parameter-wiring audit for 192 registered parameters.
- Kept the existing PiPL identity/version/outflags and CMake build configuration unchanged.

## Verification

- `python tools/validate_project.py`: PASS — 192 registered parameters and source-level regressions checked.
- `python tools/audit_parameter_wiring.py`: PASS — all non-structural parameter IDs are referenced.
- `python -m pytest -q tools/test_vhs6rift.py tools/test_glitch_cam_modes.py`: PASS — 4 Python tests.
- Native C++ render suite: SKIPPED because Cargo is not installed in this environment.
- Windows MSVC build and Adobe After Effects visual/runtime checks: NOT RUN here. This report does not claim a successful `.aex` build or verified in-host rendering.
