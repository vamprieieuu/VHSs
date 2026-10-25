# VHS6RIFT — Overlay Reference Analysis & Native Integration

This document defines the architecture, blending modes, and native implementation of video damage overlays inside the single VHS6RIFT plugin.

---

## 1. Architectural Strategy: Native Procedural & Layer-Based Overlays

In After Effects, overlays can exist in two forms:
1. **Procedural Native Overlays:**
   - Generated entirely in C++ without requiring external video assets.
   - Includes:
     - `Procedural Transparent Tape Damage`: Organic luma-attenuation band with soft borders and color fringing.
     - `Moving Tape Sweep`: Time-driven periodic disturbance bar sweeping vertically across the raster.
     - `Oxide Dropouts`: Microscopic horizontal dropouts scattered deterministically across scanlines.
2. **Layer-Based Footage Overlays:**
   - Checked out from any composition layer selected via the `Overlay Layer` parameter.
   - Processed with custom temporal remapping (playback speed, time offset, and user-defined loop period).
   - Composited using dedicated After Effects transfer modes.

---

## 2. Supported Blending Modes

| Mode Index | Mode Name | Mathematical Operation | Typical Visual Use |
|---|---|---|---|
| **1** | **Normal** | Direct alpha-weighted replace | Retains opaque black background and solid tape damage |
| **2** | **Add** | $C_{out} = \min(255, C_{src} + C_{ovl} \cdot lpha)$ | Luminous glitches, white tracking flashes, electrical sparks |
| **3** | **Screen** | $C_{out} = 255 - rac{(255 - C_{src})(255 - C_{ovl} \cdot lpha)}{255}$ | Translucent light leaks, tape dust, subtle luma scratches |
| **4** | **Multiply** | $C_{out} = rac{C_{src} \cdot C_{ovl}}{255}$ | Physical tape dirt, dark oxide shedding, emulsion stains |
| **5** | **Blend** | Linear cross-fade: $C_{src} \cdot (1 - lpha) + C_{ovl} \cdot lpha$ | Controlled opacity layering |
| **6** | **Luma Key** | Key out dark background ($Y < 32$), screen-blend bright areas | Transparent playback of stock VHS overlays with black bars |

---

## 3. Temporal Remapping & Seamless Looping

To ensure deterministic playback across scrubbing and final render:
$$	ext{Mapped Time} = \left(	ext{Current Time} \cdot rac{	ext{Speed}}{100.0} + 	ext{Offset}ight) \pmod{	ext{Loop Period}}$$
This guarantees:
1. Scrubbing backward produces the exact same frame at that timestamp.
2. The user can specify any loop duration (e.g. 5.0 seconds for a 5-second tape loop) without seam errors.
3. Layer checkout and checkin are strictly paired using SDK callbacks, ensuring no memory leaks or resource locks.

---

## 4. Frame & Temporal Inspection Notes

Visual inspection across timeline samples (timestamps 00:00:01.00, 00:00:03.15, 00:00:06.00):
- **Moving Tape Sweep:** Translates smoothly downwards/upwards across scanlines without pixel tearing or phase misalignment.
- **Oxide Dropouts:** Scattering exhibits microscopic horizontal micro-streaks localized to 2–6 contiguous pixels.
- **Procedural Transparent Tape Damage:** Attenuates luminance naturally with soft edges and subtle chromatic fringing, preserving background detail beneath the damage zone.
