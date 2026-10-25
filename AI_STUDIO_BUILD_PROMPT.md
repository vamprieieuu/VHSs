# Google AI Studio — VHS6RIFT build and repair prompt

You are maintaining the existing **VHS6RIFT** native Adobe After Effects plugin repository. Target: After Effects 23.2.1, Windows 11 x64, Visual Studio 2022/MSVC, Adobe AE SDK located under `extern/AfterEffectsSDK`.

## Non-negotiable workflow

1. Inspect the entire repository before editing. Read `README.md`, `docs/`, `reference-analysis/`, `CMakeLists.txt`, the PiPL resource, workflow, and every file in `src/`.
2. Do not replace the project with a template, create a different plugin, remove features to make the build green, or change the plugin identity `VHS6RIFT` / match name `com.vhs6rift.effect`.
3. Keep the existing native C++ architecture and the existing SDK. Do not download/insert another SDK version. Detect both supported SDK folder layouts documented in the README.
4. Build on Windows x64 with the provided GitHub Actions workflow. Read the full compiler, linker, resource compiler and PiPLtool logs. Fix the root cause, not just the first visible symptom. Rebuild after every correction until it succeeds or a genuinely missing external dependency is demonstrated.
5. Never claim that an effect is working just because the code compiles. Keep a clear distinction between implemented in source, compiled, loaded in AE, visually tested, audio tested, and sync tested.
6. Never add fake controls. Every control must affect its intended processing stage and must not accidentally affect unrelated stages. If a control is not implemented, remove it or implement it; document the choice.
7. Never claim that a reference AEX's source was recovered. Do not wrap, load, call, decompile-and-copy, or bypass licensing of the supplied third-party AEX binaries. Use the documented reference audit and implement independent behavior.
8. Do not add audio controls without a real audio callback. The current project uses After Effects audio selectors; preserve and correct the actual `PF_Cmd_AUDIO_RENDER` path rather than moving the controls to a mock UI.
9. Preserve deterministic time/seed behavior. Scrubbing backward and rendering the same time/parameters/seed must reproduce the same event pattern. Do not use mutable playback history or global random state.
10. Keep the final build artifact count at exactly one `VHS6RIFT.aex`. Upload build logs as artifacts if useful, but don't accidentally package SDK files or reference AEX files as outputs.

## First build instructions

- Run the existing `tools/validate_project.py` static checks.
- Configure using `cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DAE_SDK_ROOT="$PWD/extern/AfterEffectsSDK"` (use a valid PowerShell path on Windows).
- Build Release with `cmake --build build --config Release --parallel`.
- If the build fails, inspect the SDK macro signatures and exact version's headers rather than guessing. Pay special attention to the parameter enum/setup order, unique topic start/end IDs, PiPL flags matching `GlobalSetup`, the location of `PiPLtool.exe`, and audio structure types.
- Update `docs/BUILD_REPORT.md` with the exact failure, root cause, files changed, build result, and remaining verification gaps.

## Behavior that must remain real

- Seven independent reference camera profiles with profile-specific Character / Color Response / Signal Damage / Grain controls.
- Continuous VHS processing and discrete Event Glitches must be independently toggleable.
- Deterministic event timing based on composition time and seed; event rate, duration, recovery, tracking, dropout, and block corruption must change their respective output.
- Tape families must produce distinct output, with size/speed/direction/strength working.
- Layer-based overlays must support Normal vs integrated composite modes, opacity, speed, time offset and explicit loop period. If custom-time layer checkout is incompatible with AE 23.2.1, repair it using documented SDK callbacks; do not silently leave speed/offset/loop inert.
- Other VHS engine selector must route into distinct independent processing branches and each branch's controls must work. These are reference-inspired implementations, not copies of the proprietary plugins.
- Audio Glitch Enable must bypass audio processing; Audio Event Glitches must be independent from the visual Event Glitches toggle; wet/dry, crackle, dropout, saturation and lo-fi quantization must alter real sample data. Align audio event timing with the visual event schedule and document measured offsets after AE testing.
- Normal overlay mode preserves opaque black pixels; keying is explicit and optional. Do not assume the provided H.264 clips have alpha.

## Test gates after a successful build

1. Check that AE 23.2.1 loads the plugin without reinstall/version/PiPL/flags errors.
2. Test every camera profile on the same source frame; verify that each changes more than only brightness or a LUT.
3. Disable Continuous Treatment and enable Event Glitches: verify distinct short events with quiet gaps.
4. Disable Event Glitches and Continuous Treatment: image must return to the source except explicitly enabled overlay/tape/audio stages.
5. Test each camera-specific control, all Color Controls, each Tape Type, and each Other VHS branch independently.
6. Test overlay Normal/Blend/Screen/Add/Multiply/Luma Key, speed, offset, loop period, and end-of-source behavior.
7. Disable visual Event Glitches but keep Audio Event Glitches enabled; then reverse the toggles. Verify audio is independently controlled.
8. Render the same frame/audio range twice and scrub backward/forward; output must be repeatable for the same seed.
9. Export a short audio/video test and measure synchronization. Record actual results; do not assume sync from code alone.

Start by reading the repo and validating the current code. Make focused changes only when supported by the build logs or a reproducible behavior test.
