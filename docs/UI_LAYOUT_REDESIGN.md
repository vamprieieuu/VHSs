> **Historical note (2026-10-20):** This document describes an earlier source revision and is superseded where it mentions the Camera Engine selector, CRT TV system, or external layer-based overlay. See `WORK_COMPLETION_REPORT.md`, `IMPLEMENTED_FEATURES.md`, and `KNOWN_LIMITATIONS.md` for the current repair status.

# VHS6RIFT — Organized Effect Controls

The native After Effects effect UI is organized as a single effect parameter stack with collapsible AE topic groups. This revision keeps parameter IDs stable and gives each section a numbered, task-oriented label so it is easier to navigate in the Effect Controls panel.

## Top-level controls
- `CAMERA ENGINE | Profile`: 21 selectable profiles.
- `MASTER | VHS Strength`: global processing amount; 0 is an exact source bypass and 350 is the default.
- `MASTER | Continuous Signal Treatment`: continuous analog signal stages.
- `MASTER | Event Glitches`: enables timed damage events.
- `MASTER | Random Seed`: deterministic variation control.

## Sections
1. `02 | CAMERA ENGINE — Reference Fine Tuning`: original seven VHS reference profiles have per-profile Character, Color Response, Signal Damage and Grain controls. The first 13 compact/still/video-inspired profiles use their own baked profile response and do not borrow the legacy reference sliders.
2. `03 | IMAGE — Color and Chroma`: saturation, black level, softness, chroma bleed and offsets.
3. `04 | IMAGE — Moving Grain`: grain amount, scale and temporal motion.
4. `05 | SIGNAL — Tracking and Damage Events`: tracking, dropout, block corruption and event timing.
5. `06 | TAPE — Tape Behavior`: tape mode, strength, size, speed and direction.
6. `07 | OVERLAYS — Footage / Layer Overlay`: an optional AE layer, compositing mode, opacity, speed, time offset and looping.
7. `08 | SIGNAL ENGINES — Alternate Character`: VHS Craft, VHS Legend-style, NTSC-RS base and signal reference controls.
8. `09 | AUDIO — Tape Audio Degradation`: audio mix, event glitches, crackle, dropouts, saturation and lo-fi quantization.
9. `10 | SYSTEM — Modules and CRT TV`: independent stage switches and CRT controls.
10. `11 | OVERLAYS — Procedural Tape Artifacts`: transparent tape and moving tape sweep controls.
11. `12 | GLITCH CAM — Event Style and Timing`: sync tear, chroma breakup, block corruption, luma dropout and mixed burst styles.

## Important scope notes
- This is a reorganization of the native AE parameter UI, not a custom dockable panel; the AE SDK effect parameter stack is retained.
- The `Overlay Layer` control composites footage supplied by the user. The reference inventory in this repository documents supplied visual references, but those external reference GIFs are not automatically embedded as selectable media assets by this change.
- The 20 profile entries now have distinct parameter sets in the render table. Their exact resemblance to the named devices still needs image renders inside After Effects; static checks do not establish visual fidelity.
- Native Windows/MSVC build and AE runtime tests are not performed in this environment. Run the repository's GitHub Actions workflow and test the generated `.aex` in AE 2023 before treating it as release-ready.
