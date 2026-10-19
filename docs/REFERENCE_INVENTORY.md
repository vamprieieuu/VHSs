# Reference media inventory

Source archive: user-supplied `VHS MASTER.zip` plus stills in `repos.zip` under `more vhs overlays`. Exact included paths and SHA-256 values are in `references/MEDIA_MANIFEST.csv`.

| Supplied area | Use in project | Evidence/limits |
|---|---|---|
| `Vhs cams_/Fxc6` | Fxc6 camera-reference profile | Two short screen recordings; visual traits are reference guidance, not hardware calibration |
| `Vhs cams_/Gen 7` | Generational tape/dub reference | Before/after recordings support luma/chroma degradation observations; not full device calibration |
| `Vhs cams_/Jvc gr-c7u ( 1986)` | Vintage camcorder reference | Includes VHS, capture-code and CRT-TV examples; separate capture look from CRT display look |
| `glitch cam/` and `assets/overlays/glitch-cam/` and `new vhs overlays/glitch cam/` | Discrete glitch/overlay source media | Many assets are exact duplicates across folders; see manifest |
| `new vhs overlays/crt tvs/` | CRT display reference footage/GIFs | Reference media are not proof of a physically complete CRT engine |
| `retro vhs/` and `new vhs overlays/retro vhs/` | Retro VHS overlay/look references | Exact duplicate files are deduplicated |
| `new vhs overlays/text/` | Date, PLAY and text-style reference media | Can be imported as overlay layers; separate native editable PLAY/REC/date/timecode OSD is implemented in C++ |
| `overlays as effects/Model 1`–`Model 5` | Examples of discrete glitch states | Reference only; watermarked compilation is omitted from package |
| `more vhs overlays/` from `repos.zip` | Additional overlay stills | Copied where not duplicate |

Embedded third-party plugin binaries and nested repository copies are excluded from the distributable reference pack.
