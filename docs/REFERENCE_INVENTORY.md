# VHS6RIFT — Reference Inventory & Asset Index

This document presents a complete inventory of reference media, folders, images, and inspection material used to reconstruct the VHS6RIFT visual processing engine.

---

## 1. Reference Folders & Media Assets

| Folder / Reference ID | Associated Assets & Files | Target VHS Engine / Feature | Inspected Characteristics | Verification Status |
|---|---|---|---|---|
| **gen7** | `gen7/` (Folder 1 & Folder 2, instructions, clips) | Main VHS Engine | Generational luma bandwidth loss (~240 TV lines), Y/C delay, chroma bleed, horizontal edge overshoot, signal instability | Verified & Reconstructed in C++ |
| **uni.vhs** | `uni.vhs/` (Sample clips & instruction TXT) | Tracking Damage Engine | Bottom-raster head-switch noise, periodic tracking bands, vertical roll, side-edge tearing, horizontal line jitter | Verified & Reconstructed in C++ |
| **Model 1–5** | `Model 1` to `Model 5` folders | Reference Model Effects | Distinct glitch patterns, muted analog tape, chroma bleed & roll, highlight bloom, low-band composite quantization | Verified & Reconstructed in C++ |
| **Camera Folders** | 20 individual camera reference profiles | Camera Emulation Engine | Sensor noise, highlight shoulder roll-off, shadow response, channel gains, optical sharpness | Verified & Reconstructed in C++ |
| **CRT Reference** | CRT television & monitor captures | CRT TV Engine | Scanlines, phosphor glow, curvature barrel warp, RGB convergence, edge falloff, screen flicker, vignette | Verified & Reconstructed in C++ |
| **Overlays** | Overlay collection & reference clips | Overlays & Layers | Procedural & layer-based overlays, time-remapped looping, Normal/Add/Screen/Multiply/Luma Key blending | Verified & Reconstructed in C++ |
| **Image Reference Set** | `v00_reference.jpg` – `v21_reference.jpg` | All Modules | Visual fallback reference contact sheets for side-by-side behavioral comparison | Inspected & Mapped |

---

## 2. Reference Interpretation & Fallback Rules

1. All reference assets were systematically inspected across frame timelines (beginning, middle, end, transitions).
2. Every enabled control maps directly to pixel processing in `src/VHS6RIFT.cpp`.
3. Disabling an optional module bypasses its rendering without affecting source pixels.
