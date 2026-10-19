"""Static regressions for the VHS6RIFT signal-engine redesign.
These checks validate source wiring; they are not a substitute for a native build or AE visual test.
"""
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
SRC = (ROOT / "src" / "VHS6RIFT.cpp").read_text(encoding="utf-8")


def test_camera_selector_removed_and_core_vhs_preserved():
    assert "CAMERA ENGINE" not in SRC
    assert "P_CAMERA" not in SRC
    assert "kVhsSignalProfile" in SRC
    assert "ntscrs_process_argb8" in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("VHS Strength", 0, 2000, 0, 2000, 350' in SRC


def test_real_ntsc_rs_is_active_and_independently_switchable():
    assert 'PF_ADD_CHECKBOX("Enable NTSC-RS Signal Engine", "Enable", TRUE' in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("NTSC Chroma Delay", 0, 100, 0, 100, 18' in SRC
    assert 'baseNtscParams.chroma_delay = getSlider(params, P_NTSC_CHROMA_DELAY, 1.0f)' in SRC
    assert 'hasNtscControls' in SRC
    assert 'std::min(10.0f, std::max(0.0f, strength / 3.5f))' in SRC


def test_grain_is_temporal_smooth_and_visible():
    assert 'const float grainAmount = grainEnabled ? clamp01(s.grain) : 0.0f;' in SRC
    assert 'const float rawN = (rand01(pixelHash) - 0.5f);' in SRC
    assert 'PF_ADD_CHECKBOX("Enable Grain / Noise", "Enable", FALSE' in SRC
    assert 'motionFrame * 2246822519u' in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("Colored Grain Amount", 0, 100, 0, 100, 0' in SRC
    assert 'Grain Size' in SRC and '64mm (Very Fine)' in SRC


def test_signal_engines_have_independent_enable_gates():
    assert 'PF_ADD_CHECKBOX("Enable VHS Craft Engine", "Enable", FALSE' in SRC
    assert 'PF_ADD_CHECKBOX("Enable VHS Legend Engine", "Enable", FALSE' in SRC
    assert 'PF_ADD_CHECKBOX("Enable Signal Reference Engine", "Enable", FALSE' in SRC
    assert 'if (craftEngineEnabled)' in SRC
    assert 'if (legendEngineEnabled)' in SRC
    assert 'if (signalReferenceEnabled)' in SRC
    assert 'Other VHS Engine"' not in SRC


def test_overlay_is_checkbox_not_external_layer_selector():
    assert 'PF_ADD_CHECKBOX("Enable VHS Overlay on Source Footage", "Enable", FALSE' in SRC
    assert 'PF_ADD_LAYER' not in SRC
    assert 'overlayCheckedOut' not in SRC
    assert 'const bool builtinOverlayEnabled = overlayEnabled;' in SRC
    assert 'PF_ADD_POPUP("Built-in Overlay Style", 21, 1' in SRC


def test_tape_behavior_is_editable_and_not_forced_on_at_load():
    assert 'PF_ADD_POPUP("Tape Behavior Model", 6, 1' in SRC
    assert 'const float effTapeStrength = tapeEnabled ? tapeStrengthRaw : 0.0f;' in SRC
    assert 'if (tapeType > 0 && tapeType != 2 && effTapeStrength > 0.001f)' in SRC


def test_glitch_cam_has_own_toggle_and_visible_event_strength():
    assert 'PF_ADD_CHECKBOX("Enable Glitch Cam Events", "Enable", FALSE' in SRC
    assert 'if (chosenEvent && glitchCamEnabled)' in SRC
    assert 'const bool chosenEvent = effectiveEventRate > 0.0f' in SRC
    assert 'const float amount = clamp01(glitchCamAmount * eventEnv * (0.75f + 0.25f * drive));' in SRC


def test_crt_tv_system_removed_as_requested():
    assert 'P_CRT_ENABLE' not in SRC
    assert 'CRT TV Engine' not in SRC


def test_advanced_filters_osd_and_procedural_overlays_remain_wired():
    for control in [
        'Lowpass Filter Type', 'Composite Signal Sharpen', 'Chroma Demodulation Filter',
        'Vertical Composite Noise', 'Small Circular Dot Noise', 'Head-Switch Position',
        'Head-Switch Height', 'Head-Switch Motion', 'Head-Switch Random Bursts',
        'Built-in OSD / Text Overlay', 'Built-in Overlay Style', 'OSD Position X (%)',
        'OSD Text Scale (%)', 'Glitch Cam Color Palette', 'Glitch Cam Blend-in'
    ]:
        assert control in SRC
    assert 'if (osdEnabled && osdOpacity > 0.001f' in SRC
    assert 'if (builtinOverlayEnabled && builtinOverlayOpacity > 0.001f)' in SRC


def test_native_ntsc_tracking_and_grayscale_controls_are_wired():
    assert 'baseNtscParams.tracking_noise = getCheck(params, P_TRACKING_ENABLE)' in SRC
    assert 'effect.tracking_noise.enabled = true;' in (ROOT / 'extern/ntscrs_c/src/lib.rs').read_text(encoding='utf-8')
    assert 'if (sat <= 0.0001f)' in SRC
    assert 'const float blackOffset = s.black * 0.0035f * drive;' in SRC
    assert 'const float totalChromaX = s.chromaX * drive;' in SRC
    assert 'const float totalChromaY = s.chromaY * drive;' in SRC


def test_glitch_cam_enable_is_inside_its_section_and_native_head_switch_is_used():
    glitch_topic = SRC.index('PF_ADD_TOPIC("GLITCH CAM - Event Style and Timing"')
    advanced_topic = SRC.index('PF_ADD_TOPIC("ADVANCED - Grain, Signal Filters and OSD"')
    assert glitch_topic < SRC.index('PF_ADD_CHECKBOX("Enable Glitch Cam Events"') < advanced_topic
    assert 'requestedTapeModel == 2' in SRC
    assert 'baseNtscParams.head_switching = std::max' in SRC
