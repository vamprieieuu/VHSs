# VHS6RIFT — Analog Video Suite

VHS6RIFT is a native After Effects effect for VHS/composite-signal degradation. The source uses the included `ntsc-rs` Rust C-FFI stage as the base signal path and additional C++ processing for chroma, camera-transfer response, grain, optional colored signal lines, Glitch Cam events, signal engines, OSD, and audio degradation.

## Changes in this source revision

- Removed the standalone Moving Grain presentation and replaced it with a single **Grain and Colored Signal Lines** section. Grain amount defaults to `0`, the grain module defaults off, and size, speed, softness, and 4mm/8mm/16mm/32mm/64mm presets are available.
- Added independent optional colored signal-line controls for amount, size, and speed. They are composited after image desaturation, so the picture can be black-and-white while colored signal lines remain colored.
- Removed the Tape Behavior model, transparent-tape sweep controls, the source-footage overlay toggle, and VHS Craft.
- Kept the advanced built-in signal overlay library behind its own explicit enable checkbox.
- Restored the seven supplied reference-derived camera profiles: Fxc6, Gen 7, JVC GR-C7U (1986), VHS-R20, Fall-78, ERC-V2S, and JVC GR-SXM740U. The reference matrix describes these as qualitative matches to the supplied before/after and screen-recording references, not factory-calibrated camera measurements.
- Kept Signal Reference and Glitch Cam as separate, opt-in engines and strengthened their processing so the controls have a clearer visible effect.
- Preserved the NTSC-RS engine and its native tracking-noise implementation.

## Build requirements

- Windows x64 and Visual Studio 2022/MSVC
- CMake
- Rust/Cargo for `extern/ntscrs_c`
- The full Adobe After Effects SDK for the target AE version

The included static tests validate source structure and parameter wiring. They do **not** establish that the Windows plugin build succeeds or that the effect has been visually verified in After Effects. Run the repository's GitHub Actions workflow to confirm the MSVC build, then test the resulting `.aex` in After Effects 2023.
