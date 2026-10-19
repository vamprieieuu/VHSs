# Requirements-to-implementation map — current repair pass

| Requirement | Source path | Status / boundary |
|---|---|---|
| Native effect identity and build files | `src/VHS6RIFT.cpp`, `resources/VHS6RIFTPiPL.r`, `CMakeLists.txt` | Source and identity statically validated; Windows build not run in this environment. |
| 21 camera profiles | `kCameraProfiles[21]`, `P_CAMERA` | Registered; B&W Surveillance VHS is a separate explicit profile. Visual camera-by-camera matching still requires AE renders. |
| Low-strength camera color | `drive`, `sat`, profile transform | Camera color is independent of master drive; NTSC-RS stage now uses strength-scaled blending and is bypassed when its controls are zero. |
| Grain sizes, softness, structure, color | `P_ADV_GRAIN_*`, grain stage in `Render` | Wired in source; temporal/spatial appearance still requires AE visual validation. |
| Lowpass, sharpening, chroma demod | `P_ADV_LOWPASS_TYPE`, `P_ADV_SHARPEN`, `P_ADV_CHROMA_DEMOD` | Wired in source; filter behavior needs perceptual validation in AE. |
| Composite vertical noise and dots | `P_ADV_VERTICAL_NOISE`, `P_ADV_DOT_NOISE`, `P_ADV_NOISE_COLOR` | Procedural, deterministic by time/seed, and opt-in through nonzero sliders. |
| Head-switching controls | `P_ADV_HEADSWITCH_*` | Procedural moving band with position, height, motion, and burst randomness; disabled by default. |
| Glitch Cam | `P_ADV_GLITCH_ENABLE`, `P_GLITCH_CAM_*` | 15 event styles; separate enable switch; normalized event rate/amount/chroma/block size. |
| Built-in OSD | `P_ADV_OSD_*` | Direct 5x7 bitmap rendering; no footage layer required; preset text modes. |
| Built-in overlay library | `P_ADV_BUILTIN_OVERLAY_*` | 21 direct procedural animated styles, opacity, speed, blend, and enable. This is not a copy of the reference clips. |
| External footage overlays | `P_OVERLAY_*` | Still optional and independent of built-in overlays; checkout/loop behavior needs AE host validation. |
| Tape behavior | `P_TAPE_*`, `P_TRANSPARENT_TAPE_*`, `P_TAPE_SWEEP_*` | Strength/size/speed normalization corrected; visual calibration still pending. |
| CRT | `P_CRT_*` | Scanlines, curvature/glass mask, glow, RGB convergence/phosphor mask, flicker, vignette; opt-in. |
| Audio | `AudioRender` and `P_AUDIO_*` | Source path present; audio selector and sync validation not run in AE. |
| Parameter registration | `tools/validate_project.py` | PASS: 192 registered parameters in enum/setup order. |
| Static wiring | `tools/audit_parameter_wiring.py` | PASS: non-structural IDs referenced in render/source. |
| Python smoke/regression tests | `tools/test_*.py` | PASS: 11 Python tests. These are not native-pixel or host-render tests. |
| Native build and AE render | GitHub Actions / After Effects 23.2.1 | NOT RUN here: Cargo unavailable and full licensed SDK headers/PiPLtool are not in this archive. |
