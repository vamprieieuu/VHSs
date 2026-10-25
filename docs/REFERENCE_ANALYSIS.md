# VHS6RIFT — Reference Video & Effect Family Analysis

This document provides systematic visual analysis, temporal behavior documentation, parameter mapping, and native C++ implementation notes for the VHS reference materials and model families.

---

## 1. Inventory of Reference Families & Effect Models

| Reference Family / ID | Duration & Cadence | Effect Family | Distinctive Visual Characteristics | Implemented Controls | Implementation Architecture |
|---|---|---|---|---|---|
| **Model 1** | 29.97 fps NTSC | Heavy Analog Glitch | Aggressive horizontal Y/C delay, tape line sync tear, boosted luma transients, RF head switching | `Model Selection: Model 1`, `Model Intensity`, `Model Blend` | Luma overshoot calculation, chroma carrier rotation, high-amplitude sync line displacement in `Render()` |
| **Model 2** | 25.00 fps PAL | Muted Analog Tape | Warm golden black level, reduced dynamic contrast, gentle high-frequency softness, low-density tape hiss | `Model Selection: Model 2`, `Model Intensity`, `Model Blend` | Shadow lift (+10 Luma), gentle low-pass luma filter, subtle chroma desaturation, fine noise blending |
| **Model 3** | 29.97 fps NTSC | Chroma Bleed & Roll | Pronounced horizontal red/cyan fringe bleeding, slow vertical tracking drift, periodic micro-jitter | `Model Selection: Model 3`, `Model Intensity`, `Model Blend` | Chroma horizontal low-pass filter, phase displacement matrix, vertical roll wrapping |
| **Model 4** | 29.97 fps NTSC | High Dynamic Bloom & Edge Tear | Highlight blooming shoulder, CRT phosphor glow, top-of-raster side tearing | `Model Selection: Model 4`, `Model Intensity`, `Model Blend` | Highlight knee transfer function, non-linear phosphor bloom, top 15% sinusoidal side-tear displacement |
| **Model 5** | 29.97 fps NTSC | Degraded Low-Band Composite | Severe horizontal resolution decimation, composite comb filter cross-color artifacts, oxide streaks | `Model Selection: Model 5`, `Model Intensity`, `Model Blend` | Multi-pixel luma/chroma quantizer, oxide dropout generator, comb filtering simulation |

---

## 2. Additional VHS Modes Analysis

Based on the multi-mode demonstration footage and NTSC/PAL video signal standards:

1. **Standard VHS SP Mode:**
   - Standard 240 TV lines horizontal resolution.
   - Distinctive horizontal luma softness combined with 3-4 pixel chroma delay.
   - Modest head-switching noise in bottom 4% of active video raster.
   - Native controls: `Main VHS Engine` with `Signal Degradation` = 25%, `Luma Softness` = 15%, `Chroma Offset X` = 2.0.

2. **S-VHS Hi-Res Mode:**
   - Approximately 400 TV lines resolution.
   - Separate Y/C recording eliminates cross-color and composite dot crawl.
   - Crisp edge response with subtle high-frequency FM carrier noise.
   - Native controls: `VHS Mode: S-VHS Hi-Res`, increased edge sharpening, reduced chroma bleed.

3. **LP/EP Long Play Tape Mode:**
   - Slower tape speed reduces track width and SNR.
   - Prominent tracking instability, increased tape grain, and periodic oxide dropouts.
   - Native controls: `VHS Mode: LP/EP Long Play`, elevated `Tracking Band Strength` and `Signal Dropouts`.

4. **Hi-Fi Commercial Master Mode:**
   - High SNR master recording with deep blacks and rich analog saturation.
   - Clean signal with subtle warm analog tape compression.
   - Native controls: `VHS Mode: Hi-Fi Commercial Master`, gentle contrast boost, minimal jitter.

5. **NTSC-RS RF Demodulation Mode:**
   - Emulates RF tuner intermediate frequency bandpass demodulation.
   - High-contrast edge ringing, carrier phase drift, and color fringing.
   - Native controls: `VHS Mode: NTSC-RS RF Demodulation`, `Edge Response` overshoot and `Analog Color Shift`.

6. **Broadcast Head Misalign Mode:**
   - Emulates misaligned playback video head azimuth.
   - Top-edge tearing, vertical rolling slip, and bottom head-switch line jumps.
   - Native controls: `Tracking Damage` with `Side-Edge Tearing`, `Vertical Roll`, and `Head-Switching Noise`.

---

## 3. Tape Effect Characteristics

- **Horizontal Tape Band:** An irregular, time-dependent disturbance band that translates vertically across the raster. Implemented via deterministic temporal progress `std::fmod(timeSec * speed, interval)` displacing actual source row samples horizontally rather than superimposing a static solid line.
- **Oxide Dropouts:** Microscopic losses of magnetic coating causing instantaneous white/grey horizontal streaks across 2–6 adjacent pixels. Implemented procedurally via 2D spatial-temporal hash probability.
- **Wrinkle Damage:** Physical tape creases that produce phase jitter and localized vertical luma distortion.
- **Procedural Transparent Tape:** Attenuates underlying luminance while preserving picture information with organic color fringing.

---

## 4. Verification Boundaries & Approximations

- **Verified:** The native C++ algorithm implementations in `src/VHS6RIFT.cpp` faithfully reproduce the observed visual properties, temporal dynamics, and control mappings.
- **Approximations:** Because physical magnetic tape decks and CRT phosphors exhibit non-linear continuous physical phenomena, our native CPU implementation uses digital DSP approximations (YIQ matrix operations, polynomial curve fitting, and deterministic pseudo-random hash functions).
