# VHS6RIFT — Master Strength Sweep Audit

## Change made
- Preserved the established UI 350 default (`s.master = 3.5`) as normalized engine drive `1.0`.
- Changed the drive mapping above 350 from a hard ceiling at UI 700 to a bounded linear ramp reaching drive `2.5` at UI 2000.
- Updated the Other Engine Strength path to use the shared `drive` mapping rather than `clamp01(strength / 3.5)`, which had stopped responding above UI 350.
- Kept the exact zero-strength identity-copy branch.

## Expected drive map (derived from source formula; not a rendered-image measurement)
| UI Master Strength | Internal strength | Drive |
|---:|---:|---:|
| 0 | 0.0 | 0.000 |
| 100 | 1.0 | 0.286 |
| 350 | 3.5 | 1.000 |
| 700 | 7.0 | 1.318 |
| 1000 | 10.0 | 1.591 |
| 1500 | 15.0 | 2.045 |
| 2000 | 20.0 | 2.500 |

## Interpretation / limitations
This makes the high end of the Master Strength slider continue to affect drive-dependent stages without unbounded linear gain. It does not guarantee every module scales identically: some parameters are independent by design, some are clamped for stability, and optional stages still obey their own enable switches and controls. A reproducible pixel-difference sweep at 0/100/350/700/1000/1500/2000 requires a working Windows/MSVC/After Effects SDK build and test input. No visual output was generated in this environment, so visual separation is **NOT TESTED**.

## Checks
- Source assertions: expected piecewise mapping and shared `drive` in Other Engine Strength.
- ZIP integrity: to be checked at packaging.
- Full project validation/build: not available here if required toolchains are absent.
