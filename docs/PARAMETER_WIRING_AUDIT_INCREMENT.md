# VHS6RIFT parameter wiring audit — increment

## Confirmed behavior defects fixed

- **Other Engine Strength** previously replaced a user-entered zero with `0.50` whenever the Other Engine module was enabled. The slider therefore could not truly disable that engine's contribution. It now uses the normalized slider value directly; zero is a bypass.
- **Tape Strength** previously replaced zero with `0.35` whenever the Tape module was enabled. It now uses the normalized slider directly; zero is a bypass.
- The previous increment also removed the hidden 0.20 global grain floor and the hidden 0.35 tracking floor, and removed a second multiplication by raw master strength from tracking. Those changes remain in this package.

## Static evidence / limits

- Parameter registration count in source: 158 (the project validator's initial static check agrees).
- The source has 7 selectable camera profiles; it should not be described as having 20 selectable profiles.
- This is a source-level audit, not a visual acceptance test. The validator's first static checks pass, but its full run stops because `cargo` is not installed in this environment. No Windows MSVC build or After Effects render was run here.
