#include "AEConfig.h"
#ifdef AE_OS_WIN
  #include <Windows.h>
#endif
// The supplied SDK snapshot does not include entry.h; EffectMain is the explicit
// exported entry point for this module, so define the export decoration locally.
#ifndef DllExport
  #define DllExport __declspec(dllexport)
#endif
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"
#include "String_Utils.h"
#include "ntscrs_c.h"
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <vector>
#include <cstdio>
#include <cstring>

#define PLUGIN_NAME "VHS6RIFT"
#define PLUGIN_MATCH_NAME "com.vhs6rift.effect"
#define PLUGIN_VERSION 1

// Stable parameter IDs. Topic parameters are included in sequence; do not reorder without updating IDs.
enum {
  P_INPUT = 0, P_CAMERA, P_MAIN_ENABLE, P_MASTER, P_CONTINUOUS, P_SEED,
  P_TOPIC_COLOR, P_SATURATION, P_BLACK, P_SOFTNESS, P_CHROMA_BLEED, P_CHROMA_X, P_CHROMA_Y, P_TOPIC_COLOR_END,
  P_TOPIC_GRAIN, P_GRAIN_AMOUNT, P_GRAIN_SCALE, P_GRAIN_PRESET, P_GRAIN_SPEED, P_GRAIN_LINES_ENABLE, P_GRAIN_LINES_AMOUNT, P_GRAIN_LINES_SIZE, P_GRAIN_LINES_SPEED, P_TOPIC_GRAIN_END,
  P_TOPIC_DAMAGE, P_TRACKING_ENABLE, P_TRACKING, P_TRACK_WIDTH, P_EVENT_RATE, P_EVENT_DURATION, P_RECOVERY, P_TOPIC_DAMAGE_END,
  P_TOPIC_OTHER, P_OTHER_STRENGTH,
  P_LEGEND_TOPIC, P_LEGEND_ENABLE, P_LEGEND_BREAKUP, P_LEGEND_TEAR, P_LEGEND_SIGNAL, P_LEGEND_END,
  P_NTSC_TOPIC, P_NTSC_ENABLE, P_NTSC_CHROMA_DELAY, P_NTSC_HEAD_SWITCH, P_NTSC_EDGE_WAVE, P_NTSC_END,
  P_TOPIC_OTHER_END,
  P_TOPIC_AUDIO, P_AUDIO_ENABLE, P_AUDIO_EVENTS, P_AUDIO_MIX, P_AUDIO_EVENT_AMOUNT, P_AUDIO_CRACKLE, P_AUDIO_DROPOUT, P_AUDIO_SATURATION, P_AUDIO_LOFI, P_TOPIC_AUDIO_END,
  P_TOPIC_MODULES, P_OTHER_ENABLE, P_TOPIC_MODULES_END,
  P_TOPIC_ADVANCED, P_ADV_GRAIN_SOFTNESS, P_ADV_GRAIN_MODE, P_ADV_GRAIN_COLOR, P_ADV_GRAIN_RED, P_ADV_GRAIN_GREEN, P_ADV_GRAIN_BLUE,
  P_ADV_LOWPASS_TYPE, P_ADV_SHARPEN, P_ADV_CHROMA_DEMOD, P_ADV_VERTICAL_NOISE, P_ADV_DOT_NOISE, P_ADV_NOISE_COLOR,
  P_ADV_HEADSWITCH_POS, P_ADV_HEADSWITCH_HEIGHT, P_ADV_HEADSWITCH_MOTION, P_ADV_HEADSWITCH_RANDOM,
  P_ADV_OSD_ENABLE, P_ADV_OSD_MODE, P_ADV_OSD_OPACITY, P_ADV_OSD_COLOR, P_ADV_BUILTIN_OVERLAY_ENABLE, P_ADV_BUILTIN_OVERLAY_STYLE, P_ADV_BUILTIN_OVERLAY_OPACITY, P_ADV_BUILTIN_OVERLAY_SPEED, P_ADV_BUILTIN_OVERLAY_BLEND, P_ADV_OSD_POS_X, P_ADV_OSD_POS_Y, P_ADV_OSD_SCALE, P_TOPIC_ADVANCED_END, P_COUNT
};
static PF_Err About(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output) {
  PF_SPRINTF(out_data->return_msg, "%s 1.2 | Native VHS video processing for After Effects 23.x", PLUGIN_NAME);
  return PF_Err_NONE;
}

static PF_Err GlobalSetup(PF_OutData *out_data) {
  out_data->my_version = PF_VERSION(1, 2, 0, PF_Stage_RELEASE, 1);
  out_data->out_flags = PF_OutFlag_WIDE_TIME_INPUT | PF_OutFlag_NON_PARAM_VARY | PF_OutFlag_AUDIO_EFFECT_TOO | PF_OutFlag_AUDIO_FLOAT_ONLY;
  out_data->out_flags2 = 0;
  return PF_Err_NONE;
}

static PF_Err ParamsSetup(PF_InData *in_data, PF_OutData *out_data) {
  PF_Err err = PF_Err_NONE;
  PF_ParamDef def;

  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("VHS Camera Engine", 5, 1,
    "JVC GR-C7U (1986 VHS-C)|JVC GR-AX220U (1997 VHS-C)|JVC GR-AX920U (1997 VHS-C)|JVC GR-SXM37US (S-VHS-C)|JVC GR-SXM740U (S-VHS-C)", P_CAMERA);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable Main VHS Engine", "Enable", TRUE, 0, P_MAIN_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("VHS Strength", 0, 2000, 0, 2000, 350, PF_Precision_TENTHS, 0, 0, P_MASTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Continuous VHS Signal Treatment", "Enable", TRUE, 0, P_CONTINUOUS);
  AEFX_CLR_STRUCT(def);
  PF_ADD_SLIDER("Random Seed", 0, 9999, 0, 9999, 17, P_SEED);
  AEFX_CLR_STRUCT(def);

  PF_ADD_TOPIC("IMAGE - Color and Chroma", P_TOPIC_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Saturation", 0, 200, 0, 200, 100, PF_Precision_TENTHS, 0, 0, P_SATURATION);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Black Level", -100, 100, -100, 100, 0, PF_Precision_TENTHS, 0, 0, P_BLACK);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Luma Softness", 0, 100, 0, 100, 28, PF_Precision_TENTHS, 0, 0, P_SOFTNESS);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Chroma Bleed", 0, 100, 0, 100, 42, PF_Precision_TENTHS, 0, 0, P_CHROMA_BLEED);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Chroma Offset X", -30, 30, -30, 30, 4, PF_Precision_TENTHS, 0, 0, P_CHROMA_X);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Chroma Offset Y", -20, 20, -20, 20, 0, PF_Precision_TENTHS, 0, 0, P_CHROMA_Y);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_COLOR_END);

  AEFX_CLR_STRUCT(def);

  PF_ADD_TOPIC("IMAGE - Grain and Signal Noise", P_TOPIC_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Grain Amount", 0, 100, 0, 100, 0, PF_Precision_TENTHS, 0, 0, P_GRAIN_AMOUNT);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Grain Size", 1, 100, 1, 100, 1, PF_Precision_TENTHS, 0, 0, P_GRAIN_SCALE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Grain Size Preset", 5, 2, "4mm (Soft / Low Detail)|8mm (Soft)|16mm (Balanced)|32mm (Fine)|64mm (Very Fine)", P_GRAIN_PRESET);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Grain Motion Speed", 0, 400, 0, 400, 100, PF_Precision_TENTHS, 0, 0, P_GRAIN_SPEED);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Composite signal noise (NTSC-RS)", "Enable", FALSE, 0, P_GRAIN_LINES_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Signal Noise Intensity", 0, 100, 0, 100, 0, PF_Precision_TENTHS, 0, 0, P_GRAIN_LINES_AMOUNT);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Signal Noise Frequency", 0, 100, 0, 100, 50, PF_Precision_TENTHS, 0, 0, P_GRAIN_LINES_SIZE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Signal Noise Detail", 1, 5, 1, 5, 1, PF_Precision_TENTHS, 0, 0, P_GRAIN_LINES_SPEED);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_GRAIN_END);

  AEFX_CLR_STRUCT(def);

  PF_ADD_TOPIC("SIGNAL - Tracking and Damage Events", P_TOPIC_DAMAGE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable Tracking Damage", "Enable", FALSE, 0, P_TRACKING_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Tracking Noise", 0, 100, 0, 100, 0, PF_Precision_TENTHS, 0, 0, P_TRACKING);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Tracking Damage Size (% Frame)", 1, 100, 1, 100, 18, PF_Precision_TENTHS, 0, 0, P_TRACK_WIDTH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Events per Minute", 0, 60, 0, 60, 8, PF_Precision_TENTHS, 0, 0, P_EVENT_RATE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Event Duration (seconds)", 0.02, 2.0, 0.02, 2.0, 0.18, PF_Precision_HUNDREDTHS, 0, 0, P_EVENT_DURATION);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Recovery (seconds)", 0, 2.0, 0, 2.0, 0.12, PF_Precision_HUNDREDTHS, 0, 0, P_RECOVERY);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_DAMAGE_END);

  AEFX_CLR_STRUCT(def);

  AEFX_CLR_STRUCT(def);

  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("SIGNAL ENGINES - Alternate Character", P_TOPIC_OTHER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Signal Engines Strength", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_OTHER_STRENGTH);
  AEFX_CLR_STRUCT(def);
  AEFX_CLR_STRUCT(def);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("VHS Legend - Signal Controls", P_LEGEND_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable VHS Legend Engine", "Enable", FALSE, 0, P_LEGEND_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Legend Signal Breakdown", 0, 100, 0, 100, 25, PF_Precision_TENTHS, 0, 0, P_LEGEND_BREAKUP);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Legend Horizontal Tear", 0, 100, 0, 100, 20, PF_Precision_TENTHS, 0, 0, P_LEGEND_TEAR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Legend Signal Loss", 0, 100, 0, 100, 10, PF_Precision_TENTHS, 0, 0, P_LEGEND_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_LEGEND_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("NTSC-RS - Base Signal Controls", P_NTSC_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable NTSC-RS Signal Engine", "Enable", TRUE, 0, P_NTSC_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("NTSC Chroma Delay", 0, 100, 0, 100, 18, PF_Precision_TENTHS, 0, 0, P_NTSC_CHROMA_DELAY);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("NTSC Head Switching", 0, 100, 0, 100, 0, PF_Precision_TENTHS, 0, 0, P_NTSC_HEAD_SWITCH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("NTSC Edge Wave", 0, 100, 0, 100, 0, PF_Precision_TENTHS, 0, 0, P_NTSC_EDGE_WAVE);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_NTSC_END);
  AEFX_CLR_STRUCT(def);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_OTHER_END);

  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("AUDIO - Tape Audio Degradation", P_TOPIC_AUDIO);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Audio Glitch Enable", "Enable", FALSE, 0, P_AUDIO_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Audio Event Glitches", "Enable", FALSE, 0, P_AUDIO_EVENTS);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Audio Wet / Dry Mix", 0, 100, 0, 100, 45, PF_Precision_TENTHS, 0, 0, P_AUDIO_MIX);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Audio Event Amount", 0, 100, 0, 100, 55, PF_Precision_TENTHS, 0, 0, P_AUDIO_EVENT_AMOUNT);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Audio Crackle / Static", 0, 100, 0, 100, 18, PF_Precision_TENTHS, 0, 0, P_AUDIO_CRACKLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Audio Dropouts", 0, 100, 0, 100, 10, PF_Precision_TENTHS, 0, 0, P_AUDIO_DROPOUT);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Audio Saturation", 0, 100, 0, 100, 12, PF_Precision_TENTHS, 0, 0, P_AUDIO_SATURATION);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Lo-Fi Quantization", 0, 100, 0, 100, 8, PF_Precision_TENTHS, 0, 0, P_AUDIO_LOFI);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_AUDIO_END);

  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("SYSTEM - Module Enables", P_TOPIC_MODULES);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable Other Signal Engines", "Enable", TRUE, 0, P_OTHER_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_MODULES_END);

  AEFX_CLR_STRUCT(def);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("ADVANCED - Grain, Signal Filters and OSD", P_TOPIC_ADVANCED);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Grain Softness", 0, 100, 0, 100, 35, PF_Precision_TENTHS, 0, 0, P_ADV_GRAIN_SOFTNESS);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Grain Structure", 3, 1, "Luma Grain|Chroma Grain|Full Chroma Grain", P_ADV_GRAIN_MODE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Grain Color Mode", 3, 1, "Camera / Neutral|Monochrome|Manual RGB", P_ADV_GRAIN_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Grain Manual Red", 0, 200, 0, 200, 100, PF_Precision_TENTHS, 0, 0, P_ADV_GRAIN_RED);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Grain Manual Green", 0, 200, 0, 200, 100, PF_Precision_TENTHS, 0, 0, P_ADV_GRAIN_GREEN);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Grain Manual Blue", 0, 200, 0, 200, 100, PF_Precision_TENTHS, 0, 0, P_ADV_GRAIN_BLUE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Lowpass Filter Type", 2, 1, "Constant K - Blurry / Soft|Butterworth - Sharper", P_ADV_LOWPASS_TYPE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Composite Signal Sharpen", 0, 1000, 0, 1000, 100, PF_Precision_TENTHS, 0, 0, P_ADV_SHARPEN);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Chroma Demodulation Filter", 4, 2, "Off|Box Filter|Butterworth|Sharp / Narrow", P_ADV_CHROMA_DEMOD);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Vertical Composite Noise", 0, 100, 0, 100, 0, PF_Precision_TENTHS, 0, 0, P_ADV_VERTICAL_NOISE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Small Circular Dot Noise", 0, 100, 0, 100, 0, PF_Precision_TENTHS, 0, 0, P_ADV_DOT_NOISE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Composite Noise Color", 0, 100, 0, 100, 55, PF_Precision_TENTHS, 0, 0, P_ADV_NOISE_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Head-Switch Position", 0, 100, 0, 100, 92, PF_Precision_TENTHS, 0, 0, P_ADV_HEADSWITCH_POS);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Head-Switch Height", 0, 100, 0, 100, 0, PF_Precision_TENTHS, 0, 0, P_ADV_HEADSWITCH_HEIGHT);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Head-Switch Motion", 0, 100, 0, 100, 35, PF_Precision_TENTHS, 0, 0, P_ADV_HEADSWITCH_MOTION);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Head-Switch Random Bursts", 0, 100, 0, 100, 25, PF_Precision_TENTHS, 0, 0, P_ADV_HEADSWITCH_RANDOM);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Built-in OSD / Text Overlay", "Enable", FALSE, 0, P_ADV_OSD_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("OSD Display", 4, 1, "REC + Timecode|Date Stamp|Time Stamp|Camera Label", P_ADV_OSD_MODE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("OSD Opacity", 0, 100, 0, 100, 85, PF_Precision_TENTHS, 0, 0, P_ADV_OSD_OPACITY);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("OSD Color", 3, 1, "White|Amber|Green", P_ADV_OSD_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable Built-in Signal Overlay", "Enable", FALSE, 0, P_ADV_BUILTIN_OVERLAY_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Built-in Overlay Style", 21, 1, "Tracking Streaks|Head-Switch Band|RGB Tear|Color Bars|Horizontal Sync Lines|Vertical Hold|REC HUD|Date Stamp|Timecode HUD|Camcorder HUD|Dust and Scratches|Static Dots|Crosshair HUD|Edge Burn|CRT Scanlines|Chroma Bars|Tape Wrinkle|Ghost Echo|Signal Dropouts|Blue Tracking Band|Bad Signal Burst", P_ADV_BUILTIN_OVERLAY_STYLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Built-in Overlay Opacity", 0, 100, 0, 100, 55, PF_Precision_TENTHS, 0, 0, P_ADV_BUILTIN_OVERLAY_OPACITY);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Built-in Overlay Speed", 0, 400, 0, 400, 100, PF_Precision_TENTHS, 0, 0, P_ADV_BUILTIN_OVERLAY_SPEED);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Built-in Overlay Blend", 4, 1, "Normal|Add|Screen|Multiply", P_ADV_BUILTIN_OVERLAY_BLEND);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("OSD Position X (%)", 0, 100, 0, 100, 3, PF_Precision_TENTHS, 0, 0, P_ADV_OSD_POS_X);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("OSD Position Y (%)", 0, 100, 0, 100, 4, PF_Precision_TENTHS, 0, 0, P_ADV_OSD_POS_Y);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("OSD Text Scale (%)", 50, 300, 50, 300, 100, PF_Precision_TENTHS, 0, 0, P_ADV_OSD_SCALE);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_ADVANCED_END);

  out_data->num_params = P_COUNT;
  return err;
}

static inline float clamp01(float v) { return std::max(0.0f, std::min(1.0f, v)); }
static inline uint32_t hash32(uint32_t x) {
  x ^= x >> 16; x *= 0x7feb352dU; x ^= x >> 15; x *= 0x846ca68bU; x ^= x >> 16; return x;
}
static inline float rand01(uint32_t x) { return (hash32(x) & 0x00ffffffU) / 16777215.0f; }
static inline float smooth(float a, float b, float x) { float t = clamp01((x-a)/(b-a)); return t*t*(3.0f-2.0f*t); }
static inline int clampi(int v, int a, int b) { return std::max(a, std::min(b, v)); }

struct Settings {
  int seed=17;
  float master=.35f, continuous=1, saturation=1, black=0, softness=.12f, bleed=.20f;
  float chromaX=2, chromaY=0, grain=.08f, grainScale=28, grainSpeed=1, tracking=.18f, trackWidth=.18f;
  float eventRate=8, eventDuration=.18f, recovery=.12f;
};

static float getSlider(PF_ParamDef *params[], int id, float scale) { return (float)params[id]->u.fs_d.value / scale; }
static int getInt(PF_ParamDef *params[], int id) { return params[id]->u.sd.value; }
static bool getCheck(PF_ParamDef *params[], int id) { return params[id]->u.bd.value != 0; }
static Settings ReadSettings(PF_ParamDef *p[]) {
  Settings s;
  s.master = getSlider(p,P_MASTER,100.0f);
  s.continuous = getCheck(p,P_CONTINUOUS) ? 1.0f : 0.0f;
  s.seed = getInt(p,P_SEED);
  s.saturation = getSlider(p,P_SATURATION,100.0f);
  s.black = getSlider(p,P_BLACK,100.0f);
  s.softness = getSlider(p,P_SOFTNESS,100.0f);
  s.bleed = getSlider(p,P_CHROMA_BLEED,100.0f);
  s.chromaX = getSlider(p,P_CHROMA_X,1.0f);
  s.chromaY = getSlider(p,P_CHROMA_Y,1.0f);
  s.grain = getSlider(p,P_GRAIN_AMOUNT,100.0f);
  s.grainScale = getSlider(p,P_GRAIN_SCALE,1.0f);
  s.grainSpeed = getSlider(p,P_GRAIN_SPEED,100.0f);
  s.tracking = getSlider(p,P_TRACKING,100.0f);
  s.trackWidth = getSlider(p,P_TRACK_WIDTH,100.0f);
  s.eventRate = getSlider(p,P_EVENT_RATE,1.0f);
  s.eventDuration = getSlider(p,P_EVENT_DURATION,1.0f);
  s.recovery = getSlider(p,P_RECOVERY,1.0f);
  return s;
}

struct VhsSignalProfile {
  float saturation, contrast, warm, green, rGain, gGain, bGain, gamma, pedestal, shoulder,
        chromaBleed, chromaOffsetX, chromaOffsetY, edgeRinging, noiseFloor,
        lumaSoftness, timebaseWobble, chromaNoise, trackingInstability;
};
// Five actual JVC VHS-C / S-VHS-C camcorders. Their modes vary bandwidth, line stability,
// chroma delay/bleed, edge response, low-light noise and tracking response—not just color.
// Numerical values are engineering approximations based on documented format/features and era,
// not measurements claimed to have been taken from a lab capture of each camera.
static const VhsSignalProfile kVhsSignalProfiles[5] = {
  // GR-C7U (1986): early CCD VHS-C; visibly soft, warm, broad chroma smear, pronounced servo wander.
  {0.84f,0.91f, 0.105f,-0.020f,1.08f,0.99f,0.88f,1.10f,0.055f,0.025f,0.78f,4.0f,0.5f,0.025f,0.16f,0.56f,1.65f,0.15f,1.00f},
  // GR-AX220U (1997): entry Compact VHS/HQ; softened detail, slightly green/muted mids, moderate jitter.
  {0.91f,0.96f, 0.025f,0.060f,1.00f,1.04f,0.92f,1.07f,0.045f,0.045f,0.58f,3.0f,0.0f,0.035f,0.12f,0.40f,1.10f,0.105f,0.78f},
  // GR-AX920U (1997): stabilizer and higher-end processing; stronger edges, firmer contrast, steadier lines.
  {0.98f,1.15f, 0.015f,0.010f,1.04f,1.00f,0.94f,1.00f,0.018f,0.12f,0.43f,2.0f,0.0f,0.16f,0.075f,0.24f,0.52f,0.065f,0.48f},
  // GR-SXM37US: S-VHS-C / S-VHS ET; higher luma bandwidth and detail, cleaner chroma, visible edge crispness.
  {1.08f,1.12f, 0.008f,-0.008f,1.02f,1.01f,1.00f,0.98f,0.010f,0.16f,0.25f,1.1f,0.0f,0.21f,0.050f,0.13f,0.28f,0.038f,0.26f},
  // GR-SXM740U: later S-VHS-C with TBC/CNR and stabilizer; neutral, clean, low-noise, stable but not VHS-C soft.
  {1.02f,1.04f, -0.005f,0.0f,1.00f,1.00f,1.02f,0.98f,0.005f,0.18f,0.18f,0.7f,0.0f,0.12f,0.025f,0.09f,0.10f,0.018f,0.12f}
};

static PF_Err Render(PF_InData *in_data, PF_ParamDef *params[], PF_LayerDef *output) {
  PF_Err err = PF_Err_NONE;
  PF_EffectWorld *src = &params[P_INPUT]->u.ld;
  if (!src || !src->data || !output || !output->data) return PF_Err_NONE;

  const int w = output->width, h = output->height;
  const int inW = src->width, inH = src->height;
  if (w <= 0 || h <= 0 || inW <= 0 || inH <= 0) return PF_Err_NONE;

  Settings s = ReadSettings(params);
  const PF_Pixel *inBase = (const PF_Pixel*)src->data;
  PF_Pixel *outBase = (PF_Pixel*)output->data;
  int inStride = src->rowbytes / (int)sizeof(PF_Pixel);
  const int outStride = output->rowbytes / (int)sizeof(PF_Pixel);
  if (inStride <= 0 || outStride <= 0) return PF_Err_NONE;

  const float timeSec = in_data->time_scale ? (float)in_data->current_time / (float)in_data->time_scale : 0.0f;
  const int frame = (in_data->time_step != 0) ? (int)(in_data->current_time / in_data->time_step) : (int)std::floor(timeSec * 24.0f + 0.5f);

  // Master VHS Strength (UI 0..2000, default 350 -> s.master 3.5).
  const bool mainVhsEnabled = getCheck(params, P_MAIN_ENABLE);
  const float strength = mainVhsEnabled ? std::min(20.0f, std::max(0.0f, s.master)) : 0.0f;

  // Identity bypass when Main VHS is disabled or Master Strength is zero: source-only output.
  if (strength <= 0.0001f) {
    for (int y = 0; y < h; ++y) {
      const PF_Pixel *srcRow = inBase + clampi((int)((y + 0.5f) * inH / std::max(1, h)), 0, inH - 1) * inStride;
      PF_Pixel *dstRow = outBase + y * outStride;
      for (int x = 0; x < w; ++x) {
        int sx = clampi((int)((x + 0.5f) * inW / std::max(1, w)), 0, inW - 1);
        dstRow[x] = srcRow[sx];
      }
    }
    return PF_Err_NONE;
  }

  const int cameraProfileIndex = clampi(params[P_CAMERA]->u.pd.value - 1, 0, 4);
  const VhsSignalProfile &prof = kVhsSignalProfiles[cameraProfileIndex];

  // NTSC-RS is the base signal path, not an optional effect hidden under
  // "Other VHS Engines". Build a separate, output-sized source buffer first,
  // process it through the real ntsc-rs engine, and then let every VHS6RIFT
  // stage read from that processed buffer. Keeping it separate avoids reading
  // pixels that have already been overwritten by the later VHS render loop.
  std::vector<PF_Pixel> ntscBaseBuffer((size_t)w * (size_t)h);
  for (int y = 0; y < h; ++y) {
    const int sy = clampi((int)((y + 0.5f) * inH / std::max(1, h)), 0, inH - 1);
    const PF_Pixel *srcRow = (const PF_Pixel*)src->data + sy * (src->rowbytes / (int)sizeof(PF_Pixel));
    PF_Pixel *baseRow = ntscBaseBuffer.data() + (size_t)y * (size_t)w;
    for (int x = 0; x < w; ++x) {
      const int sx = clampi((int)((x + 0.5f) * inW / std::max(1, w)), 0, inW - 1);
      baseRow[x] = srcRow[sx];
    }
  }
  NtscrsCParams baseNtscParams;
  baseNtscParams.chroma_delay = getSlider(params, P_NTSC_CHROMA_DELAY, 1.0f);
  baseNtscParams.head_switching = getSlider(params, P_NTSC_HEAD_SWITCH, 1.0f);
  baseNtscParams.edge_wave = getSlider(params, P_NTSC_EDGE_WAVE, 1.0f);
  const bool signalNoiseEnabled = getCheck(params, P_GRAIN_LINES_ENABLE);
  baseNtscParams.composite_noise = signalNoiseEnabled ? getSlider(params, P_GRAIN_LINES_AMOUNT, 100.0f) * 100.0f : 0.0f;
  baseNtscParams.composite_noise_frequency = getSlider(params, P_GRAIN_LINES_SIZE, 100.0f);
  baseNtscParams.composite_noise_detail = (int32_t)std::round(getSlider(params, P_GRAIN_LINES_SPEED, 1.0f));
  const bool trackingEnabledForNtsc = getCheck(params, P_TRACKING_ENABLE) && getSlider(params, P_TRACKING, 100.0f) > 0.001f;
  baseNtscParams.tracking_noise = trackingEnabledForNtsc ? getSlider(params, P_TRACKING, 100.0f) * 100.0f : 0.0f;
  baseNtscParams.tracking_height = getSlider(params, P_TRACK_WIDTH, 100.0f) * 100.0f;
  const bool hasNtscControls = getCheck(params, P_NTSC_ENABLE) && (baseNtscParams.chroma_delay > 0.001f || baseNtscParams.head_switching > 0.001f ||
    baseNtscParams.edge_wave > 0.001f || baseNtscParams.composite_noise > 0.001f || baseNtscParams.tracking_noise > 0.001f);
  baseNtscParams.strength = (getCheck(params, P_CONTINUOUS) && hasNtscControls) ? std::min(10.0f, std::max(0.0f, strength / 3.5f)) : 0.0f;
  baseNtscParams.random_seed = getInt(params, P_SEED);
  baseNtscParams.frame_num = (uint32_t)((in_data->time_step != 0)
      ? (in_data->current_time / in_data->time_step) : 0);
  const int32_t ntscResult = ntscrs_process_argb8(
      &baseNtscParams, (uint8_t*)ntscBaseBuffer.data(), (uint32_t)w,
      (uint32_t)h, (size_t)w * sizeof(PF_Pixel));
  if (ntscResult == 0 && baseNtscParams.strength > 0.0001f) {
    // Blend the NTSC-RS base treatment with the clean source using master drive.
    // This prevents low VHS Strength values from inheriting a full-strength
    // chroma collapse while preserving a complete analog signal at the default.
    const float ntscMix = clamp01(baseNtscParams.strength);
    for (size_t i = 0; i < ntscBaseBuffer.size(); ++i) {
      const int oy = (int)(i / (size_t)w);
      const int ox = (int)(i % (size_t)w);
      const int rawY = clampi((int)((oy + 0.5f) * inH / std::max(1, h)), 0, inH - 1);
      const int rawX = clampi((int)((ox + 0.5f) * inW / std::max(1, w)), 0, inW - 1);
      const PF_Pixel raw = ((const PF_Pixel*)src->data)[rawY * (src->rowbytes / (int)sizeof(PF_Pixel)) + rawX];
      PF_Pixel &fx = ntscBaseBuffer[i];
      fx.red = (uint8_t)std::round(raw.red * (1.0f - ntscMix) + fx.red * ntscMix);
      fx.green = (uint8_t)std::round(raw.green * (1.0f - ntscMix) + fx.green * ntscMix);
      fx.blue = (uint8_t)std::round(raw.blue * (1.0f - ntscMix) + fx.blue * ntscMix);
      fx.alpha = raw.alpha;
    }
    inBase = ntscBaseBuffer.data();
    inStride = w;
  }

  // Master strength is 0..20 (UI 0..2000). Preserve the established default
  // at 3.5 (UI 350) as drive=1.0. Above the default, use a bounded linear
  // extension to drive=2.5 at UI 2000. This avoids the previous early ceiling
  // at drive=2.0 (UI 700), which made 700..2000 visually identical for many
  // drive-dependent stages. Values below the default retain proportional gain.
  const float drive = (strength <= 3.5f)
      ? (strength / 3.5f)
      : (1.0f + ((strength - 3.5f) / (20.0f - 3.5f)) * 1.5f);
  // Camera profile controls the transfer response; NTSC-RS remains the independent signal engine.
  const float charScale = drive;
  const float colorScale = 1.0f;
  const float sigScale = drive;

  // Color response & chroma behavior
  const float sat = s.saturation * prof.saturation * colorScale;
  const float bleed = clamp01(s.bleed * (0.25f + prof.chromaBleed * 1.35f) * (0.3f + 0.7f * drive));
  const float totalChromaX = s.chromaX * drive + prof.chromaOffsetX;
  const float totalChromaY = s.chromaY * drive + prof.chromaOffsetY;

  // Luma Softness is strictly controlled by P_SOFTNESS slider (0..100).
  const float soft = clamp01(s.softness * 0.01f + prof.lumaSoftness * 1.05f * drive);
  const int lowpassType = std::max(0, params[P_ADV_LOWPASS_TYPE]->u.pd.value - 1);
  const int blurRadius = clampi((int)std::ceil(soft * (lowpassType == 0 ? 5.0f : 2.0f)), 0, 5);

  // Module checkboxes and enablers - strictly obeys checkbox state
  const bool otherEnabled = getCheck(params, P_OTHER_ENABLE);


  // Grain is opt-in and has no forced minimum; amount zero is a true bypass.
  const float grainAmount = clamp01(s.grain) * (0.75f + prof.noiseFloor * 2.0f);
  const int advGrainSize = clampi(params[P_GRAIN_PRESET]->u.pd.value - 1, 0, 4);
  const float advGrainSoftness = getSlider(params, P_ADV_GRAIN_SOFTNESS, 100.0f);
  const int advGrainMode = clampi(params[P_ADV_GRAIN_MODE]->u.pd.value - 1, 0, 2);
  const int advGrainColor = clampi(params[P_ADV_GRAIN_COLOR]->u.pd.value - 1, 0, 2);
  const float advGrainR = getSlider(params, P_ADV_GRAIN_RED, 100.0f);
  const float advGrainG = getSlider(params, P_ADV_GRAIN_GREEN, 100.0f);
  const float advGrainB = getSlider(params, P_ADV_GRAIN_BLUE, 100.0f);
  const float compositeSharpen = getSlider(params, P_ADV_SHARPEN, 100.0f);
  const int chromaDemod = clampi(params[P_ADV_CHROMA_DEMOD]->u.pd.value - 1, 0, 3);
  const float verticalNoise = getSlider(params, P_ADV_VERTICAL_NOISE, 100.0f);
  const float dotNoise = getSlider(params, P_ADV_DOT_NOISE, 100.0f);
  const float noiseColor = getSlider(params, P_ADV_NOISE_COLOR, 100.0f);
  const float headSwitchPos = getSlider(params, P_ADV_HEADSWITCH_POS, 100.0f);
  const float headSwitchHeight = getSlider(params, P_ADV_HEADSWITCH_HEIGHT, 100.0f);
  const float headSwitchMotion = getSlider(params, P_ADV_HEADSWITCH_MOTION, 100.0f);
  const float headSwitchRandom = getSlider(params, P_ADV_HEADSWITCH_RANDOM, 100.0f);
  const bool osdEnabled = getCheck(params, P_ADV_OSD_ENABLE);
  const int osdMode = clampi(params[P_ADV_OSD_MODE]->u.pd.value - 1, 0, 3);
  const float osdOpacity = getSlider(params, P_ADV_OSD_OPACITY, 100.0f);
  const int osdColor = clampi(params[P_ADV_OSD_COLOR]->u.pd.value - 1, 0, 2);
  const float osdPosX = getSlider(params, P_ADV_OSD_POS_X, 100.0f);
  const float osdPosY = getSlider(params, P_ADV_OSD_POS_Y, 100.0f);
  const int osdScalePx = clampi((int)std::round(getSlider(params, P_ADV_OSD_SCALE, 100.0f)), 1, 3);
  const bool builtinOverlayEnabled = getCheck(params, P_ADV_BUILTIN_OVERLAY_ENABLE);
  const int builtinOverlayStyle = clampi(params[P_ADV_BUILTIN_OVERLAY_STYLE]->u.pd.value - 1, 0, 20);
  const float builtinOverlayOpacity = getSlider(params, P_ADV_BUILTIN_OVERLAY_OPACITY, 100.0f);
  const float builtinOverlaySpeed = getSlider(params, P_ADV_BUILTIN_OVERLAY_SPEED, 1.0f);
  const int builtinOverlayBlend = clampi(params[P_ADV_BUILTIN_OVERLAY_BLEND]->u.pd.value - 1, 0, 3);

  const float fps = (in_data->time_step != 0) ? (float)in_data->time_scale / (float)in_data->time_step : 24.0f;
  char osdText[32] = {0};
  if (osdEnabled) {
    if (osdMode == 0) {
      const int tc = std::max(0, (int)std::floor(timeSec * std::max(1.0f, fps)));
      std::snprintf(osdText, sizeof(osdText), "REC %02d:%02d:%02d", (tc / std::max(1, (int)(fps * 3600.0f))) % 100, (tc / std::max(1, (int)(fps * 60.0f))) % 60, (int)timeSec % 60);
    } else if (osdMode == 1) std::snprintf(osdText, sizeof(osdText), "01.01.2004");
    else if (osdMode == 2) std::snprintf(osdText, sizeof(osdText), "%02d:%02d:%02d", ((int)timeSec / 3600) % 24, ((int)timeSec / 60) % 60, (int)timeSec % 60);
    else std::snprintf(osdText, sizeof(osdText), "CAM VHS6RIFT");
  }
  const int osdTextLength = (int)std::strlen(osdText);
  const int osdLeft = clampi((int)std::round(osdPosX * (float)w), 0, std::max(0, w - 1));
  const int osdTop = clampi((int)std::round(osdPosY * (float)h), 0, std::max(0, h - 1));
  const float otherStrengthRaw = getSlider(params, P_OTHER_STRENGTH, 100.0f);
  const float otherStrength = clamp01(otherStrengthRaw) * drive;
  const bool legendEngineEnabled = getCheck(params, P_LEGEND_ENABLE) && otherEnabled;


  const float legendBreak   = getSlider(params, P_LEGEND_BREAKUP, 100.0f);
  const float legendTear    = getSlider(params, P_LEGEND_TEAR, 100.0f);
  const float legendLoss    = getSlider(params, P_LEGEND_SIGNAL, 100.0f);

  const float ntscChroma    = getSlider(params, P_NTSC_CHROMA_DELAY, 100.0f);
  const float ntscHead      = getSlider(params, P_NTSC_HEAD_SWITCH, 100.0f);
  const float ntscWave      = getSlider(params, P_NTSC_EDGE_WAVE, 100.0f);



  for (int y = 0; y < h; ++y) {
    PF_Pixel *dst = outBase + y * outStride;
    for (int x = 0; x < w; ++x) {
      const int nominalSx = (int)((x + 0.5f) * inW / std::max(1, w));
      int sy = clampi((int)((y + 0.5f) * inH / std::max(1, h)), 0, inH - 1);
      // Camera-specific timebase instability is random line jitter, not a visible sine-wave warp.
      // Early VHS-C bodies wander more; later S-VHS-C bodies remain comparatively stable.
      const uint32_t camRowHash = (uint32_t)s.seed ^ (uint32_t)(frame * 2246822519u)
          ^ (uint32_t)(y * 3266489917u) ^ 0x9e3779b9u;
      const float cameraLineJitter = (rand01(camRowHash) - 0.5f)
          * prof.timebaseWobble * (0.5f + 0.5f * drive) * 2.2f;
      int sx = clampi(nominalSx + (int)std::round(cameraLineJitter), 0, inW - 1);
      PF_Pixel p = inBase[sy * inStride + sx];
      float r = p.red / 255.0f, g = p.green / 255.0f, b = p.blue / 255.0f;

      if (s.continuous && strength > 0.0001f) {
        // Luma softness: spatial blur filter ONLY when softness slider > 0
        if (blurRadius > 0) {
          float rr = 0, gg = 0, bb = 0, weight = 0;
          for (int k = -blurRadius; k <= blurRadius; ++k) {
            const int xx = clampi(sx + k, 0, inW - 1);
            const float kw = lowpassType == 0 ? (float)(blurRadius + 1 - std::abs(k)) : (k == 0 ? 4.0f : (std::abs(k) == 1 ? 1.0f : 0.0f));
            const PF_Pixel q = inBase[sy * inStride + xx];
            rr += q.red * kw; gg += q.green * kw; bb += q.blue * kw; weight += kw;
          }
          r = r * (1.0f - soft) + (rr / weight / 255.0f) * soft;
          g = g * (1.0f - soft) + (gg / weight / 255.0f) * soft;
          b = b * (1.0f - soft) + (bb / weight / 255.0f) * soft;
        }

        // Horizontal transient edge ringing (overshoot) for analog video edge character
        if (prof.edgeRinging > 0.001f && sx > 0) {
          const PF_Pixel prevP = inBase[sy * inStride + (sx - 1)];
          const float lumCurr = 0.2126f * r + 0.7152f * g + 0.0722f * b;
          const float lumPrev = (0.2126f * prevP.red + 0.7152f * prevP.green + 0.0722f * prevP.blue) / 255.0f;
          const float diff = lumCurr - lumPrev;
          const float ringing = diff * prof.edgeRinging * charScale;
          r = clamp01(r + ringing);
          g = clamp01(g + ringing);
          b = clamp01(b + ringing);
        }

        // Separate luma from chroma before applying VHS color delay.
        // Composite/tape bandwidth loss primarily softens and displaces color;
        // it should not drag the whole luminance image sideways with it.
        const float y0 = 0.2126f * r + 0.7152f * g + 0.0722f * b;
        const float u0 = (b - y0) * 0.5389f;
        const float v0 = (r - y0) * 0.6350f;
        const int cx = clampi(sx + (int)std::round(totalChromaX), 0, inW - 1);
        const int cy = clampi(sy + (int)std::round(totalChromaY), 0, inH - 1);
        const int cxl = clampi(cx - 1, 0, inW - 1);
        const int cxr = clampi(cx + 1, 0, inW - 1);
        const PF_Pixel cp = inBase[cy * inStride + cx];
        const PF_Pixel cpl = inBase[cy * inStride + cxl];
        const PF_Pixel cpr = inBase[cy * inStride + cxr];
        const float cbR = cp.red / 255.0f, cbG = cp.green / 255.0f, cbB = cp.blue / 255.0f;
        const float clR = cpl.red / 255.0f, clG = cpl.green / 255.0f, clB = cpl.blue / 255.0f;
        const float crR = cpr.red / 255.0f, crG = cpr.green / 255.0f, crB = cpr.blue / 255.0f;
        const float cy0 = 0.2126f * cbR + 0.7152f * cbG + 0.0722f * cbB;
        const float cyl = 0.2126f * clR + 0.7152f * clG + 0.0722f * clB;
        const float cyr = 0.2126f * crR + 0.7152f * crG + 0.0722f * crB;
        const float cu0 = (cbB - cy0) * 0.5389f;
        const float cv0 = (cbR - cy0) * 0.6350f;
        const float cul = (clB - cyl) * 0.5389f;
        const float cvl = (clR - cyl) * 0.6350f;
        const float cur = (crB - cyr) * 0.5389f;
        const float cvr = (crR - cyr) * 0.6350f;
        // Three-tap chroma bandwidth limit emulates color-under tape softness.
        float chromaSoft = clamp01(bleed * 0.72f);
        if (chromaDemod == 1) chromaSoft = clamp01(chromaSoft * 1.25f);
        else if (chromaDemod == 2) chromaSoft = clamp01(chromaSoft * 0.70f);
        else if (chromaDemod == 3) chromaSoft = clamp01(chromaSoft * 0.35f);
        else chromaSoft = 0.0f;
        const float uShifted = cu0 * (1.0f - chromaSoft) + ((cul + 2.0f * cu0 + cur) * 0.25f) * chromaSoft;
        const float vShifted = cv0 * (1.0f - chromaSoft) + ((cvl + 2.0f * cv0 + cvr) * 0.25f) * chromaSoft;
        const float u = u0 * (1.0f - bleed) + uShifted * bleed;
        const float v = v0 * (1.0f - bleed) + vShifted * bleed;
        // Reconstruct RGB around the original luma, preserving edge placement.
        r = clamp01(y0 + 1.5748f * v);
        g = clamp01(y0 - 0.1873f * u - 0.4681f * v);
        b = clamp01(y0 + 1.8556f * u);

        // Composite signal sharpness: luma-only unsharp response with a wide
        // control range. This does not increase chroma saturation or edge color.
        if (compositeSharpen > 0.001f && sx > 0 && sx + 1 < inW) {
          const PF_Pixel lp = inBase[sy * inStride + sx - 1];
          const PF_Pixel rp = inBase[sy * inStride + sx + 1];
          const float ll = (0.2126f * lp.red + 0.7152f * lp.green + 0.0722f * lp.blue) / 255.0f;
          const float lr = (0.2126f * rp.red + 0.7152f * rp.green + 0.0722f * rp.blue) / 255.0f;
          const float edge = (2.0f * y0 - ll - lr) * std::min(2.5f, compositeSharpen * 0.25f);
          r = clamp01(r + edge); g = clamp01(g + edge); b = clamp01(b + edge);
        }

        // Camera-specific analog chroma noise; its level follows the selected camera engine.
        const uint32_t cameraNoiseHash = (uint32_t)s.seed ^ (uint32_t)(frame * 2246822519u)
            ^ (uint32_t)(x * 3266489917u) ^ (uint32_t)(y * 668265263u);
        const float cameraChromaNoise = (rand01(cameraNoiseHash ^ 0x68bc21ebu) - 0.5f)
            * prof.chromaNoise * drive * 0.35f;
        r = clamp01(r + cameraChromaNoise);
        b = clamp01(b - cameraChromaNoise * 0.85f);

        // Saturation
        const float lum = 0.2126f * r + 0.7152f * g + 0.0722f * b;
        r = lum + (r - lum) * sat;
        g = lum + (g - lum) * sat;
        b = lum + (b - lum) * sat;

        // Fixed VHS transfer response & tonal curve
        r += prof.warm * charScale * (0.35f + lum) - prof.green * charScale * 0.25f;
        g += prof.green * charScale * (0.25f + lum) - prof.warm * charScale * 0.08f;
        b -= prof.warm * charScale * (0.30f + lum) + prof.green * charScale * 0.12f;

        const float camContrast = 1.0f + (prof.contrast - 1.0f) * charScale;
        r = (r - 0.5f) * camContrast + 0.5f;
        g = (g - 0.5f) * camContrast + 0.5f;
        b = (b - 0.5f) * camContrast + 0.5f;

        const float shoulder = prof.shoulder * charScale;
        r -= shoulder * std::max(0.0f, r - 0.68f) * std::max(0.0f, r - 0.68f);
        g -= shoulder * std::max(0.0f, g - 0.68f) * std::max(0.0f, g - 0.68f);
        b -= shoulder * std::max(0.0f, b - 0.68f) * std::max(0.0f, b - 0.68f);

        r = clamp01(r * (1.0f + (prof.rGain - 1.0f) * colorScale));
        g = clamp01(g * (1.0f + (prof.gGain - 1.0f) * colorScale));
        b = clamp01(b * (1.0f + (prof.bGain - 1.0f) * colorScale));

        const float gamma = 1.0f + (prof.gamma - 1.0f) * charScale;
        r = std::pow(clamp01(r), gamma);
        g = std::pow(clamp01(g), gamma);
        b = std::pow(clamp01(b), gamma);

        // Use normalized master drive for the pedestal response, not raw strength (0..20).
        const float blackOffset = (s.black * 0.08f + prof.pedestal * 0.05f) * drive;
        r = clamp01(r + blackOffset);
        g = clamp01(g + blackOffset);
        b = clamp01(b + blackOffset);
        // Saturation zero must produce true monochrome for the image itself.
        // Later analog noise/tape stages remain outside this conversion, so their colored artifacts survive.
        if (sat <= 0.0001f) {
          const float mono = 0.2126f * r + 0.7152f * g + 0.0722f * b;
          r = g = b = mono;
        }

        // Tracking is deliberately expressed as horizontal image displacement above.
        // Do not add snow, glass-like pixel noise, white flashes, or colored tears here;
        // users control this damage with Enable Tracking Damage, Tracking Noise and Band Width.

        // Signal noise is not drawn as synthetic red stripes. It is processed by
        // the vendored NTSC-RS composite-noise engine before this C++ image stage.

        // Signal Grain
        if (grainAmount > 0.001f) {
          // Film-like fine moving grain: one independent sample per output pixel.
          // Grain Size selects a subtle correlation radius; it never creates square cells.
          static const float grainSizeScale[5] = { 1.80f, 1.45f, 1.20f, 1.05f, 1.0f };
          // A continuous grain lattice: size changes the spatial correlation itself (not just strength),
          // while the seed/frame offset makes the texture move without coarse square blocks.
          const float grainRadius = grainSizeScale[advGrainSize] * std::max(0.60f, s.grainScale * 0.10f);
          const uint32_t motionFrame = (uint32_t)std::floor(timeSec * std::max(0.0f, s.grainSpeed) * fps);
          const float gx = (float)x / grainRadius;
          const float gy = (float)y / grainRadius;
          const int ix = (int)std::floor(gx), iy = (int)std::floor(gy);
          const float fx0 = gx - (float)ix, fy0 = gy - (float)iy;
          const float fx = fx0 * fx0 * (3.0f - 2.0f * fx0);
          const float fy = fy0 * fy0 * (3.0f - 2.0f * fy0);
          const uint32_t grainBase = (uint32_t)s.seed ^ (motionFrame * 2246822519u);
          const auto lattice = [&](int lx, int ly, uint32_t salt) {
            const uint32_t h0 = grainBase ^ (uint32_t)(lx * 73856093u) ^ (uint32_t)(ly * 19349663u) ^ salt;
            return rand01(h0) - 0.5f;
          };
          const float n00 = lattice(ix, iy, 0x9E3779B9u);
          const float n10 = lattice(ix + 1, iy, 0x9E3779B9u);
          const float n01 = lattice(ix, iy + 1, 0x9E3779B9u);
          const float n11 = lattice(ix + 1, iy + 1, 0x9E3779B9u);
          const float c00 = lattice(ix, iy, 0xA511E9B3u);
          const float c10 = lattice(ix + 1, iy, 0xA511E9B3u);
          const float c01 = lattice(ix, iy + 1, 0xA511E9B3u);
          const float c11 = lattice(ix + 1, iy + 1, 0xA511E9B3u);
          const float rawN = (n00 * (1.0f-fx) + n10 * fx) * (1.0f-fy) + (n01 * (1.0f-fx) + n11 * fx) * fy;
          const float chromaRaw = (c00 * (1.0f-fx) + c10 * fx) * (1.0f-fy) + (c01 * (1.0f-fx) + c11 * fx) * fy;
          const float grainAmplitude = grainAmount * 0.72f * (1.0f - 0.72f * advGrainSoftness) / std::max(1.0f, grainRadius * 0.18f);
          const float n = rawN * grainAmplitude;
          const float chromaN = chromaRaw * grainAmount * 0.48f / std::max(1.0f, grainRadius * 0.20f);
          const float lumaWeight = advGrainMode == 1 ? 0.0f : 1.0f;
          const float chromaWeight = advGrainMode == 0 ? 0.0f : 1.0f;
          const float softMix = advGrainSoftness;
          const float gn = n * (1.0f - 0.7f * softMix);
          if (advGrainColor == 1) {
            r = clamp01(r + gn); g = clamp01(g + gn); b = clamp01(b + gn);
          } else if (advGrainColor == 2) {
            r = clamp01(r + gn * 1.15f * (advGrainR * 0.01f) + chromaN * chromaWeight * noiseColor);
            g = clamp01(g + gn * 0.80f * (advGrainG * 0.01f) - chromaN * chromaWeight * 0.25f * noiseColor);
            b = clamp01(b + gn * 1.30f * (advGrainB * 0.01f) - chromaN * chromaWeight * noiseColor);
          } else {
            r = clamp01(r + gn * lumaWeight * 1.15f + chromaN * chromaWeight * 0.65f);
            g = clamp01(g + gn * lumaWeight * 0.80f - chromaN * chromaWeight * 0.15f);
            b = clamp01(b + gn * lumaWeight * 1.30f - chromaN * chromaWeight * 0.65f);
          }
        }
      }

      // Composite noise families: moving vertical interference and small circular-like dots.
      // The pattern is deterministic per seed/frame, but changes naturally over time.
      if (s.continuous && (verticalNoise > 0.001f || dotNoise > 0.001f)) {
        const uint32_t vh = (uint32_t)s.seed ^ (uint32_t)(frame * 2246822519u) ^ (uint32_t)((x / 2) * 3266489917u);
        if (verticalNoise > 0.001f && rand01(vh) < verticalNoise * 0.12f) {
          const float vn = (rand01(vh + 9u) - 0.5f) * verticalNoise * 0.22f * sigScale;
          const float tint = noiseColor * (rand01(vh + 13u) * 2.0f - 1.0f);
          r = clamp01(r + vn + tint * vn); g = clamp01(g + vn * 0.55f); b = clamp01(b + vn - tint * vn);
        }
        if (dotNoise > 0.001f) {
          const int dotCell = 3 + (int)std::round(dotNoise * 8.0f);
          const uint32_t dh2 = (uint32_t)s.seed ^ (uint32_t)(frame * 668265263u) ^ (uint32_t)((x / dotCell) * 73856093u) ^ (uint32_t)((y / dotCell) * 19349663u);
          if (rand01(dh2) < dotNoise * 0.035f && (x % dotCell) * (x % dotCell) + (y % dotCell) * (y % dotCell) < dotCell * dotCell / 3) {
            const float dn = (0.25f + 0.65f * rand01(dh2 + 3u)) * dotNoise * sigScale;
            const float tint = noiseColor * (rand01(dh2 + 5u) * 2.0f - 1.0f);
            r = clamp01(r + dn * (0.35f + tint)); g = clamp01(g + dn * 0.25f); b = clamp01(b + dn * (0.35f - tint));
          }
        }
      }

      // Head switching is a moving, edge-anchored signal failure, not a static PNG.
      if (s.continuous && headSwitchHeight > 0.001f) {
        const float burstPick = rand01((uint32_t)s.seed ^ (uint32_t)(std::floor(timeSec * (2.0f + headSwitchMotion * 14.0f)) * 747796405u));
        const float burst = burstPick < headSwitchRandom * 0.45f ? (0.45f + 0.55f * rand01((uint32_t)(frame * 2654435761u) ^ (uint32_t)s.seed)) : 0.35f;
        const float center = headSwitchPos + std::sin(timeSec * (1.0f + headSwitchMotion * 12.0f) + s.seed * 0.013f) * headSwitchMotion * 0.06f;
        const float yn = (float)y / (float)std::max(1, h);
        const float bandH = std::max(0.003f, headSwitchHeight * 0.18f);
        const float edgeDist = std::fabs(yn - center);
        const float hs = (1.0f - smooth(bandH, bandH * 1.7f, edgeDist)) * burst * sigScale;
        if (hs > 0.001f) {
          const uint32_t hh = (uint32_t)s.seed ^ (uint32_t)(frame * 2246822519u) ^ (uint32_t)(x * 3266489917u) ^ (uint32_t)(y * 668265263u);
          const float hn = (rand01(hh) - 0.5f) * hs * 0.85f;
          const float tint = noiseColor * hs * (rand01(hh + 1u) * 2.0f - 1.0f);
          r = clamp01(r + hn + tint); g = clamp01(g + hn * 0.25f); b = clamp01(b + hn - tint);
        }
      }

      // Other VHS Engines
      if (otherStrength > 0.0001f) {
        const float xn = (float)x / std::max(1, w), yn = (float)y / std::max(1, h);
        const uint32_t oh = (uint32_t)s.seed ^ (uint32_t)(frame * 2246822519u) ^ (uint32_t)(x * 3266489917u) ^ (uint32_t)(y * 668265263u);
        if (legendEngineEnabled) { // VHS Legend: analog sync instability, not neon RGB stripes.
          const uint32_t lineHash = (uint32_t)s.seed ^ (uint32_t)(frame * 41) ^ (uint32_t)((y / 2) * 2654435761u);
          const float line = rand01(lineHash) - 0.5f;
          const bool tear = rand01(lineHash ^ (uint32_t)(x / 8)) < clamp01(legendTear * otherStrength * 0.012f);
          const uint32_t lossHash = (uint32_t)s.seed ^ (uint32_t)(frame * 73) ^ (uint32_t)((x / 24) * 73856093u) ^ (uint32_t)((y / 3) * 19349663u);
          const bool loss = rand01(lossHash) < clamp01(legendLoss * otherStrength * 0.010f);
          const float lumaJitter = line * legendBreak * otherStrength * 0.055f;
          r = clamp01(r + lumaJitter); g = clamp01(g + lumaJitter); b = clamp01(b + lumaJitter);
          if (tear) { const float shift = 0.04f + 0.10f * rand01(lineHash + 9u); r = clamp01(r + shift); g = clamp01(g + shift * 0.6f); b = clamp01(b - shift * 0.25f); }
          if (loss) { const float lossMix = 0.25f + 0.45f * rand01(lossHash + 3u); r *= 1.0f-lossMix; g *= 1.0f-lossMix; b *= 1.0f-lossMix; }
        }
      }

      // Built-in OSD: tiny native 5x7 glyph overlay (REC/timecode/date/time/camera label).
      // Keep this independent of external footage layers and animate the timecode from comp time.
      if (osdEnabled && osdOpacity > 0.001f && x >= osdLeft && y >= osdTop &&
          y < osdTop + 7 * osdScalePx && x < osdLeft + osdTextLength * 6 * osdScalePx) {
        const int gy = (y - osdTop) / osdScalePx;
        const int glyphIndex = (x - osdLeft) / (6 * osdScalePx);
        const int gx = ((x - osdLeft) / osdScalePx) % 6;
        if (glyphIndex >= 0 && glyphIndex < osdTextLength && gx < 5) {
          const char ch = osdText[glyphIndex];
          unsigned char bits = 0;
          switch (ch) {
            case '0': { static const unsigned char a[7]={14,17,19,21,25,17,14}; bits=a[gy]; break; }
            case '1': { static const unsigned char a[7]={4,12,4,4,4,4,14}; bits=a[gy]; break; }
            case '2': { static const unsigned char a[7]={14,17,1,2,4,8,31}; bits=a[gy]; break; }
            case '3': { static const unsigned char a[7]={30,1,1,14,1,1,30}; bits=a[gy]; break; }
            case '4': { static const unsigned char a[7]={2,6,10,18,31,2,2}; bits=a[gy]; break; }
            case '5': { static const unsigned char a[7]={31,16,16,30,1,1,30}; bits=a[gy]; break; }
            case '6': { static const unsigned char a[7]={6,8,16,30,17,17,14}; bits=a[gy]; break; }
            case '7': { static const unsigned char a[7]={31,1,2,4,8,8,8}; bits=a[gy]; break; }
            case '8': { static const unsigned char a[7]={14,17,17,14,17,17,14}; bits=a[gy]; break; }
            case '9': { static const unsigned char a[7]={14,17,17,15,1,2,12}; bits=a[gy]; break; }
            case 'R': { static const unsigned char a[7]={30,17,17,30,20,18,17}; bits=a[gy]; break; }
            case 'E': { static const unsigned char a[7]={31,16,16,30,16,16,31}; bits=a[gy]; break; }
            case 'C': { static const unsigned char a[7]={14,17,16,16,16,17,14}; bits=a[gy]; break; }
            case 'A': { static const unsigned char a[7]={14,17,17,31,17,17,17}; bits=a[gy]; break; }
            case 'M': { static const unsigned char a[7]={17,27,21,21,17,17,17}; bits=a[gy]; break; }
            case 'V': { static const unsigned char a[7]={17,17,17,17,17,10,4}; bits=a[gy]; break; }
            case 'H': { static const unsigned char a[7]={17,17,17,31,17,17,17}; bits=a[gy]; break; }
            case 'S': { static const unsigned char a[7]={15,16,16,14,1,1,30}; bits=a[gy]; break; }
            case 'I': { static const unsigned char a[7]={14,4,4,4,4,4,14}; bits=a[gy]; break; }
            case 'F': { static const unsigned char a[7]={31,16,16,30,16,16,16}; bits=a[gy]; break; }
            case 'T': { static const unsigned char a[7]={31,4,4,4,4,4,4}; bits=a[gy]; break; }
            case ':': { static const unsigned char a[7]={0,4,4,0,4,4,0}; bits=a[gy]; break; }
            case '.': { static const unsigned char a[7]={0,0,0,0,0,4,4}; bits=a[gy]; break; }
            case '-': { static const unsigned char a[7]={0,0,0,31,0,0,0}; bits=a[gy]; break; }
            default: bits=0; break;
          }
          if (bits & (1u << (4 - gx))) {
            const float osdR = osdColor == 1 ? 1.0f : (osdColor == 2 ? 0.35f : 0.92f);
            const float og = osdColor == 1 ? 0.62f : (osdColor == 2 ? 1.0f : 0.92f);
            const float ob = osdColor == 1 ? 0.18f : (osdColor == 2 ? 0.42f : 0.92f);
            r = clamp01(r * (1.0f - osdOpacity) + osdR * osdOpacity);
            g = clamp01(g * (1.0f - osdOpacity) + og * osdOpacity);
            b = clamp01(b * (1.0f - osdOpacity) + ob * osdOpacity);
          }
        }
      }

      // Built-in Overlay Library: 21 procedural, time-driven signal overlays.
      // No separate footage layer is required; the selected style is rendered directly.
      if (builtinOverlayEnabled && builtinOverlayOpacity > 0.001f) {
        const float speedT = timeSec * (builtinOverlaySpeed / 100.0f);
        const float xn = (float)x / (float)std::max(1, w - 1);
        const float yn = (float)y / (float)std::max(1, h - 1);
        const uint32_t oh = (uint32_t)s.seed ^ (uint32_t)(frame * 747796405u) ^ (uint32_t)((x / 5) * 2246822519u) ^ (uint32_t)((y / 3) * 3266489917u);
        float oa = 0.0f, rr = 0.9f, gg = 0.9f, bb = 0.9f;
        const float scan = std::fabs(std::sin((float)y * 0.42f + speedT * 8.0f));
        switch (builtinOverlayStyle) {
          case 0: oa = (std::fabs(yn - std::fmod(speedT * 0.22f, 1.0f)) < 0.004f) ? 0.9f : 0.0f; rr=0.82f; gg=0.94f; bb=1.0f; break;
          case 1: oa = yn > 0.91f && yn < 0.91f + 0.04f * (0.5f + headSwitchHeight) ? 0.75f * rand01(oh) : 0.0f; rr=0.8f; gg=0.9f; bb=1.0f; break;
          case 2: oa = std::fabs(std::sin(yn * 26.0f + speedT * 4.0f)) > 0.96f ? 0.85f : 0.0f; rr=1.0f; gg=0.2f; bb=0.35f; break;
          case 3: { const int bar = (int)(xn * 7.0f); oa = yn > 0.72f ? 0.78f : 0.0f; rr=bar==0||bar==1?1.0f:0.1f; gg=bar==1||bar==2||bar==4?1.0f:0.1f; bb=bar==3||bar==4||bar==5?1.0f:0.1f; break; }
          case 4: oa = (y % 4 == 0) ? 0.25f : 0.0f; rr=0.7f; gg=0.85f; bb=1.0f; break;
          case 5: oa = std::fabs(yn - std::fmod(speedT * 0.14f,1.0f)) < 0.012f ? 0.8f : 0.0f; rr=0.9f; gg=0.95f; bb=1.0f; break;
          case 6: oa = (x < 24 && y < 10 && ((x+y+(int)(speedT*4))%9 < 5)) ? 0.9f : 0.0f; rr=1.0f; gg=0.2f; bb=0.18f; break;
          case 7: oa = (y < 9 && x < 62 && ((x/6+y)%5 != 0)) ? 0.85f : 0.0f; rr=1.0f; gg=0.78f; bb=0.42f; break;
          case 8: oa = (y < 9 && x < 82 && (x%6 < 4)) ? 0.85f : 0.0f; rr=0.82f; gg=0.92f; bb=1.0f; break;
          case 9: oa = (x < 52 && y < 10 && (x%7 < 4)) ? 0.7f : 0.0f; rr=0.65f; gg=1.0f; bb=0.72f; break;
          case 10: oa = rand01(oh) > 0.994f ? 0.9f : 0.0f; rr=0.92f; gg=0.88f; bb=0.76f; break;
          case 11: oa = rand01(oh) > 0.975f ? 0.65f : 0.0f; rr=0.75f; gg=0.9f; bb=1.0f; break;
          case 12: oa = (std::fabs(xn-0.5f)<0.002f || std::fabs(yn-0.5f)<0.003f) ? 0.7f : 0.0f; rr=0.35f; gg=1.0f; bb=0.65f; break;
          case 13: { const float edge=std::min(std::min(xn,1.0f-xn),std::min(yn,1.0f-yn)); oa=(1.0f-smooth(0.015f,0.11f,edge))*0.72f; rr=1.0f; gg=0.28f; bb=0.06f; break; }
          case 14: oa = (y%2==0) ? 0.22f : 0.0f; rr=0.85f; gg=0.92f; bb=1.0f; break;
          case 15: oa = (std::fabs(std::sin(yn*35.0f+speedT*7.0f))>0.93f) ? 0.55f : 0.0f; rr=0.95f; gg=0.25f; bb=0.7f; break;
          case 16: oa = (std::fabs(yn-(0.5f+0.22f*std::sin(xn*9.0f+speedT*2.0f)))<0.006f) ? 0.8f : 0.0f; rr=0.8f; gg=0.88f; bb=1.0f; break;
          case 17: oa = (rand01(oh)>0.985f) ? 0.45f : 0.0f; rr=0.82f; gg=0.9f; bb=1.0f; break;
          case 18: oa = (rand01(oh)>0.992f && (y%3==0)) ? 0.85f : 0.0f; rr=0.95f; gg=0.95f; bb=1.0f; break;
          case 19: oa = std::fabs(yn-(0.89f+0.012f*std::sin(speedT*11.0f)))<0.008f ? 0.75f : 0.0f; rr=0.2f; gg=0.65f; bb=1.0f; break;
          case 20: oa = (scan>0.985f && rand01(oh)>0.6f) ? 0.85f : 0.0f; rr=1.0f; gg=0.15f; bb=0.45f; break;
        }
        oa = clamp01(oa * builtinOverlayOpacity);
        if (oa > 0.001f) {
          if (builtinOverlayBlend == 1) { r=clamp01(r+rr*oa); g=clamp01(g+gg*oa); b=clamp01(b+bb*oa); }
          else if (builtinOverlayBlend == 2) { r=1.0f-(1.0f-r)*(1.0f-rr*oa); g=1.0f-(1.0f-g)*(1.0f-gg*oa); b=1.0f-(1.0f-b)*(1.0f-bb*oa); }
          else if (builtinOverlayBlend == 3) { r=clamp01(r*(1.0f-oa)+r*rr*oa); g=clamp01(g*(1.0f-oa)+g*gg*oa); b=clamp01(b*(1.0f-oa)+b*bb*oa); }
          else { r=clamp01(r*(1.0f-oa)+rr*oa); g=clamp01(g*(1.0f-oa)+gg*oa); b=clamp01(b*(1.0f-oa)+bb*oa); }
        }
      }

      dst[x].red = (uint8_t)std::round(clamp01(r) * 255.0f);
      dst[x].green = (uint8_t)std::round(clamp01(g) * 255.0f);
      dst[x].blue = (uint8_t)std::round(clamp01(b) * 255.0f);
      dst[x].alpha = p.alpha;
    }
  }

  // The ntsc-rs signal stage has already generated the base image above;
  // this final pass contains only the additional VHS6RIFT layers.

  return err;
}

static PF_Err AudioRender(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[]) {
  PF_SoundWorld *src = &in_data->src_snd;
  PF_SoundWorld *dst = &out_data->dest_snd;
  if (!src->dataP || !dst->dataP || src->num_samples <= 0 || dst->num_samples <= 0) return PF_Err_NONE;
  const A_long srcChannels = std::max<A_long>(1, std::min<A_long>(2, src->fi.num_channels));
  const A_long dstChannels = std::max<A_long>(1, std::min<A_long>(2, dst->fi.num_channels));
  const A_long frames = std::min(src->num_samples, dst->num_samples);
  const float mix = clamp01((float)params[P_AUDIO_MIX]->u.fs_d.value / 100.0f);
  const float eventAmount = clamp01((float)params[P_AUDIO_EVENT_AMOUNT]->u.fs_d.value / 100.0f);
  const float crackle = clamp01((float)params[P_AUDIO_CRACKLE]->u.fs_d.value / 100.0f);
  const float dropout = clamp01((float)params[P_AUDIO_DROPOUT]->u.fs_d.value / 100.0f);
  const float saturation = clamp01((float)params[P_AUDIO_SATURATION]->u.fs_d.value / 100.0f);
  const float lofi = clamp01((float)params[P_AUDIO_LOFI]->u.fs_d.value / 100.0f);
  const int seed = params[P_SEED]->u.sd.value;
  const float rate = (float)std::max(1.0, (double)dst->fi.rateF);
  const bool enabled = params[P_AUDIO_ENABLE]->u.bd.value != 0;
  const float eventRate = (float)params[P_EVENT_RATE]->u.fs_d.value;
  const float eventDuration = (float)params[P_EVENT_DURATION]->u.fs_d.value;
  const float recovery = (float)params[P_RECOVERY]->u.fs_d.value;
  const float eventPeriod = eventRate > 0.001f ? 60.0f / eventRate : 1.0e9f;
  const float *inSamples = (const float*)src->dataP;
  float *outSamples = (float*)dst->dataP;
  for (A_long i=0; i<frames; ++i) {
    const double t = ((double)in_data->start_sampL + (double)i) / rate;
    const int slot = (int)std::floor(t / eventPeriod);
    const float slotTime = (float)(t - (double)slot * eventPeriod);
    const bool eventSelected = params[P_AUDIO_EVENTS]->u.bd.value != 0 && eventRate > 0.0f && rand01((uint32_t)(slot * 747796405u) ^ (uint32_t)seed) < 0.68f;
    float env = 0.0f;
    if (eventSelected) {
      const float attack=0.015f;
      if (slotTime<attack) env=smooth(0.0f,attack,slotTime);
      else if (slotTime<eventDuration) env=1.0f;
      else if (recovery>0.0f) env=1.0f-smooth(eventDuration,eventDuration+recovery,slotTime);
    }
    const uint32_t sampleKey=(uint32_t)(in_data->start_sampL+i)*2246822519u ^ (uint32_t)seed;
    const float noise=(rand01(sampleKey)-0.5f)*2.0f*crackle*eventAmount*env*0.35f;
    const uint32_t dropoutKey=(uint32_t)((in_data->start_sampL+i)/256)*3266489917u ^ (uint32_t)(slot*668265263u) ^ (uint32_t)seed;
    const bool drop = eventSelected && env>0.0f && rand01(dropoutKey) < dropout*eventAmount*env*0.45f;
    for (A_long ch=0; ch<dstChannels; ++ch) {
      const A_long srcIndex=i*srcChannels+std::min(ch,srcChannels-1);
      const A_long dstIndex=i*dstChannels+ch;
      const float dry=inSamples[srcIndex];
      float wet=drop ? 0.0f : dry;
      wet += noise;
      // Soft saturation is a real sample-domain nonlinear stage.
      if (saturation>0.0f) {
        const float drive=1.0f+saturation*8.0f;
        wet=std::tanh(wet*drive)/std::tanh(drive);
      }
      // Deterministic bit-depth reduction; does not rely on previous playback state.
      if (lofi>0.0f) {
        const int bits=clampi((int)std::round(16.0f-lofi*12.0f),4,16);
        const float levels=(float)((1u<<bits)-1u);
        wet=std::round(clamp01(wet*0.5f+0.5f)*levels)/levels*2.0f-1.0f;
      }
      outSamples[dstIndex]=enabled ? dry*(1.0f-mix)+wet*mix : dry;
    }
  }
  // If destination has more samples than source, clear the tail rather than read out of bounds.
  for (A_long i=frames*dstChannels; i<dst->num_samples*dstChannels; ++i) outSamples[i]=0.0f;
  return PF_Err_NONE;
}

extern "C" DllExport PF_Err EffectMain(PF_Cmd cmd, PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output, void *) {
  PF_Err err = PF_Err_NONE;
  switch (cmd) {
    case PF_Cmd_ABOUT: err = About(in_data, out_data, params, output); break;
    case PF_Cmd_GLOBAL_SETUP: err = GlobalSetup(out_data); break;
    case PF_Cmd_PARAMS_SETUP: err = ParamsSetup(in_data, out_data); break;
    case PF_Cmd_RENDER: err = Render(in_data, params, output); break;
    case PF_Cmd_AUDIO_RENDER: err = AudioRender(in_data, out_data, params); break;
    default: break;
  }
  return err;
}
