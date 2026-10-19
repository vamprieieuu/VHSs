# VHS6RIFT — Parameter-to-render audit (2026-10-19)

## Confirmed code-path issue repaired

- **Block Corruption (`P_BLOCK_GLITCH`)** was read into `Settings::blocks` in `ReadSettings`, but had no render-path consumer. This made the UI slider inert.
- Added an independent deterministic block-corruption stage in `Render`, after event glitches and before tape-band processing. It is controlled by `s.continuous`, `s.blocks`, and the normalized signal drive (`sigScale`); zero amount bypasses it. Cell decisions are hashed from seed, frame, and cell coordinates, so the same frame/seed remains reproducible.
- The stage uses small source-derived horizontal cell displacement/blending rather than replacing image cells with arbitrary random colors.

## Static source audit findings

- `P_NTSC_CHROMA_DELAY`, `P_NTSC_HEAD_SWITCH`, `P_NTSC_EDGE_WAVE`, and `P_NTSC_COMPOSITE` are passed into the `ntscrs_process_argb8` argument structure in the NTSC post-pass.
- Audio controls are read by `AudioRender` and affect sample-domain processing (wet/dry, event envelope, crackle, dropout, saturation, quantization). This is source inspection only; audio rendering has not been tested in AE.
- CRT, transparent tape, tape sweep, overlay timing/blend, camera-profile controls, and Glitch Cam settings have explicit reads and pixel-path branches. Source presence does not prove that their perceived strength is correct in actual AE output.
- The audit found `P_BLOCK_GLITCH` as a registered/read parameter that previously lacked a downstream use; it now has one.

## Verification status

- PASS: source-level assertion confirms `s.blocks` is consumed by the new render branch and that the stage has a zero-strength bypass.
- PASS: package ZIP integrity check.
- NOT TESTED: Windows/MSVC build and AE 2023 runtime; this environment lacks `cargo`, so the full project validator cannot complete its Rust-side check.
- NOT TESTED: rendered frame comparisons, visual strength calibration, audio sync, and performance at full resolution.

This report distinguishes static code inspection from runtime verification; it does not claim all 158 parameters have been perceptually validated.
