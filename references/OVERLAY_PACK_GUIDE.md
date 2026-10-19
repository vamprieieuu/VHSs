# VHS6RIFT reference overlay pack

The `VHS_MASTER_MEDIA` folder preserves the supplied reference folder names where possible, including `new vhs overlays`, `glitch cam`, `retro vhs`, `crt tvs`, `text`, `Vhs cams_`, `Capture codes_`, and `Crt-Tv_`. The `MORE_VHS_OVERLAYS` folder contains extra still references found in `repos.zip`. Exact duplicate files are represented in `MEDIA_MANIFEST.csv` and point to a canonical copy so the package does not waste space storing identical files multiple times.

## Use in After Effects

1. Import the GIF/MOV/MP4/PNG/JPG asset you want into the AE Project panel.
2. Place it in the composition as a layer.
3. Apply `VHS6RIFT` to the footage layer you want to process.
4. In `Overlay Source Layer`, choose the imported overlay layer.
5. Enable overlay and set `Overlay Opacity`, `Overlay Speed`, `Overlay Time Offset`, `Loop Overlay`, `Loop Period`, and `Overlay Blend Mode`.
6. Use the dedicated CRT controls for the CRT engine; do not use a CRT footage overlay as a substitute if you want the native CRT processing.

The native overlay path supports the available layer source and blend modes. This archive is a reference-media pack, not a promise that every GIF is auto-loaded as a built-in preset in the effect's popup.
