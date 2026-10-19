# Visual validation status

No After Effects render was produced from this fresh repository during packaging. Previously bundled preview claims were not copied into this new repository as proof.

| Check | Status |
|---|---|
| Python model test suite | PASS (four model tests) |
| C++ syntax-only check | PASS (local g++ / SDK shim) |
| Native C++ render harness with temporary NTSC stub | PASS (15/15 groups; does not validate real Rust engine) |
| Real NTSC-RS bridge execution | NOT TESTED locally |
| 251 parameter registration and PiPL static checks | PASS |
| Same source frame through each camera profile in AE | NOT TESTED |
| VHS low/medium/high perceptual comparison in AE | NOT TESTED |
| CRT on/off passthrough and geometry comparison in AE | NOT TESTED |
| Overlay alpha/blend/time/loop comparison in AE | NOT TESTED |
| Native audio export and sync | NOT TESTED |
| AE 23.2.1 load/stability | NOT TESTED |


Current source additions include 19 camera profiles, 15 procedural Glitch Cam variants plus 21 built-in native overlay effects, and editable PLAY/REC/date/timecode OSD. These features have source/static inventory checks but still need actual AE visual verification on Windows.
