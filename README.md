# VHS6RIFT — NTSC-RS Native AE Edition

A new repository combining the open-source **ntsc-rs** signal-processing engine with the native C++ **VHS6RIFT** Adobe After Effects effect. Target: After Effects 2023 (23.2.1), Windows x64, Visual Studio 2022/MSVC.

## What is here

- `src/VHS6RIFT.cpp`: native AE effect with 19 distinct camera profiles, VHS/tape/signal processing, 15 procedural Glitch Cam looks, 21 selectable native overlay looks, CRT barrel-warped aperture, scanlines, phosphor bloom/RGB triad, flicker and vignette, editable PLAY/REC/date/timecode OSD, source-layer overlays, and sample-domain audio effects.
- `extern/ntsc-rs/`: full upstream ntsc-rs source tree and its original licenses.
- `extern/ntscrs_c/`: C ABI bridge used by the AE effect to invoke ntsc-rs.
- `extern/AfterEffectsSDK/`: the SDK snapshot supplied with the user's existing project.
- `assets/overlays/`: organized import-ready reference media for Glitch Cam, Retro VHS, CRT TV, a date-stamp reference and the five supplied tape-signal clips.
- `references/VHS_MASTER_MEDIA/`: deduplicated source-reference media extracted from the user's VHS MASTER archive.
- `references/MORE_VHS_OVERLAYS/`: additional overlay stills from `repos.zip`.
- `docs/FEATURE_MATRIX.md`, `docs/`, and `reference-analysis/`: implementation inventory, build notes, and reference documentation.
- `.github/workflows/build.yml`: Windows 2022 build pipeline; intended to produce exactly one `VHS6RIFT.aex`.

## Build

On Windows with Visual Studio 2022 Build Tools, CMake, Rust stable (MSVC target), and the supplied compatible Adobe AE SDK:

```powershell
python tools/validate_project.py --static-only
python tools/test_source_contract.py
python tools/test_vhs6rift.py
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DAE_SDK_ROOT="$PWD/extern/AfterEffectsSDK"
cmake --build build --config Release --parallel
.\build\Release\VHS6RIFT_NativeTests.exe
```

The expected output name is `VHS6RIFT.aex`. This source package has not been represented as AE-runtime-tested unless an actual Windows build and After Effects render test is recorded in `docs/BUILD_REPORT.md`.

## Important scope notes

- This is a fresh repository layout, not a patch archive for the old repo. The upstream ntsc-rs source is vendored intact under `extern/ntsc-rs`; its license notices are retained.
- The supplied reference assets are for visual guidance and user-authorized local use. Watermarked compilation footage and embedded third-party binaries/repositories are not redistributed.
- The source-layer overlay input expects you to import desired reference media into After Effects and select it as the overlay layer. Separately, 15 procedural Glitch Cam variants, 21 selectable native overlay effects, and PLAY/REC/date/timecode OSD are native code paths and do not require extra footage layers. Grain/noise, tape, CRT, external overlays, and secondary signal engines default Off unless selected; the main VHS camera/signal treatment starts at Master Strength 1000. The 19 camera profiles are independently parameterized and use different tonal, chroma, gain, softness, edge, and noise-floor response data.
- Adobe SDK redistribution/use remains subject to Adobe's SDK terms.
