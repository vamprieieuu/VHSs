# VHS6RIFT — verified camera selector inventory

The native selector now exposes **19 independently parameterized look profiles**, backed by `PF_ADD_POPUP` and `kCameraProfiles[19]` in `src/VHS6RIFT.cpp`. The seven original VHS reference groups are retained. Additional authored profiles include CPM35, Classic U, IR, FQS, CCDR, FXN, D Half, and five Retro VHS variants. These additional profiles are authored emulation looks informed by the available reference pack; they are not claims of measured hardware calibration.

| Selector | Grounded reference | Distinction implemented in profile data |
|---:|---|---|
| 1 | Fxc6 Reference | Consumer recording chain; moderate chroma lift and soft signal contribution |
| 2 | Gen 7 Reference | Multi-generation dub; lower saturation, stronger contrast/pedestal and signal texture |
| 3 | JVC GR-C7U (1986) | Warm, saturated vintage profile with lifted shoulder, chroma displacement and ringing |
| 4 | VHS-R20 Reference | Cooler, lower-saturation/security-deck-like tonal response and stronger edge/noise character |
| 5 | Fall-78 Reference | Soft, warm vintage transfer with lifted shadows and low native noise floor |
| 6 | ERC-V2S Reference | More neutral/professional reference response with reduced noise contribution |
| 7 | JVC GR-SXM740U | Compact VHS-C/S-VHS-ET inspired response with cooler tint and stronger edge response |

## What this proves—and what it does not

The profile selector and per-profile parameter groups are present in the C++ source. The Python model test checks 19 distinct illustrative profile outputs, but does **not** establish that After Effects produces the same results. Real same-frame comparison renders in AE remain outstanding.

## Source inventory

The supplied `VHS MASTER.zip` has three top-level folders under `Vhs cams_`: `Fxc6`, `Gen 7`, and `Jvc gr-c7u ( 1986)`. Other labels used by the code are reference/preset labels in the existing project, not a claim that matching physical-camera footage exists for every selector.
