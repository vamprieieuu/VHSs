# VHS6RIFT Repair Report — 2026-10-20

## Source changes made

- Removed the Camera Engine selector and its camera-specific profile banks / color response transforms.
- Re-established a fixed VHS transfer response and enabled the real `ntsc-rs` base signal path by default, with an independent NTSC-RS checkbox and controls.
- Replaced the mutually exclusive alternate-engine dropdown with independent VHS Craft, VHS Legend-style, and Signal Reference enable switches; their settings remain in their respective groups.
- Increased moving grain visibility and replaced square-cell sampling with interpolated temporal noise.
- Reworked tape behavior with irregular animated band edges, luma loss, chroma separation, vertical roll displacement, and side-tear motion; Tape Behavior remains Off by default.
- Replaced the external layer input with a yes/no source-footage overlay switch; the switch drives the built-in procedural overlay library.
- Removed the CRT TV system and all CRT controls from the source/UI.
- Adjusted the native C++ render test source to target the revised parameter inventory.
- Updated source validation, wiring audit, regression tests, README, and limitation notes.

## Checks run

- `python tools/validate_project.py` — PASS.
- `python tools/audit_parameter_wiring.py` — PASS.
- `python -m pytest -q` — PASS (13 tests).
- C++ delimiter-balance scan — PASS.

## Not verified

- No Windows/MSVC build was run.
- The Rust `ntscrs_c` static library and native C++ render harness were not executed because Cargo/Rust and the complete licensed Adobe SDK/PiPLtool were not available in this environment.
- No After Effects 23.2.1 visual/runtime test was performed. Do not treat static checks as proof that every effect is visually correct or that the plugin loads successfully.
- No `.aex` file was produced or included.
