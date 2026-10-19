# VHS6RIFT — Analog Video Suite

VHS6RIFT is a native After Effects effect for VHS/composite-signal degradation. The source uses the included `ntsc-rs` Rust C-FFI stage as the base signal path and additional C++ processing for chroma, camera-transfer response, grain, optional colored signal lines, Glitch Cam events, signal engines, OSD, and audio degradation.

## Changes in this source revision

- Grain amount defaults to `0` and directly drives the grain stage when raised; size, motion speed, softness, and 4mm/8mm/16mm/32mm/64mm presets are available. No separate hidden grain-enable checkbox is required.
- Added independent optional colored signal-line controls for amount, size, and speed. They are composited after image desaturation, so the picture can be black-and-white while colored signal lines remain colored.
- Removed the Tape Behavior model, transparent-tape sweep controls, the source-footage overlay toggle, and VHS Craft.
- Kept the advanced built-in signal overlay library behind its own explicit enable checkbox.
- Replaced the earlier visual-reference selector with five real JVC VHS-C/S-VHS-C camera engines, each changing luminance bandwidth, line stability, chroma behavior, edge response, and noise characteristics.
- Removed the Signal Reference engine, standalone Tape Dropouts and Block Corruption controls. The Glitch Cam's former block-artifact style is now a coherent Horizontal Slip effect.
- Preserved the NTSC-RS engine and its native tracking-noise implementation.

## Build requirements

- Windows x64 and Visual Studio 2022/MSVC
- CMake
- Rust/Cargo for `extern/ntscrs_c`
- The full Adobe After Effects SDK for the target AE version

The included static tests validate source structure and parameter wiring. They do **not** establish that the Windows plugin build succeeds or that the effect has been visually verified in After Effects. Run the repository's GitHub Actions workflow to confirm the MSVC build, then test the resulting `.aex` in After Effects 2023.

## Current camera engines

The camera selector now uses five real JVC analog models: GR-C7U (1986 VHS-C), GR-AX220U (Compact VHS), GR-AX920U (Compact VHS), GR-SXM37US (S-VHS-C), and GR-SXM740U (S-VHS-C). The per-camera response varies luma softness, line timing stability, chroma bleed/delay, edge response, noise floor, and tracking behavior—not only RGB color. See `docs/CAMERA_ENGINE_RESEARCH_2026-10-20.md` for documented distinctions and references. These are approximate emulations, not measured pixel-exact reproductions.

`Enable Main VHS Engine` is on by default. `Enable Tracking Damage` is located in the Tracking and Damage section and is processed as part of the main VHS signal path. The standalone Tape Dropouts and Block Corruption controls, and the Signal Reference engine, have been removed. Grain amount directly controls grain (0 means off); colored signal lines have their own enable, amount, size, and speed controls.
