"""Static source regressions for the 2026-10-19 VHS6RIFT repair pass.
These tests validate wiring/text patterns, not compiled pixels or AE runtime behavior.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC = (ROOT / "src" / "VHS6RIFT.cpp").read_text(encoding="utf-8")


def test_21_camera_profiles_and_bw_isolation():
    assert 'PF_ADD_POPUP("CAMERA ENGINE | Profile", 21, 1' in SRC
    assert 'kCameraProfiles[21]' in SRC
    assert 'B&W Surveillance VHS' in SRC
    assert 'const float sat = (ci == 20 ? 0.0f' in SRC


def test_low_master_ntsc_is_scaled_and_controls_default_off():
    assert 'PF_ADD_FLOAT_SLIDERX("NTSC Chroma Delay", 0, 100, 0, 100, 0' in SRC
    assert 'baseNtscParams.chroma_delay = getSlider(params, P_NTSC_CHROMA_DELAY, 1.0f)' in SRC
    assert 'hasNtscControls' in SRC
    assert 'std::min(10.0f, std::max(0.0f, strength / 3.5f))' in SRC


def test_grain_is_true_bypass_and_time_varying():
    assert 'const float grainAmount = grainEnabled ? clamp01(s.grain * (cameraGrain / 28.0f)) : 0.0f;' in SRC
    assert 'motionFrame * 2246822519u' in SRC
    assert 'Grain Size' in SRC and '64mm (Very Fine)' in SRC
    assert 'PF_ADD_POPUP("Grain Size", 5, 3' in SRC
    assert 'PF_ADD_POPUP("Lowpass Filter Type", 2, 1' in SRC
    assert 'Grain Softness' in SRC and 'Grain Manual Red' in SRC


def test_signal_controls_are_not_double_normalized():
    assert 'const float glitchCamAmount = getSlider(params, P_GLITCH_CAM_AMOUNT, 100.0f);' in SRC
    assert 'const int glitchCamChroma = std::max(0, (int)getSlider(params, P_GLITCH_CAM_CHROMA, 1.0f));' in SRC
    assert 'const int glitchCamBlockSize = std::max(2, (int)getSlider(params, P_GLITCH_CAM_BLOCKS, 1.0f));' in SRC
    assert 'const float effTapeStrength = tapeEnabled ? tapeStrengthRaw : 0.0f;' in SRC
    assert '(effTapeStrength / 100.0f)' not in SRC


def test_glitch_cam_requires_its_own_enable_toggle():
    assert 'PF_ADD_CHECKBOX("Enable Glitch Cam Events", "Enable", FALSE' in SRC
    assert 'if (chosenEvent && glitchCamEnabled)' in SRC


def test_advanced_signal_filters_are_present_and_wired():
    for control in [
        'Lowpass Filter Type', 'Composite Signal Sharpen', 'Chroma Demodulation Filter',
        'Vertical Composite Noise', 'Small Circular Dot Noise', 'Head-Switch Position',
        'Head-Switch Height', 'Head-Switch Motion', 'Head-Switch Random Bursts',
    ]:
        assert control in SRC
    assert 'if (compositeSharpen > 0.001f' in SRC
    assert 'if (s.continuous && headSwitchHeight > 0.001f)' in SRC


def test_builtin_osd_and_overlay_library_are_direct_render_stages():
    assert 'PF_ADD_CHECKBOX("Built-in OSD / Text Overlay", "Enable", FALSE' in SRC
    assert 'PF_ADD_CHECKBOX("Built-in Overlay Library", "Enable", FALSE' in SRC
    assert 'PF_ADD_POPUP("Built-in Overlay Style", 21, 1' in SRC
    assert 'Built-in Overlay Library: 21 procedural, time-driven signal overlays.' in SRC
    assert 'if (osdEnabled && osdOpacity > 0.001f' in SRC
    assert 'OSD Position X (%)' in SRC and 'OSD Text Scale (%)' in SRC
    assert 'Glitch Cam Color Palette' in SRC and 'Glitch Cam Blend-in' in SRC
