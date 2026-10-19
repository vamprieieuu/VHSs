> **Historical note (2026-10-20):** This document describes an earlier source revision and is superseded where it mentions the Camera Engine selector, CRT TV system, or external layer-based overlay. See `WORK_COMPLETION_REPORT.md`, `IMPLEMENTED_FEATURES.md`, and `KNOWN_LIMITATIONS.md` for the current repair status.

> **Superseded status note:** This historical note predates the 2026-10-19 repair pass. Current parameter count and verification status are in `WORK_COMPLETION_REPORT.md` and `FEATURE_VALIDATION.md`.

# VHS6RIFT — Final Parameter Wiring and Independence Audit

Date: 2026-10-19

## Source-level corrections in this revision

- Removed the implicit **Other VHS Engine: Off → VHS Craft** fallback. When `P_OTHER_ENGINE` is Off, the engine now remains Off even if `P_OTHER_ENABLE` is checked. This prevents an unrequested secondary engine from changing the default VHS look.
- Removed the implicit **Tape Type: Off → Horizontal Band** fallback. When Tape Type is Off, the tape engine remains neutral even if Enable Tape Effects is checked. Users must explicitly choose a tape type.
- Preserved independent module switches and the Master Strength zero bypass. These two corrections do not change the PiPL identity, version, parameter order, or registered parameter count.

## Module dependency map (source inspection)

| Module / controls | Required activation | Important dependency |
|---|---|---|
| Main camera/color response | Master Strength > 0 and Continuous VHS Treatment on | Only the selected camera profile's Character/Color/Signal/Grain controls are read |
| Grain | Enable Grain / Noise | Runs inside Continuous VHS Treatment; profile grain contributes to the amount |
| Tracking/dropouts/block damage | Enable Tracking Damage where applicable, Continuous VHS Treatment on | Signal amount is normalized by selected camera signal and Master Strength |
| Tape type / direction / size / speed | Enable Tape Effects, non-Off Tape Type, nonzero Tape Strength | Off now remains Off; no hidden fallback |
| CRT | Enable CRT TV Engine | Scanlines, curve, glow, RGB mask, flicker and vignette are gated by CRT enable |
| Overlay | Enable Overlay Layer and assign a valid Overlay Layer | Overlay opacity, blend mode, speed, offset and looping require a valid layer |
| Other VHS engine | Enable Other Signal Engines, choose a non-Off engine, nonzero Other Strength | Craft, Legend, NTSC-RS and Signal Reference have distinct branches; Off remains Off |
| Signal Reference controls | Other engine = Signal Reference | Signal Strength scales Luma Noise, Chroma Noise and Tape Errors; these controls are not a global signal stage |
| Glitch Cam | Event Glitches on and effective event rate > 0 | Event Rate override takes precedence over the base event rate when positive |
| Audio processing | Audio Glitch Enable on | Requires audio render support; effect is wet/dry mixed and uses event controls |
| Procedural transparent tape / tape sweep | Their individual checkboxes | Separate from the selected Tape Type module |

## Static checks performed

- Parameter enum / registration order: 192 registered parameters (excluding implicit input and sentinel) is expected by the project validator.
- All non-structural registered control IDs have source references beyond the enum and registration declarations.
- Verified source contains explicit neutral selection for Other VHS Engine Off and Tape Type Off.
- Verified the master-strength zero-copy bypass, CRT opt-in, and Glitch Cam effective-rate gate remain present.
- ZIP archive integrity is checked after packaging.

## Limits — not represented as passes

This is not a substitute for a Windows/MSVC build or a rendered-frame comparison. The available environment does not have Cargo, and no Adobe After Effects runtime is available here. Therefore AEX compilation/loading, image-difference sweeps, audio behavior, temporal motion and performance remain unverified in After Effects. The parameter wiring audit is based on source inspection, not claims that every control has been visually calibrated.

## Recommended AE smoke test

1. Apply VHS6RIFT to a still image with text and saturated color edges; keep Master Strength at 350.
2. Verify Tape Type Off causes no tape-band effect even with Enable Tape Effects checked.
3. Verify Other VHS Engine Off causes no Craft/Legend/Signal Reference effect even with Enable Other Signal Engines checked.
4. Toggle CRT, then change only one CRT slider at a time.
5. Select each camera profile and test its own four profile sliders.
6. Select Signal Reference and test Signal Strength, Luma Noise, Chroma Noise and Tape Errors one by one.
7. Enable Event Glitches and test event rate/mode across multiple frames; a still frame cannot establish motion.
8. Set Master Strength to zero and confirm the output is an identity copy.
