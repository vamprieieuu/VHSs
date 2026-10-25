# VHS6RIFT native feature matrix

This matrix describes the code paths in `src/VHS6RIFT.cpp` and the bundled assets. “Integrated” means the feature has a native C++ processing/control path; it does not mean it has been visually certified inside After Effects in this environment.

| Requested area | Native integration in the single effect | User controls / assets |
|---|---|---|
| NTSC-RS engine | Rust `ntsc-rs` source is vendored intact and invoked through `ntscrs_c` C ABI for the NTSC-RS signal model | Chroma delay, head switching, edge/luma smear, composite noise, strength, deterministic seed/frame |
| VHS base engine | Luma softness, chroma bleed and offset, saturation/black response, profile color/gamma/shoulder, edge ringing, deterministic moving grain | Master Strength (default 1000), continuous VHS treatment, seed, color/grain controls |
| 19 camera/look profiles | Seven supplied VHS-reference profile families plus CPM35, Classic U, IR, FQS, CCDR, FXN, D Half, and five Retro VHS look variants; every selector item maps to its own tuned profile data and its own four controls | Character, color response, signal damage, grain per profile; profile labels are authored emulation looks, not lab-calibrated hardware claims |
| Tracking and sudden damage | Moving tracking band, time-slot glitch envelope, dropouts, block corruption and recovery envelope | Tracking amount/frequency/width, dropouts, block corruption, events per minute, duration, recovery |
| Tape damage | Horizontal band, bottom head-switch, vertical roll, side tear, signal snow; direction, size, strength, speed; separate transparent-tape and periodic sweep passes | Tape type defaults Off; procedural transparent tape and moving sweep default Off |
| Overlay library | 21 native selectable overlay looks plus layer checkout at mapped time, time offset/speed/loop, opacity, six compositing modes | Native presets include Bad Signal Tear, Tracking Bar, Head-Switch Noise, Capture Code, REC HUD, CRT Burn-In, Tape Dust, Sync Loss, Edge Rainbow, Ghosting, and more; import reference footage for source-layer overlay use |
| Native overlay effects | 21 native selectable looks: Bad Signal Tear, Moving Tracking Bar, Head-Switch Noise, RF Interference, Tape Wrinkle, Signal Dropout, Chroma Bleed/Phase Drift, Vertical Roll, Frame Jitter, Line Echo, Capture Code, REC HUD, CRT Burn-In, Tape Dust, Sync Loss, Luma Pump, Edge Rainbow, Ghosting, Bottom Tracking Noise, Analog Line Damage | Strength, speed, event rate, color-channel toggle and blend-in; Off by default |
| Glitch Cam | 15 selectable native procedural Glitch Cam families with time-dependent tearing, channel displacement, scanline jitter, chroma dropout and signal loss; avoids the old random neon-block look | Preset Off by default, strength, motion speed, events per minute, colored-channel toggle; original source assets retained in `assets/overlays/glitch-cam/` and `references/` |
| CRT TV | Coordinate barrel warp with black curved corners, scanline modulation, RGB convergence and subtle phosphor triad, neighbor-sampled phosphor bloom, flicker and edge vignette | CRT disabled by default; independent controls; original CRT reference GIFs retained under `assets/overlays/crt-tv/` and `references/` |
| Capture OSD / Date / PLAY | Embedded bitmap-font renderer for PLAY, REC, editable YYYY-MM-DD date and timeline HH:MM:SS timecode | Toggle each display; opacity, text scale, four-corner position, safe margin; arbitrary custom text remains available via source-layer overlay |
| Audio | Native AE audio-render callback with wet/dry mix, event-based dropout/crackle, soft saturation and bit-depth reduction | Enable and event toggles, event amount, crackle, dropout, saturation, lo-fi quantization |
| Reference asset pack | User-provided VHS Master and additional overlay reference files are retained and inventoried | VHS cameras, Retro VHS, Glitch Cam, CRT TV, text/date/PLAY, capture-code and other overlay assets |

## Default-state intent

Applying the effect starts with the core VHS camera/signal treatment. The secondary engine selector and tape type both remain genuinely Off unless explicitly selected. Glitch Cam, built-in overlay selector (Off by default), CRT, source-layer overlay, transparent-tape damage and tape sweep and OSD are opt-in. The base VHS camera profile may contribute its subtle profile-dependent noise floor; the dedicated grain amount defaults to zero. The 21 native overlay selector and 15 Glitch Cam selector are both Off by default.

## Evidence status

- `python tools/validate_project.py --static-only`: checks parameter registration order, 19 profile routing, 15 Glitch Cam choices, 21 native overlay choices, OSD controls, PiPL identity/version/outflags and default-Off logic.
- `g++ -std=c++17 -fsyntax-only ... tools/test_native_render.cpp`: C++ syntax check against the supplied SDK headers and test shim.
- `tools/test_native_render.cpp`: native render harness now exercises all 21 built-in overlays against a matched Off baseline. The ntsc-rs integration test requires the real Rust static library; a local identity stub is not a valid substitute for that test.
- `python tools/test_vhs6rift.py`: lightweight Python model checks; not a native renderer test. The C++ harness covers 15 test groups, including 21 overlay choices and native audio DSP, but its local test run used a temporary NTSC bridge stub.
- Native MSVC/Rust build and visual testing in AE 23.2.1 remain unverified in this Linux environment. A local C++ syntax check passed; a no-op Rust bridge test intentionally fails only the NTSC-RS integration assertion.
