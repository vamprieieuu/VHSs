# Build and validation report — VHS6RIFT NTSC-RS repository

## Package contents
- Fresh repository root: `VHS6RIFT_NTSC_RS`.
- Native AE effect identity: `VHS6RIFT`; target is After Effects 2023 (23.2.1), Windows x64, Visual Studio 2022/MSVC.
- 251 registered native AE parameter slots; 19 camera/look profiles, 15 Glitch Cam looks, 21 built-in overlay effects, OSD, CRT, tape/signal and audio modules.
- Complete upstream `ntsc-rs` source under `extern/ntsc-rs`, the `ntscrs_c` bridge, CMake, PiPL resources, Windows GitHub Actions, and the native C++ test harness.
- User reference media is organized under `assets/overlays/` for import and under `references/` for provenance/inventory.

## Checks actually run in this environment
- `python tools/validate_project.py --static-only`: **PASS** — 251 parameter slots in matching enum/registration order, required controls, camera profile routing, 15 Glitch Cam choices, 21 native overlay choices, PiPL identity/version/outflags.
- `python tools/test_vhs6rift.py`: **PASS** — four Python model/scheduler tests. These are illustrative model checks, not proof of native AE rendering.
- `g++ -std=c++17 -fsyntax-only ... tools/test_native_render.cpp`: **PASS** — native C++ test harness and included render source parse successfully against the SDK shim.
- Native C++ harness using a temporary `ntscrs_process_argb8` test stub: **15/15 test groups passed**, including all 19 camera profiles, CRT, tape/tracking, procedural tape effects, Glitch Cam, OSD, all 21 native overlays against a matched baseline, and sample-domain audio callback checks. The stub is not the real NTSC-RS engine and this is not a substitute for the Windows Rust/MSVC build.
- Rust/Cargo build and Windows MSVC/PiPL build: **NOT RUN** here (Cargo and the Windows toolchain are unavailable).
- After Effects 23.2.1 load/render/audio test: **NOT RUN**.

The repository intentionally contains source/build/test files, not a prebuilt `.aex`. GitHub Actions is configured to compile the real Rust C ABI library, build the effect and native test executable, run the native suite, and upload one `VHS6RIFT.aex` artifact plus logs.

- CRT source was upgraded with a warped-screen aperture (black curved corners), neighbor-sampled phosphor bloom, and subtle RGB phosphor triad modulation. The native CRT toggle test still passes; this is algorithmic source verification, not an AE screenshot certification.
