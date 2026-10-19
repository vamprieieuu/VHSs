# VHS6RIFT — Continuous Tape Dropout Wiring Fix

## Change
- Uses deterministic frame/column/row hashing keyed by the user seed, so a frame is repeatable when re-rendered or scrubbed.
- Amount is scaled by the normalized master signal scale and zero bypasses this stage.

## Validation status
- Source-level inspection confirms `P_DROPOUT` now affects the continuous pixel path.
- Not built with MSVC/Windows SDK and not visually tested in After Effects in this environment. `cargo` is unavailable here, so the full project validator cannot complete.
