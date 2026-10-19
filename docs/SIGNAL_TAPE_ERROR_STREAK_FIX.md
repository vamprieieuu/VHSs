# VHS6RIFT — Signal Tape Error Streak Fix

## Change
The Signal Reference engine's `Signal Tape Errors` mask previously darkened coarse rectangular cells. It now uses short horizontal segmented streaks, with the mask derived deterministically from coordinates, frame index, and the user seed. The normalized `Signal Tape Errors` value still gates the probability; luma and chroma noise remain independently controlled.

## Scope
- Changed: `src/VHS6RIFT.cpp` only, plus this report.
- Preserved the existing NTSC source and plug-in identity.
- No claim of exact physical tape simulation; this is a procedural signal-error approximation.

## Verification
- Source patch assertion and ZIP integrity check: to be run for this package.
- Windows/MSVC build and visual test in After Effects: NOT TESTED in this environment (`cargo` is unavailable, and this is not a Windows AE runtime).
