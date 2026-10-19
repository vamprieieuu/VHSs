# Tracking, grain, and master scaling correction

## Changes
- `Colored Grain Amount = 0` no longer silently injects a fixed 0.20 global grain amount. Camera-specific grain contribution remains controlled by the selected profile's Grain slider and its `noiseFloor`.
- `Tracking Noise = 0` now means no tracking band. Previously a fallback of 0.35 forced tracking even with the slider at zero.
- Tracking strength now uses the normalized `sigScale` once. Previously it multiplied by raw master strength a second time, which could overdrive tracking at the default master setting.
- Grain amount is clamped to its normalized range before being used by the pixel noise stage.

## Verification status
- Source-level assertions and ZIP integrity can be checked locally.
- This is not a Windows/MSVC build result and does not prove the effect loads or renders in After Effects 23.2.1. Verify on the target Windows build and compare zero/low/default/high values in AE.
