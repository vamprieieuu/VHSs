# Feature validation — evidence-based status

This document replaces inherited claims of comprehensive visual verification. The repository now includes the requested feature code paths and automated structural/model checks. Native Windows/MSVC compilation and After Effects runtime rendering were not available in this packaging environment.

| Area | Source implementation | Evidence currently available | Status
|---|---|---|
| Native AE effect identity | `VHS6RIFT` PiPL and C++ entry point | Static validator confirms identity/version/outflags | Static PASS; host load untested |
| Parameter registration | 251 registered parameter slots | `python tools/validate_project.py --static-only` | PASS |
| Camera profiles | 19 selector profiles with per-profile controls (7 VHS references + 7 named camera looks + 5 Retro VHS variants) | `python tools/test_vhs6rift.py` model test | Model PASS; AE render untested |
| VHS/tape processing | luma softness, chroma bleed/delay, grain, tracking, dropouts, block/event damage, tape modes and moving sweep | Source inspection and model tests | Implemented in source; perceptual AE validation outstanding |
| NTSC-RS processing | Rust C-ABI bridge invokes vendored `ntsc-rs` engine | Build configuration/source linkage | Source integrated; Rust/MSVC build untested here |
| CRT | scanline modulation, curvature-like falloff, glow, RGB convergence, flicker and vignette | Source inspection | Implemented in source; full display simulation not claimed |
| Layer overlays | overlay layer checkout, time speed/offset/loop and six blend modes | Source inspection | Implemented in source; AE layer checkout untested |
| Audio | AE audio render callback with wet/dry, deterministic events, crackle/dropout, saturation and quantization | Python DSP model only | Native callback not AE-runtime-tested |
| Built-in editable date/PLAY OSD | Native PLAY/REC indicators, editable date stamp, and running timecode are rendered procedurally with an embedded bitmap font | Source inspection | Implemented for PLAY/REC, editable YYYY-MM-DD, and HH:MM:SS; custom arbitrary text is still supplied through the overlay layer |
| Native effect presets and reference media | 15 distinct procedural Glitch Cam looks plus 21 native overlay looks, with user-supplied GIF/JPG/MP4 references organized under `assets/overlays/` | Source inspection and C++ harness source | Procedural effects are native; external animated media still requires importing as an AE layer |
| Windows `.aex` build | CMake/MSVC/PiPL/Cargo workflow | Workflow configured | Not run in this Linux packaging session |
| AE 23.2.1 load and render | Manual host test | No current test evidence | Not tested |

## Automated model checks

`python tools/test_source_contract.py` passed source-path checks; `python tools/test_vhs6rift.py` reported four Python model tests passing: master-strength identity behavior in the model, nineteen distinct model profile outputs, deterministic glitch scheduling, and illustrative audio DSP math. These are not a substitute for the native harness linked to the real Rust bridge or After Effects validation. A local C++ harness run linked to a temporary NTSC stub passed 15/15 test groups; this does not validate the real NTSC-RS algorithm.
