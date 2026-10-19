# VHS6RIFT — Native effect implementation inventory

This catalog describes the current `src/VHS6RIFT.cpp` implementation. “Implemented in source” means the control is routed to a native processing stage; it does not mean its visual calibration has been approved in After Effects 23.2.1.

## Core VHS and camera response

- Master VHS Strength, default 1000 on a 0–2000 range; setting it to zero performs an identity copy/bypass.
- Continuous VHS treatment and deterministic, separately toggleable event glitches.
- Nineteen camera/look profiles, each routed through its own Character, Color Response, Signal Damage and Grain controls. Seven labels are based on supplied camera reference folders; the remaining entries are authored look profiles, not measured physical camera calibrations.
- Profile-dependent saturation, contrast, warmth/tint, channel gain, gamma, shadow pedestal, highlight shoulder, chroma bleed/offset, edge ringing and noise floor.
- Luma softness, saturation, black level, chroma bleed and X/Y chroma offset.

## Tape, signal and event damage

- Time/seed-deterministic tracking bands, event windows, dropout, block corruption, attack/duration/recovery, and per-minute event frequency.
- Tape family selector: Off, Horizontal Band, Bottom Head-Switch, Vertical Roll, Side Tear and Signal Snow, with strength, size, speed and direction.
- Optional procedural Transparent Tape and Moving Tape Sweep stages, both disabled by default.
- Four independent “Other VHS” branches: VHS Craft-inspired tracking/chroma/head-switch, VHS Legend-inspired breakup/tear/loss/block damage, the actual vendored NTSC-RS Rust engine through a C ABI, and a separate Signal branch.
- NTSC-RS bridge receives the four signal controls in their intended 0–100 percentage units, plus overall strength, deterministic seed and frame index.

## Native Glitch Cam and overlay library

- Fifteen separate procedural Glitch Cam looks. Each has a distinct native branch for its characteristic tear, channel shift, wave, block, smear, dropout, snow, scratch, sync, echo or mixed signal behavior. Controls include preset, strength, speed, event rate and color-channel mode.
- Twenty-one separately selectable native overlay looks: Bad Signal Tear, Moving Tracking Bar, Head-Switch Noise, RF Interference, Tape Wrinkle, Signal Dropout, Chroma Bleed, Chroma Phase Drift, Vertical Roll, Frame Jitter, Line Echo, Capture Code, REC HUD, CRT Burn-In, Tape Dust, Sync Loss, Luma Pump, Edge Rainbow, Ghosting, Bottom Tracking Noise and Analog Line Damage. Each maps to a distinct native processing branch; strength, motion speed, event rate, color and blend-in are connected to the renderer.
- Optional source-layer overlay for the supplied animated media, with time mapping, speed, offset, loop period, opacity and Normal/Blend/Add/Screen/Multiply/Luma Key modes. External footage must be imported into the AE project; the plugin does not create footage items from its own file directory.

## CRT, OSD and audio

- Optional CRT-inspired stage: barrel-like coordinate warp, scanlines, glow/halo, RGB convergence, flicker and vignette; disabled by default.
- Native bitmap OSD: PLAY, REC, editable YYYY-MM-DD date and composition-time HH:MM:SS, with opacity, scale, four-corner position and safe margin.
- Native AE `PF_Cmd_AUDIO_RENDER` callback: independent Audio Glitch Enable and Audio Event Glitches switches, wet/dry mix, event amount, crackle, dropout, soft saturation and signed waveform lo-fi quantization.

## Default-state behavior

Only the core VHS camera/signal treatment is active on first apply. The dedicated grain/noise module, event glitches, tape family, secondary engine, CRT, source-layer overlay, transparent tape, tape sweep, Glitch Cam, native overlay preset, OSD and audio glitching are opt-in. Tape Type and secondary VHS engine remain genuinely Off unless selected.

## Evidence and limits

- `tools/validate_project.py --static-only`: 251 registered parameter slots, ordering, control presence and PiPL identity/version/outflags.
- `tools/test_source_contract.py`: source checks for 15 Glitch Cam branches, 21 native overlay branches, OSD, signed audio quantization, NTSC bridge and bundled media.
- `tools/test_vhs6rift.py`: four lightweight Python model tests; not a native render test.
- `tools/test_native_render.cpp`: native C++ render harness with 15 test groups, including a loop over all 21 native overlays and all 19 camera profiles. The Windows Actions workflow runs it against the real Rust static library. A local harness run against a temporary NTSC bridge stub is not evidence for the real NTSC-RS algorithm.
- Windows MSVC/Rust/PiPL build, actual NTSC-RS integration execution, and AE 23.2.1 visual/audio/synchronization checks remain outstanding until the workflow and host tests are run.
