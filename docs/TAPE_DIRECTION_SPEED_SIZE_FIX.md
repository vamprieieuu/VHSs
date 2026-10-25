# VHS6RIFT — Tape Direction / Speed / Size Wiring Fix

## Source changes
- Unified the tape-band travel axis and direction polarity for all moving band types: Top-to-Bottom, Bottom-to-Top, Left-to-Right, and Right-to-Left now consistently select the travel axis and reverse direction.
- Made Tape Speed affect movement and the temporal rate of irregular tape displacement/phase, rather than leaving edge-anchored damage effectively static.
- Made Tape Size control band width and head-switch reach; Signal Snow uses it to control clump size/density.
- Signal Snow now uses a deterministic time tick controlled by speed and a seed, rather than hashing every raw frame at a fixed rate.
- Preserved the existing tape-strength/enable bypass and original NTSC engine.

## Validation status
- Source assertions and ZIP integrity can be checked locally.
- Windows MSVC build and in-After-Effects visual verification were not run in this environment. Do not treat this source audit as proof of final visual behavior.
