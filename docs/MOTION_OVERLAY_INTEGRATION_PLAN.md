# VHS6RIFT — Motion overlay integration (reference-backed)

This file is a source-level implementation checklist derived from the supplied `VHS MASTER.zip` and repo bundle. It deliberately separates what is present from what still requires a native-code change and AE verification.

## Asset groups verified in the reference archive

- `new vhs overlays/crt tvs/`: `crt 1.gif`, `crt2.gif`; these are moving CRT/display references, not static scanline presets.
- `retro vhs/` and `new vhs overlays/retro vhs/`: four unique designs by content, with repeated files across folders.
- `new vhs overlays/text/`: date, play/OSD, and text references; some reuse the numbered glitch assets.
- `overlays as effects/`: five short model videos and one long compilation. The long compilation has a visible stock watermark and is reference-only; it must not be shipped as a bundled asset.
- `more vhs overlays/` in the repo upload: additional still JPGs, intended as designs to animate procedurally rather than simply freeze as static overlays.

## Motion strategy by reference type

1. **GIF/video sources:** preserve their temporal behavior by using a time-remapped AE footage layer, with deterministic time mapping and an explicit loop duration. Do not convert them to one still frame. The existing `Overlay Layer (optional)` controls provide the user-supplied footage path.
2. **Still glitch frames:** build a procedural animated variant that uses the still as a visual guide: short event bursts, horizontal slice displacement, variable-height bands, chroma separation, and alpha/opacity envelope. Event timing must be deterministic from composition time + seed so scrubbing and renders match.
3. **Still text/OSD:** use stable glyph/text geometry with controlled flicker, slight horizontal jitter, and optional dropout; avoid randomly changing the readable characters every frame.
4. **CRT references:** separate CRT screen curvature/scanline/glow treatment from CRT *content overlay* animation. They are different stages and need independent toggles.
5. **VHS damage overlays:** preserve source black when using Normal mode; Screen/Add/Luma Key are optional interpretations, never silently applied to all assets. Black-background source videos are opaque unless explicitly keyed.

## Current source observations

- `src/VHS6RIFT.cpp` already has an optional user-selected overlay layer with opacity, speed, offset, loop toggle, and loop period.
- It also has procedural CRT and tape sweep stages, but source-level presence is not proof of visual correctness in After Effects.
- The popup is `Normal|Blend|Add|Screen|Multiply|Luma Key`. The prior `Blend` implementation was a contrast-centered additive adjustment, not an alpha-weighted crossfade. It has been corrected to the expected linear crossfade in this revision.
- Built-in reference assets are not currently loaded from the ZIP automatically by the native effect. The still-to-motion engines above are follow-up implementation work; this document does not claim they already exist.

## Verification still required

- Build with Windows MSVC + the target AE SDK, then load into AE 23.2.1.
- Render matched footage at multiple timestamps for each overlay mode and compare against source references.
- Test scrubbing, frame-rate changes, loop boundaries, source layer end, alpha, and opaque black behavior.
- Verify that disabling overlays and CRT leaves the base NTSC/VHS processing unchanged.
