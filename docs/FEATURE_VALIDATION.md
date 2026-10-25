# Feature Validation — Current Repair Pass

| Check | Result |
|---|---|
| Parameter enum/registration order | PASS — 135 registered parameters |
| Camera Engine selector removed | PASS — no camera-profile selector or profile bank in source |
| NTSC-RS base engine and dedicated enable | PASS — static wiring only |
| Independent Craft / Legend-style / Signal Reference enables | PASS — static wiring only |
| Moving grain interpolation and temporal seed | PASS — static wiring only |
| Source-footage overlay checkbox; no layer input | PASS — static wiring only |
| Tape behavior models and strength gate | PASS — static wiring only |
| CRT TV system removal | PASS — no CRT TV system controls remain |
| Python tests | PASS — 13 tests |
| Windows/MSVC + Cargo native build | NOT RUN — required toolchain/SDK unavailable here |
| After Effects 23.2.1 visual/runtime test | NOT RUN |

Static success must not be described as proof of visual fidelity or a working installed AEX.
