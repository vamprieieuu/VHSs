#pragma once
#ifndef VHS6RIFT_H
#define VHS6RIFT_H

#include <cstdint>
#include <cmath>
#include <cstring>
#include <algorithm>

#define MAJOR_VERSION   2
#define MINOR_VERSION   0
#define BUG_VERSION     0
#define STAGE_VERSION   1 // PF_Stage_RELEASE
#define BUILD_VERSION   1

#define EFFECT_NAME         "VHS6RIFT"
#define EFFECT_MATCH_NAME   "com.vhs6rift.effect"
#define EFFECT_CATEGORY     "Analog & Tape Effects"
#define EFFECT_DESCRIPTION  "VHS6RIFT: Professional Procedural VHS, Analog Signal, & VCR Degradation Engine"

#if defined(__has_include)
  #if __has_include("AE_Effect.h")
    #include "AEConfig.h"
    #include "entry.h"
    #include "AE_Effect.h"
    #include "AE_EffectCB.h"
    #include "AE_EffectCBSuites.h"
    #include "AEFX_SuiteHandlerTemplate.h"
    #include "AE_Macros.h"
    #include "Param_Utils.h"
    #include "String_Utils.h"
    #define VHS6RIFT_HAS_OFFICIAL_AE_SDK 1
  #endif
#endif

#ifndef VHS6RIFT_HAS_OFFICIAL_AE_SDK
  #define VHS6RIFT_HAS_OFFICIAL_AE_SDK 0

  typedef int32_t       PF_Err;
  typedef int32_t       PF_Cmd;
  typedef int32_t       PF_Boolean;
  typedef uint8_t       A_u_char;
  typedef uint16_t      A_u_short;
  typedef float         PF_FpShort;
  typedef double        PF_FpLong;
  typedef int32_t       A_long;
  typedef uint32_t      A_u_long;
  typedef char          A_char;

  #define PF_Err_NONE                    0
  #define PF_Err_OUT_OF_MEMORY           512
  #define PF_Err_INTERNAL_STRUCT_DAMAGED 513
  #define PF_Err_INVALID_INDEX           514
  #define PF_Err_UNRECOGNIZED_PARAM_TYPE 515
  #define PF_Err_BAD_CALLBACK_PARAM      518

  #define PF_Cmd_ABOUT                   0
  #define PF_Cmd_GLOBAL_SETUP            1
  #define PF_Cmd_PARAMS_SETUP            2
  #define PF_Cmd_SEQUENCE_SETUP          3
  #define PF_Cmd_SEQUENCE_SETDOWN        4
  #define PF_Cmd_SEQUENCE_RESETUP        5
  #define PF_Cmd_RENDER                  6
  #define PF_Cmd_FRAME_SETUP             7
  #define PF_Cmd_FRAME_SETDOWN           8
  #define PF_Cmd_AUDIO_RENDER            23
  #define PF_Cmd_SMART_PRE_RENDER        42
  #define PF_Cmd_SMART_RENDER            43

  #define PF_OutFlag_NONE                0
  #define PF_OutFlag_DEEP_COLOR_AWARE    (1 << 2)
  #define PF_OutFlag_WIDE_TIME_INPUT     (1 << 6)
  #define PF_OutFlag_AUDIO_EFFECT_TOO    (1 << 1)
  #define PF_OutFlag_AUDIO_FLOAT_ONLY    (1 << 30)
  #define PF_OutFlag2_NONE               0
  #define PF_OutFlag2_SUPPORTS_THREADED_RENDERING (1 << 26)

  #define PF_Param_SLIDER                1
  #define PF_Param_CHECKBOX              2
  #define PF_Param_COLOR                 3
  #define PF_Param_POPUP                 4
  #define PF_Param_GROUP_START           10
  #define PF_Param_GROUP_END             11
  #define PF_Param_FLOAT_SLIDER          17

  #define PF_ValueDisplayFlag_NONE       0
  #define PF_ValueDisplayFlag_PERCENT    (1 << 0)

  #define PF_Stage_DEVELOP               0
  #define PF_Stage_RELEASE               1

  #define PF_VERSION(MAJ, MIN, BUG, STG, BLD) \
    ((((MAJ) & 0xFF) << 24) | (((MIN) & 0xFF) << 16) | (((BUG) & 0x0F) << 12) | (((STG) & 0x03) << 10) | ((BLD) & 0x03FF))

  #pragma pack(push, 1)
  typedef struct {
    A_u_char alpha, red, green, blue;
  } PF_Pixel8, PF_Pixel;

  typedef struct {
    A_u_short alpha, red, green, blue;
  } PF_Pixel16;

  typedef struct {
    PF_FpShort alpha, red, green, blue;
  } PF_PixelFloat, PF_Pixel32;
  #pragma pack(pop)

  typedef struct {
    A_long    data_type;
    void      *data;
    A_long    width;
    A_long    height;
    A_long    rowbytes;
  } PF_EffectWorld, PF_LayerDef;

  typedef struct {
    A_long    value;
    A_u_long  scale;
  } PF_Time;

  typedef struct {
    PF_Time   current_time;
    PF_Time   time_step;
    PF_Time   total_time;
    A_long    width;
    A_long    height;
    void      *pica_basicP;
    A_long    composite_frame;
  } PF_InData;

  typedef struct {
    A_long    my_version;
    A_long    out_flags;
    A_long    out_flags2;
    A_char    return_msg[256];
  } PF_OutData;

  typedef struct {
    A_long    value;
  } PF_SliderDef;

  typedef struct {
    PF_Boolean value;
  } PF_BooleanDef;

  typedef struct {
    A_long    value;
  } PF_PopupDef;

  typedef struct {
    PF_FpShort value;
  } PF_FloatSliderDef;

  typedef struct {
    A_u_char  red, green, blue, alpha;
  } PF_ColorDef;

  typedef struct {
    A_long    param_type;
    A_char    name[32];
    union {
      PF_SliderDef      sd;
      PF_BooleanDef     bd;
      PF_PopupDef       pd;
      PF_FloatSliderDef fsd;
      PF_ColorDef       cd;
    } u;
  } PF_ParamDef;

  typedef struct {
    A_long      num_samples;
    PF_FpShort  *data[2]; // Stereo L & R channels
    A_long      rate;
    A_long      num_channels;
  } PF_SoundWorld;

  #define AEFX_CLR_STRUCT(s) std::memset(&(s), 0, sizeof(s))
#endif

// Parameter Enum Definitions (100% matched with UI and Render logic)
enum VHS6RIFT_Params {
  P_INPUT = 0,

  // Topic 1: Master Engine Controls
  P_TOPIC_MASTER_START,
  P_MASTER_STRENGTH,          // 0 to 1000 (0 = exact clean identity bypass, 350 = nominal)
  P_MIX_ORIGINAL,             // 0 to 100% blend with original footage
  P_CONTINUOUS_ENABLE,        // Continuous signal treatment toggle
  P_EVENT_ENABLE,             // Event glitches toggle
  P_RANDOM_SEED,              // Deterministic seed 1 - 32767
  P_ANIMATION_SPEED,          // 0 to 500% temporal rate
  P_TOPIC_MASTER_END,

  // Topic 2: Style Presets
  P_TOPIC_PRESETS_START,
  P_STYLE_PRESET,             // 17 distinct choices
  P_PRESET_INTENSITY,         // 0 to 100%
  P_TOPIC_PRESETS_END,

  // Topic 3: Camera & Format Profiles
  P_TOPIC_CAMERA_START,
  P_CAMERA_PROFILE,           // 10 distinct camera models
  P_CAMERA_INTENSITY,         // 0 to 100%
  P_CAMERA_WARMTH,            // -100 to +100 color bias
  P_TOPIC_CAMERA_END,

  // Topic 4: Luma & Chroma Degradation
  P_TOPIC_SIGNAL_START,
  P_SIGNAL_ENABLE,            // Toggle
  P_LUMA_SOFTNESS,            // 0 to 100 horizontal bandwidth roll-off
  P_LUMA_BLUR,                // 0 to 50 general luma blur
  P_CHROMA_BLUR,              // 0 to 50 horizontal chroma lowpass
  P_CHROMA_BLEED,             // 0 to 50 color bleeding smear
  P_CHROMA_OFFSET_X,          // -30 to +30 horizontal chroma delay
  P_CHROMA_OFFSET_Y,          // -30 to +30 vertical chroma misregistration
  P_CHANNEL_OFFSET_RED,       // -20 to +20 red phase displacement
  P_CHANNEL_OFFSET_BLUE,      // -20 to +20 blue phase displacement
  P_COLOR_SHIFT_AMOUNT,       // 0 to 100 hue instability
  P_COLOR_SHIFT_SPEED,        // 0 to 200 temporal color drift
  P_EDGE_RINGING,             // 0 to 100 analog overshoot ringing
  P_SATURATION,               // 0 to 200% color saturation
  P_BLACK_LEVEL,              // -50 to +50 pedestal / shadow lift
  P_TOPIC_SIGNAL_END,

  // Topic 5: Tracking & Temporal Instability
  P_TOPIC_TRACKING_START,
  P_TRACKING_ENABLE,          // Toggle
  P_TRACKING_MODE,            // Normal, Bad Tracking, Very Bad Tracking
  P_TRACKING_SEVERITY,        // 0 to 100%
  P_TRACKING_FREQUENCY,       // 0 to 100
  P_HORIZONTAL_JITTER,        // 0 to 100 scanline jitter
  P_FRAME_JITTER,             // 0 to 100 vertical roll / V-hold jitter
  P_SYNC_LOSS_AMOUNT,         // 0 to 100 intermittent sync loss
  P_ALIGNMENT_INSTABILITY,    // 0 to 100 tape head azimuth wobble
  P_TOPIC_TRACKING_END,

  // Topic 6: Moving Bands & Tape Damage
  P_TOPIC_DAMAGE_START,
  P_DAMAGE_ENABLE,            // Toggle
  P_MOVING_BAND_AMOUNT,       // 0 to 100%
  P_BAND_HEIGHT,              // 2 to 100 pixels
  P_BAND_SPEED,               // -200 to +200 pixels/sec
  P_BAND_DIRECTION,           // Up, Down, Erratic
  P_LOCALIZED_DISTORTION,     // 0 to 100% footage warping inside band
  P_BOTTOM_DAMAGE_AMOUNT,     // 0 to 100% head-switch disturbance
  P_BOTTOM_DAMAGE_HEIGHT,     // 1 to 60 scanlines at bottom
  P_DROPOUT_AMOUNT,           // 0 to 100 oxide dropout streaks
  P_DROPOUT_LENGTH,           // 2 to 30 pixels streak length
  P_STATIC_BURST_RATE,        // 0 to 100 RF static bursts
  P_MULTIPLE_DAMAGE_REGIONS,  // Toggle multiple concurrent bands
  P_TOPIC_DAMAGE_END,

  // Topic 7: Ghosting & RF Multipath Echoes
  P_TOPIC_GHOSTING_START,
  P_GHOSTING_ENABLE,          // Toggle
  P_GHOSTING_AMOUNT,          // 0 to 100%
  P_GHOSTING_OFFSET,          // 1 to 120 pixels horizontal displacement
  P_GHOSTING_DECAY,           // 10 to 90% echo attenuation
  P_SIGNAL_TRAILS,            // 0 to 100% phosphor / ghost trails
  P_TOPIC_GHOSTING_END,

  // Topic 8: Procedural Noise System
  P_TOPIC_NOISE_START,
  P_NOISE_ENABLE,             // Toggle
  P_NOISE_GRAIN_AMOUNT,       // 0 to 100
  P_NOISE_GRAIN_SIZE,         // 1 to 5 scale
  P_NOISE_LUMA_AMOUNT,        // 0 to 100
  P_NOISE_CHROMA_AMOUNT,      // 0 to 100 colored chroma noise
  P_NOISE_DETAIL,             // 0 to 100 fine detail
  P_NOISE_SHADOW_BIAS,        // 0 to 100 weighting in darker regions
  P_TOPIC_NOISE_END,

  // Topic 9: Audio Tape Degradation
  P_TOPIC_AUDIO_START,
  P_AUDIO_ENABLE,             // Toggle
  P_AUDIO_WET_DRY,            // 0 to 100%
  P_AUDIO_SATURATION,         // 0 to 100% non-linear tape drive
  P_AUDIO_QUANTIZATION,       // 4 to 16 bits resolution
  P_AUDIO_DOWNSAMPLE_RATE,    // 4000 to 48000 Hz sample-and-hold
  P_AUDIO_CRACKLE,            // 0 to 100 magnetic crackle
  P_AUDIO_DROPOUT,            // 0 to 100 tape dropout muting
  P_TOPIC_AUDIO_END,

  P_COUNT
};

// Style Presets Enumeration
enum VHS_Style_Presets {
  STYLE_CUSTOM = 1,
  STYLE_CLEAN_VHS,
  STYLE_VINTAGE_VHS,
  STYLE_HEAVY_DEGRADATION,
  STYLE_BAD_TRACKING,
  STYLE_VERY_BAD_TRACKING,
  STYLE_UNSTABLE_VCR,
  STYLE_MOVING_BAND_NOISE,
  STYLE_TAPE_DAMAGE,
  STYLE_BOTTOM_TAPE_DAMAGE,
  STYLE_STATIC_INTERFERENCE,
  STYLE_COLOR_BLEED,
  STYLE_COLOR_SHIFT,
  STYLE_GHOSTING,
  STYLE_SIGNAL_LOSS,
  STYLE_SEVERE_ANALOG_GLITCH,
  STYLE_DAMAGED_TAPE_PLAYBACK,
  STYLE_TOTAL_COUNT = 17
};

// Camera Profiles Enumeration
enum VHS_Camera_Profiles {
  CAM_OFF = 1,
  CAM_JVC_GR_C7U,             // 1986 VHS-C
  CAM_JVC_GR_AX220U,          // 1997 Compact VHS
  CAM_JVC_GR_AX920U,          // 1990s Compact VHS HQ
  CAM_JVC_GR_SXM37US,         // S-VHS-C / S-VHS ET
  CAM_JVC_GR_SXM740U,         // S-VHS-C Flagship
  CAM_SONY_CCD_TR55,          // 1989 Video8
  CAM_JVC_GR_S55,             // S-VHS High-Band
  CAM_PANASONIC_NV_M1,        // 1985 Full-Size VHS
  CAM_RCA_CMR300,             // 1987 Solid State VHS
  CAM_TOTAL_COUNT = 10
};

// Tracking Modes Enumeration
enum VHS_Tracking_Modes {
  TRACKING_NORMAL = 1,
  TRACKING_BAD,
  TRACKING_VERY_BAD
};

// Band Direction Modes
enum VHS_Band_Directions {
  DIR_UP = 1,
  DIR_DOWN,
  DIR_ERRATIC
};

// Render Pipeline Context
struct RenderContext {
  double      timeSec;
  uint32_t    seed;
  int32_t     width;
  int32_t     height;
  int32_t     rowBytes;
  float       masterStrength; // 0.0 to 10.0 (from 0 to 1000 UI)
  float       mixOriginal;    // 0.0 to 1.0
  bool        continuousEnable;
  bool        eventEnable;

  // Active parameter values (post-preset merge)
  int32_t     presetId;
  float       presetIntensity;
  int32_t     cameraId;
  float       cameraIntensity;
  float       cameraWarmth;

  bool        signalEnable;
  float       lumaSoftness;
  float       lumaBlur;
  float       chromaBlur;
  float       chromaBleed;
  float       chromaOffsetX;
  float       chromaOffsetY;
  float       channelOffsetRed;
  float       channelOffsetBlue;
  float       colorShiftAmount;
  float       colorShiftSpeed;
  float       edgeRinging;
  float       saturation;
  float       blackLevel;

  bool        trackingEnable;
  int32_t     trackingMode;
  float       trackingSeverity;
  float       trackingFreq;
  float       hJitter;
  float       frameJitter;
  float       syncLoss;
  float       alignmentInstability;

  bool        damageEnable;
  float       movingBandAmount;
  float       bandHeight;
  float       bandSpeed;
  int32_t     bandDirection;
  float       localizedDistortion;
  float       bottomDamageAmount;
  float       bottomDamageHeight;
  float       dropoutAmount;
  float       dropoutLength;
  float       staticBurstRate;
  bool        multiRegions;

  bool        ghostingEnable;
  float       ghostingAmount;
  float       ghostingOffset;
  float       ghostingDecay;
  float       signalTrails;

  bool        noiseEnable;
  float       grainAmount;
  float       grainSize;
  float       noiseLumaAmount;
  float       noiseChromaAmount;
  float       noiseDetail;
  float       noiseShadowBias;
};

// Fast Deterministic Pseudo-Random Hash Functions (Thread-safe, repeatable)
inline uint32_t VHS_Hash(uint32_t x) {
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = (x >> 16) ^ x;
  return x;
}

inline float VHS_HashFloat(uint32_t x) {
  return static_cast<float>(VHS_Hash(x) & 0x00FFFFFF) / 16777216.0f;
}

inline float VHS_Hash2D(int32_t x, int32_t y, uint32_t seed) {
  uint32_t h = seed ^ (static_cast<uint32_t>(x) * 374761393u) ^ (static_cast<uint32_t>(y) * 668265263u);
  return VHS_HashFloat(h);
}

// Entry Point Signatures
extern "C" {
  #ifdef _WIN32
    __declspec(dllexport)
  #endif
  PF_Err EffectMain(
    PF_Cmd          cmd,
    PF_InData       *in_data,
    PF_OutData      *out_data,
    PF_ParamDef     *params[],
    PF_LayerDef     *output,
    void            *extra
  );

  #ifdef _WIN32
    __declspec(dllexport)
  #endif
  PF_Err PluginDataEntryFunction2(
    void            *in_data,
    void            *out_data,
    void            *cmd,
    void            *extra
  );
}

#endif // VHS6RIFT_H
