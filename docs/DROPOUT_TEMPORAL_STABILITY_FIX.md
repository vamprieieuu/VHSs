# VHS6RIFT — dropout temporal stability fix

## Change
- Continuous Tape Dropouts now hash the current frame in three-frame buckets, rather than changing the dropout mask every single frame.
- This makes a short dropout mark persist briefly (about three frames at the composition's frame rate) instead of looking like independent per-frame digital flicker.
- The mask remains deterministic from composition frame, seed, segment coordinates, and streak row; no global random state or render-history state was introduced.
- Existing `Tape Dropouts = 0` bypass remains unchanged.

## Validation status
- Source-level target replacement checked.
- ZIP integrity should be checked after packaging.
- Not tested with MSVC, the After Effects SDK build, or a rendered frame inside After Effects in this environment. No claim of visual verification is made.
