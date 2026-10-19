# Test plan — current source revision

1. Run `python tools/validate_project.py`, `python tools/audit_parameter_wiring.py`, and `python -m pytest -q`.
2. Run the GitHub Actions Windows x64 / MSVC Release build with Rust/Cargo available and confirm that `VHS6RIFT.aex` is emitted.
3. In After Effects 2023, confirm Master Strength 0 is an identity copy and that NTSC-RS changes the source at normal strength.
4. Confirm grain amount defaults to 0 and grain is disabled on load. Enable it, compare 8mm and 16mm presets, and move amount, size, and speed independently at preview and full resolution.
5. Set Saturation to 0. Confirm the image is monochrome. Enable Colored Signal Lines and verify only the independent line artifacts retain color; adjust their amount, size, and speed separately.
6. Test all seven camera profiles against the supplied reference-analysis contact sheets and clips, especially the Gen 7 breakdown, Fall-78 warm/low-detail look, and JVC GR-C7U chroma/softness behavior.
7. Enable Signal Reference and Glitch Cam separately. Preview across several seconds to see the time-based event windows; test each Glitch Cam style and vary amount/rate.
8. Confirm the removed Tape Behavior, tape sweep, source-footage overlay toggle, and VHS Craft controls do not appear. Verify the advanced built-in overlay and OSD each remain off until explicitly enabled.
9. Check transparency, multiple frame sizes, and playback scrubbing. Do not mark visual checks as passed until performed in AE.
