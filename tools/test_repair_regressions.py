"""Static regressions for the requested VHS6RIFT cleanup and engine controls.
These checks validate source wiring; they are not a substitute for a native build or AE visual test.
"""
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
SRC = (ROOT / "src" / "VHS6RIFT.cpp").read_text(encoding="utf-8")


def test_camera_profiles_are_restored_from_reference_analysis():
    assert 'PF_ADD_POPUP("VHS Camera Profile", 7, 1' in SRC
    assert "kVhsSignalProfiles[7]" in SRC
    assert "const VhsSignalProfile &prof = kVhsSignalProfiles[cameraProfileIndex];" in SRC
    assert "Fxc6 Reference|Gen 7 Reference|JVC GR-C7U (1986)|VHS-R20 Reference|Fall-78 Reference|ERC-V2S Reference|JVC GR-SXM740U" in SRC
    assert "ntscrs_process_argb8" in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("VHS Strength", 0, 2000, 0, 2000, 350' in SRC


def test_real_ntsc_rs_is_active_and_independently_switchable():
    assert 'PF_ADD_CHECKBOX("Enable NTSC-RS Signal Engine", "Enable", TRUE' in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("NTSC Chroma Delay", 0, 100, 0, 100, 18' in SRC
    assert 'baseNtscParams.chroma_delay = getSlider(params, P_NTSC_CHROMA_DELAY, 1.0f)' in SRC
    assert 'baseNtscParams.tracking_noise = getCheck(params, P_TRACKING_ENABLE)' in SRC
    assert 'effect.tracking_noise.enabled = true;' in (ROOT / "extern/ntscrs_c/src/lib.rs").read_text(encoding="utf-8")


def test_grain_is_zero_by_default_and_has_size_speed_amount_controls():
    assert 'PF_ADD_FLOAT_SLIDERX("Grain Amount", 0, 100, 0, 100, 0' in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("Grain Size", 1, 100, 1, 100, 1' in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("Grain Motion Speed", 0, 400, 0, 400, 100' in SRC
    assert 'PF_ADD_CHECKBOX("Enable Grain / Noise", "Enable", FALSE' in SRC
    assert 'const float grainAmount = grainEnabled ? clamp01(s.grain * (0.75f + prof.noiseFloor * 2.0f)) : 0.0f;' in SRC
    assert "const float rawN = (n00 * (1.0f-fx) + n10 * fx) * (1.0f-fy) + (n01 * (1.0f-fx) + n11 * fx) * fy;" in SRC
    assert 'PF_ADD_POPUP("Grain Size Preset", 5, 2' in SRC
    assert SRC.index('PF_ADD_TOPIC("IMAGE - Grain and Colored Signal Lines"') < SRC.index('PF_ADD_POPUP("Grain Size Preset"') < SRC.index('PF_END_TOPIC(P_TOPIC_GRAIN_END)')
    assert '64mm (Very Fine)' in SRC
    assert 'const float grainAmplitude = grainAmount * 0.18f' in SRC


def test_colored_lines_are_separate_and_survive_monochrome():
    assert 'PF_ADD_CHECKBOX("Enable Colored Signal Lines", "Enable", FALSE' in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("Colored Lines Amount"' in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("Colored Lines Size"' in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("Colored Lines Speed"' in SRC
    assert 'Optional colored analog signal lines are composited after monochrome conversion' in SRC
    assert 'if (coloredLinesEnabled && coloredLinesAmount > 0.001f)' in SRC


def test_removed_features_are_absent():
    for removed in [
        'Tape Behavior Model', 'Enable Tape Effects', 'Procedural Transparent Tape',
        'Moving Tape Sweep', 'Enable VHS Overlay on Source Footage',
        'VHS Craft - Signal Controls', 'Enable VHS Craft Engine',
    ]:
        assert removed not in SRC
    for removed_id in ['P_TAPE_TYPE', 'P_TAPE_ENABLE', 'P_TAPE_SWEEP_ENABLE', 'P_OVERLAY_ENABLE', 'P_CRAFT_ENABLE']:
        assert removed_id not in SRC
    assert 'PF_ADD_LAYER' not in SRC


def test_signal_reference_and_legend_have_distinct_active_processing():
    assert 'PF_ADD_CHECKBOX("Enable VHS Legend Engine", "Enable", FALSE' in SRC
    assert 'PF_ADD_CHECKBOX("Enable Signal Reference Engine", "Enable", FALSE' in SRC
    assert 'if (legendEngineEnabled)' in SRC
    assert 'if (signalReferenceEnabled)' in SRC
    assert 'const bool signalReferenceEnabled = getCheck(params, P_SIGNAL_ENABLE) && otherEnabled;' in SRC
    assert 'const bool otherEnabled = getCheck(params, P_OTHER_ENABLE);' in SRC
    assert 'const bool error = rand01(errorHash) < errorDrive * 0.12f;' in SRC


def test_glitch_cam_has_own_toggle_and_visible_event_strength():
    assert 'PF_ADD_CHECKBOX("Enable Glitch Cam Events", "Enable", FALSE' in SRC
    assert 'if (chosenEvent && glitchCamEnabled)' in SRC
    assert 'const bool chosenEvent = effectiveEventRate > 0.0f' in SRC
    assert 'const float amount = clamp01(glitchCamAmount * eventEnv * (0.75f + 0.25f * drive));' in SRC
    assert 'const float tearChance = (glitchCamMode == 0 || glitchCamMode == 1 || glitchCamMode == 4 ? 0.24f : 0.06f) * amount;' in SRC
    assert 'const float eventLength = std::max(s.eventDuration, 0.60f);' in SRC


def test_advanced_filters_osd_and_optional_overlay_remain_wired():
    for control in [
        'Lowpass Filter Type', 'Composite Signal Sharpen', 'Chroma Demodulation Filter',
        'Vertical Composite Noise', 'Small Circular Dot Noise', 'Head-Switch Position',
        'Head-Switch Height', 'Head-Switch Motion', 'Head-Switch Random Bursts',
        'Built-in OSD / Text Overlay', 'Built-in Overlay Style', 'OSD Position X (%)',
        'OSD Text Scale (%)', 'Glitch Cam Color Palette', 'Glitch Cam Blend-in',
        'Enable Built-in Signal Overlay',
    ]:
        assert control in SRC
    assert 'if (osdEnabled && osdOpacity > 0.001f' in SRC
    assert 'if (builtinOverlayEnabled && builtinOverlayOpacity > 0.001f)' in SRC


def test_black_level_chroma_offsets_and_saturation_are_wired():
    assert 'if (sat <= 0.0001f)' in SRC
    assert 'const float blackOffset = (s.black * 0.08f + prof.pedestal * 0.05f) * drive;' in SRC
    assert 'const float totalChromaX = s.chromaX * drive + prof.chromaOffsetX;' in SRC
    assert 'const float totalChromaY = s.chromaY * drive + prof.chromaOffsetY;' in SRC


def test_crt_tv_system_remains_removed():
    assert 'P_CRT_ENABLE' not in SRC
    assert 'CRT TV Engine' not in SRC
