# Implementation map — current source revision

| Area | Current implementation | Verification status |
|---|---|---|
| VHS camera engines | Five JVC VHS-C/S-VHS-C models with per-model luma bandwidth, chroma bleed/delay, line timing wobble, edge response, noise floor and tracking response | Approximate engineering models; validate visually against captured footage |
| Base signal | Included `ntscrs_c` Rust C-FFI wrapper calling the actual `ntsc-rs` signal engine | Source wired; native build/runtime not run here |
| Image color/chroma | Saturation, black level, luma softness, chroma bleed and X/Y offsets | Static wiring tests pass |
| Grain | Amount, continuous size, speed, softness, and 4mm/8mm/16mm/32mm/64mm preset; disabled and zero amount by default; interpolated lattice texture rather than per-pixel square cells | Static tests pass; visual test pending |
| Colored signal lines | Separate opt-in amount/size/speed controls; applied after monochrome conversion | Static tests pass; visual test pending |
| Signal engines | NTSC-RS, VHS Legend-style, Signal Reference; no VHS Craft | Source wired; visual test pending |
| Glitch Cam | Fifteen modes, amount, chroma split, block size, event rate, palette, blend-in, explicit enable | Static tests pass; native render/AE test pending |
| Overlay/OSD | Optional built-in overlay library and OSD behind explicit checkboxes; no source-footage overlay selector | Static tests pass; visual test pending |
| Removed features | Tape Behavior, transparent tape/tape sweep, source-footage overlay toggle, VHS Craft | Confirmed absent from current source/UI |
| Audio | Optional audio degradation and event controls | Source wired; native build/runtime not run here |

Static checks are not evidence of a successful Windows/MSVC build or correct rendered output in After Effects.
