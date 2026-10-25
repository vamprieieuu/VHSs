"""Static regressions for the requested VHS6RIFT cleanup and engine controls.
These checks validate source wiring; they are not a substitute for a native build or AE visual test.
"""
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
SRC = (ROOT / "src" / "VHS6RIFT.cpp").read_text(encoding="utf-8")


def test_main_vhs_defaults_on_and_five_real_jvc_camera_engines_are_selected():
    assert 'PF_ADD_POPUP("VHS Camera Engine", 5, 1' in SRC
    assert "kVhsSignalProfiles[5]" in SRC
    assert "const VhsSignalProfile &prof = kVhsSignalProfiles[cameraProfileIndex];" in SRC
    for model in ["JVC GR-C7U (1986 VHS-C)", "JVC GR-AX220U (1997 VHS-C)", "JVC GR-AX920U (1997 VHS-C)", "JVC GR-SXM37US (S-VHS-C)", "JVC GR-SXM740U (S-VHS-C)"]:
        assert model in SRC
    assert 'PF_ADD_CHECKBOX("Enable Main VHS Engine", "Enable", TRUE' in SRC
    assert "mainVhsEnabled ? std::min" in SRC
    assert "ntscrs_process_argb8" in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("VHS Strength", 0, 2000, 0, 2000, 350' in SRC



def test_camera_profiles_have_materially_different_signal_signatures():
    profile_lines = [line.strip() for line in SRC.splitlines() if line.strip().startswith("{") and "f," in line and "//" not in line and line.count("f") >= 10]
    assert len(profile_lines) >= 5
    assert len(set(profile_lines[-5:])) == 5
    # The render path gives per-camera softness and chroma response substantial weight.
    assert "prof.lumaSoftness * 1.05f * drive" in SRC
    assert "0.25f + prof.chromaBleed * 1.35f" in SRC
    assert "prof.timebaseWobble * (0.5f + 0.5f * drive) * 2.2f" in SRC
    assert "prof.chromaNoise * drive * 0.35f" in SRC


def test_real_ntsc_rs_is_active_and_independently_switchable():
    assert 'PF_ADD_CHECKBOX("Enable NTSC-RS Signal Engine", "Enable", TRUE' in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("NTSC Chroma Delay", 0, 100, 0, 100, 18' in SRC
    assert 'baseNtscParams.chroma_delay = getSlider(params, P_NTSC_CHROMA_DELAY, 1.0f)' in SRC
    assert 'baseNtscParams.tracking_noise = trackingEnabledForNtsc ? getSlider(params, P_TRACKING, 100.0f) * 100.0f : 0.0f;' in SRC
    assert 'effect.tracking_noise.enabled = true;' in (ROOT / "extern/ntscrs_c/src/lib.rs").read_text(encoding="utf-8")


def test_grain_is_zero_by_default_and_has_size_speed_amount_controls():
    assert 'PF_ADD_FLOAT_SLIDERX("Grain Amount", 0, 100, 0, 100, 0' in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("Grain Size", 1, 100, 1, 100, 1' in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("Grain Motion Speed", 0, 400, 0, 400, 100' in SRC
    assert 'const float grainAmount = clamp01(s.grain) * (0.75f + prof.noiseFloor * 2.0f);' in SRC
    assert 'P_GRAIN_ENABLE' not in SRC
    assert "const float rawN = (n00 * (1.0f-fx) + n10 * fx) * (1.0f-fy) + (n01 * (1.0f-fx) + n11 * fx) * fy;" in SRC
    assert 'PF_ADD_POPUP("Grain Size Preset", 5, 2' in SRC
    assert SRC.index('PF_ADD_TOPIC("IMAGE - Grain and Signal Noise"') < SRC.index('PF_ADD_POPUP("Grain Size Preset"') < SRC.index('PF_END_TOPIC(P_TOPIC_GRAIN_END)')
    assert '64mm (Very Fine)' in SRC
    assert 'const float grainAmplitude = grainAmount * 0.72f' in SRC


def test_signal_noise_uses_native_ntsc_rs_controls_not_synthetic_lines():
    assert 'PF_ADD_CHECKBOX("Composite signal noise (NTSC-RS)", "Enable", FALSE' in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("Signal Noise Intensity"' in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("Signal Noise Frequency"' in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("Signal Noise Detail"' in SRC
    assert 'effect.composite_noise.settings.frequency = p.composite_noise_frequency.clamp(0.0, 1.0);' in (ROOT / 'extern/ntscrs_c/src/lib.rs').read_text(encoding='utf-8')
    assert 'if (coloredLinesEnabled && coloredLinesAmount > 0.001f)' not in SRC
    assert 'r = clamp01(r * (1.0f-a) + (0.85f + 0.15f * stripe) * a);' not in SRC


def test_removed_features_are_absent():
    for removed in [
        'Tape Behavior Model', 'Enable Tape Effects', 'Procedural Transparent Tape',
        'Moving Tape Sweep', 'Enable VHS Overlay on Source Footage',
        'VHS Craft - Signal Controls', 'Enable VHS Craft Engine', 'Tape Dropouts', 'PF_ADD_FLOAT_SLIDERX("Block Corruption"', 'Legend Block Corruption', 'Sync Tear|Chroma Breakup|Block Corruption|Luma Dropout|' 'Signal Reference - Controls', 'Enable Signal Reference Engine',
    ]:
        assert removed not in SRC
    for removed_id in ['P_TAPE_TYPE', 'P_TAPE_ENABLE', 'P_TAPE_SWEEP_ENABLE', 'P_OVERLAY_ENABLE', 'P_CRAFT_ENABLE']:
        assert removed_id not in SRC
    assert 'PF_ADD_LAYER' not in SRC


def test_signal_reference_is_removed_and_legend_remains_independent():
    assert 'PF_ADD_CHECKBOX("Enable VHS Legend Engine", "Enable", FALSE' in SRC
    assert 'if (legendEngineEnabled)' in SRC
    assert 'P_SIGNAL_ENABLE' not in SRC
    assert 'Signal Reference - Controls' not in SRC
    assert 'const bool otherEnabled = getCheck(params, P_OTHER_ENABLE);' in SRC


def test_glitch_cam_is_removed_and_tracking_is_a_real_main_vhs_stage():
    assert "GLITCH_CAM" not in SRC
    assert "Glitch Cam" not in SRC
    assert 'PF_ADD_CHECKBOX("Enable Tracking Damage", "Enable", FALSE' in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("Tracking Noise", 0, 100, 0, 100, 0' in SRC
    assert 'PF_ADD_FLOAT_SLIDERX("Tracking Damage Size (% Frame)", 1, 100, 1, 100, 18' in SRC and 'baseNtscParams.tracking_height = getSlider(params, P_TRACK_WIDTH, 100.0f) * 100.0f;' in SRC
    assert "baseNtscParams.tracking_height = getSlider(params, P_TRACK_WIDTH, 100.0f) * 100.0f;" in SRC
    assert "effect.tracking_noise.settings.snow_intensity = 0.0;" in (ROOT / "extern/ntscrs_c/src/lib.rs").read_text(encoding="utf-8")
    assert 'PF_ADD_CHECKBOX("Enable Tracking Damage", "Enable", FALSE' in SRC


def test_advanced_filters_osd_and_optional_overlay_remain_wired():
    for control in [
        'Lowpass Filter Type', 'Composite Signal Sharpen', 'Chroma Demodulation Filter',
        'Vertical Composite Noise', 'Small Circular Dot Noise', 'Head-Switch Position',
        'Head-Switch Height', 'Head-Switch Motion', 'Head-Switch Random Bursts',
        'Built-in OSD / Text Overlay', 'Built-in Overlay Style', 'OSD Position X (%)',
        'OSD Text Scale (%)',
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
