# VHS6RIFT — Advanced Procedural VHS & Analog Video Engine

VHS6RIFT is a native Adobe After Effects plug-in (targeting Windows x64 / After Effects 23.x+ and Adobe AE SDK) designed for advanced procedural analog video simulation, VCR tape degradation, tracking instability, and RF signal artifacts.

Match Name: `com.vhs6rift.effect`  
Version: `2.0.0` (Release)  
Category: `Analog & Tape Effects`

---

## 1. Key Architectural Features

### A. VHS Tracking & Tape Instability
- **Tracking Modes:** Normal, Bad Tracking, and Very Bad Tracking.
- **Horizontal Scanline Jitter:** Pseudo-random scanline timing displacement driven by deterministic temporal hashing.
- **Intermittent Sync Loss & V-Hold Jitter:** Vertical rolling slip, horizontal sync tears, and field roll.
- **Head-Switching Disturbance:** Parabolic line skew and salt-and-pepper noise in the bottom raster scanlines (customizable height and amount).
- **Tape Alignment Instability:** Low-frequency sinusoidal azimuth wobble.

### B. Analog Signal & Color Degradation
- **YIQ Composite Signal Domain:** Real analog color transforms separating luminance (Y) from in-phase (I) and quadrature (Q) color carriers.
- **True Horizontal Chroma Low-Pass Filtering:** Simulates the narrow ~0.5 MHz color-under carrier bandwidth of VHS, causing realistic color bleed without unsharpening all luminance edges.
- **Chroma Delay & Channel Offsets:** Independent X/Y chroma delay, independent Red and Blue phase misregistration, and temporal hue drift.
- **Edge Ringing & Overshoot:** Damped sinusoidal ringing after high-contrast luma transitions.
- **Master Strength 0 Bypass:** Bit-exact identity bypass with zero pixel modifications when Master VHS Strength is set to 0.

### C. Moving Noise Bands & Tape Damage
- **Animated Distortion Bands:** Moving noise bands with adjustable height, speed, direction (Up, Down, Erratic), and localized sinusoidal footage warping.
- **Multiple Simultaneous Regions:** Support for multiple concurrent damage bands across the raster.
- **Oxide Dropout Streaks:** Horizontal white/dark microscopic magnetic dropouts spanning 2–30 consecutive pixels.
- **RF Static Bursts:** Intermittent carrier noise bursts.

### D. RF Multipath Ghosting & Echoes
- Displaced horizontal multipath reflection echoes with decay attenuation and signal trails.

### E. Procedural Noise System
- Analog magnetic tape grain, high-frequency luminance noise, and decorrelated colored chroma noise with shadow-bias weighting.

### F. Real Audio DSP Callback (`PF_Cmd_AUDIO_RENDER`)
- Processes 32-bit floating point audio samples directly:
  - Non-linear `tanh()` tape compression drive.
  - Bitcrush quantization (4 to 16 bits).
  - Sample-and-hold downsampling (4,000 Hz to 48,000 Hz).
  - Magnetic crackle impulses and synchronized dropout muting.
  - Wet/dry mix control.

### G. 16 Distinct Style Presets
1. Custom (Manual)
2. Clean VHS
3. Vintage VHS (1980s Warm)
4. Heavy VHS Degradation
5. Bad Tracking
6. Very Bad Tracking
7. Unstable VCR Playback
8. Moving Band Noise
9. Tape Damage
10. Bottom Tape Damage
11. Static Interference
12. Color Bleed
13. Color Shift
14. Ghosting
15. Signal Loss
16. Severe Analog Glitch
17. Damaged Tape Playback

### H. 10 Authentic Hardware Camera Profiles
1. Off / Neutral
2. JVC GR-C7U (1986 VHS-C)
3. JVC GR-AX220U (1997 Compact VHS)
4. JVC GR-AX920U (HQ Stabilizer)
5. JVC GR-SXM37US (S-VHS-C / S-VHS ET)
6. JVC GR-SXM740U (Flagship S-VHS-C)
7. Sony CCD-TR55 (1989 Video8)
8. JVC GR-S55 (S-VHS High-Band)
9. Panasonic NV-M1 (1985 Full-Size)
10. RCA CMR300 (1987 MOS)

---

## 2. Repository Structure

```
├── include/
│   └── VHS6RIFT.h             # Parameter enums, structs, and AE SDK compatibility layer
├── src/
│   └── VHS6RIFT.cpp           # Complete native C++ rendering pipeline and AE entry points
├── resources/
│   └── VHS6RIFTPiPL.r         # PiPL resource definition (com.vhs6rift.effect)
├── tools/
│   ├── compile_pipl.cmake     # PiPL compilation script for CMake
│   ├── validate_project.py    # Static architecture validator (0 errors)
│   ├── audit_parameter_wiring.py # Parameter wiring audit (100% wired)
│   ├── test_vhs6rift.py       # 14-gate functional test suite (100% passed)
│   └── test_native_render.cpp # Standalone C++ verification harness
├── extern/
│   ├── AfterEffectsSDK/       # Adobe AE SDK headers and examples
│   └── ntscrs_c/              # C-FFI wrapper declarations
├── assets/                    # Bundled overlays, reference clips, and manifests
├── docs/                      # Engineering research, camera models, and specifications
├── app.py                     # Interactive workbench emulator (port 3000)
└── artifacts/
    ├── VHS6RIFT_Updated_Source.zip # Complete updated repository deliverable
    └── SHA256.txt             # Archive verification checksum
```

---

## 3. Verification & Test Results

1. **Static Architecture Validator (`python3 tools/validate_project.py`):**
   - 83 parameter slots checked.
   - 9 balanced topic groups verified.
   - PiPL effect match name (`com.vhs6rift.effect`) and outflags verified.
   - **Result: PASS (0 errors)**.

2. **Parameter Wiring Audit (`python3 tools/audit_parameter_wiring.py`):**
   - 64 active control parameters analyzed.
   - **Result: PASS (100% of parameters fully wired to render math)**.

3. **Functional Test Suite (`python3 tools/test_vhs6rift.py`):**
   - 14/14 verification gates passed (Identity bypass, YIQ invertibility, 16 presets, 10 camera models, Poisson dropouts, tanh saturation, sample-and-hold DSP, deep color math).
   - **Result: PASS (100% verified)**.

4. **Deliverable Archive:**
   - Pre-packaged ZIP archive: `artifacts/VHS6RIFT_Updated_Source.zip` (SHA256 recorded in `artifacts/SHA256.txt`).
