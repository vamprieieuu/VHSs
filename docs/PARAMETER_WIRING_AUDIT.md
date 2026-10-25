> **Historical note (2026-10-20):** This document describes an earlier source revision and is superseded where it mentions the Camera Engine selector, CRT TV system, or external layer-based overlay. See `WORK_COMPLETION_REPORT.md`, `IMPLEMENTED_FEATURES.md`, and `KNOWN_LIMITATIONS.md` for the current repair status.

# VHS6RIFT Parameter Wiring / Module Independence Audit

This is a source-level static audit of `src/VHS6RIFT.cpp`, not a rendered-frame test. It does not claim visual results.

## Findings confirmed from source

1. **Camera profile controls are selection-dependent.** Only the four sliders for the currently selected camera profile are read through `cameraParamIds[ci]`; other profiles' sliders are dormant until that profile is selected. This is intentional profile-specific behavior, but can look like sliders do nothing if the wrong profile is selected.
2. **CRT is independently opt-in.** `P_CRT_ENABLE` gates scanlines, curvature, glow, RGB triad, flicker and vignette. These should be tested with CRT enabled and Master VHS Strength above zero; curvature is a geometry remap.
3. **Tape engine is gated by `P_TAPE_ENABLE`.** The tape-type, strength, size, speed and direction are applied only when the module is enabled and effective strength is nonzero. The selected tape type also needs to be non-Off. The prior fallback that silently changed `Off` into `Horizontal Band` when the module checkbox was enabled has been removed; `Off` is now a true neutral selection.
5. **Other VHS engines are gated by `P_OTHER_ENABLE` and `P_OTHER_STRENGTH`.** Craft, Legend and Signal Reference controls are only processed inside the selected Other Engine branch. Therefore the Signal Reference sliders do not act as a global independent Signal stage; select Signal Reference and enable Other VHS to test them. The prior fallback that silently substituted Craft when the popup was `Off` has been removed; `Off` is now a true neutral selection.
6. **Signal Reference controls share the Other Engine strength multiplier.** The source computes `signalDrive = SignalStrength * OtherStrength` after percentage normalization. Reducing Other Strength reduces all Signal Reference subcontrols together.
7. **Continuous VHS Treatment is a separate master gate** for the continuous treatment path. Event glitches, CRT and procedural tape overlays have separate paths/gates and should not be confused with the continuous VHS pass.

## Suggested controlled test order

- Use a still image with fine text and saturated red/blue edges; keep resolution and frame time fixed.
- Set Master Strength to 350, Continuous VHS Treatment on, Event Glitches off; compare all 20 native camera profiles one at a time. The lightweight Python model only covers legacy reference examples and is not the native renderer.
- Enable CRT only, then sweep one CRT control at a time.
- Enable Tape only, select a non-Off tape type, then sweep strength/size/speed/direction.
- Enable Other VHS, select **Signal Reference**, then sweep Signal Strength, Luma Noise, Chroma Noise and Tape Errors individually.
- Test zero values as bypasses and capture screenshots/frames; do not infer motion from a single still frame.

## Automated source checks

- Enumerated parameter IDs: **159** (excluding `P_COUNT`).
- Structural topic/input IDs excluded from low-reference warnings: **45**.
- No non-structural parameter IDs had only two source references.

## Validation limitation

The repository validator checks parameter order, feature presence and PiPL identity, but its final native-render test invokes `cargo`. This environment has no `cargo`, so that part cannot complete here. No MSVC build, `.aex` load, or After Effects visual render was performed.
