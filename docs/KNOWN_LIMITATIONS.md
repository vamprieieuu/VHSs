# Known limitations

- The five JVC camera engines are engineering approximations based on documented VHS-C/S-VHS-C formats and camera processing features. Pixel-exact reproduction requires sample footage from each exact model, captured through a controlled playback/capture chain and measured against the emulation.
- Grain, colored signal lines, tracking, and camera-profile wiring have static source-level regression tests, but still require visual confirmation inside After Effects at several resolutions and playback speeds.
- NTSC-RS is the included Rust signal-processing engine. VHS Legend remains a separate C++ signal approximation; the removed Signal Reference and Glitch Cam engines are not part of the UI or render path. Camera-engine profiles are researched approximations, not calibrated captures from the exact physical cameras.
- The current validation environment did not run a Windows/MSVC build or the effect inside After Effects. Static checks are not proof of a successful `.aex` build or visual correctness.
