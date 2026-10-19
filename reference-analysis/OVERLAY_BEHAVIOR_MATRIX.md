# Overlay behavior matrix

| Asset | Measured duration / frames | Visible pattern from sampled frames | Recommended treatment path | Loop caution |
|---|---|---|---|---|
| Model 1 | 1.528 s / 51 | Dark/black field, horizontal white tracking/static, color speckle, EP/timecode-like element | Normal for original opaque look; integrated Blend or Screen only when desired | Inspect end-to-start; OSD and changing static can jump |
| Model 2 | 0.150 s / 3 | Very short saturated colored tear/static | Normal or keyed test; use as a one-shot burst | Only 3 frames; likely obvious repetition/stutter |
| Model 3 | 1.758 s / 22 | Colored horizontal tracking bands and moving/wavy vertical tear | Blend, Screen or Normal depending on whether black should remain | Test wave phase and band position at boundary |
| Model 4 | 1.798 s / 60 | Broad intermittent horizontal corruption over black with white noise and color fragments | Normal for exact opaque look; Blend/Screen for integration; explicit Luma Key when desired | Intermittent events may jump at boundary |
| Model 5 | 1.678 s / 42 | Multicolor block bars, distorted silhouette and digital corruption | Normal or integrated Blend; key only if tested on the actual asset | Complex subject and block positions likely jump |
| Long overlay demo | 63.300 s / 1899 | PLAY, FF, REWIND, monochrome snow, color bars, signal loss, fine static, bottom tracking; visible stock watermark | Behavioral reference only; do not bundle as a redistributable asset | Sections change at different times; not a single seamless loop |

None of the five short MP4 files reports an alpha-bearing pixel format. Their black backgrounds are opaque video pixels. Normal mode must preserve black; any keying must be an explicit choice.
