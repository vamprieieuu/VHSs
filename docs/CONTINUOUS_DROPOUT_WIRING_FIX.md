# VHS6RIFT — Continuous Tape Dropout Wiring Fix

## Change
- Wired the `Tape Dropouts` (`P_DROPOUT`) slider into the continuous image path independently of discrete Glitch Cam events.
- Uses deterministic frame/column/row hashing keyed by the user seed, so a frame is repeatable when re-rendered or scrubbed.
- Amount is scaled by the normalized master signal scale and zero bypasses this stage.
- Existing discrete Glitch Cam dropout behavior remains separate.

## Validation status
- Source-level inspection confirms `P_DROPOUT` now affects the continuous pixel path.
- Not built with MSVC/Windows SDK and not visually tested in After Effects in this environment. `cargo` is unavailable here, so the full project validator cannot complete.
