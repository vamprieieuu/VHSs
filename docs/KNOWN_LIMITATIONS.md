# Known limitations

- Camera profiles are qualitative transfer-response matches based on the supplied camera-reference clips and before/after screen recordings. They are not claimed to reproduce each camera's sensor or tape transport with laboratory accuracy.
- The Grain and Colored Signal Lines controls have static source-level wiring tests, but need visual confirmation in After Effects at several resolutions and playback speeds.
- Signal Reference and Glitch Cam are deterministic time-based C++ approximations; NTSC-RS is the included Rust signal-processing engine. The reference engines are not third-party plugin binaries.
- The current validation environment did not run a Windows/MSVC build or the effect inside After Effects. Static checks are not proof of a successful `.aex` build or visual correctness.
