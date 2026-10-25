# Fresh repository manifest

This repository was assembled from the user's supplied `repos.zip`, `VHS MASTER.zip`, and latest `VHS6RIFT` handoff. The top-level project is the native VHS6RIFT effect; the complete upstream `ntsc-rs-main` source is retained at `extern/ntsc-rs`, and the C ABI bridge compiles the `ntscrs` crate from that vendored source.

## Feature groups present in source

- Camera/reference profiles currently selectable in the native effect: Fxc6, Gen 7, JVC GR-C7U (1986), VHS-R20, Fall-78, ERC-V2S, JVC GR-SXM740U.
- VHS image treatment: chroma bleed/delay, luma softness, saturation/black level, moving seeded grain, tracking/head-switching, dropouts, block/event glitches, tape generation/type and advanced tape sweeps.
- NTSC-RS bridge: chroma delay, head switching, luma smear and composite noise are passed into the upstream ntsc-rs engine.
- CRT controls: scanlines, curvature, glow, RGB convergence, flicker and vignette.
- Overlay source layer: time speed/offset/looping, opacity, and blend modes including normal/blend/add/screen/multiply/luma-key paths.
- Audio render path: wet/dry, deterministic event glitches, crackle, dropout, saturation and lo-fi quantization, exposed through the AE audio callback. Host-level audio callback behavior still needs a real AE render test.

## Explicit validation status

- `python tools/validate_project.py`: run when packaging.
- `python tools/test_vhs6rift.py`: run when packaging.
- Native Windows CMake/MSVC + PiPL + Cargo build: requires Windows; not executed in this Linux packaging session.
- After Effects 23.2.1 load, visual output, audio sync, and per-control perceptual validation: not claimed unless supported by recorded test evidence.
