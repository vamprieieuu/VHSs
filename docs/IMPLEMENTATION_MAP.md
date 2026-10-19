# Requirements-to-implementation map

| Requirement | Current source path | Current status before first Windows build |
|---|---|---|
| Native AEX, not JSX / stacked built-in effects | `src/VHS6RIFT.cpp`, `resources/VHS6RIFTPiPL.r` | Implemented in source; not compiled yet |
| 20 camera profiles | 20-entry camera selector + 20-entry native profile table; legacy VHS references retain per-profile fine-tuning groups | Profile table wired in source; device-accurate visual matching still requires AE render comparisons |
| Per-camera Character / Color / Signal / Grain controls | `cameraParamIds` and profile-specific arrays in `Render` | Implemented in source; visual isolation tests pending |
| Continuous treatment separate from event glitches | `P_CONTINUOUS`, `P_EVENTS` and separate render branches | Implemented in source; tests pending |
| Deterministic events | `hash32`, `rand01`, timeline-based event slots | Implemented in source; frame-rate and scrubbing tests pending |
| Color controls | Saturation, black level, luma softness, chroma bleed/offset | Implemented in source; output-range and perceptual tuning pending |
| Colored grain | spatial/time-seeded hash noise with scale/speed controls and opt-in toggle | Implemented in source; noise distribution and temporal quality pending |
| Tape band families | Tape Type / Strength / Size / Speed / Direction | Source-sample displacement and distinct band behaviors implemented; visual comparison pending |
| Video overlay layer | layer parameter checkout at mapped time + Normal/Add/Screen/Multiply/Luma Key | Implemented in source; SDK checkout behavior and source-end/loop semantics require AE verification |
| Overlay speed/offset/loop | time remap via custom-time checkout and explicit loop-period control | Implemented in source; loop duration is user-specified because an arbitrary layer's source duration is not inferred here; test required |
| Other VHS reference-inspired engines | VHS Craft, VHS Legend-named binary, NTSC-RS, Signal branches | Independent processing paths and controls implemented in source; not the original algorithms; opt-in toggle and visual tests pending |
| CRT TV | Scanline, curvature, glow, RGB convergence, flicker, vignette | New optional CRT stage implemented in source; reference comparison and AE render tests pending |
| Procedural Transparent Tape | Time-driven irregular band with luma/chroma integration | New optional generated effect; not a substitute for the supplied overlay assets; visual calibration pending |
| Moving Tape Sweep | Time-driven moving band with source displacement and chroma/luma disturbance | New optional effect; sweep behavior and interval require AE visual validation |
| Real audio processing | `PF_Cmd_AUDIO_RENDER`, `PF_SoundWorld` sample path, independent audio toggles and controls | Implemented in source; audio callback/sample-format/sync tests pending |
| SDK/PiPL build | CMake PiPL tool pipeline + GitHub Actions | Code/PiPL version and outflags declarations aligned in source; Windows Actions build still required |
| AE 23.2.1 load and render | Manual test plan | Not verified |
| Audio/video synchronization | deterministic timeline event schedule shared by video/audio code | Designed for alignment; not verified |

## Intentional boundaries

- No third-party AEX is loaded, wrapped, or included in the output.
- The source recordings and proprietary binaries are not copied into the output repository.
- The long overlay reference clip includes a visible stock watermark; it remains reference-only.
- The supplied VHS Legend-named binary contains internal Crates VHS Master identifiers and GPU/runtime strings; its filename alone does not establish product identity.
- The first implementation uses CPU processing. It does not promise GPU acceleration or a physical electrical circuit simulation.
