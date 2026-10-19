# VHS6RIFT Native and After Effects Test Plan

## Before building
1. Add the licensed After Effects SDK headers and `PiPLtool.exe` under `extern/AfterEffectsSDK/`.
2. Install Rust/Cargo for the `ntscrs_c` static library.
3. Run `python tools/validate_project.py`, `python tools/audit_parameter_wiring.py`, and `python -m pytest -q`.

## Native build / render checks
1. Run the Windows/MSVC CMake build and confirm exactly one `VHS6RIFT.aex` artifact is produced.
2. Run the native render harness; confirm Master Strength 0 is an identity copy, the NTSC-RS base changes the source, Glitch Cam alters the whole image during an event, and moving grain changes across frames.
3. Record any compile or test failure; do not infer native success from static Python tests.

## AE 23.2.1 visual checks
1. Apply VHS6RIFT to a color-bar chart, portrait, and detailed street footage. Confirm the default is a recognizable VHS signal, not only a color tint.
2. Toggle NTSC-RS and adjust chroma delay/head switching/edge wave/composite noise individually.
3. Enable each alternate signal engine separately and verify that its controls produce visible, distinct effects.
4. Test all Tape Behavior models one at a time; confirm Off is neutral and each enabled model is moving/temporal.
5. Enable the source-footage overlay checkbox and cycle through the 21 procedural overlay styles; confirm no external layer selection is required.
6. Enable Glitch Cam and scrub through at least 10 seconds; check event visibility, full-frame coverage, rate, amount, palette and blend-in.
7. Compare grain at multiple sizes and softness values; confirm there are no square pixel-cell artifacts and motion follows timeline time.
8. Verify alpha edges, full-resolution performance, preview/render consistency, and saved-project parameter behavior.
9. Confirm the removed Camera Engine selector and CRT TV system no longer appear in the UI.
