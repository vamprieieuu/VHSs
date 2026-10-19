# Built-in OSD and bundled overlay reference library

## New native controls
The `13 | BUILT-IN OSD — Camcorder Display` section provides:
- `Off`
- `REC Indicator` (red REC text and a time-driven blinking red lamp)
- `PLAY Timecode` (green PLAY/timecode)
- `Camcorder OSD` (REC/CAM01/timecode)
- `Security Camera` (amber CAM01/time stamp)
- opacity and scale controls

Text is rendered by a compact 3x5 bitmap font directly in the native pixel path. The timecode is composition elapsed time; it is not a real wall-clock/date stamp. Animation is deterministic from composition time so scrubbing backwards does not rely on global random state.

## Bundled reference assets
`assets/overlay-library/` contains 21 user-provided source media files from `VHS MASTER.zip`, grouped into Glitch Cam, Retro VHS and CRT TV. `manifest.json` records paths, sizes and SHA-256 hashes.

**Important limitation:** the source GIF/JPG files are bundled for convenient manual use/reference, but the current native plug-in does not decode these files as an internal animated preset library. To use one as footage, import it into After Effects, place it on a layer and choose that layer in `Overlay Layer (optional)`. The native procedural OSD is selectable directly inside VHS6RIFT.

## Validation
Static parameter-order validation, Glitch Cam mode tests and source-level wiring audit are expected to pass. Windows/MSVC compilation and an After Effects visual test are still required to establish runtime correctness.
