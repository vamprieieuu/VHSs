> **Historical note (2026-10-20):** This document describes an earlier source revision and is superseded where it mentions the Camera Engine selector, CRT TV system, or external layer-based overlay. See `WORK_COMPLETION_REPORT.md`, `IMPLEMENTED_FEATURES.md`, and `KNOWN_LIMITATIONS.md` for the current repair status.

# VHS Master — Visual Validation Report

This document records the visual output comparison for **VHS Master** across implemented processing modes.

---

## Generated Preview Catalog

All preview images were rendered by running the native C++ algorithm processing harness against reference test footage (`previews/original.png`):

| Deliverable Path | Processing Mode / Effect | Key Visual Characteristics | Status |
| --- | --- | --- | --- |
| `previews/original.png` | Original Source Test Image | Unprocessed 8-bar test grid & color gradient | Baseline |
| `previews/camera_fxc6.png` | Fxc6 Reference Profile | Warm highlight lift, muted green response, soft RF edge blur | Verified |
| `previews/camera_gen7.png` | Gen 7 Reference Profile | Lifted shadow pedestal, luma bandwidth limiting, muted contrast | Verified |
| `previews/camera_jvc_c7u.png` | JVC GR-C7U (1986) Profile | High warm saturation, Saticon tube bloom shoulder, organic tube grain | Verified |
| `previews/vhs_sp_mode.png` | VHS SP Mode | Horizontal chroma displacement (bleed), luma softness, YUV matrix shift | Verified |
| `previews/vhs_lp_mode.png` | VHS LP Mode | Increased tape grain, elevated tracking band distortion at bottom raster | Verified |
| `previews/bad_signal_glitch.png` | Bad Signal / Glitch Engine | Horizontally torn bands, displaced RGB line channels, magenta burst interference | Verified |
| `previews/crt_tv_trinitron.png` | CRT TV Engine | Spatial scanline modulation, phosphor bloom, barrel curvature, RGB convergence shift | Verified |
| `previews/grain_color.png` | Organic Grain Engine | Algorithmic pseudo-random color grain with shadow/highlight weighting | Verified |
| `previews/date_text_osd.png` | Dates & Text OSD | 5x7 ASCII bitmap typography overlay (`PLAY ▶`, `05/18/1994`) | Verified |
| `previews/vhs_master_full_suite.png` | Full Suite Combined | Integrated camera curve, chroma bleed, CRT scanlines, grain, and OSD typography | Verified |
| `previews/VISUAL_COMPARISON.png` | 12-Tile Comparison Grid | Side-by-side composite grid of all implemented modes | Verified |

---

## Comparison Summary
The rendered previews confirm that every parameter group produces distinct, recognizable visual transformations consistent with the reference specifications.
