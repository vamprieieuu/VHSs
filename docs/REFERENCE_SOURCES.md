# Reference sources and interpretation rules

## Supplied material

See `reference-analysis/REFERENCE_AUDIT.md`, `BINARY_REFERENCE_INSPECTION.md`, `video-inventory.csv`, and `binary-inventory.json`.

## Public technical references

- Maxon Universe VHS official guide: https://help.maxon.net/uni/en-us/Content/html/vhs.html
  - Documents Color Controls, Chroma Offset/Weave, Channel Controls, Tape Noise Type, noise scale, pop-line frequency, frame jitter, interlace error, Tape Wrinkle, Tape Style, Post Noise, Random Seed and Blend with Original.
- ntsc-rs source repository: https://github.com/ntsc-rs/ntsc-rs
  - Open-source reference for modeling NTSC/VHS artifacts with signal-processing stages. It describes itself as a Rust port of ntscqt/composite-video-simulator and explicitly notes visible differences from the original processing passes.
- After Effects C++ SDK Guide, command selectors: https://ae-plugins.docsforadobe.dev/effect-basics/command-selectors/
  - Documents the native effect entry point and `PF_Cmd_RENDER`, `PF_Cmd_AUDIO_SETUP`, `PF_Cmd_AUDIO_RENDER`, and `PF_Cmd_AUDIO_SETDOWN` selectors.
- After Effects C++ SDK Guide, audio: https://ae-plugins.docsforadobe.dev/audio/audio/
  - Documents the audio processing pipeline and formats.
- After Effects C++ SDK Guide, PiPL resources: https://ae-plugins.docsforadobe.dev/intro/pipl-resources/
  - Documents the PiPL metadata and Windows resource-generation pipeline.

## How references are used

- Reference behavior and control categories inform independently implemented processing; proprietary AEX files are not linked or embedded.
- Static strings/imports can suggest internal processing stages and dependencies but are not proof of exact runtime behavior.
- Screen-recording comparisons are qualitative unless they are demonstrated to be aligned, equal-duration, equal-framing and frame-identical inputs.
- Overlay clips without alpha retain opaque black backgrounds in Normal mode. Keying is an explicit optional mode.
