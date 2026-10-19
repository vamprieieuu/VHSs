# VHS6RIFT

**Native Adobe After Effects effect source repository — target: After Effects 23.2.1 / Windows 11 x64 / Visual Studio 2022.**

VHS6RIFT is a native C++ effect with a real `ntsc-rs` Rust signal-processing stage. The project is intended to produce VHS signal degradation, not merely a camera-color preset. This repair removes the selectable **Camera Engine** and its per-camera color transforms; a fixed VHS transfer response remains, while the main look comes from NTSC composite processing, chroma separation, softness, tracking, dropouts, tape behavior, grain, and signal engines.

> **Verification boundary:** the source changes and static tests have been run locally. Cargo and the complete licensed Adobe SDK/PiPLtool were not available here, so neither the Rust/C++ native build nor a visual test inside After Effects has been completed. Do not treat the source checks as proof of runtime appearance or a successful `.aex` load.

## Changes in this repair

- Removed the `CAMERA ENGINE | Profile` selector and all per-camera tuning banks so camera tinting no longer substitutes for the VHS effect.
- Made the real `ntsc-rs` processing path an independently switchable signal engine with non-zero starting controls.
- Added separate enable switches for VHS Craft, VHS Legend-style processing, NTSC-RS, and Signal Reference; the engines are no longer selected from one mutually exclusive dropdown.
- Increased default moving grain and replaced coarse square-cell sampling with interpolated time-varying noise.
- Reworked tape behavior with moving band-edge jitter, luma loss, chroma separation, side tears, and vertical-roll displacement. Tape behavior remains off by default to avoid unwanted damage on load.
- Replaced the external footage layer selector with a simple yes/no toggle that enables the built-in procedural VHS overlay library directly on the source footage.
- Removed the CRT TV system and its controls, per the requested scope reduction.
- Kept Glitch Cam as a separate opt-in, timeline-driven whole-frame event stage with independent style, amount, rate, chroma split, block size, palette, and blend-in controls.

## Main groups

- VHS Strength and Continuous VHS Signal Treatment
- Color and Chroma
- Moving Grain and Advanced Grain
- Signal / Tracking and Damage Events
- Tape Behavior Model, Strength, Size, Speed, Direction
- Source-footage VHS Overlay toggle and procedural overlay style controls
- Independent signal engines: VHS Craft, VHS Legend-style, NTSC-RS, Signal Reference
- Built-in OSD: REC/timecode/date/time/camera label
- Advanced lowpass, composite sharpening/noise, chroma demodulation, head-switch position/height/motion/bursts
- Audio render controls (present in source, still requires host verification)

The VHS Craft, VHS Legend-style, and Signal Reference paths are separate native processing implementations. They do not load or call third-party AEX plugins. The NTSC-RS stage calls the included Rust C-FFI wrapper and requires Cargo/Rust to build its static library.

## SDK setup and build

Add the licensed Adobe After Effects SDK to `extern/AfterEffectsSDK/`. Supported layouts:

- `extern/AfterEffectsSDK/Headers/AE_Effect.h` plus `extern/AfterEffectsSDK/Resources/PiPLtool.exe`
- `extern/AfterEffectsSDK/Examples/Headers/AE_Effect.h` plus `extern/AfterEffectsSDK/Examples/Resources/PiPLtool.exe`

The GitHub Actions workflow is configured for Windows/MSVC and expects the Rust toolchain for `ntscrs_c`. The intended output is exactly `VHS6RIFT.aex`; this repair deliverable is source-only and contains no `.aex` binary.

## Validation performed

- `python tools/validate_project.py` — static parameter registration and source checks.
- `python tools/audit_parameter_wiring.py` — static parameter-wiring audit.
- `python -m pytest -q` — Python source/model tests.
- A delimiter-balance scan of the changed C++ source.

These checks do not replace the Windows native build or AE 23.2.1 visual/runtime validation.
