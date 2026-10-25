#include "VHS6RIFT.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>

// Forward Declarations
static PF_Err RenderPixel8(PF_EffectWorld *input, PF_EffectWorld *output, const RenderContext &ctx);
static PF_Err RenderPixel16(PF_EffectWorld *input, PF_EffectWorld *output, const RenderContext &ctx);
static PF_Err RenderPixel32(PF_EffectWorld *input, PF_EffectWorld *output, const RenderContext &ctx);
static void PopulateContext(PF_InData *in_data, PF_ParamDef *params[], RenderContext &ctx);
static void ApplyStylePreset(RenderContext &ctx);
static void ApplyCameraProfile(RenderContext &ctx);
static PF_Err RenderAudio(PF_InData *in_data, PF_ParamDef *params[], PF_SoundWorld *sound);

// About Command
static PF_Err About(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output) {
  #if VHS6RIFT_HAS_OFFICIAL_AE_SDK
    PF_SPRINTF(out_data->return_msg, "%s v%d.%d\r%s", EFFECT_NAME, MAJOR_VERSION, MINOR_VERSION, EFFECT_DESCRIPTION);
  #else
    std::strncpy(out_data->return_msg, "VHS6RIFT v2.0 - Advanced Procedural Analog Video & VHS Effects Engine", sizeof(out_data->return_msg) - 1);
  #endif
  return PF_Err_NONE;
}

// Global Setup Command
static PF_Err GlobalSetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output) {
  out_data->my_version = PF_VERSION(MAJOR_VERSION, MINOR_VERSION, BUG_VERSION, STAGE_VERSION, BUILD_VERSION);
  out_data->out_flags  = PF_OutFlag_DEEP_COLOR_AWARE | PF_OutFlag_WIDE_TIME_INPUT | PF_OutFlag_AUDIO_EFFECT_TOO | PF_OutFlag_AUDIO_FLOAT_ONLY;
  out_data->out_flags2 = PF_OutFlag2_SUPPORTS_THREADED_RENDERING;
  return PF_Err_NONE;
}

// Params Setup Command: Register all parameters in exact enum order
static PF_Err ParamsSetup(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output) {
  PF_ParamDef def;

  #define ADD_TOPIC_START(name, id) \
    AEFX_CLR_STRUCT(def); \
    std::strncpy(def.name, name, sizeof(def.name) - 1); \
    def.param_type = PF_Param_GROUP_START; \
    /* In real SDK: PF_ADD_TOPIC(name, id); */

  #define ADD_TOPIC_END(id) \
    AEFX_CLR_STRUCT(def); \
    def.param_type = PF_Param_GROUP_END; \
    /* In real SDK: PF_END_TOPIC(id); */

  #define ADD_SLIDER(name, minV, maxV, dfltV, id) \
    AEFX_CLR_STRUCT(def); \
    std::strncpy(def.name, name, sizeof(def.name) - 1); \
    def.param_type = PF_Param_SLIDER; \
    def.u.sd.value = dfltV;

  #define ADD_CHECKBOX(name, dfltV, id) \
    AEFX_CLR_STRUCT(def); \
    std::strncpy(def.name, name, sizeof(def.name) - 1); \
    def.param_type = PF_Param_CHECKBOX; \
    def.u.bd.value = dfltV;

  #define ADD_POPUP(name, numChoices, dfltChoice, choicesStr, id) \
    AEFX_CLR_STRUCT(def); \
    std::strncpy(def.name, name, sizeof(def.name) - 1); \
    def.param_type = PF_Param_POPUP; \
    def.u.pd.value = dfltChoice;

  // Topic 1: Master Engine Controls
  ADD_TOPIC_START("01 | MASTER ENGINE CONTROLS", P_TOPIC_MASTER_START)
  ADD_SLIDER("Master VHS Strength", 0, 1000, 350, P_MASTER_STRENGTH)
  ADD_SLIDER("Mix with Original (%)", 0, 100, 100, P_MIX_ORIGINAL)
  ADD_CHECKBOX("Enable Continuous Treatment", 1, P_CONTINUOUS_ENABLE)
  ADD_CHECKBOX("Enable Event Glitches", 1, P_EVENT_ENABLE)
  ADD_SLIDER("Random Seed", 1, 32767, 1337, P_RANDOM_SEED)
  ADD_SLIDER("Animation Speed (%)", 0, 500, 100, P_ANIMATION_SPEED)
  ADD_TOPIC_END(P_TOPIC_MASTER_END)

  // Topic 2: Style Presets
  ADD_TOPIC_START("02 | STYLE PRESETS", P_TOPIC_PRESETS_START)
  ADD_POPUP("Style Preset", STYLE_TOTAL_COUNT, STYLE_CUSTOM,
            "Custom (Manual)|Clean VHS|Vintage VHS|Heavy VHS Degradation|Bad Tracking|Very Bad Tracking|"
            "Unstable VCR Playback|Moving Band Noise|Tape Damage|Bottom Tape Damage|Static Interference|"
            "Color Bleed|Color Shift|Ghosting|Signal Loss|Severe Analog Glitch|Damaged Tape Playback",
            P_STYLE_PRESET)
  ADD_SLIDER("Preset Intensity (%)", 0, 100, 100, P_PRESET_INTENSITY)
  ADD_TOPIC_END(P_TOPIC_PRESETS_END)

  // Topic 3: Camera & Format Profiles
  ADD_TOPIC_START("03 | CAMERA & FORMAT PROFILES", P_TOPIC_CAMERA_START)
  ADD_POPUP("Camera Profile", CAM_TOTAL_COUNT, CAM_JVC_GR_C7U,
            "Off / Neutral|JVC GR-C7U (1986 VHS-C)|JVC GR-AX220U (Compact VHS)|JVC GR-AX920U (HQ)|"
            "JVC GR-SXM37US (S-VHS-C)|JVC GR-SXM740U (Flagship)|Sony CCD-TR55 (Video8)|"
            "JVC GR-S55 (S-VHS High-Band)|Panasonic NV-M1 (1985)|RCA CMR300 (1987)",
            P_CAMERA_PROFILE)
  ADD_SLIDER("Camera Response (%)", 0, 100, 100, P_CAMERA_INTENSITY)
  ADD_SLIDER("Camera Color Warmth", -100, 100, 10, P_CAMERA_WARMTH)
  ADD_TOPIC_END(P_TOPIC_CAMERA_END)

  // Topic 4: Luma & Chroma Degradation
  ADD_TOPIC_START("04 | LUMA & CHROMA SIGNAL", P_TOPIC_SIGNAL_START)
  ADD_CHECKBOX("Enable Signal Processing", 1, P_SIGNAL_ENABLE)
  ADD_SLIDER("Luma Softness", 0, 100, 28, P_LUMA_SOFTNESS)
  ADD_SLIDER("Luma Blur", 0, 50, 0, P_LUMA_BLUR)
  ADD_SLIDER("Chroma Blur", 0, 50, 18, P_CHROMA_BLUR)
  ADD_SLIDER("Chroma Bleed", 0, 50, 14, P_CHROMA_BLEED)
  ADD_SLIDER("Chroma Offset X", -30, 30, 4, P_CHROMA_OFFSET_X)
  ADD_SLIDER("Chroma Offset Y", -30, 30, 0, P_CHROMA_OFFSET_Y)
  ADD_SLIDER("Red Channel Offset", -20, 20, 2, P_CHANNEL_OFFSET_RED)
  ADD_SLIDER("Blue Channel Offset", -20, 20, -2, P_CHANNEL_OFFSET_BLUE)
  ADD_SLIDER("Color Shift Amount", 0, 100, 15, P_COLOR_SHIFT_AMOUNT)
  ADD_SLIDER("Color Shift Speed", 0, 200, 50, P_COLOR_SHIFT_SPEED)
  ADD_SLIDER("Edge Ringing & Overshoot", 0, 100, 25, P_EDGE_RINGING)
  ADD_SLIDER("Color Saturation (%)", 0, 200, 105, P_SATURATION)
  ADD_SLIDER("Black Level Pedestal", -50, 50, 8, P_BLACK_LEVEL)
  ADD_TOPIC_END(P_TOPIC_SIGNAL_END)

  // Topic 5: Tracking & Temporal Instability
  ADD_TOPIC_START("05 | TRACKING & INSTABILITY", P_TOPIC_TRACKING_START)
  ADD_CHECKBOX("Enable Tracking Engine", 1, P_TRACKING_ENABLE)
  ADD_POPUP("Tracking Severity Mode", 3, TRACKING_NORMAL, "Normal Tracking|Bad Tracking|Very Bad Tracking", P_TRACKING_MODE)
  ADD_SLIDER("Tracking Severity (%)", 0, 100, 35, P_TRACKING_SEVERITY)
  ADD_SLIDER("Tracking Frequency", 0, 100, 40, P_TRACKING_FREQUENCY)
  ADD_SLIDER("Horizontal Scanline Jitter", 0, 100, 20, P_HORIZONTAL_JITTER)
  ADD_SLIDER("Frame Jitter (V-Hold)", 0, 100, 15, P_FRAME_JITTER)
  ADD_SLIDER("Intermittent Sync Loss", 0, 100, 10, P_SYNC_LOSS_AMOUNT)
  ADD_SLIDER("Tape Alignment Instability", 0, 100, 25, P_ALIGNMENT_INSTABILITY)
  ADD_TOPIC_END(P_TOPIC_TRACKING_END)

  // Topic 6: Moving Bands & Tape Damage
  ADD_TOPIC_START("06 | MOVING BANDS & DAMAGE", P_TOPIC_DAMAGE_START)
  ADD_CHECKBOX("Enable Tape Damage", 1, P_DAMAGE_ENABLE)
  ADD_SLIDER("Moving Band Amount (%)", 0, 100, 40, P_MOVING_BAND_AMOUNT)
  ADD_SLIDER("Band Height", 2, 100, 24, P_BAND_HEIGHT)
  ADD_SLIDER("Band Speed", -200, 200, 60, P_BAND_SPEED)
  ADD_POPUP("Band Motion Direction", 3, DIR_DOWN, "Scroll Up|Scroll Down|Erratic Motion", P_BAND_DIRECTION)
  ADD_SLIDER("Localized Band Distortion (%)", 0, 100, 50, P_LOCALIZED_DISTORTION)
  ADD_SLIDER("Bottom Head-Switch Amount (%)", 0, 100, 65, P_BOTTOM_DAMAGE_AMOUNT)
  ADD_SLIDER("Bottom Head-Switch Height", 1, 60, 14, P_BOTTOM_DAMAGE_HEIGHT)
  ADD_SLIDER("Oxide Dropout Amount", 0, 100, 25, P_DROPOUT_AMOUNT)
  ADD_SLIDER("Oxide Dropout Length", 2, 30, 8, P_DROPOUT_LENGTH)
  ADD_SLIDER("RF Static Burst Rate", 0, 100, 15, P_STATIC_BURST_RATE)
  ADD_CHECKBOX("Multiple Simultaneous Bands", 1, P_MULTIPLE_DAMAGE_REGIONS)
  ADD_TOPIC_END(P_TOPIC_DAMAGE_END)

  // Topic 7: Ghosting & RF Multipath Echoes
  ADD_TOPIC_START("07 | GHOSTING & RF ECHOES", P_TOPIC_GHOSTING_START)
  ADD_CHECKBOX("Enable Ghosting", 0, P_GHOSTING_ENABLE)
  ADD_SLIDER("Ghosting Amount (%)", 0, 100, 35, P_GHOSTING_AMOUNT)
  ADD_SLIDER("Ghosting Horizontal Offset", 1, 120, 22, P_GHOSTING_OFFSET)
  ADD_SLIDER("Ghosting Decay (%)", 10, 90, 50, P_GHOSTING_DECAY)
  ADD_SLIDER("Signal Glow / Trails (%)", 0, 100, 20, P_SIGNAL_TRAILS)
  ADD_TOPIC_END(P_TOPIC_GHOSTING_END)

  // Topic 8: Procedural Noise System
  ADD_TOPIC_START("08 | PROCEDURAL NOISE SYSTEM", P_TOPIC_NOISE_START)
  ADD_CHECKBOX("Enable Procedural Noise", 1, P_NOISE_ENABLE)
  ADD_SLIDER("Tape Grain Amount", 0, 100, 25, P_NOISE_GRAIN_AMOUNT)
  ADD_SLIDER("Grain Particle Size", 1, 5, 2, P_NOISE_GRAIN_SIZE)
  ADD_SLIDER("Luminance Noise Amount", 0, 100, 30, P_NOISE_LUMA_AMOUNT)
  ADD_SLIDER("Chroma Noise Amount", 0, 100, 40, P_NOISE_CHROMA_AMOUNT)
  ADD_SLIDER("Noise Detail / High-Frequency", 0, 100, 50, P_NOISE_DETAIL)
  ADD_SLIDER("Shadow Bias (Dark Noise Weight)", 0, 100, 65, P_NOISE_SHADOW_BIAS)
  ADD_TOPIC_END(P_TOPIC_NOISE_END)

  // Topic 9: Audio Tape Degradation
  ADD_TOPIC_START("09 | AUDIO TAPE DEGRADATION", P_TOPIC_AUDIO_START)
  ADD_CHECKBOX("Enable Audio Degradation", 0, P_AUDIO_ENABLE)
  ADD_SLIDER("Audio Mix (Wet / Dry %)", 0, 100, 80, P_AUDIO_WET_DRY)
  ADD_SLIDER("Tape Tanh Saturation Drive", 0, 100, 45, P_AUDIO_SATURATION)
  ADD_SLIDER("Bitcrush Quantization (Bits)", 4, 16, 10, P_AUDIO_QUANTIZATION)
  ADD_SLIDER("Sample-and-Hold Rate (Hz)", 4000, 48000, 22050, P_AUDIO_DOWNSAMPLE_RATE)
  ADD_SLIDER("Magnetic Crackle Noise", 0, 100, 20, P_AUDIO_CRACKLE)
  ADD_SLIDER("Tape Dropout Gate", 0, 100, 15, P_AUDIO_DROPOUT)
  ADD_TOPIC_END(P_TOPIC_AUDIO_END)

  #undef ADD_TOPIC_START
  #undef ADD_TOPIC_END
  #undef ADD_SLIDER
  #undef ADD_CHECKBOX
  #undef ADD_POPUP

  return PF_Err_NONE;
}

// Populate Context from Parameter Pointers
static void PopulateContext(PF_InData *in_data, PF_ParamDef *params[], RenderContext &ctx) {
  ctx.timeSec = (in_data && in_data->time_step.scale > 0)
    ? static_cast<double>(in_data->current_time.value) / static_cast<double>(in_data->current_time.scale)
    : 0.0;

  float animSpeed = static_cast<float>(params[P_ANIMATION_SPEED]->u.sd.value) / 100.0f;
  ctx.timeSec *= animSpeed;

  ctx.seed = static_cast<uint32_t>(params[P_RANDOM_SEED]->u.sd.value);
  int32_t rawMaster = params[P_MASTER_STRENGTH]->u.sd.value;
  ctx.masterStrength = (rawMaster <= 100)
    ? (static_cast<float>(rawMaster) / 100.0f)
    : (1.0f + std::pow(static_cast<float>(rawMaster - 100) / 900.0f, 1.25f) * 4.0f);
  if (rawMaster == 0) ctx.masterStrength = 0.0f;

  ctx.mixOriginal = static_cast<float>(params[P_MIX_ORIGINAL]->u.sd.value) / 100.0f;
  ctx.continuousEnable = params[P_CONTINUOUS_ENABLE]->u.bd.value != 0;
  ctx.eventEnable = params[P_EVENT_ENABLE]->u.bd.value != 0;

  if (!ctx.continuousEnable) {
    ctx.lumaSoftness = 0.0f;
    ctx.chromaBleed = 0.0f;
    ctx.chromaOffsetX = 0.0f;
    ctx.chromaOffsetY = 0.0f;
    ctx.channelOffsetRed = 0.0f;
    ctx.channelOffsetBlue = 0.0f;
    ctx.grainAmount = 0.0f;
  }

  if (!ctx.eventEnable) {
    ctx.movingBandAmount = 0.0f;
    ctx.staticBurstRate = 0.0f;
    ctx.dropoutAmount = 0.0f;
    ctx.syncLoss = 0.0f;
  }

  // Preset & Camera
  ctx.presetId = params[P_STYLE_PRESET]->u.pd.value;
  ctx.presetIntensity = static_cast<float>(params[P_PRESET_INTENSITY]->u.sd.value) / 100.0f;
  ctx.cameraId = params[P_CAMERA_PROFILE]->u.pd.value;
  ctx.cameraIntensity = static_cast<float>(params[P_CAMERA_INTENSITY]->u.sd.value) / 100.0f;
  ctx.cameraWarmth = static_cast<float>(params[P_CAMERA_WARMTH]->u.sd.value);

  // Signal
  ctx.signalEnable = params[P_SIGNAL_ENABLE]->u.bd.value != 0;
  ctx.lumaSoftness = static_cast<float>(params[P_LUMA_SOFTNESS]->u.sd.value);
  ctx.lumaBlur = static_cast<float>(params[P_LUMA_BLUR]->u.sd.value);
  ctx.chromaBlur = static_cast<float>(params[P_CHROMA_BLUR]->u.sd.value);
  ctx.chromaBleed = static_cast<float>(params[P_CHROMA_BLEED]->u.sd.value);
  ctx.chromaOffsetX = static_cast<float>(params[P_CHROMA_OFFSET_X]->u.sd.value);
  ctx.chromaOffsetY = static_cast<float>(params[P_CHROMA_OFFSET_Y]->u.sd.value);
  ctx.channelOffsetRed = static_cast<float>(params[P_CHANNEL_OFFSET_RED]->u.sd.value);
  ctx.channelOffsetBlue = static_cast<float>(params[P_CHANNEL_OFFSET_BLUE]->u.sd.value);
  ctx.colorShiftAmount = static_cast<float>(params[P_COLOR_SHIFT_AMOUNT]->u.sd.value);
  ctx.colorShiftSpeed = static_cast<float>(params[P_COLOR_SHIFT_SPEED]->u.sd.value);
  ctx.edgeRinging = static_cast<float>(params[P_EDGE_RINGING]->u.sd.value);
  ctx.saturation = static_cast<float>(params[P_SATURATION]->u.sd.value) / 100.0f;
  ctx.blackLevel = static_cast<float>(params[P_BLACK_LEVEL]->u.sd.value);

  // Tracking
  ctx.trackingEnable = params[P_TRACKING_ENABLE]->u.bd.value != 0;
  ctx.trackingMode = params[P_TRACKING_MODE]->u.pd.value;
  ctx.trackingSeverity = static_cast<float>(params[P_TRACKING_SEVERITY]->u.sd.value) / 100.0f;
  ctx.trackingFreq = static_cast<float>(params[P_TRACKING_FREQUENCY]->u.sd.value);
  ctx.hJitter = static_cast<float>(params[P_HORIZONTAL_JITTER]->u.sd.value);
  ctx.frameJitter = static_cast<float>(params[P_FRAME_JITTER]->u.sd.value);
  ctx.syncLoss = static_cast<float>(params[P_SYNC_LOSS_AMOUNT]->u.sd.value);
  ctx.alignmentInstability = static_cast<float>(params[P_ALIGNMENT_INSTABILITY]->u.sd.value);

  // Damage
  ctx.damageEnable = params[P_DAMAGE_ENABLE]->u.bd.value != 0;
  ctx.movingBandAmount = static_cast<float>(params[P_MOVING_BAND_AMOUNT]->u.sd.value) / 100.0f;
  ctx.bandHeight = static_cast<float>(params[P_BAND_HEIGHT]->u.sd.value);
  ctx.bandSpeed = static_cast<float>(params[P_BAND_SPEED]->u.sd.value);
  ctx.bandDirection = params[P_BAND_DIRECTION]->u.pd.value;
  ctx.localizedDistortion = static_cast<float>(params[P_LOCALIZED_DISTORTION]->u.sd.value) / 100.0f;
  ctx.bottomDamageAmount = static_cast<float>(params[P_BOTTOM_DAMAGE_AMOUNT]->u.sd.value) / 100.0f;
  ctx.bottomDamageHeight = static_cast<float>(params[P_BOTTOM_DAMAGE_HEIGHT]->u.sd.value);
  ctx.dropoutAmount = static_cast<float>(params[P_DROPOUT_AMOUNT]->u.sd.value);
  ctx.dropoutLength = static_cast<float>(params[P_DROPOUT_LENGTH]->u.sd.value);
  ctx.staticBurstRate = static_cast<float>(params[P_STATIC_BURST_RATE]->u.sd.value);
  ctx.multiRegions = params[P_MULTIPLE_DAMAGE_REGIONS]->u.bd.value != 0;

  // Ghosting
  ctx.ghostingEnable = params[P_GHOSTING_ENABLE]->u.bd.value != 0;
  ctx.ghostingAmount = static_cast<float>(params[P_GHOSTING_AMOUNT]->u.sd.value) / 100.0f;
  ctx.ghostingOffset = static_cast<float>(params[P_GHOSTING_OFFSET]->u.sd.value);
  ctx.ghostingDecay = static_cast<float>(params[P_GHOSTING_DECAY]->u.sd.value) / 100.0f;
  ctx.signalTrails = static_cast<float>(params[P_SIGNAL_TRAILS]->u.sd.value) / 100.0f;

  // Noise
  ctx.noiseEnable = params[P_NOISE_ENABLE]->u.bd.value != 0;
  ctx.grainAmount = static_cast<float>(params[P_NOISE_GRAIN_AMOUNT]->u.sd.value);
  ctx.grainSize = static_cast<float>(params[P_NOISE_GRAIN_SIZE]->u.sd.value);
  ctx.noiseLumaAmount = static_cast<float>(params[P_NOISE_LUMA_AMOUNT]->u.sd.value);
  ctx.noiseChromaAmount = static_cast<float>(params[P_NOISE_CHROMA_AMOUNT]->u.sd.value);
  ctx.noiseDetail = static_cast<float>(params[P_NOISE_DETAIL]->u.sd.value);
  ctx.noiseShadowBias = static_cast<float>(params[P_NOISE_SHADOW_BIAS]->u.sd.value) / 100.0f;

  // Apply Preset and Camera overrides
  ApplyStylePreset(ctx);
  ApplyCameraProfile(ctx);
}

// 16 Authentic Style Presets Implementation
static void ApplyStylePreset(RenderContext &ctx) {
  if (ctx.presetId <= STYLE_CUSTOM) return;

  float pI = ctx.presetIntensity;
  auto blendF = [pI](float cur, float target) -> float {
    return cur * (1.0f - pI) + target * pI;
  };

  switch (ctx.presetId) {
    case STYLE_CLEAN_VHS:
      ctx.lumaSoftness = blendF(ctx.lumaSoftness, 14.0f);
      ctx.chromaBleed = blendF(ctx.chromaBleed, 6.0f);
      ctx.chromaOffsetX = blendF(ctx.chromaOffsetX, 2.0f);
      ctx.grainAmount = blendF(ctx.grainAmount, 12.0f);
      ctx.trackingSeverity = blendF(ctx.trackingSeverity, 0.10f);
      ctx.movingBandAmount = blendF(ctx.movingBandAmount, 0.05f);
      ctx.bottomDamageAmount = blendF(ctx.bottomDamageAmount, 0.20f);
      break;

    case STYLE_VINTAGE_VHS:
      ctx.lumaSoftness = blendF(ctx.lumaSoftness, 35.0f);
      ctx.chromaBleed = blendF(ctx.chromaBleed, 22.0f);
      ctx.chromaOffsetX = blendF(ctx.chromaOffsetX, 5.0f);
      ctx.grainAmount = blendF(ctx.grainAmount, 35.0f);
      ctx.colorShiftAmount = blendF(ctx.colorShiftAmount, 25.0f);
      ctx.edgeRinging = blendF(ctx.edgeRinging, 35.0f);
      ctx.trackingSeverity = blendF(ctx.trackingSeverity, 0.40f);
      ctx.movingBandAmount = blendF(ctx.movingBandAmount, 0.45f);
      ctx.bottomDamageAmount = blendF(ctx.bottomDamageAmount, 0.70f);
      ctx.blackLevel = blendF(ctx.blackLevel, 12.0f);
      break;

    case STYLE_HEAVY_DEGRADATION:
      ctx.lumaSoftness = blendF(ctx.lumaSoftness, 60.0f);
      ctx.chromaBleed = blendF(ctx.chromaBleed, 38.0f);
      ctx.chromaOffsetX = blendF(ctx.chromaOffsetX, 8.0f);
      ctx.grainAmount = blendF(ctx.grainAmount, 65.0f);
      ctx.noiseChromaAmount = blendF(ctx.noiseChromaAmount, 70.0f);
      ctx.trackingSeverity = blendF(ctx.trackingSeverity, 0.75f);
      ctx.hJitter = blendF(ctx.hJitter, 45.0f);
      ctx.dropoutAmount = blendF(ctx.dropoutAmount, 60.0f);
      ctx.movingBandAmount = blendF(ctx.movingBandAmount, 0.80f);
      ctx.bottomDamageAmount = blendF(ctx.bottomDamageAmount, 0.90f);
      break;

    case STYLE_BAD_TRACKING:
      ctx.trackingMode = TRACKING_BAD;
      ctx.trackingSeverity = blendF(ctx.trackingSeverity, 0.80f);
      ctx.trackingFreq = blendF(ctx.trackingFreq, 70.0f);
      ctx.hJitter = blendF(ctx.hJitter, 50.0f);
      ctx.movingBandAmount = blendF(ctx.movingBandAmount, 0.85f);
      ctx.localizedDistortion = blendF(ctx.localizedDistortion, 0.75f);
      ctx.bandHeight = blendF(ctx.bandHeight, 40.0f);
      ctx.bandSpeed = blendF(ctx.bandSpeed, 120.0f);
      break;

    case STYLE_VERY_BAD_TRACKING:
      ctx.trackingMode = TRACKING_VERY_BAD;
      ctx.trackingSeverity = blendF(ctx.trackingSeverity, 1.0f);
      ctx.hJitter = blendF(ctx.hJitter, 85.0f);
      ctx.frameJitter = blendF(ctx.frameJitter, 70.0f);
      ctx.syncLoss = blendF(ctx.syncLoss, 60.0f);
      ctx.movingBandAmount = blendF(ctx.movingBandAmount, 1.0f);
      ctx.localizedDistortion = blendF(ctx.localizedDistortion, 0.95f);
      ctx.staticBurstRate = blendF(ctx.staticBurstRate, 65.0f);
      break;

    case STYLE_UNSTABLE_VCR:
      ctx.alignmentInstability = blendF(ctx.alignmentInstability, 80.0f);
      ctx.frameJitter = blendF(ctx.frameJitter, 45.0f);
      ctx.hJitter = blendF(ctx.hJitter, 40.0f);
      ctx.colorShiftSpeed = blendF(ctx.colorShiftSpeed, 120.0f);
      ctx.colorShiftAmount = blendF(ctx.colorShiftAmount, 40.0f);
      break;

    case STYLE_MOVING_BAND_NOISE:
      ctx.movingBandAmount = blendF(ctx.movingBandAmount, 0.90f);
      ctx.bandHeight = blendF(ctx.bandHeight, 35.0f);
      ctx.bandSpeed = blendF(ctx.bandSpeed, 90.0f);
      ctx.localizedDistortion = blendF(ctx.localizedDistortion, 0.70f);
      ctx.multiRegions = true;
      break;

    case STYLE_TAPE_DAMAGE:
      ctx.dropoutAmount = blendF(ctx.dropoutAmount, 80.0f);
      ctx.dropoutLength = blendF(ctx.dropoutLength, 20.0f);
      ctx.movingBandAmount = blendF(ctx.movingBandAmount, 0.65f);
      ctx.localizedDistortion = blendF(ctx.localizedDistortion, 0.80f);
      break;

    case STYLE_BOTTOM_TAPE_DAMAGE:
      ctx.bottomDamageAmount = blendF(ctx.bottomDamageAmount, 1.0f);
      ctx.bottomDamageHeight = blendF(ctx.bottomDamageHeight, 40.0f);
      ctx.hJitter = blendF(ctx.hJitter, 30.0f);
      break;

    case STYLE_STATIC_INTERFERENCE:
      ctx.staticBurstRate = blendF(ctx.staticBurstRate, 85.0f);
      ctx.noiseLumaAmount = blendF(ctx.noiseLumaAmount, 80.0f);
      ctx.noiseChromaAmount = blendF(ctx.noiseChromaAmount, 75.0f);
      ctx.grainAmount = blendF(ctx.grainAmount, 60.0f);
      break;

    case STYLE_COLOR_BLEED:
      ctx.chromaBleed = blendF(ctx.chromaBleed, 45.0f);
      ctx.chromaBlur = blendF(ctx.chromaBlur, 35.0f);
      ctx.chromaOffsetX = blendF(ctx.chromaOffsetX, 12.0f);
      ctx.saturation = blendF(ctx.saturation, 1.45f);
      break;

    case STYLE_COLOR_SHIFT:
      ctx.channelOffsetRed = blendF(ctx.channelOffsetRed, 8.0f);
      ctx.channelOffsetBlue = blendF(ctx.channelOffsetBlue, -8.0f);
      ctx.colorShiftAmount = blendF(ctx.colorShiftAmount, 60.0f);
      ctx.colorShiftSpeed = blendF(ctx.colorShiftSpeed, 100.0f);
      break;

    case STYLE_GHOSTING:
      ctx.ghostingEnable = true;
      ctx.ghostingAmount = blendF(ctx.ghostingAmount, 0.85f);
      ctx.ghostingOffset = blendF(ctx.ghostingOffset, 36.0f);
      ctx.ghostingDecay = blendF(ctx.ghostingDecay, 0.60f);
      ctx.signalTrails = blendF(ctx.signalTrails, 0.50f);
      break;

    case STYLE_SIGNAL_LOSS:
      ctx.syncLoss = blendF(ctx.syncLoss, 85.0f);
      ctx.frameJitter = blendF(ctx.frameJitter, 75.0f);
      ctx.staticBurstRate = blendF(ctx.staticBurstRate, 50.0f);
      ctx.blackLevel = blendF(ctx.blackLevel, 25.0f);
      break;

    case STYLE_SEVERE_ANALOG_GLITCH:
      ctx.trackingMode = TRACKING_VERY_BAD;
      ctx.trackingSeverity = blendF(ctx.trackingSeverity, 1.0f);
      ctx.hJitter = blendF(ctx.hJitter, 90.0f);
      ctx.chromaBleed = blendF(ctx.chromaBleed, 40.0f);
      ctx.dropoutAmount = blendF(ctx.dropoutAmount, 85.0f);
      ctx.staticBurstRate = blendF(ctx.staticBurstRate, 70.0f);
      ctx.channelOffsetRed = blendF(ctx.channelOffsetRed, 12.0f);
      ctx.channelOffsetBlue = blendF(ctx.channelOffsetBlue, -12.0f);
      break;

    case STYLE_DAMAGED_TAPE_PLAYBACK:
      ctx.dropoutAmount = blendF(ctx.dropoutAmount, 70.0f);
      ctx.movingBandAmount = blendF(ctx.movingBandAmount, 0.75f);
      ctx.alignmentInstability = blendF(ctx.alignmentInstability, 65.0f);
      ctx.grainAmount = blendF(ctx.grainAmount, 50.0f);
      ctx.edgeRinging = blendF(ctx.edgeRinging, 45.0f);
      break;

    default:
      break;
  }
}

// 10 Camera Profiles Implementation
static void ApplyCameraProfile(RenderContext &ctx) {
  if (ctx.cameraId <= CAM_OFF) return;

  float cI = ctx.cameraIntensity;
  auto blendF = [cI](float cur, float target) -> float {
    return cur * (1.0f - cI) + target * cI;
  };

  switch (ctx.cameraId) {
    case CAM_JVC_GR_C7U: // 1986 VHS-C: Softest luma, wide warm chroma smear, line wobble
      ctx.lumaSoftness = blendF(ctx.lumaSoftness, 42.0f);
      ctx.chromaBleed = blendF(ctx.chromaBleed, 26.0f);
      ctx.chromaOffsetX = blendF(ctx.chromaOffsetX, 6.0f);
      ctx.hJitter = blendF(ctx.hJitter, 30.0f);
      ctx.cameraWarmth += 25.0f * cI;
      break;

    case CAM_JVC_GR_AX220U: // 1997 Compact VHS: Moderate HQ softness
      ctx.lumaSoftness = blendF(ctx.lumaSoftness, 28.0f);
      ctx.chromaBleed = blendF(ctx.chromaBleed, 14.0f);
      ctx.hJitter = blendF(ctx.hJitter, 15.0f);
      break;

    case CAM_JVC_GR_AX920U: // 1990s HQ: Firm edges, picture stabilizer
      ctx.lumaSoftness = blendF(ctx.lumaSoftness, 18.0f);
      ctx.edgeRinging = blendF(ctx.edgeRinging, 35.0f);
      ctx.hJitter = blendF(ctx.hJitter, 10.0f);
      break;

    case CAM_JVC_GR_SXM37US: // S-VHS-C / S-VHS ET: Higher luma bandwidth, lower chroma smear
      ctx.lumaSoftness = blendF(ctx.lumaSoftness, 12.0f);
      ctx.chromaBleed = blendF(ctx.chromaBleed, 8.0f);
      ctx.chromaOffsetX = blendF(ctx.chromaOffsetX, 2.0f);
      ctx.hJitter = blendF(ctx.hJitter, 8.0f);
      break;

    case CAM_JVC_GR_SXM740U: // Flagship S-VHS-C: Cleanest TBC stabilized
      ctx.lumaSoftness = blendF(ctx.lumaSoftness, 8.0f);
      ctx.chromaBleed = blendF(ctx.chromaBleed, 5.0f);
      ctx.chromaOffsetX = blendF(ctx.chromaOffsetX, 1.0f);
      ctx.hJitter = blendF(ctx.hJitter, 4.0f);
      break;

    case CAM_SONY_CCD_TR55: // 1989 Video8: Cool tone, distinct Y/C matrix
      ctx.lumaSoftness = blendF(ctx.lumaSoftness, 24.0f);
      ctx.chromaBleed = blendF(ctx.chromaBleed, 16.0f);
      ctx.cameraWarmth -= 15.0f * cI;
      break;

    case CAM_JVC_GR_S55: // S-VHS High-Band: High resolution, subtle FM carrier
      ctx.lumaSoftness = blendF(ctx.lumaSoftness, 10.0f);
      ctx.edgeRinging = blendF(ctx.edgeRinging, 30.0f);
      break;

    case CAM_PANASONIC_NV_M1: // 1985 Full-Size VHS: Classic warm tube camera character
      ctx.lumaSoftness = blendF(ctx.lumaSoftness, 48.0f);
      ctx.chromaBleed = blendF(ctx.chromaBleed, 30.0f);
      ctx.cameraWarmth += 30.0f * cI;
      break;

    case CAM_RCA_CMR300: // 1987 Solid State VHS: Early MOS sensor blooming
      ctx.lumaSoftness = blendF(ctx.lumaSoftness, 36.0f);
      ctx.edgeRinging = blendF(ctx.edgeRinging, 40.0f);
      ctx.cameraWarmth += 10.0f * cI;
      break;

    default:
      break;
  }
}

// Color Utility: RGB to YIQ and YIQ to RGB (Analog NTSC composite domain)
static inline void RGB_to_YIQ(float r, float g, float b, float &y, float &i, float &q) {
  y = 0.299f * r + 0.587f * g + 0.114f * b;
  i = 0.5959f * r - 0.2746f * g - 0.3213f * b;
  q = 0.2115f * r - 0.5227f * g + 0.3112f * b;
}

static inline void YIQ_to_RGB(float y, float i, float q, float &r, float &g, float &b) {
  r = y + 0.95605f * i + 0.62075f * q;
  g = y - 0.27205f * i - 0.64721f * q;
  b = y - 1.10670f * i + 1.70442f * q;
}

// 8-bit ARGB Pixel Rendering Pipeline
static PF_Err RenderPixel8(PF_EffectWorld *input, PF_EffectWorld *output, const RenderContext &ctx) {
  const int32_t w = ctx.width;
  const int32_t h = ctx.height;
  const int32_t srcPitch = input->rowbytes / sizeof(PF_Pixel8);
  const int32_t dstPitch = output->rowbytes / sizeof(PF_Pixel8);
  const PF_Pixel8 *srcBase = reinterpret_cast<const PF_Pixel8 *>(input->data);
  PF_Pixel8 *dstBase = reinterpret_cast<PF_Pixel8 *>(output->data);

  // If Master Strength is zero: bit-exact identity bypass
  if (ctx.masterStrength <= 0.0001f) {
    for (int32_t y = 0; y < h; ++y) {
      std::memcpy(dstBase + y * dstPitch, srcBase + y * srcPitch, w * sizeof(PF_Pixel8));
    }
    return PF_Err_NONE;
  }

  const float drive = ctx.masterStrength;
  const double t = ctx.timeSec;
  const uint32_t seed = ctx.seed;

  // Compute Moving Bands geometry
  float band1_y = -100.0f;
  float band2_y = -100.0f;
  if (ctx.damageEnable && ctx.movingBandAmount > 0.01f) {
    float speed = ctx.bandSpeed;
    if (ctx.bandDirection == DIR_UP) speed = -std::abs(speed);
    else if (ctx.bandDirection == DIR_DOWN) speed = std::abs(speed);
    else speed = std::sin(static_cast<float>(t) * 1.5f) * std::abs(speed);

    band1_y = static_cast<float>(std::fmod(t * speed, h + ctx.bandHeight * 2.0));
    if (band1_y < -ctx.bandHeight) band1_y += (h + ctx.bandHeight * 2.0f);
    if (ctx.multiRegions) {
      band2_y = static_cast<float>(std::fmod(t * (speed * 0.7) + h * 0.5, h + ctx.bandHeight * 2.0));
      if (band2_y < -ctx.bandHeight) band2_y += (h + ctx.bandHeight * 2.0f);
    }
  }

  // Intermittent signal loss calculation
  bool inSignalLoss = false;
  if (ctx.trackingEnable && ctx.syncLoss > 0.1f) {
    float lossHash = VHS_HashFloat(seed ^ static_cast<uint32_t>(t * 4.0));
    if (lossHash < (ctx.syncLoss * 0.003f * drive)) inSignalLoss = true;
  }

  // Static burst calculation
  bool inStaticBurst = false;
  if (ctx.damageEnable && ctx.staticBurstRate > 0.1f) {
    float burstHash = VHS_HashFloat(seed + 999 ^ static_cast<uint32_t>(t * 8.0));
    if (burstHash < (ctx.staticBurstRate * 0.004f * drive)) inStaticBurst = true;
  }

  // Vertical roll jitter (V-hold instability)
  int32_t vRollOffset = 0;
  if (ctx.trackingEnable && ctx.frameJitter > 0.1f) {
    float fj = VHS_HashFloat(seed + 101 ^ static_cast<uint32_t>(t * 30.0)) - 0.5f;
    vRollOffset = static_cast<int32_t>(fj * ctx.frameJitter * 0.35f * drive);
  }

  // Temporal Hue Drift
  float huePhase = static_cast<float>(t * (ctx.colorShiftSpeed * 0.02f));
  float hueCos = std::cos(huePhase);
  float hueSin = std::sin(huePhase);
  float hueScale = (ctx.colorShiftAmount * 0.005f * drive);

  // Line-by-Line Processing
  for (int32_t y = 0; y < h; ++y) {
    int32_t mappedY = std::clamp(y + vRollOffset, 0, h - 1);
    const PF_Pixel8 *srcRow = srcBase + mappedY * srcPitch;
    PF_Pixel8 *dstRow = dstBase + y * dstPitch;

    // Scanline horizontal jitter & tape alignment wobble
    float lineJitter = 0.0f;
    if (ctx.trackingEnable) {
      if (ctx.hJitter > 0.1f) {
        float jHash = VHS_Hash2D(y, static_cast<int32_t>(t * 60.0), seed) - 0.5f;
        lineJitter += jHash * (ctx.hJitter * 0.25f * drive);
      }
      if (ctx.alignmentInstability > 0.1f) {
        lineJitter += std::sin(y * 0.05f + static_cast<float>(t) * 3.0f) * (ctx.alignmentInstability * 0.08f * drive);
      }
      if (ctx.trackingMode == TRACKING_BAD) {
        lineJitter += (VHS_Hash2D(y / 4, static_cast<int32_t>(t * 30.0), seed + 50) - 0.5f) * (ctx.trackingSeverity * 12.0f * drive);
      } else if (ctx.trackingMode == TRACKING_VERY_BAD) {
        lineJitter += (VHS_Hash2D(y / 2, static_cast<int32_t>(t * 60.0), seed + 50) - 0.5f) * (ctx.trackingSeverity * 35.0f * drive);
      }
    }

    // Bottom Head-Switch Damage Check
    bool inHeadSwitch = false;
    float headSwitchFactor = 0.0f;
    float hsBoundary = static_cast<float>(h) - ctx.bottomDamageHeight;
    if (ctx.damageEnable && static_cast<float>(y) >= hsBoundary) {
      inHeadSwitch = true;
      headSwitchFactor = (static_cast<float>(y) - hsBoundary) / std::max(1.0f, ctx.bottomDamageHeight);
      lineJitter += std::pow(headSwitchFactor, 2.0f) * (ctx.bottomDamageAmount * 30.0f * drive);
    }

    // Moving Tape Band Distortion Check
    bool inMovingBand = false;
    float bandFactor = 0.0f;
    auto checkBand = [&](float bY) {
      if (bY > -50.0f && static_cast<float>(y) >= bY && static_cast<float>(y) <= bY + ctx.bandHeight) {
        inMovingBand = true;
        float dY = (static_cast<float>(y) - bY) / ctx.bandHeight;
        bandFactor = std::max(bandFactor, std::sin(dY * 3.14159f));
      }
    };
    checkBand(band1_y);
    checkBand(band2_y);

    if (inMovingBand) {
      lineJitter += (VHS_Hash2D(y, static_cast<int32_t>(t * 60.0), seed + 202) - 0.5f)
                    * (ctx.localizedDistortion * 25.0f * bandFactor * drive);
    }

    int32_t roundedJitter = static_cast<int32_t>(std::round(lineJitter));

    // Pixel processing across scanline
    for (int32_t x = 0; x < w; ++x) {
      const PF_Pixel8 &origPix = srcRow[x];

      // Displaced source coordinate for geometry jitter
      int32_t sx = std::clamp(x + roundedJitter, 0, w - 1);
      const PF_Pixel8 &basePix = srcRow[sx];

      // Chroma delay offset coordinates
      int32_t cx = std::clamp(sx + static_cast<int32_t>(std::round(ctx.chromaOffsetX * drive)), 0, w - 1);
      int32_t cy = std::clamp(mappedY + static_cast<int32_t>(std::round(ctx.chromaOffsetY * drive)), 0, h - 1);
      const PF_Pixel8 &chromaPix = srcBase[cy * srcPitch + cx];

      // Color Channel Offsets (Chromatic Aberration / Phase Drift)
      int32_t rx = std::clamp(sx + static_cast<int32_t>(std::round(ctx.channelOffsetRed * drive)), 0, w - 1);
      int32_t bx = std::clamp(sx + static_cast<int32_t>(std::round(ctx.channelOffsetBlue * drive)), 0, w - 1);
      float rVal = static_cast<float>(srcRow[rx].red);
      float gVal = static_cast<float>(basePix.green);
      float bVal = static_cast<float>(srcRow[bx].blue);

      // Convert to YIQ color domain for authentic analog composite processing
      float lumaY, chromaI, chromaQ;
      RGB_to_YIQ(rVal, gVal, bVal, lumaY, chromaI, chromaQ);

      // Chroma low-pass filter / color bleeding
      if (ctx.signalEnable && (ctx.chromaBleed > 0.1f || ctx.chromaBlur > 0.1f)) {
        float bleedTap = (ctx.chromaBleed + ctx.chromaBlur) * 0.08f * drive;
        int32_t bleedX1 = std::clamp(cx - 2, 0, w - 1);
        int32_t bleedX2 = std::clamp(cx + 2, 0, w - 1);
        float y1, i1, q1, y2, i2, q2;
        RGB_to_YIQ(srcRow[bleedX1].red, srcRow[bleedX1].green, srcRow[bleedX1].blue, y1, i1, q1);
        RGB_to_YIQ(srcRow[bleedX2].red, srcRow[bleedX2].green, srcRow[bleedX2].blue, y2, i2, q2);
        chromaI = (chromaI * 0.6f) + ((i1 + i2) * 0.2f * std::min(1.0f, bleedTap));
        chromaQ = (chromaQ * 0.6f) + ((q1 + q2) * 0.2f * std::min(1.0f, bleedTap));
      }

      // Luma bandwidth roll-off (analog softness)
      if (ctx.signalEnable && ctx.lumaSoftness > 0.1f) {
        int32_t lx1 = std::clamp(sx - 1, 0, w - 1);
        int32_t lx2 = std::clamp(sx + 1, 0, w - 1);
        float ly1, li1, lq1, ly2, li2, lq2;
        RGB_to_YIQ(srcRow[lx1].red, srcRow[lx1].green, srcRow[lx1].blue, ly1, li1, lq1);
        RGB_to_YIQ(srcRow[lx2].red, srcRow[lx2].green, srcRow[lx2].blue, ly2, li2, lq2);
        float softFactor = std::min(0.5f, ctx.lumaSoftness * 0.005f * drive);
        lumaY = lumaY * (1.0f - softFactor * 2.0f) + (ly1 + ly2) * softFactor;
      }

      // Edge ringing / overshoot simulation
      if (ctx.signalEnable && ctx.edgeRinging > 0.1f && x > 2) {
        float prevLuma, pi, pq;
        RGB_to_YIQ(srcRow[sx - 2].red, srcRow[sx - 2].green, srcRow[sx - 2].blue, prevLuma, pi, pq);
        float delta = lumaY - prevLuma;
        lumaY += delta * (ctx.edgeRinging * 0.004f * drive) * std::sin(x * 1.8f);
      }

      // Color Hue drift rotation
      if (ctx.colorShiftAmount > 0.1f) {
        float rotI = chromaI * hueCos - chromaQ * hueSin;
        float rotQ = chromaI * hueSin + chromaQ * hueCos;
        chromaI = chromaI * (1.0f - hueScale) + rotI * hueScale;
        chromaQ = chromaQ * (1.0f - hueScale) + rotQ * hueScale;
      }

      // Saturation control
      chromaI *= ctx.saturation;
      chromaQ *= ctx.saturation;

      // Convert back to RGB
      float outR, outG, outB;
      YIQ_to_RGB(lumaY, chromaI, chromaQ, outR, outG, outB);

      // Camera Warmth Bias & Pedestal Black Level
      outR += ctx.cameraWarmth * 0.15f * drive;
      outB -= ctx.cameraWarmth * 0.15f * drive;
      outR += ctx.blackLevel * drive;
      outG += ctx.blackLevel * drive;
      outB += ctx.blackLevel * drive;

      // RF Multipath Ghost Echoes
      if (ctx.ghostingEnable && ctx.ghostingAmount > 0.01f) {
        int32_t gx = std::clamp(sx - static_cast<int32_t>(ctx.ghostingOffset), 0, w - 1);
        float gWeight = ctx.ghostingAmount * drive;
        outR = outR * (1.0f - gWeight * 0.4f) + static_cast<float>(srcRow[gx].red) * (gWeight * 0.4f);
        outG = outG * (1.0f - gWeight * 0.4f) + static_cast<float>(srcRow[gx].green) * (gWeight * 0.4f);
        outB = outB * (1.0f - gWeight * 0.4f) + static_cast<float>(srcRow[gx].blue) * (gWeight * 0.4f);
      }

      // Procedural Noise System (Analog Tape Grain, Luma & Chroma Noise)
      if (ctx.noiseEnable) {
        float normLuma = std::clamp(lumaY / 255.0f, 0.0f, 1.0f);
        float shadowWeight = (1.0f - normLuma) * ctx.noiseShadowBias + (1.0f - ctx.noiseShadowBias);

        int32_t gScale = std::max(1, static_cast<int32_t>(ctx.grainSize));
        float pNoise = VHS_Hash2D(x / gScale, (y + static_cast<int32_t>(t * 60.0)) / gScale, seed + 777) - 0.5f;
        float grainAdd = pNoise * (ctx.grainAmount * 0.4f * drive * shadowWeight);

        float lNoise = (VHS_Hash2D(x, y, seed + static_cast<uint32_t>(t * 120.0)) - 0.5f)
                       * (ctx.noiseLumaAmount * 0.35f * drive * shadowWeight);
        outR += grainAdd + lNoise;
        outG += grainAdd + lNoise;
        outB += grainAdd + lNoise;

        if (ctx.noiseChromaAmount > 0.1f) {
          float cNoiseR = (VHS_Hash2D(x / 2, y, seed + 111) - 0.5f) * (ctx.noiseChromaAmount * 0.35f * drive);
          float cNoiseB = (VHS_Hash2D(x / 2, y, seed + 222) - 0.5f) * (ctx.noiseChromaAmount * 0.35f * drive);
          outR += cNoiseR;
          outB += cNoiseB;
        }
      }

      // Oxide Dropout Streaks (Horizontal white/black speckles)
      if (ctx.damageEnable && ctx.dropoutAmount > 0.1f) {
        float dropHash = VHS_Hash2D(x / static_cast<int32_t>(ctx.dropoutLength), y, seed + static_cast<uint32_t>(t * 60.0));
        if (dropHash < (ctx.dropoutAmount * 0.00035f * drive)) {
          float dropCol = (dropHash > (ctx.dropoutAmount * 0.00015f * drive)) ? 245.0f : 15.0f;
          outR = dropCol; outG = dropCol; outB = dropCol;
        }
      }

      // Tape Band Snow inside damaged regions
      if (inMovingBand && bandFactor > 0.4f) {
        float snowHash = VHS_Hash2D(x, y, seed + static_cast<uint32_t>(t * 240.0));
        if (snowHash > 0.55f) {
          float snowVal = snowHash * 255.0f;
          outR = outR * 0.3f + snowVal * 0.7f;
          outG = outG * 0.3f + snowVal * 0.7f;
          outB = outB * 0.3f + snowVal * 0.7f;
        }
      }

      // Bottom Head-Switch Snow
      if (inHeadSwitch) {
        float hsSnowHash = VHS_Hash2D(x, y, seed + static_cast<uint32_t>(t * 240.0) + 55);
        if (hsSnowHash > (1.0f - headSwitchFactor * 0.7f)) {
          float hsVal = hsSnowHash * 255.0f;
          outR = hsVal; outG = hsVal; outB = hsVal;
        }
      }

      // Intermittent Signal Loss / Static Bursts
      if (inSignalLoss) {
        outR = outR * 0.1f + 180.0f * 0.9f; // VCR Blue screen tint or drop
        outG = outG * 0.1f + 30.0f * 0.9f;
        outB = outB * 0.1f + 20.0f * 0.9f;
      }
      if (inStaticBurst) {
        float burstNoise = VHS_Hash2D(x, y, seed + static_cast<uint32_t>(t * 300.0)) * 255.0f;
        outR = burstNoise; outG = burstNoise; outB = burstNoise;
      }

      // Final Mix with Original Footage & Clamping
      float finalR = origPix.red * (1.0f - ctx.mixOriginal) + outR * ctx.mixOriginal;
      float finalG = origPix.green * (1.0f - ctx.mixOriginal) + outG * ctx.mixOriginal;
      float finalB = origPix.blue * (1.0f - ctx.mixOriginal) + outB * ctx.mixOriginal;

      dstRow[x].alpha = origPix.alpha;
      dstRow[x].red   = static_cast<A_u_char>(std::clamp(std::round(finalR), 0.0f, 255.0f));
      dstRow[x].green = static_cast<A_u_char>(std::clamp(std::round(finalG), 0.0f, 255.0f));
      dstRow[x].blue  = static_cast<A_u_char>(std::clamp(std::round(finalB), 0.0f, 255.0f));
    }
  }

  return PF_Err_NONE;
}

// 16-bit ARGB Pixel Rendering Pipeline
static PF_Err RenderPixel16(PF_EffectWorld *input, PF_EffectWorld *output, const RenderContext &ctx) {
  const int32_t w = ctx.width;
  const int32_t h = ctx.height;
  const int32_t srcPitch = input->rowbytes / sizeof(PF_Pixel16);
  const int32_t dstPitch = output->rowbytes / sizeof(PF_Pixel16);
  const PF_Pixel16 *srcBase = reinterpret_cast<const PF_Pixel16 *>(input->data);
  PF_Pixel16 *dstBase = reinterpret_cast<PF_Pixel16 *>(output->data);

  if (ctx.masterStrength <= 0.0001f) {
    for (int32_t y = 0; y < h; ++y) {
      std::memcpy(dstBase + y * dstPitch, srcBase + y * srcPitch, w * sizeof(PF_Pixel16));
    }
    return PF_Err_NONE;
  }

  const float drive = ctx.masterStrength;
  const double t = ctx.timeSec;
  const uint32_t seed = ctx.seed;

  for (int32_t y = 0; y < h; ++y) {
    const PF_Pixel16 *srcRow = srcBase + y * srcPitch;
    PF_Pixel16 *dstRow = dstBase + y * dstPitch;

    float lineJitter = 0.0f;
    if (ctx.trackingEnable && ctx.hJitter > 0.1f) {
      lineJitter = (VHS_Hash2D(y, static_cast<int32_t>(t * 60.0), seed) - 0.5f) * (ctx.hJitter * 0.25f * drive);
    }
    int32_t sx = std::clamp(static_cast<int32_t>(lineJitter), -w + 1, w - 1);

    for (int32_t x = 0; x < w; ++x) {
      int32_t px = std::clamp(x + sx, 0, w - 1);
      const PF_Pixel16 &p = srcRow[px];

      float r = static_cast<float>(p.red) / 128.0f; // Scale 0-32768 to 0-255 range for math
      float g = static_cast<float>(p.green) / 128.0f;
      float b = static_cast<float>(p.blue) / 128.0f;

      float lumaY, chromaI, chromaQ;
      RGB_to_YIQ(r, g, b, lumaY, chromaI, chromaQ);

      if (ctx.signalEnable && ctx.lumaSoftness > 0.1f && x > 0 && x < w - 1) {
        float rL = static_cast<float>(srcRow[std::clamp(px - 1, 0, w - 1)].red) / 128.0f;
        float gL = static_cast<float>(srcRow[std::clamp(px - 1, 0, w - 1)].green) / 128.0f;
        float bL = static_cast<float>(srcRow[std::clamp(px - 1, 0, w - 1)].blue) / 128.0f;
        float yL, iL, qL;
        RGB_to_YIQ(rL, gL, bL, yL, iL, qL);
        lumaY = lumaY * 0.7f + yL * 0.3f;
      }

      float outR, outG, outB;
      YIQ_to_RGB(lumaY, chromaI * ctx.saturation, chromaQ * ctx.saturation, outR, outG, outB);

      outR = (static_cast<float>(srcRow[x].red) / 128.0f) * (1.0f - ctx.mixOriginal) + outR * ctx.mixOriginal;
      outG = (static_cast<float>(srcRow[x].green) / 128.0f) * (1.0f - ctx.mixOriginal) + outG * ctx.mixOriginal;
      outB = (static_cast<float>(srcRow[x].blue) / 128.0f) * (1.0f - ctx.mixOriginal) + outB * ctx.mixOriginal;

      dstRow[x].alpha = p.alpha;
      dstRow[x].red   = static_cast<A_u_short>(std::clamp(std::round(outR * 128.0f), 0.0f, 32768.0f));
      dstRow[x].green = static_cast<A_u_short>(std::clamp(std::round(outG * 128.0f), 0.0f, 32768.0f));
      dstRow[x].blue  = static_cast<A_u_short>(std::clamp(std::round(outB * 128.0f), 0.0f, 32768.0f));
    }
  }

  return PF_Err_NONE;
}

// 32-bit Float Pixel Rendering Pipeline
static PF_Err RenderPixel32(PF_EffectWorld *input, PF_EffectWorld *output, const RenderContext &ctx) {
  const int32_t w = ctx.width;
  const int32_t h = ctx.height;
  const int32_t srcPitch = input->rowbytes / sizeof(PF_PixelFloat);
  const int32_t dstPitch = output->rowbytes / sizeof(PF_PixelFloat);
  const PF_PixelFloat *srcBase = reinterpret_cast<const PF_PixelFloat *>(input->data);
  PF_PixelFloat *dstBase = reinterpret_cast<PF_PixelFloat *>(output->data);

  if (ctx.masterStrength <= 0.0001f) {
    for (int32_t y = 0; y < h; ++y) {
      std::memcpy(dstBase + y * dstPitch, srcBase + y * srcPitch, w * sizeof(PF_PixelFloat));
    }
    return PF_Err_NONE;
  }

  const float drive = ctx.masterStrength;
  const double t = ctx.timeSec;
  const uint32_t seed = ctx.seed;

  for (int32_t y = 0; y < h; ++y) {
    const PF_PixelFloat *srcRow = srcBase + y * srcPitch;
    PF_PixelFloat *dstRow = dstBase + y * dstPitch;

    float lineJitter = 0.0f;
    if (ctx.trackingEnable && ctx.hJitter > 0.1f) {
      lineJitter = (VHS_Hash2D(y, static_cast<int32_t>(t * 60.0), seed) - 0.5f) * (ctx.hJitter * 0.25f * drive);
    }
    int32_t sx = std::clamp(static_cast<int32_t>(lineJitter), -w + 1, w - 1);

    for (int32_t x = 0; x < w; ++x) {
      int32_t px = std::clamp(x + sx, 0, w - 1);
      const PF_PixelFloat &p = srcRow[px];

      float r = p.red * 255.0f;
      float g = p.green * 255.0f;
      float b = p.blue * 255.0f;

      float lumaY, chromaI, chromaQ;
      RGB_to_YIQ(r, g, b, lumaY, chromaI, chromaQ);

      float outR, outG, outB;
      YIQ_to_RGB(lumaY, chromaI * ctx.saturation, chromaQ * ctx.saturation, outR, outG, outB);

      outR = srcRow[x].red * (1.0f - ctx.mixOriginal) + (outR / 255.0f) * ctx.mixOriginal;
      outG = srcRow[x].green * (1.0f - ctx.mixOriginal) + (outG / 255.0f) * ctx.mixOriginal;
      outB = srcRow[x].blue * (1.0f - ctx.mixOriginal) + (outB / 255.0f) * ctx.mixOriginal;

      dstRow[x].alpha = p.alpha;
      dstRow[x].red   = std::clamp(outR, 0.0f, 1.0f);
      dstRow[x].green = std::clamp(outG, 0.0f, 1.0f);
      dstRow[x].blue  = std::clamp(outB, 0.0f, 1.0f);
    }
  }

  return PF_Err_NONE;
}

// Audio Render Callback (PF_Cmd_AUDIO_RENDER)
static PF_Err RenderAudio(PF_InData *in_data, PF_ParamDef *params[], PF_SoundWorld *sound) {
  if (!sound || sound->num_samples <= 0 || !sound->data[0]) return PF_Err_NONE;
  if (!params[P_AUDIO_ENABLE]->u.bd.value) return PF_Err_NONE;

  float wet = static_cast<float>(params[P_AUDIO_WET_DRY]->u.sd.value) / 100.0f;
  float satDrive = 1.0f + static_cast<float>(params[P_AUDIO_SATURATION]->u.sd.value) * 0.03f;
  int32_t qBits = std::clamp(params[P_AUDIO_QUANTIZATION]->u.sd.value, 4, 16);
  float qLevels = static_cast<float>(1 << qBits);
  float crackleRate = static_cast<float>(params[P_AUDIO_CRACKLE]->u.sd.value) * 0.0002f;
  float dropoutRate = static_cast<float>(params[P_AUDIO_DROPOUT]->u.sd.value) * 0.0001f;

  int32_t targetRate = std::clamp(params[P_AUDIO_DOWNSAMPLE_RATE]->u.sd.value, 4000, 48000);
  int32_t nativeRate = (sound->rate > 0) ? sound->rate : 48000;
  int32_t holdStep = std::max(1, nativeRate / targetRate);

  for (int32_t ch = 0; ch < sound->num_channels; ++ch) {
    PF_FpShort *samples = sound->data[ch];
    float heldSample = 0.0f;
    for (int32_t i = 0; i < sound->num_samples; ++i) {
      if (i % holdStep == 0) {
        heldSample = samples[i];
      }
      float orig = samples[i];
      float s = heldSample;

      // Tape saturation: non-linear tanh compression
      s = std::tanh(s * satDrive);

      // Bitcrush Quantization
      s = std::round(s * qLevels) / qLevels;

      // Magnetic Crackle Noise
      float rHash = VHS_HashFloat(i ^ (ch * 1000));
      if (rHash < crackleRate) {
        s += (VHS_HashFloat(i + 500) - 0.5f) * 0.8f;
      }

      // Synchronized dropout
      if (rHash < dropoutRate) {
        s = 0.0f;
      }

      // Wet/Dry mix
      samples[i] = orig * (1.0f - wet) + s * wet;
    }
  }

  return PF_Err_NONE;
}

// Master EffectMain Entry Point
extern "C" PF_Err EffectMain(
  PF_Cmd          cmd,
  PF_InData       *in_data,
  PF_OutData      *out_data,
  PF_ParamDef     *params[],
  PF_LayerDef     *output,
  void            *extra
) {
  PF_Err err = PF_Err_NONE;

  try {
    switch (cmd) {
      case PF_Cmd_ABOUT:
        err = About(in_data, out_data, params, output);
        break;

      case PF_Cmd_GLOBAL_SETUP:
        err = GlobalSetup(in_data, out_data, params, output);
        break;

      case PF_Cmd_PARAMS_SETUP:
        err = ParamsSetup(in_data, out_data, params, output);
        break;

      case PF_Cmd_RENDER: {
        if (!in_data || !params || !params[P_INPUT] || !output) return PF_Err_BAD_CALLBACK_PARAM;

        RenderContext ctx;
        std::memset(&ctx, 0, sizeof(ctx));
        ctx.width    = output->width;
        ctx.height   = output->height;
        ctx.rowBytes = output->rowbytes;

        PopulateContext(in_data, params, ctx);

        PF_EffectWorld *input = reinterpret_cast<PF_EffectWorld *>(&params[P_INPUT]->u);

        // Dispatch based on layer data format
        if (output->data_type == 0 || output->rowbytes == output->width * sizeof(PF_Pixel8)) {
          err = RenderPixel8(input, output, ctx);
        } else if (output->data_type == 1) { // 16-bit
          err = RenderPixel16(input, output, ctx);
        } else if (output->data_type == 2) { // 32-bit float
          err = RenderPixel32(input, output, ctx);
        } else {
          err = RenderPixel8(input, output, ctx);
        }
        break;
      }

      case PF_Cmd_AUDIO_RENDER:
        if (extra) {
          PF_SoundWorld *sound = reinterpret_cast<PF_SoundWorld *>(extra);
          err = RenderAudio(in_data, params, sound);
        }
        break;

      default:
        break;
    }
  } catch (...) {
    err = PF_Err_INTERNAL_STRUCT_DAMAGED;
  }

  return err;
}

// PluginDataEntryFunction2 (AE 23.x+ Entry Point)
extern "C" PF_Err PluginDataEntryFunction2(
  void *in_data,
  void *out_data,
  void *cmd,
  void *extra
) {
  return PF_Err_NONE;
}
