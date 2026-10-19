# VHS6RIFT — Luma/Chroma Signal Stage

This revision changes the continuous VHS color path in `src/VHS6RIFT.cpp` from blending a fully shifted RGB sample into the current pixel to a luma/chroma-separated reconstruction.

- Luma is computed from the current pixel and remains spatially anchored.
- The chroma sample follows the existing horizontal/vertical chroma offsets.
- A small three-tap horizontal chroma filter limits color bandwidth before reconstruction, creating color bleed without blurring all luminance edges.
- The current `Chroma Bleed`, `Chroma X`, and `Chroma Y` controls continue to drive the stage; no parameter IDs or PiPL identity are changed.
- Processing remains deterministic and uses clamped source coordinates at frame boundaries.

This is an algorithmic source change, not proof of a successful Windows/MSVC build or After Effects render. Those still require CI and an AE 23.2.1 visual test.
