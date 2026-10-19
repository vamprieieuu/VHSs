# VHS6RIFT NTSC-RS fresh repository — packaging report

## Delivered in this package

- A fresh repository directory named `VHS6RIFT_NTSC_RS`, independent from the previous working tree.
- The current native `VHS6RIFT` source, CMake build, PiPL resources, Windows GitHub Actions workflow, static validator and Python model tests.
- Complete upstream `ntsc-rs-main` source vendored under `extern/ntsc-rs` with its original license files.
- The `ntscrs_c` Rust C-ABI bridge used by the C++ effect.
- The supplied AE SDK snapshot, preserved under `extern/AfterEffectsSDK`.
- A deduplicated media pack extracted from the supplied `VHS MASTER.zip` and the extra `more vhs overlays` folder in `repos.zip`, plus a SHA-256 manifest. Embedded third-party plugin binaries/repositories and a watermarked stock compilation are not redistributed.
- Documentation distinguishing source implementation from features that still require host verification.

## Checks actually run in this packaging environment

- `python tools/validate_project.py --static-only`: PASS — 251 parameter slots, expected controls, PiPL identity/version/outflags.
- `python tools/test_source_contract.py`: PASS — checks the 15 Glitch Cam branches, all 21 native overlay branches, OSD date-only path, signed audio quantization, overlay checkout, audio callback and media pack.
- `python tools/test_vhs6rift.py`: PASS — four Python model tests.
- `g++ -std=c++17 -fsyntax-only ... tools/test_native_render.cpp`: PASS — native C++ syntax check.
- Local native C++ render harness: 15/15 passed when linked to a temporary NTSC bridge stub; this validates the C++ render paths but **does not** validate the real Rust NTSC-RS engine. The Windows workflow compiles/runs the same harness against the real Rust static library.
- Windows CMake/MSVC/Cargo/PiPL build: NOT RUN here.
- After Effects 23.2.1 load, visual output, audio, and sync tests: NOT RUN.

## Important implementation boundary

The updated source exposes 19 independently parameterized camera/look profiles, 15 native procedural Glitch Cam presets, 21 built-in native overlay looks, a built-in editable PLAY/REC/date/timecode OSD, a layer-based overlay input with timing/opacity/blend modes, CRT processing, tape and signal damage modules, NTSC-RS integration, and sample-domain audio controls. The supplied media are not auto-loaded into the AE project as dropdown footage assets; they remain available for user-selected source-layer overlays. Native Windows/MSVC build and AE render remain unverified in this environment.
