# VHS6RIFT asset pack

These are user-provided reference assets organized into one importable folder tree. The live effect implementation is in `src/VHS6RIFT.cpp`; the 15 Glitch Cam choices are native procedural pixel engines and do not simply paint these reference files over the footage.

- `overlays/glitch-cam/`: numbered supplied Glitch Cam GIF/JPG references, normalized to `glitch-cam-01` through `glitch-cam-15`.
- `overlays/retro-vhs/`: supplied Retro VHS loop/still references.
- `overlays/crt-tv/`: supplied CRT-TV animated references.
- `overlays/tape-models/`: supplied Model 1–5 VHS signal/tape clips. Their black backgrounds are opaque; use Normal to preserve the reference look or choose a composite/key mode intentionally.
- `overlays/text-date/`: supplied date-stamp still reference.

## How these assets connect to the native effect

After Effects effect plug-ins cannot safely create project footage items from files in a plugin-side directory merely because a popup was selected. For actual supplied animated media, import the asset into the AE project and assign that layer to **Overlay Layer (optional)**. The effect applies its native time mapping, speed, offset, looping, opacity, and composite mode. Built-in procedural Glitch Cam looks and PLAY/REC/date/timecode OSD do not require an overlay layer.

The 63.3-second stock-watermarked overlay compilation is intentionally excluded. Third-party AEX binaries and nested repositories are also excluded.
