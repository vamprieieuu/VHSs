# VHS6RIFT — Master Strength Scaling Audit

## Source changes
- `drive` now preserves the established 3.5 default as 1x, scales down to zero, and continues scaling up to a bounded 2x through the upper UI range. Previously `clamp01(strength / 3.5)` saturated every drive-based camera/signal stage at the default, so values above 350 often did not make those stages stronger.
- Camera black-level response now uses the same normalized `drive` instead of multiplying by raw master strength (0..20), which could make this component grow disproportionately compared with other camera stages.
- The existing early return at master strength zero remains the exact bypass for the master-controlled VHS processing path.

## Scope and caveats
- Optional independent modules such as CRT, procedural overlays, and external overlay compositing are not forcibly scaled by Master Strength; their own enable/strength controls remain independent. This is intentional to avoid Master Strength silently changing unrelated stages.
- Some individual effects clamp their own normalized amount to 0..1. Their response may therefore reach its local maximum before the global 2x ceiling. This patch fixes the master drive saturation and black-level over-scaling; it does not claim every control has been fully visually validated.
- Static source checks only. No Windows/MSVC build, AEX output, or After Effects visual render was available in this environment.
