# Work completion report — requested UI and engine revision

Implemented in source:
- Removed the separate Moving Grain UI and replaced it with unified Grain and NTSC-RS Composite Signal Noise controls. Grain is off and amount is zero by default; amount, size, speed, softness, and 4mm/8mm/16mm/32mm/64mm presets are available.
- Removed the synthetic red-line overlay. Signal Noise now calls NTSC-RS native composite-noise processing with intensity, frequency, and detail controls.
- Removed Tape Behavior, transparent tape/tape sweep, source-footage overlay toggle, and VHS Craft from the parameter UI and render path.
- Added an independent checkbox for the advanced built-in overlay library.
- Restored seven reference-derived camera profiles using the existing camera profile matrix and supplied before/after reference contact sheets.

Checks actually run:
- `tools/validate_project.py`: PASS, 112 registered parameter slots in matching order.
- `tools/audit_parameter_wiring.py`: PASS.
- `python -m pytest -q`: PASS, 14 tests.
- C++ delimiter scan: PASS.

Not run: Windows/MSVC/CMake build, Rust/Cargo build, native pixel harness, or visual render inside After Effects. The source ZIP must be built and visually checked before these changes can be called runtime-verified.
