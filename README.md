# VHS6RIFT — Analog Video Suite

VHS6RIFT is a native After Effects effect for VHS/composite-signal degradation. The source uses the included `ntsc-rs` Rust C-FFI stage as the base signal path and additional C++ processing for chroma, camera-transfer response, grain, native NTSC-RS composite signal noise, native NTSC-RS tracking-noise processing, signal engines, OSD, and audio degradation.

## Changes in this source revision

- Grain amount defaults to `0` and directly drives the grain stage when raised; size, motion speed, softness, and 4mm/8mm/16mm/32mm/64mm presets are available. No separate hidden grain-enable checkbox is required.
- Added independent optional native NTSC-RS composite noise controls for intensity, frequency, and detail. The noise is injected into the composite signal and decoded by NTSC-RS; it is not a red-line overlay.
- Removed the Tape Behavior model, transparent-tape sweep controls, the source-footage overlay toggle, and VHS Craft.
- Kept the advanced built-in signal overlay library behind its own explicit enable checkbox.
- Replaced the earlier visual-reference selector with five real JVC VHS-C/S-VHS-C camera engines, each changing luminance bandwidth, line stability, chroma behavior, edge response, and noise characteristics.
- Removed the Signal Reference engine, standalone Tape Dropouts, Block Corruption, and the entire Glitch Cam module. Tracking damage is part of Main VHS.
- Preserved the NTSC-RS engine and its native tracking-noise implementation.

## Build requirements

- Windows x64 and Visual Studio 2022/MSVC
- CMake
- Rust/Cargo for `extern/ntscrs_c`
- The full Adobe After Effects SDK for the target AE version

The included static tests validate source structure and parameter wiring. They do **not** establish that the Windows plugin build succeeds or that the effect has been visually verified in After Effects. Run the repository's GitHub Actions workflow to confirm the MSVC build, then test the resulting `.aex` in After Effects 2023.

## Current camera engines

The camera selector now uses five real JVC analog models: GR-C7U (1986 VHS-C), GR-AX220U (Compact VHS), GR-AX920U (Compact VHS), GR-SXM37US (S-VHS-C), and GR-SXM740U (S-VHS-C). The per-camera response varies luma softness, line timing stability, chroma bleed/delay, edge response, noise floor, and tracking behavior—not only RGB color. See `docs/CAMERA_ENGINE_RESEARCH_2026-10-20.md` for documented distinctions and references. These are approximate emulations, not measured pixel-exact reproductions.

`Enable Main VHS Engine` is on by default. `Enable Tracking Damage` is located in the Tracking and Damage section and is processed as part of the main VHS signal path. The standalone Tape Dropouts and Block Corruption controls, and the Signal Reference engine, have been removed. Grain amount directly controls grain (0 means off); NTSC-RS composite signal noise has its own enable, intensity, frequency, and detail controls.


### Defaults after the signal-artifact rollback

The Main VHS engine remains enabled, but optional damage is not forced onto a newly applied effect: Tracking Damage is off with Tracking Noise at 0; NTSC-RS Composite Signal Noise is off with Amount at 0; NTSC Edge Wave, NTSC Head Switching, and NTSC-RS Composite Signal Noise defaults to 0. Increase the controls or enable their toggles when you want those effects. The native NTSC-RS tracking disturbance height remains adjustable up to the full frame. Tracking uses the original NTSC-RS glitch-like tracking waveform; its snow component is disabled to avoid glass-like noise. The synthetic red-line overlay has been removed. Camera selection changes transfer softness/bandwidth, chroma displacement/bleed, edge response, and line stability—not just RGB tint.
