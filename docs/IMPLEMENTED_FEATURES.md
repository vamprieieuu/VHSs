# VHS Master — Implemented Features Catalog

**VHS Master** includes complete native image-processing and audio degradation algorithms connected directly to 112 parameter slots across 7 structured UI groups.

---

## 1. Camera Engines Group
1. **Fxc6 Reference Profile:** Softened luma response with subtle warm highlight lift and balanced RF edge characteristics.
2. **Gen 7 Reference Profile:** Heavy generational tape dub emulation with lifted shadow pedestal, luma bandwidth limiting, and muted contrast.
3. **JVC GR-C7U (1986):** Saticon tube pickup simulation featuring highlight bloom shoulder, high warm saturation, and optical tube grain.
4. **VHS-R20 Reference Profile:** Standard home VCR SP mode with characteristic horizontal luma softness and trailing edge response.
5. **Fall-78 Reference Profile:** Late 1970s color film transfer aesthetic with golden amber highlights and teal-biased shadows.
6. **ERC-V2S Reference Profile:** Broadcast Betacam / U-matic SP emulation with neutral color fidelity and studio pedestal levels.
7. **JVC GR-SXM740U Profile:** Super VHS-C camcorder profile with expanded luma bandwidth and crisp edge response.

---

## 2. VHS Degradation Engine
* **Processing Modes:** SP Standard, LP Long Play, EP Extended Play, Hi-Fi Commercial Master.
* **Luma & Chroma Separation:** Independent YUV matrix processing with horizontal chroma displacement (bleed) and optical softness.
* **Tape Transport Simulation:** Horizontal tape band displacement, phase jitter, micro-wrinkles, and magnetic oxide dropouts.

---

## 3. Bad Signal / Glitch Engine
* **Procedural Signal Failures:** Seeded time-deterministic horizontal line tearing, channel displacement bursts, and block corruption.
* **Temporal Stability:** Driven strictly by composition time / frame timestamp and deterministic random seeds (`PF_ParamDef` seeds).
* **Glitch Controls:** Frequency, Duration, Intensity, Variation, Recovery Speed, Tearing, Band Disruption, Tracking Fail, Chroma Instability, Burst Flicker.

---

## 4. CRT TV Engine
* **CRT Presets:** Trinitron CRT, Consumer TV, JVC Studio Monitor.
* **Display Artifacts:** Spatial scanlines, phosphor glow/bloom, barrel curvature geometry warp, RGB convergence channel offset, 60Hz temporal flicker, and edge vignette.

---

## 5. Grain Engine
* **Algorithmic Generation:** Pseudo-random 2D spatial-temporal hash generator without static image overlays.
* **Grain Modes:** Monochrome, Color Organic, RF Hiss Noise.
* **Tonal Weighting:** Independent noise response controls for Shadows, Midtones, and Highlights.

---

## 6. Dates & Text OSD
* **Typography:** Built-in 5x7 ASCII bitmap font renderer supporting camcorder OSD aesthetics.
* **Editable Fields:** Year (1970–2030), Month, Day, Hour, Minute.
* **Preset Strings:** `PLAY`, `REC ●`, `PAUSE`, `AUTO SP`, `STOP`, `VHS MASTER`.
* **Transform Controls:** Position X/Y (0–100%), Text Scale (50–300%), Text Opacity (0–100%).

---

## 7. Advanced Overlays & Audio Engine
* **Overlay Layer Support:** Native After Effects layer checkout (`PF_CHECKOUT_PARAM`) with Normal, Blend, Add, Screen, Multiply, and Luma Key modes.
* **Audio Glitch Processing:** Native AE audio selectors (`PF_Cmd_AUDIO_RENDER`) with sample-domain `std::tanh` saturation, bit-depth quantization, crackle static, and dropouts.
