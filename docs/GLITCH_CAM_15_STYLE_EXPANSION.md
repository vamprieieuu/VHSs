# Glitch Cam — 15 procedural styles (2026-10-19)

## Added in this revision
The existing five styles remain in place, preserving their popup order and parameter IDs. Ten additional styles now have separate mode-gated pixel operations in `src/VHS6RIFT.cpp`:

6. **RGB Convergence Burst** — samples red/blue channels from horizontally offset source pixels.
7. **Vertical Hold Roll** — blends a displaced source row during an event.
8. **Color Smear** — shifts source chroma laterally.
9. **Head-Switch Noise** — creates intermittent noisy breakup near the bottom of the raster.
10. **Horizontal Fold** — blends a laterally displaced source sample into a short event band.
11. **Solarized Flash** — briefly blends toward inverted luma.
12. **Tape Stretch** — produces a longer lateral smear.
13. **Ghost Echo** — mixes a neighboring source sample for a short ghost image.
14. **Signal Crush** — quantizes RGB values to fewer levels during events.
15. **Chroma Bar Interference** — adds a time-varying colored band/interference pattern.

All new styles are gated by the existing Glitch Cam event envelope and amount. The math uses composition time, coordinates, and the user seed so the result is deterministic for a given frame and settings. The existing five style names and their branches were preserved.

## Validation
- `python tools/test_glitch_cam_modes.py`: PASS (15 unique popup labels; branches for mode indices 5–14 present).
- `python tools/test_vhs6rift.py`: PASS (four Python-model tests only).
- `python tools/validate_project.py`: static project validation PASS; native C++ suite skipped because Cargo is unavailable in this environment.
- Windows MSVC/CMake build: NOT RUN here.
- After Effects 23.2.1 render comparison: NOT RUN here.

These checks verify the selector and source structure, not the final visual quality of each mode. The modes are procedural recreations, not bundled reference GIFs, and do not claim exact matches to the supplied footage.
