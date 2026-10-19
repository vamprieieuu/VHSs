# Bundled Overlay Library Quality Update

- Internal native overlay previews increased from 96x72 / 6 temporal samples to 160x90 / 10 temporal samples per source asset.
- Frames are sampled across the available GIF timeline and resized with Lanczos filtering.
- The native frame selector now derives frame timing from `kBundledOverlayFrames`, so the sampling count stays in sync with the generated header.
- Full original GIF/JPG files remain bundled under `assets/overlay-library/`.
- Regenerate deterministically with `python tools/generate_bundled_overlay_frames.py` (requires Pillow).
- This improves internal preset detail and temporal sampling but is still a compact embedded representation, not full-resolution source playback. No AEX artifact is produced by this repository update.
