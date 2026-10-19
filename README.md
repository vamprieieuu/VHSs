# VHS6RIFT

**Native Adobe After Effects effect source repository — After Effects 23.2.1 / Windows 11 / x64 / Visual Studio 2022.**

VHS6RIFT is designed as a native image-and-audio effect, not a JSX script that stacks built-in AE effects. The source contains CPU pixel processing for camera-character pipelines, color response, chroma displacement/bleed, time-varying grain, tracking bands, deterministic Glitch Cam events, tape-band families, optional footage overlays, a direct procedural overlay library, CRT and built-in OSD processing, independent reference-inspired signal paths, and an audio render path using After Effects audio selectors.

> **Honest status:** this repository is a first implementation baseline, not a release-certified plugin. The uploaded reference archive contains no usable Adobe SDK and the original repo's `src/` contained only a placeholder. This source has not yet been compiled against your exact SDK or loaded into AE 23.2.1. The first GitHub Actions build is the next verification gate. Audio/video synchronization, overlay checkout/loop timing, rendering at different bit depths, and visual fidelity must be verified inside After Effects before being called complete.

## Current repair-pass additions

- 21 camera profiles including a dedicated B&W Surveillance VHS profile.
- Advanced grain size/softness/structure/color controls, NTSC filter choices, composite sharpening/noise, and head-switch motion controls.
- Built-in OSD and 21 procedural animated overlay styles with their own enable, opacity, speed, and blend controls.
- Source-only repair package; native MSVC and After Effects visual validation must still be run with the licensed SDK installed.

## Included camera reference profiles

- Panasonic Lumix TZ7 / ZS3; Sony Cyber-shot DSC-W180; Sony H7 / H9; Sony Handycam Video8 / Hi8; Canon PowerShot A-series; Canon AE-1 Film Transfer; Sony NEX-5; Canon EOS 400D / XTi; Sony a6000; Sony Cyber-shot DSC-T70; Kodak FZ55; Olympus mju 5000; Excel 2 (Balanced); JVC GR-C7U (1986); JVC GR-SXM740U; B&W Surveillance VHS; Fxc6 Reference; Gen 7 Reference; VHS-R20 Reference; Fall-78 Reference; ERC-V2S Reference

The seven legacy VHS/reference profiles expose dedicated Character, Color Response, Signal Damage, and Grain controls. The compact-camera profiles use their own baked processing response; B&W Surveillance VHS is the only dedicated monochrome profile. These are **reference-derived processing profiles**, not claims of hardware-calibrated sensor emulation. The profiles feed different processing stages; they are not just LUT names.

## Main groups

- Camera profile selector and camera-specific parameter banks
- Color Controls
- Grain and Signal Noise
- Tracking and Damage Events (deterministic from timeline time and seed; no playback-state dependency)
- Tape Band / Tape Noise (horizontal band, bottom head-switch, vertical roll, side tear, signal snow; direction, size, speed, strength)
- Optional external footage overlay (Normal, Blend, Add, Screen, Multiply, Luma Key; opacity, time offset, speed and loop) plus 21 direct procedural built-in overlay styles
- Advanced filters: Constant K / Butterworth lowpass, composite sharpen, chroma demodulation, vertical/dot noise, and adjustable head-switch position / height / motion / random bursts
- Built-in OSD: REC/timecode, date, time, camera label; opacity and white/amber/green color
- Other VHS Engines (independent implementations inspired by observed reference behavior from VHS Craft, the supplied VHS Legend-named binary, ntsc-rs, and Signal)
- Audio Glitches (audio enable, audio-event enable, wet/dry, event amount, crackle, dropout, saturation and lo-fi quantization)

The Other VHS section does **not** load, wrap, or call any reference AEX. It contains separate processing paths built from observed categories. The supplied `VHS LEGEND.aex` has internal strings identifying `Crates VHS Master`/`CratesVHSMaster`; the external filename is not proof that it is a genuine VHS Legend build.

## SDK setup

Add your licensed Adobe After Effects SDK to `extern/AfterEffectsSDK/`. The build supports either of these layouts:

- `extern/AfterEffectsSDK/Headers/AE_Effect.h` plus `extern/AfterEffectsSDK/Resources/PiPLtool.exe`
- `extern/AfterEffectsSDK/Examples/Headers/AE_Effect.h` plus `extern/AfterEffectsSDK/Examples/Resources/PiPLtool.exe`

Do not replace the SDK with a different SDK template. Do not commit a prebuilt AEX from another plugin. The GitHub workflow intentionally fails early with a useful message if the SDK or PiPLtool is missing.

## Build on GitHub Actions

1. Create a repository named `VHS6RIFT` and upload these files/folders.
2. Add the Adobe SDK under `extern/AfterEffectsSDK/`.
3. Push to `main` or run **Actions → VHS6RIFT Windows Build → Run workflow**.
4. Download the `VHS6RIFT-aex` artifact only after the job succeeds.
5. Install the `.aex` into a test copy of AE 23.2.1 and run the tests in `docs/TEST_PLAN.md`.

The intended plugin output is exactly one file: `VHS6RIFT.aex`. The current repair deliverable is source-only; no AEX was built in this session.

## Reference audit

The archive audit and frame contact sheets are in [`reference-analysis/`](reference-analysis/). The source video clips and reference AEX binaries are intentionally not bundled into the repository: the clips include screen recordings/watermarks and the AEX files are third-party binaries. Their measured metadata, SHA-256 hashes, visible behavior notes, and static-inspection findings are documented separately.

## Audio note

The After Effects effect SDK does provide audio selectors (`PF_Cmd_AUDIO_SETUP`, `PF_Cmd_AUDIO_RENDER`, and `PF_Cmd_AUDIO_SETDOWN`). This prototype implements a sample-domain audio render path instead of presenting audio-only sliders as if they were working. The callback path has not yet been tested in AE 23.2.1, and timing alignment with the visual event schedule must be measured in a real render.

## License and reference use

The VHS6RIFT implementation in this repository is original project code. It does not contain copied source code or embedded third-party plugin binaries. `ntsc-rs` is an open-source reference project; if future changes copy/adapt code rather than merely use it as a behavioral reference, preserve the upstream license/notice obligations and review the specific files' licenses first. Proprietary AEX files are for behavioral/static reference only.
