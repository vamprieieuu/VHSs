# VHS6RIFT — Source Feature Inventory

This document describes what is present in the current native C++ source. Static checks and Python model tests are not equivalent to a Windows MSVC build or a visual test inside After Effects.

## Camera engine

- 21 selectable camera profiles: the 20 existing compact-camera / camcorder / reference profiles plus **B&W Surveillance VHS**.
- Per-profile saturation, contrast, channel response, warmth/tint, gamma, pedestal, shoulder, chroma bleed/offset, edge ringing, and signal-noise response.
- Seven legacy reference profiles retain individual Character, Color Response, Signal Damage, and Grain controls.
- Camera color response is kept independent of low master strength to avoid the grayscale-at-low-strength regression.

## VHS and signal processing

- Separate master VHS strength, color/chroma controls, luma softness, chroma displacement, tracking, dropouts, block corruption, tape types, tape movement, and optional alternate signal engines.
- NTSC-RS wrapper parameters are passed in the wrapper's documented 0–100 range. The base signal stage is bypassed when its four NTSC controls are all zero, and its result is mixed according to master strength.
- Lowpass choice: Constant K (soft / broad) or Butterworth (sharper / narrower).
- Composite signal sharpen with a wider range; selectable chroma demodulation response; vertical composite interference and small moving dot noise.
- Head-switch controls for position, height, movement, and sudden seeded bursts.
- Continuous tape dropouts and Glitch Cam events use deterministic time/seed-driven masks. Glitch Cam has its own explicit enable checkbox and 15 selectable event styles.

## Grain

- Grain is generated procedurally from coordinates, composition time, and Random Seed; it is not a static image overlay.
- Five size choices: 4mm, 8mm, 16mm, 32mm, and 64mm.
- Grain softness changes the noise field, not only its opacity. Grain structure can be luma, chroma, or full chroma; color mode can be camera/neutral, monochrome, or manual RGB.
- A zero global grain amount is a true bypass.

## CRT, OSD, and overlays

- Optional CRT processing includes scanlines, curvature / glass mask, glow, RGB convergence / phosphor triad, flicker, and edge vignette.
- Built-in OSD is rendered directly by the effect with REC/timecode, date, time, and camera-label modes, opacity, position, scale, and white/amber/green colors.
- The built-in overlay library has 21 procedural, animated styles with enable, style, opacity, speed, blend, plus Glitch Cam color palette and blend-in controls. It does not require a separate footage layer.
- Optional external footage overlays remain available for users who specifically want to supply their own footage; the built-in overlay library works independently.
- Procedural transparent tape and moving tape sweep are direct in-effect overlays.

## Audio

- The source contains an audio render selector with wet/dry, event, crackle, dropout, saturation, and lo-fi controls. It still needs validation in the target AE host.

## Verification status

- `python tools/validate_project.py`: static parameter registration, required control, profile, PiPL identity, and regression checks.
- `python tools/audit_parameter_wiring.py`: static ID wiring audit.
- `python -m pytest -q tools/test_vhs6rift.py tools/test_glitch_cam_modes.py`: Python model / source smoke tests only.
- Native C++ render tests were skipped in this environment because Cargo is unavailable. No Windows MSVC build or After Effects visual/runtime test is claimed here.
