# VHS6RIFT Implementation Map — Current Repair Pass

| Stage | Current implementation | Status boundary |
|---|---|---|
| Core VHS transfer | Fixed VHS signal response plus real `ntsc-rs` Rust C-FFI processing | Source-wired; native build and AE visuals not run here |
| Signal engines | Independent VHS Craft, VHS Legend-style, NTSC-RS, and Signal Reference switches and settings | Separate C++/Rust paths; perceptual matching not validated |
| Moving grain | Time/seed-driven interpolated noise, size/softness/structure/color controls | Static source checks pass; compiled render test pending |
| Tape behavior | Horizontal band, head-switch, vertical roll, side tear, signal snow; animated edge irregularity and chroma/luma damage | Source-wired; AE visual check pending |
| Overlay | One yes/no switch gates 21 procedural overlays on the source footage; no layer selector | Procedural graphics, not source GIF playback |
| Glitch Cam | Separate opt-in temporal event stage and 15 styles | Source-wired; runtime visual check pending |
| CRT TV system | Removed from source/UI per request | Not present |
| OSD | Native bitmap text and timeline-driven timecode/date/time/camera label | Source-wired; host render pending |
| Audio | AE audio render selector and signal controls | Host validation pending |
| Parameters | 135 registered parameters, checked against the enum order | Static validator passes |

## Validation

- `python tools/validate_project.py` — PASS (static source and parameter checks).
- `python tools/audit_parameter_wiring.py` — PASS (static wiring audit).
- `python -m pytest -q` — PASS (13 Python tests).
- Native C++/Rust build and After Effects visual test — NOT RUN in this environment.
