#!/usr/bin/env python3
"""Static regression checks for the embedded VHS6RIFT overlay asset selector."""
from pathlib import Path
import json

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "src" / "VHS6RIFT.cpp").read_text(encoding="utf-8")
header = (ROOT / "src" / "BundledOverlayFrames.h").read_text(encoding="utf-8")
manifest = json.loads((ROOT / "assets" / "overlay-library" / "embedded-assets-manifest.json").read_text(encoding="utf-8"))
assert manifest["embedded_overlay_count"] == 21
assert manifest["frame_samples_per_asset"] == 10
assert manifest["embedded_preview_size"] == [160, 90]
assert len(manifest["assets"]) == 21
assert "PF_ADD_POPUP(\"Built-in Overlay Asset\", 22, 1" in source
assert "P_BUNDLED_OVERLAY_STYLE" in source and "P_BUNDLED_OVERLAY_OPACITY" in source and "P_BUNDLED_OVERLAY_SPEED" in source
assert "kBundledOverlayRGB565[bundledOverlayStyle][bundledOverlayFrame][sy][sx]" in source
assert "kBundledOverlayRGB565[21][10][90][160]" in header
assert "Off|Glitch 01|" in source and "Retro 01" in source and "CRT TV 02" in source
assert len(list((ROOT / "assets" / "overlay-library" / "glitch-cam").glob("*"))) >= 15
print("PASS: 21 bundled assets are embedded in the source as ten RGB565 frame samples each.")
print("PASS: the effect exposes Off + 21 selectable assets with opacity and playback-speed controls.")
print("PASS: source includes the in-render overlay compositing path; no external file path is needed.")
print("NOTE: embedded playback samples are 160x90; full-resolution originals remain in assets/overlay-library.")
