# VHS6RIFT Signal Engine Percentage Normalization Fix

## Change
The Signal Reference branch previously multiplied raw 0–100 UI percentages together for luma noise, chroma noise, tape-error probability, Signal Strength, and Other Engine Strength. This could drive values far beyond the intended range, clipping channels and making separate controls feel coupled.

The branch now normalizes each percentage to 0–1 independently, then applies the independent luma, chroma, and error controls to their own paths. Error probability remains bounded by the branch's 0.08 maximum scale.

## Verification status
- Source assertions: run when packaging.
- ZIP integrity: run when packaging.
- Full project validator / native Windows build / After Effects visual test: not completed in this environment; `cargo` was previously unavailable.
