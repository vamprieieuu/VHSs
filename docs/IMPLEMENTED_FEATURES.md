# Implemented features in the current source revision

- Native VHS6RIFT effect with unchanged PiPL identity and NTSC-RS C-FFI processing.
- Seven selectable reference-derived camera transfer profiles.
- Saturation, black level, luma softness, chroma bleed, and X/Y chroma offset controls.
- Grain with amount, continuous size, speed, softness, and 4mm/8mm/16mm/32mm/64mm presets; default amount is zero and the module is disabled on load.
- Separate optional colored signal lines with amount, size, and speed controls; colored lines are processed after the monochrome image conversion.
- Tracking and dropout controls, VHS Legend, NTSC-RS, and Signal Reference engines.
- Optional advanced built-in overlays and OSD, each separately gated.
- Audio degradation controls.

Removed from the current UI/render path: Moving Grain as a separate feature, Tape Behavior and tape-sweep systems, the source-footage overlay toggle, and VHS Craft.

Verification performed for this revision: static parameter-registration audit, source wiring audit, and Python tests. Windows/MSVC compilation and After Effects visual testing remain to be performed.
