# Built-in Overlay Library Implementation

## What changed
- Added the `14 | BUILT-IN OVERLAY LIBRARY — Animated Reference Assets` section.
- Added an `Off + 21 asset` selector: 15 Glitch Cam references, 4 Retro VHS references, and 2 CRT TV references.
- Added independent opacity and playback-speed controls. The selected asset composites in the pixel render path and does not require a user-supplied layer or an external disk path.
- Added `src/BundledOverlayFrames.h`, generated from the bundled source assets. Each reference is represented by six evenly sampled animation frames at 96x72 pixels in RGB565 format to keep the source-embedded payload relatively small.
- Kept the full-resolution reference files under `assets/overlay-library/`; `embedded-assets-manifest.json` records the mapping.

## Important quality boundary
This makes the library selectable and self-contained, but the embedded rendition is deliberately compact: 96x72 samples are enlarged to the composition dimensions, and only six frames are sampled from each animated reference. It is suitable as a functional first pass for selectable looks, not a pixel-perfect or full-resolution playback of the source GIF. The original full-resolution assets are still present in the repository for a future resource-pipeline improvement.

## Verification
- `python tools/test_bundled_overlay_library.py` checks selector wiring, embedded data dimensions, and manifest consistency.
- `python tools/validate_project.py` checks parameter registration and project metadata.
- `python tools/test_glitch_cam_modes.py` checks the 15 procedural Glitch Cam modes.
- `python tools/test_vhs6rift.py` checks the Python reference model only.
- Native MSVC build and in-After-Effects visual playback are not performed by these checks. This change is source/repository work only; it does not generate or include an `.aex` binary.
