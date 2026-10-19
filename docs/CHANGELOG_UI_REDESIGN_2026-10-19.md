# UI / Camera Selector Revision — 2026-10-19

## Changed
- Replaced the old 7-item camera dropdown with a 20-item `CAMERA ENGINE | Profile` selector.
- Added 13 model-inspired camera profile entries (Lumix TZ7/ZS3, Sony DSC-W180, Sony H7/H9, Sony Handycam, Canon PowerShot A-series, Canon AE-1 transfer, Sony NEX-5, Canon 400D/XTi, Sony a6000, Sony DSC-T70, Kodak FZ55, Olympus mju 5000, Excel 2 Balanced) and retained the seven VHS reference profiles.
- Added a 20-entry native profile table with distinct tonal, color, channel gain, gamma, pedestal, chroma, ringing and noise parameters.
- Kept the original seven VHS reference fine-tuning groups connected to the correct matching entries in the new selector.
- Renamed effect section topics with a numbered category prefix and clear role labels; parameter IDs/order and PiPL identity were preserved.
- Added a UI map and updated stale documentation that treated source presence as visual verification.

## Validation actually run
- `python tools/validate_project.py`: PASS for parameter registration, control presence, PiPL identity/outflags, and the 20-entry selector/profile table.
- `python tools/audit_parameter_wiring.py`: PASS for static wiring guards.
- `python tools/test_vhs6rift.py`: PASS for its Python-model tests (not native C++/After Effects renders).
- Native MSVC/CMake build and AE visual/render validation: NOT RUN in this environment. No `.aex` binary is included in this ZIP.

## Not falsely claimed as complete
The built-in numbered animated overlay asset library (Glitch Cam GIFs, Retro VHS GIFs, CRT GIFs, and Capture Code footage) is not yet loaded as selectable media inside the native effect. The existing layer-overlay input remains available, and procedural tape/CRT/glitch controls remain in the effect UI. Native reference-matched rendering still requires a Windows build and AE comparison renders.

## Follow-up addition — Glitch Cam style library
- Expanded `Glitch Cam Style` from 5 to 15 choices without changing parameter IDs/order.
- Added separate event-gated render branches for RGB convergence, vertical hold roll, color smear, head-switch noise, horizontal fold, solarized flash, tape stretch, ghost echo, signal crush, and chroma-bar interference.
- Added `tools/test_glitch_cam_modes.py` to guard selector count, unique labels, and render-branch presence.
- See `docs/GLITCH_CAM_15_STYLE_EXPANSION.md` for validation and limitations.
