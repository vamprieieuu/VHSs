# VHS6RIFT — Continuous Dropout Streak Fix

## Change
- Reworked the continuous tape dropout stage in `src/VHS6RIFT.cpp`.
- Replaced the previous fine 2D tile hash, which read visually like square speckle, with short horizontal streak segments grouped by row.
- Dropout probability and loss depth now scale directly with the dropout amount; zero remains a true bypass.
- Hashing is deterministic for the same frame, seed, and segment.

## Validation
- Source-level checks verify the old tile grid and nonzero baseline probability were removed and the streak-grid logic is present.
- ZIP integrity checked after packaging.
- Full project validation / MSVC build / After Effects visual render were not run here; the existing validator requires `cargo`, which is unavailable in this environment.
