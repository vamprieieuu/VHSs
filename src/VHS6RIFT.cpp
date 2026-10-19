#include "AEConfig.h"
#if defined(AE_OS_WIN) && !defined(VHS6RIFT_NATIVE_TEST)
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
#include "ntscrs_c.h"
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <cstdio>

#define PLUGIN_NAME "VHS6RIFT"
#define PLUGIN_MATCH_NAME "com.vhs6rift.effect"
#define PLUGIN_VERSION 1

// Stable parameter IDs. Topic parameters are included in sequence; do not reorder without updating IDs.
enum {
  P_INPUT = 0, P_CAMERA, P_MASTER, P_CONTINUOUS, P_EVENTS, P_SEED,
  P_TOPIC_CAMERA,
  P_FXC6_TOPIC, P_FXC6_CHARACTER, P_FXC6_COLOR, P_FXC6_SIGNAL, P_FXC6_GRAIN, P_FXC6_END,
  P_GEN7_TOPIC, P_GEN7_CHARACTER, P_GEN7_COLOR, P_GEN7_SIGNAL, P_GEN7_GRAIN, P_GEN7_END,
  P_JVC_C7_TOPIC, P_JVC_C7_CHARACTER, P_JVC_C7_COLOR, P_JVC_C7_SIGNAL, P_JVC_C7_GRAIN, P_JVC_C7_END,
  P_R20_TOPIC, P_R20_CHARACTER, P_R20_COLOR, P_R20_SIGNAL, P_R20_GRAIN, P_R20_END,
  P_FALL78_TOPIC, P_FALL78_CHARACTER, P_FALL78_COLOR, P_FALL78_SIGNAL, P_FALL78_GRAIN, P_FALL78_END,
  P_ERC_TOPIC, P_ERC_CHARACTER, P_ERC_COLOR, P_ERC_SIGNAL, P_ERC_GRAIN, P_ERC_END,
  P_JVC_SXM_TOPIC, P_JVC_SXM_CHARACTER, P_JVC_SXM_COLOR, P_JVC_SXM_SIGNAL, P_JVC_SXM_GRAIN, P_JVC_SXM_END,
  P_CAM08_TOPIC, P_CAM08_CHARACTER, P_CAM08_COLOR, P_CAM08_SIGNAL, P_CAM08_GRAIN, P_CAM08_END,
  P_CAM09_TOPIC, P_CAM09_CHARACTER, P_CAM09_COLOR, P_CAM09_SIGNAL, P_CAM09_GRAIN, P_CAM09_END,
  P_CAM10_TOPIC, P_CAM10_CHARACTER, P_CAM10_COLOR, P_CAM10_SIGNAL, P_CAM10_GRAIN, P_CAM10_END,
  P_CAM11_TOPIC, P_CAM11_CHARACTER, P_CAM11_COLOR, P_CAM11_SIGNAL, P_CAM11_GRAIN, P_CAM11_END,
  P_CAM12_TOPIC, P_CAM12_CHARACTER, P_CAM12_COLOR, P_CAM12_SIGNAL, P_CAM12_GRAIN, P_CAM12_END,
  P_CAM13_TOPIC, P_CAM13_CHARACTER, P_CAM13_COLOR, P_CAM13_SIGNAL, P_CAM13_GRAIN, P_CAM13_END,
  P_CAM14_TOPIC, P_CAM14_CHARACTER, P_CAM14_COLOR, P_CAM14_SIGNAL, P_CAM14_GRAIN, P_CAM14_END,
  P_CAM15_TOPIC, P_CAM15_CHARACTER, P_CAM15_COLOR, P_CAM15_SIGNAL, P_CAM15_GRAIN, P_CAM15_END,
  P_CAM16_TOPIC, P_CAM16_CHARACTER, P_CAM16_COLOR, P_CAM16_SIGNAL, P_CAM16_GRAIN, P_CAM16_END,
  P_CAM17_TOPIC, P_CAM17_CHARACTER, P_CAM17_COLOR, P_CAM17_SIGNAL, P_CAM17_GRAIN, P_CAM17_END,
  P_CAM18_TOPIC, P_CAM18_CHARACTER, P_CAM18_COLOR, P_CAM18_SIGNAL, P_CAM18_GRAIN, P_CAM18_END,
  P_CAM19_TOPIC, P_CAM19_CHARACTER, P_CAM19_COLOR, P_CAM19_SIGNAL, P_CAM19_GRAIN, P_CAM19_END,
  P_TOPIC_CAMERA_END,
  P_TOPIC_COLOR, P_SATURATION, P_BLACK, P_SOFTNESS, P_CHROMA_BLEED, P_CHROMA_X, P_CHROMA_Y, P_TOPIC_COLOR_END,
  P_TOPIC_GRAIN, P_GRAIN_AMOUNT, P_GRAIN_SCALE, P_GRAIN_SPEED, P_TOPIC_GRAIN_END,
  P_TOPIC_DAMAGE, P_TRACKING, P_TRACK_FREQ, P_TRACK_WIDTH, P_DROPOUT, P_BLOCK_GLITCH, P_EVENT_RATE, P_EVENT_DURATION, P_RECOVERY, P_TOPIC_DAMAGE_END,
  P_TOPIC_TAPE, P_TAPE_TYPE, P_TAPE_STRENGTH, P_TAPE_SIZE, P_TAPE_SPEED, P_TAPE_DIRECTION, P_TOPIC_TAPE_END,
  P_TOPIC_OVERLAY, P_OVERLAY_LAYER, P_OVERLAY_MODE, P_OVERLAY_OPACITY, P_OVERLAY_SPEED, P_OVERLAY_OFFSET, P_OVERLAY_LOOP, P_OVERLAY_LOOP_PERIOD, P_TOPIC_OVERLAY_END,
  P_TOPIC_OTHER, P_OTHER_ENGINE, P_OTHER_STRENGTH,
  P_CRAFT_TOPIC, P_CRAFT_TRACKING, P_CRAFT_CHROMA, P_CRAFT_HEADSWITCH, P_CRAFT_SMEAR, P_CRAFT_END,
  P_LEGEND_TOPIC, P_LEGEND_BREAKUP, P_LEGEND_TEAR, P_LEGEND_SIGNAL, P_LEGEND_BLOCKS, P_LEGEND_END,
  P_NTSC_TOPIC, P_NTSC_CHROMA_DELAY, P_NTSC_HEAD_SWITCH, P_NTSC_EDGE_WAVE, P_NTSC_COMPOSITE, P_NTSC_END,
  P_SIGNAL_TOPIC, P_SIGNAL_STRENGTH, P_SIGNAL_LUMA_NOISE, P_SIGNAL_CHROMA_NOISE, P_SIGNAL_TAPE_ERRORS, P_SIGNAL_END,
  P_TOPIC_OTHER_END,
  P_TOPIC_AUDIO, P_AUDIO_ENABLE, P_AUDIO_EVENTS, P_AUDIO_MIX, P_AUDIO_EVENT_AMOUNT, P_AUDIO_CRACKLE, P_AUDIO_DROPOUT, P_AUDIO_SATURATION, P_AUDIO_LOFI, P_TOPIC_AUDIO_END,
  P_TOPIC_MODULES, P_GRAIN_ENABLE, P_TRACKING_ENABLE, P_TAPE_ENABLE, P_OVERLAY_ENABLE, P_OTHER_ENABLE,
  P_CRT_ENABLE, P_CRT_SCANLINES, P_CRT_CURVE, P_CRT_GLOW, P_CRT_RGB, P_CRT_FLICKER, P_CRT_VIGNETTE, P_TOPIC_MODULES_END,
  P_TOPIC_ADV_TAPE, P_TRANSPARENT_TAPE_ENABLE, P_TRANSPARENT_TAPE_STRENGTH, P_TRANSPARENT_TAPE_WIDTH, P_TRANSPARENT_TAPE_SPEED,
  P_TAPE_SWEEP_ENABLE, P_TAPE_SWEEP_STRENGTH, P_TAPE_SWEEP_WIDTH, P_TAPE_SWEEP_SPEED, P_TAPE_SWEEP_INTERVAL, P_TOPIC_ADV_TAPE_END,
  P_TOPIC_GLITCH_PRESETS, P_GLITCH_PRESET, P_GLITCH_PRESET_STRENGTH, P_GLITCH_PRESET_SPEED, P_GLITCH_PRESET_COUNT, P_GLITCH_PRESET_COLOR, P_TOPIC_GLITCH_PRESETS_END,
  P_TOPIC_OSD, P_OSD_PLAY, P_OSD_REC, P_OSD_TIMECODE, P_OSD_DATE, P_OSD_YEAR, P_OSD_MONTH, P_OSD_DAY, P_OSD_OPACITY, P_OSD_SCALE, P_OSD_POSITION, P_OSD_SAFE_MARGIN, P_TOPIC_OSD_END,
  P_TOPIC_BUILTIN_OVERLAYS, P_BUILTIN_OVERLAY, P_BUILTIN_OVERLAY_STRENGTH, P_BUILTIN_OVERLAY_SPEED, P_BUILTIN_OVERLAY_RATE, P_BUILTIN_OVERLAY_COLOR, P_BUILTIN_OVERLAY_BLEND, P_TOPIC_BUILTIN_OVERLAYS_END,
  P_COUNT
};
static PF_Err About(PF_InData *in_data, PF_OutData *out_data, PF_ParamDef *params[], PF_LayerDef *output) {
  std::snprintf(out_data->return_msg, sizeof(out_data->return_msg), "%s 1.2 | Native VHS video processing for After Effects 23.x", PLUGIN_NAME);
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

  PF_ADD_POPUP("VHS Camera Engine", 19, 1,
    "Fxc6 Reference|Gen 7 Reference|JVC GR-C7U (1986)|VHS-R20 Reference|Fall-78 Reference|ERC-V2S Reference|JVC GR-SXM740U|CPM35|Classic U|IR|FQS|CCDR|FXN|D Half|Retro VHS 01|Retro VHS 02|Retro VHS 03|Retro VHS 04|Retro VHS 05", P_CAMERA);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Master VHS Strength", 0, 2000, 0, 2000, 1000, PF_Precision_TENTHS, 0, 0, P_MASTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Continuous VHS Treatment", "Enable", TRUE, 0, P_CONTINUOUS);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Event Glitches", "Enable", FALSE, 0, P_EVENTS);
  AEFX_CLR_STRUCT(def);
  PF_ADD_SLIDER("Random Seed", 0, 9999, 0, 9999, 17, P_SEED);

  AEFX_CLR_STRUCT(def);

  PF_ADD_TOPIC("Camera-Specific Controls", P_TOPIC_CAMERA);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Fxc6 Reference Profile", P_FXC6_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Fxc6 Character", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_FXC6_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Fxc6 Color Response", 0, 100, 0, 100, 55, PF_Precision_TENTHS, 0, 0, P_FXC6_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Fxc6 Signal Damage", 0, 100, 0, 100, 35, PF_Precision_TENTHS, 0, 0, P_FXC6_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Fxc6 Grain", 0, 100, 0, 100, 28, PF_Precision_TENTHS, 0, 0, P_FXC6_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_FXC6_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Gen 7 Reference Profile", P_GEN7_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Gen 7 Character", 0, 100, 0, 100, 75, PF_Precision_TENTHS, 0, 0, P_GEN7_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Gen 7 Color Response", 0, 100, 0, 100, 80, PF_Precision_TENTHS, 0, 0, P_GEN7_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Gen 7 Signal Damage", 0, 100, 0, 100, 85, PF_Precision_TENTHS, 0, 0, P_GEN7_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Gen 7 Grain", 0, 100, 0, 100, 70, PF_Precision_TENTHS, 0, 0, P_GEN7_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_GEN7_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("JVC GR-C7U (1986) Profile", P_JVC_C7_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("JVC GR-C7U Character", 0, 100, 0, 100, 82, PF_Precision_TENTHS, 0, 0, P_JVC_C7_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("JVC GR-C7U Color Response", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_JVC_C7_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("JVC GR-C7U Signal Damage", 0, 100, 0, 100, 75, PF_Precision_TENTHS, 0, 0, P_JVC_C7_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("JVC GR-C7U Grain", 0, 100, 0, 100, 60, PF_Precision_TENTHS, 0, 0, P_JVC_C7_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_JVC_C7_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("VHS-R20 Reference Profile", P_R20_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("VHS-R20 Character", 0, 100, 0, 100, 55, PF_Precision_TENTHS, 0, 0, P_R20_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("VHS-R20 Color Response", 0, 100, 0, 100, 70, PF_Precision_TENTHS, 0, 0, P_R20_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("VHS-R20 Signal Damage", 0, 100, 0, 100, 50, PF_Precision_TENTHS, 0, 0, P_R20_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("VHS-R20 Grain", 0, 100, 0, 100, 30, PF_Precision_TENTHS, 0, 0, P_R20_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_R20_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Fall-78 Reference Profile", P_FALL78_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Fall-78 Character", 0, 100, 0, 100, 30, PF_Precision_TENTHS, 0, 0, P_FALL78_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Fall-78 Color Response", 0, 100, 0, 100, 45, PF_Precision_TENTHS, 0, 0, P_FALL78_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Fall-78 Signal Damage", 0, 100, 0, 100, 25, PF_Precision_TENTHS, 0, 0, P_FALL78_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Fall-78 Grain", 0, 100, 0, 100, 15, PF_Precision_TENTHS, 0, 0, P_FALL78_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_FALL78_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("ERC-V2S Reference Profile", P_ERC_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("ERC-V2S Character", 0, 100, 0, 100, 60, PF_Precision_TENTHS, 0, 0, P_ERC_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("ERC-V2S Color Response", 0, 100, 0, 100, 90, PF_Precision_TENTHS, 0, 0, P_ERC_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("ERC-V2S Signal Damage", 0, 100, 0, 100, 45, PF_Precision_TENTHS, 0, 0, P_ERC_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("ERC-V2S Grain", 0, 100, 0, 100, 40, PF_Precision_TENTHS, 0, 0, P_ERC_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_ERC_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("JVC GR-SXM740U Profile", P_JVC_SXM_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("JVC GR-SXM740U Character", 0, 100, 0, 100, 50, PF_Precision_TENTHS, 0, 0, P_JVC_SXM_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("JVC GR-SXM740U Color Response", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_JVC_SXM_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("JVC GR-SXM740U Signal Damage", 0, 100, 0, 100, 35, PF_Precision_TENTHS, 0, 0, P_JVC_SXM_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("JVC GR-SXM740U Grain", 0, 100, 0, 100, 25, PF_Precision_TENTHS, 0, 0, P_JVC_SXM_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_JVC_SXM_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("CPM35 Camera Engine", P_CAM08_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("CPM35 Character", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_CAM08_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("CPM35 Color Response", 0, 100, 0, 100, 60, PF_Precision_TENTHS, 0, 0, P_CAM08_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("CPM35 Signal Damage", 0, 100, 0, 100, 40, PF_Precision_TENTHS, 0, 0, P_CAM08_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("CPM35 Grain", 0, 100, 0, 100, 30, PF_Precision_TENTHS, 0, 0, P_CAM08_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_CAM08_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Classic U Camera Engine", P_CAM09_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Classic U Character", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_CAM09_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Classic U Color Response", 0, 100, 0, 100, 60, PF_Precision_TENTHS, 0, 0, P_CAM09_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Classic U Signal Damage", 0, 100, 0, 100, 40, PF_Precision_TENTHS, 0, 0, P_CAM09_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Classic U Grain", 0, 100, 0, 100, 30, PF_Precision_TENTHS, 0, 0, P_CAM09_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_CAM09_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("IR Camera Engine", P_CAM10_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("IR Character", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_CAM10_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("IR Color Response", 0, 100, 0, 100, 60, PF_Precision_TENTHS, 0, 0, P_CAM10_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("IR Signal Damage", 0, 100, 0, 100, 40, PF_Precision_TENTHS, 0, 0, P_CAM10_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("IR Grain", 0, 100, 0, 100, 30, PF_Precision_TENTHS, 0, 0, P_CAM10_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_CAM10_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("FQS Camera Engine", P_CAM11_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("FQS Character", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_CAM11_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("FQS Color Response", 0, 100, 0, 100, 60, PF_Precision_TENTHS, 0, 0, P_CAM11_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("FQS Signal Damage", 0, 100, 0, 100, 40, PF_Precision_TENTHS, 0, 0, P_CAM11_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("FQS Grain", 0, 100, 0, 100, 30, PF_Precision_TENTHS, 0, 0, P_CAM11_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_CAM11_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("CCDR Camera Engine", P_CAM12_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("CCDR Character", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_CAM12_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("CCDR Color Response", 0, 100, 0, 100, 60, PF_Precision_TENTHS, 0, 0, P_CAM12_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("CCDR Signal Damage", 0, 100, 0, 100, 40, PF_Precision_TENTHS, 0, 0, P_CAM12_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("CCDR Grain", 0, 100, 0, 100, 30, PF_Precision_TENTHS, 0, 0, P_CAM12_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_CAM12_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("FXN Camera Engine", P_CAM13_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("FXN Character", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_CAM13_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("FXN Color Response", 0, 100, 0, 100, 60, PF_Precision_TENTHS, 0, 0, P_CAM13_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("FXN Signal Damage", 0, 100, 0, 100, 40, PF_Precision_TENTHS, 0, 0, P_CAM13_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("FXN Grain", 0, 100, 0, 100, 30, PF_Precision_TENTHS, 0, 0, P_CAM13_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_CAM13_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("D Half Camera Engine", P_CAM14_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("D Half Character", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_CAM14_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("D Half Color Response", 0, 100, 0, 100, 60, PF_Precision_TENTHS, 0, 0, P_CAM14_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("D Half Signal Damage", 0, 100, 0, 100, 40, PF_Precision_TENTHS, 0, 0, P_CAM14_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("D Half Grain", 0, 100, 0, 100, 30, PF_Precision_TENTHS, 0, 0, P_CAM14_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_CAM14_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Retro VHS 01 Camera Engine", P_CAM15_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 01 Character", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_CAM15_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 01 Color Response", 0, 100, 0, 100, 60, PF_Precision_TENTHS, 0, 0, P_CAM15_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 01 Signal Damage", 0, 100, 0, 100, 40, PF_Precision_TENTHS, 0, 0, P_CAM15_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 01 Grain", 0, 100, 0, 100, 30, PF_Precision_TENTHS, 0, 0, P_CAM15_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_CAM15_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Retro VHS 02 Camera Engine", P_CAM16_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 02 Character", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_CAM16_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 02 Color Response", 0, 100, 0, 100, 60, PF_Precision_TENTHS, 0, 0, P_CAM16_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 02 Signal Damage", 0, 100, 0, 100, 40, PF_Precision_TENTHS, 0, 0, P_CAM16_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 02 Grain", 0, 100, 0, 100, 30, PF_Precision_TENTHS, 0, 0, P_CAM16_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_CAM16_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Retro VHS 03 Camera Engine", P_CAM17_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 03 Character", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_CAM17_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 03 Color Response", 0, 100, 0, 100, 60, PF_Precision_TENTHS, 0, 0, P_CAM17_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 03 Signal Damage", 0, 100, 0, 100, 40, PF_Precision_TENTHS, 0, 0, P_CAM17_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 03 Grain", 0, 100, 0, 100, 30, PF_Precision_TENTHS, 0, 0, P_CAM17_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_CAM17_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Retro VHS 04 Camera Engine", P_CAM18_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 04 Character", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_CAM18_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 04 Color Response", 0, 100, 0, 100, 60, PF_Precision_TENTHS, 0, 0, P_CAM18_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 04 Signal Damage", 0, 100, 0, 100, 40, PF_Precision_TENTHS, 0, 0, P_CAM18_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 04 Grain", 0, 100, 0, 100, 30, PF_Precision_TENTHS, 0, 0, P_CAM18_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_CAM18_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Retro VHS 05 Camera Engine", P_CAM19_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 05 Character", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_CAM19_CHARACTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 05 Color Response", 0, 100, 0, 100, 60, PF_Precision_TENTHS, 0, 0, P_CAM19_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 05 Signal Damage", 0, 100, 0, 100, 40, PF_Precision_TENTHS, 0, 0, P_CAM19_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Retro VHS 05 Grain", 0, 100, 0, 100, 30, PF_Precision_TENTHS, 0, 0, P_CAM19_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_CAM19_END);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_CAMERA_END);

  AEFX_CLR_STRUCT(def);

  PF_ADD_TOPIC("Color Controls", P_TOPIC_COLOR);
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

  PF_ADD_TOPIC("Grain and Signal Noise", P_TOPIC_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Colored Grain Amount", 0, 100, 0, 100, 0, PF_Precision_TENTHS, 0, 0, P_GRAIN_AMOUNT);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Grain Scale", 1, 100, 1, 100, 28, PF_Precision_TENTHS, 0, 0, P_GRAIN_SCALE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Grain Motion Speed", 0, 400, 0, 400, 100, PF_Precision_TENTHS, 0, 0, P_GRAIN_SPEED);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_GRAIN_END);

  AEFX_CLR_STRUCT(def);

  PF_ADD_TOPIC("Tracking and Damage Events", P_TOPIC_DAMAGE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Tracking Noise", 0, 100, 0, 100, 0, PF_Precision_TENTHS, 0, 0, P_TRACKING);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Tracking Event Frequency", 0, 100, 0, 100, 25, PF_Precision_TENTHS, 0, 0, P_TRACK_FREQ);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Tracking Band Width", 1, 100, 1, 100, 18, PF_Precision_TENTHS, 0, 0, P_TRACK_WIDTH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Tape Dropouts", 0, 100, 0, 100, 0, PF_Precision_TENTHS, 0, 0, P_DROPOUT);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Block Corruption", 0, 100, 0, 100, 0, PF_Precision_TENTHS, 0, 0, P_BLOCK_GLITCH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Events per Minute", 0, 60, 0, 60, 8, PF_Precision_TENTHS, 0, 0, P_EVENT_RATE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Event Duration (seconds)", 0.02, 2.0, 0.02, 2.0, 0.18, PF_Precision_HUNDREDTHS, 0, 0, P_EVENT_DURATION);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Recovery (seconds)", 0, 2.0, 0, 2.0, 0.12, PF_Precision_HUNDREDTHS, 0, 0, P_RECOVERY);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_DAMAGE_END);

  AEFX_CLR_STRUCT(def);

  PF_ADD_TOPIC("Tape Band / Tape Noise", P_TOPIC_TAPE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Tape Type", 6, 1, "Off|Horizontal Band|Bottom Head-Switch|Vertical Roll|Side Tear|RF Hiss / Chroma Noise", P_TAPE_TYPE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Tape Strength", 0, 100, 0, 100, 35, PF_Precision_TENTHS, 0, 0, P_TAPE_STRENGTH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Tape Size", 1, 100, 1, 100, 18, PF_Precision_TENTHS, 0, 0, P_TAPE_SIZE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Tape Speed", 0, 300, 0, 300, 60, PF_Precision_TENTHS, 0, 0, P_TAPE_SPEED);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Tape Direction", 4, 1, "Top to Bottom|Bottom to Top|Left to Right|Right to Left", P_TAPE_DIRECTION);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_TAPE_END);

  AEFX_CLR_STRUCT(def);

  PF_ADD_TOPIC("VHS Overlay Library / Source Layer", P_TOPIC_OVERLAY);
  AEFX_CLR_STRUCT(def);
  PF_ADD_LAYER("Overlay Layer (optional)", PF_LayerDefault_NONE, P_OVERLAY_LAYER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Overlay Display Mode", 6, 1, "Normal|Blend|Add|Screen|Multiply|Luma Key", P_OVERLAY_MODE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Overlay Opacity", 0, 100, 0, 100, 60, PF_Precision_TENTHS, 0, 0, P_OVERLAY_OPACITY);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Overlay Speed", 0, 400, 0, 400, 100, PF_Precision_TENTHS, 0, 0, P_OVERLAY_SPEED);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Overlay Time Offset (seconds)", -60, 60, -60, 60, 0, PF_Precision_TENTHS, 0, 0, P_OVERLAY_OFFSET);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Loop Overlay", "Enable", FALSE, 0, P_OVERLAY_LOOP);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Overlay Loop Period (seconds)", 0.1, 120, 0.1, 120, 2, PF_Precision_TENTHS, 0, 0, P_OVERLAY_LOOP_PERIOD);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_OVERLAY_END);

  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Other VHS Engines (independent implementations)", P_TOPIC_OTHER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Other VHS Engine", 5, 1, "Off|VHS Craft Reference|VHS Legend Reference File|NTSC-RS Signal Model|Signal Reference", P_OTHER_ENGINE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Other Engine Strength", 0, 100, 0, 100, 50, PF_Precision_TENTHS, 0, 0, P_OTHER_STRENGTH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("VHS Craft Reference Controls", P_CRAFT_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Craft Tracking Interference", 0, 100, 0, 100, 25, PF_Precision_TENTHS, 0, 0, P_CRAFT_TRACKING);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Craft Chroma Smear", 0, 100, 0, 100, 30, PF_Precision_TENTHS, 0, 0, P_CRAFT_CHROMA);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Craft Head-Switch Spikes", 0, 100, 0, 100, 20, PF_Precision_TENTHS, 0, 0, P_CRAFT_HEADSWITCH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Craft Signal Blur", 0, 100, 0, 100, 15, PF_Precision_TENTHS, 0, 0, P_CRAFT_SMEAR);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_CRAFT_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("VHS Legend Reference Controls", P_LEGEND_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Legend Signal Breakdown", 0, 100, 0, 100, 25, PF_Precision_TENTHS, 0, 0, P_LEGEND_BREAKUP);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Legend Horizontal Tear", 0, 100, 0, 100, 20, PF_Precision_TENTHS, 0, 0, P_LEGEND_TEAR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Legend Signal Loss", 0, 100, 0, 100, 10, PF_Precision_TENTHS, 0, 0, P_LEGEND_SIGNAL);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Legend Block Corruption", 0, 100, 0, 100, 15, PF_Precision_TENTHS, 0, 0, P_LEGEND_BLOCKS);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_LEGEND_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("NTSC-RS Reference Controls", P_NTSC_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("NTSC Chroma Delay", 0, 100, 0, 100, 25, PF_Precision_TENTHS, 0, 0, P_NTSC_CHROMA_DELAY);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("NTSC Head Switching", 0, 100, 0, 100, 18, PF_Precision_TENTHS, 0, 0, P_NTSC_HEAD_SWITCH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("NTSC Edge Wave", 0, 100, 0, 100, 15, PF_Precision_TENTHS, 0, 0, P_NTSC_EDGE_WAVE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("NTSC Composite Signal", 0, 100, 0, 100, 35, PF_Precision_TENTHS, 0, 0, P_NTSC_COMPOSITE);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_NTSC_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Signal Reference Controls", P_SIGNAL_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Signal Strength", 0, 100, 0, 100, 35, PF_Precision_TENTHS, 0, 0, P_SIGNAL_STRENGTH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Signal Luma Noise", 0, 100, 0, 100, 20, PF_Precision_TENTHS, 0, 0, P_SIGNAL_LUMA_NOISE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Signal Chroma Noise", 0, 100, 0, 100, 25, PF_Precision_TENTHS, 0, 0, P_SIGNAL_CHROMA_NOISE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Signal Tape Errors", 0, 100, 0, 100, 20, PF_Precision_TENTHS, 0, 0, P_SIGNAL_TAPE_ERRORS);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_SIGNAL_END);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_OTHER_END);

  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Audio Glitches (real audio processing)", P_TOPIC_AUDIO);
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
  PF_ADD_TOPIC("Independent Effect Modules", P_TOPIC_MODULES);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable Grain / Noise", "Enable", FALSE, 0, P_GRAIN_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable Tracking Damage", "Enable", TRUE, 0, P_TRACKING_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable Tape Effects", "Enable", TRUE, 0, P_TAPE_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable Overlay Layer", "Enable", FALSE, 0, P_OVERLAY_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable Other Signal Engines", "Enable", TRUE, 0, P_OTHER_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable CRT TV Engine", "Enable", FALSE, 0, P_CRT_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("CRT Scanline Strength", 0, 100, 0, 100, 35, PF_Precision_TENTHS, 0, 0, P_CRT_SCANLINES);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("CRT Curvature", 0, 100, 0, 100, 20, PF_Precision_TENTHS, 0, 0, P_CRT_CURVE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("CRT Glow", 0, 100, 0, 100, 25, PF_Precision_TENTHS, 0, 0, P_CRT_GLOW);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("CRT RGB Convergence", 0, 100, 0, 100, 18, PF_Precision_TENTHS, 0, 0, P_CRT_RGB);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("CRT Flicker", 0, 100, 0, 100, 8, PF_Precision_TENTHS, 0, 0, P_CRT_FLICKER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("CRT Edge Vignette", 0, 100, 0, 100, 28, PF_Precision_TENTHS, 0, 0, P_CRT_VIGNETTE);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_MODULES_END);

  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Transparent Tape and Tape Sweep", P_TOPIC_ADV_TAPE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Procedural Transparent Tape", "Enable", FALSE, 0, P_TRANSPARENT_TAPE_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Transparent Tape Strength", 0, 100, 0, 100, 35, PF_Precision_TENTHS, 0, 0, P_TRANSPARENT_TAPE_STRENGTH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Transparent Tape Width", 1, 100, 1, 100, 30, PF_Precision_TENTHS, 0, 0, P_TRANSPARENT_TAPE_WIDTH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Transparent Tape Speed", 0, 300, 0, 300, 45, PF_Precision_TENTHS, 0, 0, P_TRANSPARENT_TAPE_SPEED);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Moving Tape Sweep", "Enable", FALSE, 0, P_TAPE_SWEEP_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Tape Sweep Strength", 0, 100, 0, 100, 50, PF_Precision_TENTHS, 0, 0, P_TAPE_SWEEP_STRENGTH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Tape Sweep Width", 1, 100, 1, 100, 20, PF_Precision_TENTHS, 0, 0, P_TAPE_SWEEP_WIDTH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Tape Sweep Speed", 1, 300, 1, 300, 80, PF_Precision_TENTHS, 0, 0, P_TAPE_SWEEP_SPEED);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Tape Sweep Interval (seconds)", 0.1, 30, 0.1, 30, 5, PF_Precision_TENTHS, 0, 0, P_TAPE_SWEEP_INTERVAL);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_ADV_TAPE_END);

  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Glitch Cam / Animated Signal Presets", P_TOPIC_GLITCH_PRESETS);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Glitch Cam Preset", 16, 1, "Off|Glitch Cam 01|Glitch Cam 02|Glitch Cam 03|Glitch Cam 04|Glitch Cam 05|Glitch Cam 06|Glitch Cam 07|Glitch Cam 08|Glitch Cam 09|Glitch Cam 10|Glitch Cam 11|Glitch Cam 12|Glitch Cam 13|Glitch Cam 14|Glitch Cam 15", P_GLITCH_PRESET);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Glitch Cam Strength", 0, 100, 0, 100, 35, PF_Precision_TENTHS, 0, 0, P_GLITCH_PRESET_STRENGTH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Glitch Cam Motion Speed", 0, 400, 0, 400, 100, PF_Precision_TENTHS, 0, 0, P_GLITCH_PRESET_SPEED);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Glitch Events per Minute", 0, 120, 0, 120, 12, PF_Precision_TENTHS, 0, 0, P_GLITCH_PRESET_COUNT);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Use Colored Glitch Channels", "Enable", TRUE, 0, P_GLITCH_PRESET_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_GLITCH_PRESETS_END);

  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Capture OSD / Date / PLAY / REC", P_TOPIC_OSD);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Show PLAY Indicator", "Enable", FALSE, 0, P_OSD_PLAY);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Show REC Indicator", "Enable", FALSE, 0, P_OSD_REC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Show Running Timecode", "Enable", FALSE, 0, P_OSD_TIMECODE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Show Editable Date Stamp", "Enable", FALSE, 0, P_OSD_DATE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_SLIDER("OSD Date Year", 1980, 2099, 1980, 2099, 1998, P_OSD_YEAR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_SLIDER("OSD Date Month", 1, 12, 1, 12, 10, P_OSD_MONTH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_SLIDER("OSD Date Day", 1, 31, 1, 31, 24, P_OSD_DAY);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("OSD Opacity", 0, 100, 0, 100, 85, PF_Precision_TENTHS, 0, 0, P_OSD_OPACITY);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("OSD Text Scale", 1, 8, 1, 8, 2, PF_Precision_TENTHS, 0, 0, P_OSD_SCALE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("OSD Position", 4, 1, "Bottom Left|Bottom Right|Top Left|Top Right", P_OSD_POSITION);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("OSD Safe Margin", 0, 20, 0, 20, 4, PF_Precision_TENTHS, 0, 0, P_OSD_SAFE_MARGIN);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_OSD_END);

  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Built-in VHS Overlay Effects", P_TOPIC_BUILTIN_OVERLAYS);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Native Overlay Preset", 22, 1,
    "Off|Bad Signal Tear|Moving Tracking Bar|Head-Switch Noise|RF Interference|Tape Wrinkle|Signal Dropout|Chroma Bleed|Chroma Phase Drift|Vertical Roll|Frame Jitter|Line Echo|Capture Code|REC HUD|CRT Burn-In|Tape Dust|Sync Loss|Luma Pump|Edge Rainbow|Ghosting|Bottom Tracking Noise|Analog Line Damage", P_BUILTIN_OVERLAY);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Native Overlay Strength", 0, 100, 0, 100, 35, PF_Precision_TENTHS, 0, 0, P_BUILTIN_OVERLAY_STRENGTH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Native Overlay Motion Speed", 0, 400, 0, 400, 100, PF_Precision_TENTHS, 0, 0, P_BUILTIN_OVERLAY_SPEED);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Overlay Glitch Events per Minute", 0, 120, 0, 120, 12, PF_Precision_TENTHS, 0, 0, P_BUILTIN_OVERLAY_RATE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Overlay Color Channels", "Enable", TRUE, 0, P_BUILTIN_OVERLAY_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Overlay Blend-In", 0, 100, 0, 100, 35, PF_Precision_TENTHS, 0, 0, P_BUILTIN_OVERLAY_BLEND);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_BUILTIN_OVERLAYS_END);

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
static inline uint8_t glyphRow(char c, int row) {
  static const uint8_t digits[10][7] = {
    {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},{30,1,1,14,1,1,30},{2,6,10,18,31,2,2},
    {31,16,16,30,1,1,30},{6,8,16,30,17,17,14},{31,1,2,4,8,8,8},{14,17,17,14,17,17,14},{14,17,17,15,1,2,12}};
  static const uint8_t P[7]={30,17,17,30,16,16,16}, L[7]={16,16,16,16,16,16,31}, A[7]={14,17,17,31,17,17,17}, Y[7]={17,17,10,4,4,4,4}, R[7]={30,17,17,30,20,18,17}, E[7]={31,16,16,30,16,16,31}, C[7]={14,17,16,16,16,17,14}, colon[7]={0,4,4,0,4,4,0}, dash[7]={0,0,0,31,0,0,0};
  if (c>='0'&&c<='9') return digits[c-'0'][row];
  const uint8_t *g = c=='P'?P:c=='L'?L:c=='A'?A:c=='Y'?Y:c=='R'?R:c=='E'?E:c=='C'?C:c==':'?colon:c=='-'?dash:nullptr;
  return g ? g[row] : 0;
}
static inline bool osdGlyphPixel(char c, int gx, int gy) { return gx>=0&&gx<5&&gy>=0&&gy<7&&((glyphRow(c,gy)>>(4-gx))&1u)!=0; }

struct Settings {
  int camera=1, seed=17, tapeType=0, tapeDir=0, overlayMode=0;
  float master=.35f, continuous=1, events=1, saturation=1, black=0, softness=.12f, bleed=.20f;
  float chromaX=2, chromaY=0, grain=.08f, grainScale=28, grainSpeed=1, tracking=.18f, trackFreq=.25f, trackWidth=.18f;
  float dropout=.10f, blocks=.05f, eventRate=8, eventDuration=.18f, recovery=.12f, tapeStrength=.35f, tapeSize=.18f, tapeSpeed=.6f;
};

static float getSlider(PF_ParamDef *params[], int id, float scale) { return (float)params[id]->u.fs_d.value / scale; }
static int getInt(PF_ParamDef *params[], int id) { return params[id]->u.sd.value; }
static bool getCheck(PF_ParamDef *params[], int id) { return params[id]->u.bd.value != 0; }
static Settings ReadSettings(PF_ParamDef *p[]) {
  Settings s;
  s.camera = p[P_CAMERA]->u.pd.value;
  s.master = getSlider(p,P_MASTER,100.0f);
  s.continuous = getCheck(p,P_CONTINUOUS) ? 1.0f : 0.0f;
  s.events = getCheck(p,P_EVENTS) ? 1.0f : 0.0f;
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
  s.trackFreq = getSlider(p,P_TRACK_FREQ,100.0f);
  s.trackWidth = getSlider(p,P_TRACK_WIDTH,100.0f);
  s.dropout = getSlider(p,P_DROPOUT,100.0f);
  s.blocks = getSlider(p,P_BLOCK_GLITCH,100.0f);
  s.eventRate = getSlider(p,P_EVENT_RATE,1.0f);
  s.eventDuration = getSlider(p,P_EVENT_DURATION,1.0f);
  s.recovery = getSlider(p,P_RECOVERY,1.0f);
  s.tapeType = p[P_TAPE_TYPE]->u.pd.value - 1;
  s.tapeStrength = getSlider(p,P_TAPE_STRENGTH,100.0f);
  s.tapeSize = getSlider(p,P_TAPE_SIZE,100.0f);
  s.tapeSpeed = getSlider(p,P_TAPE_SPEED,100.0f);
  s.tapeDir = p[P_TAPE_DIRECTION]->u.pd.value - 1;
  s.overlayMode = p[P_OVERLAY_MODE]->u.pd.value - 1;
  return s;
}

struct CameraProfileData {
  float saturation;     // Chroma saturation multiplier
  float contrast;       // Contrast multiplier
  float warm;           // Red/Amber warmth (+ = red/amber, - = cyan/blue)
  float green;          // Green/Magenta tint (+ = green, - = magenta)
  float rGain, gGain, bGain; // Channel gain multipliers
  float gamma;          // Gamma curve exponent
  float pedestal;       // Shadow lift / black level offset
  float shoulder;       // Highlight compression / shoulder roll-off
  float chromaBleed;    // Default chroma bleed intensity
  float chromaOffsetX;  // Default horizontal chroma offset (pixels)
  float chromaOffsetY;  // Default vertical chroma offset (pixels)
  float edgeRinging;    // Transient overshoot on horizontal luma transitions
  float noiseFloor;     // Native signal noise contribution
};

static const CameraProfileData kCameraProfiles[19] = {
  // 1: Fxc6 Reference - Consumer 8mm/VHS camcorder
  { 1.10f, 1.08f,  0.08f, -0.02f, 1.06f, 1.00f, 0.94f, 1.02f, 0.02f, 0.08f, 0.25f,  3.0f, 0.0f, 0.05f, 0.04f },
  // 2: Gen 7 Reference - Multi-generation tape dub
  { 0.75f, 1.18f,  0.05f,  0.06f, 0.96f, 1.05f, 0.88f, 1.12f, 0.08f, 0.16f, 0.50f,  6.0f, 1.0f, 0.08f, 0.12f },
  // 3: JVC GR-C7U (1986) - Retro 80s camcorder (Back to the Future)
  { 1.35f, 1.22f,  0.22f, -0.05f, 1.25f, 0.96f, 0.76f, 0.92f, 0.03f, 0.20f, 0.38f,  5.0f,-1.0f, 0.18f, 0.06f },
  // 4: VHS-R20 Reference - Industrial / security deck
  { 0.60f, 1.30f, -0.15f,  0.08f, 0.82f, 1.06f, 1.28f, 0.86f, 0.04f, 0.12f, 0.15f, -4.0f, 0.0f, 0.25f, 0.16f },
  // 5: Fall-78 Reference - Early vintage 70s home tape
  { 0.70f, 0.88f,  0.18f,  0.03f, 1.14f, 0.98f, 0.80f, 1.16f, 0.06f, 0.05f, 0.45f,  2.0f, 2.0f, 0.04f, 0.08f },
  // 6: ERC-V2S Reference - Professional S-VHS
  { 1.15f, 1.08f,  0.02f,  0.00f, 1.02f, 1.02f, 1.00f, 0.98f, 0.00f, 0.02f, 0.10f,  1.0f, 0.0f, 0.12f, 0.02f },
  // 7: JVC GR-SXM740U - Compact VHS (S-VHS ET)
  { 1.10f, 1.12f, -0.05f, -0.06f, 1.08f, 0.94f, 1.08f, 0.96f, 0.01f, 0.08f, 0.20f,  3.0f, 0.0f, 0.20f, 0.05f },
  // 8: CPM35 — distinct tuned signal/color response
  { 0.78f, 1.12f, 0.10f, -0.04f, 1.08f, 0.97f, 0.88f, 1.08f, 0.05f, 0.12f, 0.32f, 4.00f, 1.00f, 0.12f, 0.08f },
  // 9: Classic U — distinct tuned signal/color response
  { 0.68f, 0.94f, 0.18f, 0.04f, 1.13f, 0.99f, 0.82f, 1.15f, 0.07f, 0.04f, 0.38f, 2.00f, 2.00f, 0.05f, 0.09f },
  // 10: IR — distinct tuned signal/color response
  { 0.45f, 1.28f, -0.10f, 0.12f, 0.82f, 1.08f, 1.18f, 0.88f, 0.02f, 0.18f, 0.12f, -2.00f, 0.00f, 0.24f, 0.14f },
  // 11: FQS — distinct tuned signal/color response
  { 1.20f, 1.04f, 0.02f, -0.02f, 1.04f, 1.00f, 0.98f, 1.00f, 0.01f, 0.06f, 0.18f, 2.00f, 0.00f, 0.10f, 0.04f },
  // 12: CCDR — distinct tuned signal/color response
  { 0.82f, 1.16f, -0.03f, 0.07f, 0.94f, 1.06f, 1.04f, 0.96f, 0.03f, 0.14f, 0.28f, 5.00f, 1.00f, 0.16f, 0.10f },
  // 13: FXN — distinct tuned signal/color response
  { 1.28f, 1.10f, 0.12f, -0.06f, 1.12f, 0.98f, 0.84f, 0.98f, 0.02f, 0.10f, 0.24f, 3.00f, 0.00f, 0.14f, 0.06f },
  // 14: D Half — distinct tuned signal/color response
  { 0.62f, 0.92f, 0.16f, 0.02f, 1.10f, 0.99f, 0.85f, 1.14f, 0.06f, 0.02f, 0.42f, 1.00f, 1.00f, 0.06f, 0.10f },
  // 15: Retro VHS 01 — distinct tuned signal/color response
  { 0.72f, 1.12f, 0.12f, 0.04f, 1.10f, 0.98f, 0.83f, 1.08f, 0.05f, 0.12f, 0.38f, 4.00f, 1.00f, 0.12f, 0.09f },
  // 16: Retro VHS 02 — distinct tuned signal/color response
  { 0.92f, 1.05f, 0.04f, -0.03f, 1.02f, 1.01f, 0.96f, 1.02f, 0.02f, 0.08f, 0.24f, 3.00f, 0.00f, 0.09f, 0.05f },
  // 17: Retro VHS 03 — distinct tuned signal/color response
  { 0.55f, 1.22f, -0.12f, 0.08f, 0.86f, 1.05f, 1.20f, 0.90f, 0.05f, 0.16f, 0.18f, -3.00f, 1.00f, 0.22f, 0.13f },
  // 18: Retro VHS 04 — distinct tuned signal/color response
  { 1.16f, 1.10f, 0.18f, -0.04f, 1.15f, 0.97f, 0.78f, 0.94f, 0.02f, 0.14f, 0.34f, 5.00f, -1.00f, 0.17f, 0.07f },
  // 19: Retro VHS 05 — distinct tuned signal/color response
  { 0.80f, 0.98f, 0.02f, 0.06f, 0.98f, 1.04f, 0.92f, 1.06f, 0.04f, 0.06f, 0.30f, 2.00f, 1.00f, 0.11f, 0.08f }
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
  const int inStride = src->rowbytes / (int)sizeof(PF_Pixel);
  const int outStride = output->rowbytes / (int)sizeof(PF_Pixel);
  if (inStride <= 0 || outStride <= 0) return PF_Err_NONE;

  const float timeSec = in_data->time_scale ? (float)in_data->current_time / (float)in_data->time_scale : 0.0f;
  const int frame = (in_data->time_step != 0) ? (int)(in_data->current_time / in_data->time_step) : (int)std::floor(timeSec * 24.0f + 0.5f);

  // Master VHS Strength (UI 0..2000, default 1000 -> s.master 10.0).
  const float strength = std::min(20.0f, std::max(0.0f, s.master));

  // Identity bypass when Master Strength is zero: bit-exact copy of input pixels.
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

  // Camera profile selection (1 to 19)
  const int ci = clampi(s.camera - 1, 0, 18);
  const CameraProfileData &prof = kCameraProfiles[ci];

  static const int cameraParamIds[19][4] = {
    {P_FXC6_CHARACTER, P_FXC6_COLOR, P_FXC6_SIGNAL, P_FXC6_GRAIN},
    {P_GEN7_CHARACTER, P_GEN7_COLOR, P_GEN7_SIGNAL, P_GEN7_GRAIN},
    {P_JVC_C7_CHARACTER, P_JVC_C7_COLOR, P_JVC_C7_SIGNAL, P_JVC_C7_GRAIN},
    {P_R20_CHARACTER, P_R20_COLOR, P_R20_SIGNAL, P_R20_GRAIN},
    {P_FALL78_CHARACTER, P_FALL78_COLOR, P_FALL78_SIGNAL, P_FALL78_GRAIN},
    {P_ERC_CHARACTER, P_ERC_COLOR, P_ERC_SIGNAL, P_ERC_GRAIN},
    {P_JVC_SXM_CHARACTER, P_JVC_SXM_COLOR, P_JVC_SXM_SIGNAL, P_JVC_SXM_GRAIN},
    {P_CAM08_CHARACTER, P_CAM08_COLOR, P_CAM08_SIGNAL, P_CAM08_GRAIN},
    {P_CAM09_CHARACTER, P_CAM09_COLOR, P_CAM09_SIGNAL, P_CAM09_GRAIN},
    {P_CAM10_CHARACTER, P_CAM10_COLOR, P_CAM10_SIGNAL, P_CAM10_GRAIN},
    {P_CAM11_CHARACTER, P_CAM11_COLOR, P_CAM11_SIGNAL, P_CAM11_GRAIN},
    {P_CAM12_CHARACTER, P_CAM12_COLOR, P_CAM12_SIGNAL, P_CAM12_GRAIN},
    {P_CAM13_CHARACTER, P_CAM13_COLOR, P_CAM13_SIGNAL, P_CAM13_GRAIN},
    {P_CAM14_CHARACTER, P_CAM14_COLOR, P_CAM14_SIGNAL, P_CAM14_GRAIN},
    {P_CAM15_CHARACTER, P_CAM15_COLOR, P_CAM15_SIGNAL, P_CAM15_GRAIN},
    {P_CAM16_CHARACTER, P_CAM16_COLOR, P_CAM16_SIGNAL, P_CAM16_GRAIN},
    {P_CAM17_CHARACTER, P_CAM17_COLOR, P_CAM17_SIGNAL, P_CAM17_GRAIN},
    {P_CAM18_CHARACTER, P_CAM18_COLOR, P_CAM18_SIGNAL, P_CAM18_GRAIN},
    {P_CAM19_CHARACTER, P_CAM19_COLOR, P_CAM19_SIGNAL, P_CAM19_GRAIN}
  };

  const float cameraCharacter = getSlider(params, cameraParamIds[ci][0], 100.0f);
  const float cameraColor     = getSlider(params, cameraParamIds[ci][1], 100.0f);
  const float cameraSignal    = getSlider(params, cameraParamIds[ci][2], 100.0f);
  const float cameraGrain     = getSlider(params, cameraParamIds[ci][3], 100.0f);

  const float drive = clamp01(strength / 10.0f); // 1000 UI units (10.0 internal) is the full VHS engine drive
  const float charScale = (cameraCharacter / 0.65f) * drive;
  const float colorScale = (cameraColor / 0.60f) * drive;
  const float sigScale = (cameraSignal / 0.40f) * drive;

  // Color response & chroma behavior
  const float sat = s.saturation * prof.saturation * colorScale;
  const float bleed = clamp01(s.bleed * prof.chromaBleed * (0.3f + 0.7f * drive));
  const float totalChromaX = s.chromaX * drive + prof.chromaOffsetX * colorScale;
  const float totalChromaY = s.chromaY * drive + prof.chromaOffsetY * colorScale;

  // Luma Softness is strictly controlled by P_SOFTNESS slider (0..100) and NOT forced by strength/camera.
  const float soft = clamp01(s.softness);
  const int blurRadius = clampi((int)std::ceil(soft * 3.0f), 0, 3);

  // Module checkboxes and enablers — strictly obeys checkbox state
  const bool grainEnabled = getCheck(params, P_GRAIN_ENABLE);
  const bool trackingEnabled = getCheck(params, P_TRACKING_ENABLE);
  const bool tapeEnabled = getCheck(params, P_TAPE_ENABLE);
  const bool overlayEnabled = getCheck(params, P_OVERLAY_ENABLE);
  const bool otherEnabled = getCheck(params, P_OTHER_ENABLE);
  // CRT TV Engine is strictly opt-in via P_CRT_ENABLE
  const bool crtEnabled = getCheck(params, P_CRT_ENABLE);

  const bool transparentTapeEnabled = getCheck(params, P_TRANSPARENT_TAPE_ENABLE);
  const bool tapeSweepEnabled = getCheck(params, P_TAPE_SWEEP_ENABLE);

  const float transparentTapeStrength = getSlider(params, P_TRANSPARENT_TAPE_STRENGTH, 100.0f);
  const float transparentTapeWidth = getSlider(params, P_TRANSPARENT_TAPE_WIDTH, 100.0f);
  const float transparentTapeSpeed = getSlider(params, P_TRANSPARENT_TAPE_SPEED, 100.0f);
  const float tapeSweepStrength = getSlider(params, P_TAPE_SWEEP_STRENGTH, 100.0f);
  const float tapeSweepWidth = getSlider(params, P_TAPE_SWEEP_WIDTH, 100.0f);
  const float tapeSweepSpeed = getSlider(params, P_TAPE_SWEEP_SPEED, 1.0f);
  const float tapeSweepInterval = std::max(0.1f, getSlider(params, P_TAPE_SWEEP_INTERVAL, 1.0f));

  const float crtScanlines = crtEnabled ? getSlider(params, P_CRT_SCANLINES, 100.0f) : 0.0f;
  const float crtCurveAmount = crtEnabled ? getSlider(params, P_CRT_CURVE, 100.0f) : 0.0f;
  const float crtGlowAmount = crtEnabled ? getSlider(params, P_CRT_GLOW, 100.0f) : 0.0f;
  const float crtRgbAmount = crtEnabled ? getSlider(params, P_CRT_RGB, 100.0f) : 0.0f;
  const float crtFlickerAmount = crtEnabled ? getSlider(params, P_CRT_FLICKER, 100.0f) : 0.0f;
  const float crtVignetteAmount = crtEnabled ? getSlider(params, P_CRT_VIGNETTE, 100.0f) : 0.0f;

  const float baseGrain = s.grain;
  const float grainAmount = grainEnabled ? (baseGrain + prof.noiseFloor * cameraGrain * 0.25f) : 0.0f;

  const float fps = (in_data->time_step != 0) ? (float)in_data->time_scale / (float)in_data->time_step : 24.0f;
  const float eventPeriod = s.eventRate > 0.001f ? 60.0f / s.eventRate : 1.0e9f;
  const int eventSlot = (int)std::floor(timeSec / eventPeriod);
  const float slotTime = timeSec - eventSlot * eventPeriod;
  const bool chosenEvent = s.events && s.eventRate > 0.0f && rand01((uint32_t)(eventSlot * 747796405u) ^ (uint32_t)s.seed) < 0.68f;
  float eventEnv = 0.0f;
  if (chosenEvent) {
    const float attack = 0.015f;
    if (slotTime < attack) eventEnv = smooth(0.0f, attack, slotTime);
    else if (slotTime < s.eventDuration) eventEnv = 1.0f;
    else if (s.recovery > 0.0f) eventEnv = 1.0f - smooth(s.eventDuration, s.eventDuration + s.recovery, slotTime);
  }

  // Damage overlay checkout
  PF_ParamDef overlayCheckout;
  AEFX_CLR_STRUCT(overlayCheckout);
  PF_EffectWorld *overlayWorld = nullptr;
  bool overlayCheckedOut = false;
  if (overlayEnabled) {
    const float overlaySpeed = getSlider(params, P_OVERLAY_SPEED, 100.0f);
    const float overlayOffset = getSlider(params, P_OVERLAY_OFFSET, 1.0f);
    const bool overlayLoop = getCheck(params, P_OVERLAY_LOOP);
    const float loopPeriod = std::max(0.1f, getSlider(params, P_OVERLAY_LOOP_PERIOD, 1.0f));
    float overlayTimeSec = timeSec * overlaySpeed + overlayOffset;
    if (overlayLoop && loopPeriod > 0.0f) {
      overlayTimeSec = std::fmod(overlayTimeSec, loopPeriod);
      if (overlayTimeSec < 0.0f) overlayTimeSec += loopPeriod;
    }
    const A_long overlayTime = (A_long)std::round(overlayTimeSec * (float)in_data->time_scale);
    if (PF_CHECKOUT_PARAM(in_data, P_OVERLAY_LAYER, overlayTime, in_data->time_step, in_data->time_scale, &overlayCheckout) == PF_Err_NONE) {
      overlayCheckedOut = true;
      if (overlayCheckout.u.ld.data) overlayWorld = &overlayCheckout.u.ld;
    }
  }
  const float overlayOpacity = getSlider(params, P_OVERLAY_OPACITY, 100.0f);
  const int overlayMode = params[P_OVERLAY_MODE]->u.pd.value - 1;
  const int overlayStride = overlayWorld ? overlayWorld->rowbytes / (int)sizeof(PF_Pixel) : 0;
  const PF_Pixel *overlayBase = overlayWorld ? (const PF_Pixel*)overlayWorld->data : nullptr;

  const int selOtherEngine = params[P_OTHER_ENGINE]->u.pd.value - 1;
  const int otherEngine = otherEnabled ? selOtherEngine : 0; // Off stays genuinely off; choose a separate engine explicitly
  const float otherStrengthRaw = getSlider(params, P_OTHER_STRENGTH, 100.0f);
  const float otherStrength = otherEnabled ? otherStrengthRaw * drive : 0.0f;

  const float craftTracking = getSlider(params, P_CRAFT_TRACKING, 100.0f);
  const float craftChroma   = getSlider(params, P_CRAFT_CHROMA, 100.0f);
  const float craftHead     = getSlider(params, P_CRAFT_HEADSWITCH, 100.0f);
  const float craftSmear    = getSlider(params, P_CRAFT_SMEAR, 100.0f);

  const float legendBreak   = getSlider(params, P_LEGEND_BREAKUP, 100.0f);
  const float legendTear    = getSlider(params, P_LEGEND_TEAR, 100.0f);
  const float legendLoss    = getSlider(params, P_LEGEND_SIGNAL, 100.0f);
  const float legendBlocks  = getSlider(params, P_LEGEND_BLOCKS, 100.0f);

  const float ntscChroma    = getSlider(params, P_NTSC_CHROMA_DELAY, 100.0f);
  const float ntscHead      = getSlider(params, P_NTSC_HEAD_SWITCH, 100.0f);
  const float ntscWave      = getSlider(params, P_NTSC_EDGE_WAVE, 100.0f);
  const float ntscComposite = getSlider(params, P_NTSC_COMPOSITE, 100.0f);

  const float signalStrength= getSlider(params, P_SIGNAL_STRENGTH, 100.0f);
  const float signalLuma    = getSlider(params, P_SIGNAL_LUMA_NOISE, 100.0f);
  const float signalChroma  = getSlider(params, P_SIGNAL_CHROMA_NOISE, 100.0f);
  const float signalErrors  = getSlider(params, P_SIGNAL_TAPE_ERRORS, 100.0f);

  const int glitchPreset = params[P_GLITCH_PRESET]->u.pd.value - 1;
  const float glitchPresetStrength = getSlider(params, P_GLITCH_PRESET_STRENGTH, 100.0f);
  const float glitchPresetSpeed = getSlider(params, P_GLITCH_PRESET_SPEED, 100.0f);
  const float glitchPresetRate = getSlider(params, P_GLITCH_PRESET_COUNT, 1.0f);
  const bool glitchPresetColor = getCheck(params, P_GLITCH_PRESET_COLOR);

  // Built-in overlay library: these are native per-pixel effect paths, not
  // image thumbnails. Timing is based on composition time and remains deterministic.
  const int builtinOverlay = params[P_BUILTIN_OVERLAY]->u.pd.value - 1;
  const float builtinOverlayStrength = getSlider(params, P_BUILTIN_OVERLAY_STRENGTH, 100.0f);
  const float builtinOverlaySpeed = getSlider(params, P_BUILTIN_OVERLAY_SPEED, 100.0f);
  const float builtinOverlayRate = getSlider(params, P_BUILTIN_OVERLAY_RATE, 1.0f);
  const bool builtinOverlayColor = getCheck(params, P_BUILTIN_OVERLAY_COLOR);
  const float builtinOverlayBlend = getSlider(params, P_BUILTIN_OVERLAY_BLEND, 100.0f);
  const float builtinOverlayPeriod = builtinOverlayRate > 0.001f ? 60.0f / builtinOverlayRate : 1.0e9f;
  const int builtinOverlaySlot = (int)std::floor(timeSec / builtinOverlayPeriod);
  const float builtinOverlaySlotTime = timeSec - builtinOverlaySlot * builtinOverlayPeriod;
  const bool builtinOverlayEvent = builtinOverlayRate > 0.001f && builtinOverlaySlotTime < builtinOverlayPeriod * 0.40f;
  const float blendPeriod = builtinOverlayRate > 0.001f ? builtinOverlayPeriod * 0.40f : 1.0f;
  const float builtinOverlayFade = builtinOverlayBlend <= 0.001f ? 1.0f : smooth(0.0f, std::max(0.02f, blendPeriod * builtinOverlayBlend), builtinOverlaySlotTime);
  const bool builtinOverlayIsEvent = builtinOverlay == 1 || builtinOverlay == 3 || builtinOverlay == 5 || builtinOverlay == 6 || builtinOverlay == 9 || builtinOverlay == 10 || builtinOverlay == 11 || builtinOverlay == 16 || builtinOverlay == 20 || builtinOverlay == 21;
  // Blend-in controls affect continuous looks as well; event looks also obey the deterministic event window.
  const float builtinOverlayEventGain = builtinOverlayIsEvent ? (builtinOverlayEvent ? builtinOverlayFade : 0.0f) : builtinOverlayFade;

  const bool osdPlay = getCheck(params, P_OSD_PLAY);
  const bool osdRec = getCheck(params, P_OSD_REC);
  const bool osdTimecode = getCheck(params, P_OSD_TIMECODE);
  const bool osdDate = getCheck(params, P_OSD_DATE);
  const int osdYear = clampi(getInt(params, P_OSD_YEAR), 1980, 2099);
  const int osdMonth = clampi(getInt(params, P_OSD_MONTH), 1, 12);
  const int osdDay = clampi(getInt(params, P_OSD_DAY), 1, 31);
  const float osdOpacity = getSlider(params, P_OSD_OPACITY, 100.0f);
  const int osdScale = clampi((int)std::round(getSlider(params, P_OSD_SCALE, 1.0f)), 1, 8);
  const int osdPosition = params[P_OSD_POSITION]->u.pd.value - 1;
  const int osdMargin = clampi((int)std::round(getSlider(params, P_OSD_SAFE_MARGIN, 1.0f) * 0.01f * std::min(w,h)), 0, std::min(w,h)/3);
  char osdText[40]; int osdCharCount=0;
  if(osdPlay) { const char t[]="PLAY"; for(int q=0;q<4;++q) osdText[osdCharCount++]=t[q]; osdText[osdCharCount++]=' '; }
  if(osdRec) { const char t[]="REC"; for(int q=0;q<3;++q) osdText[osdCharCount++]=t[q]; osdText[osdCharCount++]=' '; }
  if(osdTimecode) { int sec=std::max(0,(int)std::floor(timeSec)); int hh=(sec/3600)%100, mm=(sec/60)%60, ss=sec%60; osdText[osdCharCount++]=(char)('0'+hh/10); osdText[osdCharCount++]=(char)('0'+hh%10); osdText[osdCharCount++]=':'; osdText[osdCharCount++]=(char)('0'+mm/10); osdText[osdCharCount++]=(char)('0'+mm%10); osdText[osdCharCount++]=':'; osdText[osdCharCount++]=(char)('0'+ss/10); osdText[osdCharCount++]=(char)('0'+ss%10); osdText[osdCharCount++]=' '; }
  if(osdDate) { osdText[osdCharCount++]=(char)('0'+(osdYear/1000)%10); osdText[osdCharCount++]=(char)('0'+(osdYear/100)%10); osdText[osdCharCount++]=(char)('0'+(osdYear/10)%10); osdText[osdCharCount++]=(char)('0'+osdYear%10); osdText[osdCharCount++]='-'; osdText[osdCharCount++]=(char)('0'+osdMonth/10); osdText[osdCharCount++]=(char)('0'+osdMonth%10); osdText[osdCharCount++]='-'; osdText[osdCharCount++]=(char)('0'+osdDay/10); osdText[osdCharCount++]=(char)('0'+osdDay%10); }
  const int osdGlyphW=6*osdScale, osdGlyphH=7*osdScale;
  const bool osdRight=(osdPosition==1 || osdPosition==3), osdTop=(osdPosition==2 || osdPosition==3);
  const int osdLeft=osdRight ? w-osdMargin-osdCharCount*osdGlyphW : osdMargin;
  const int osdBaseY=osdTop ? osdMargin : h-osdMargin-osdGlyphH;

  const int selTapeType = params[P_TAPE_TYPE]->u.pd.value - 1;
  const int tapeType = tapeEnabled ? selTapeType : 0; // Off stays genuinely off; choose a tape-damage mode explicitly
  const float tapeStrengthRaw = getSlider(params, P_TAPE_STRENGTH, 100.0f);
  const float effTapeStrength = tapeEnabled ? (tapeStrengthRaw > 0.001f ? tapeStrengthRaw : 0.35f) : 0.0f;

  const float baseTracking = s.tracking;
  const float effTracking = trackingEnabled ? baseTracking * sigScale : 0.0f;

  for (int y = 0; y < h; ++y) {
    PF_Pixel *dst = outBase + y * outStride;
    for (int x = 0; x < w; ++x) {
      int sx = clampi((int)((x + 0.5f) * inW / std::max(1, w)), 0, inW - 1);
      int sy = clampi((int)((y + 0.5f) * inH / std::max(1, h)), 0, inH - 1);

      // CRT barrel geometry warp. Track the warped aperture separately so the
      // curved glass has real black corners instead of stretching edge pixels.
      float crtFrameMask = 1.0f;
      if (crtCurveAmount > 0.001f) {
        const float nx = ((float)x / std::max(1, w - 1) - 0.5f) * 2.0f;
        const float ny = ((float)y / std::max(1, h - 1) - 0.5f) * 2.0f;
        const float rr = nx * nx + ny * ny;
        const float k = 1.0f + crtCurveAmount * 0.12f * rr;
        const float ux = nx * k * 0.5f + 0.5f;
        const float uy = ny * k * 0.5f + 0.5f;
        const float aperture = std::min(std::min(ux, 1.0f - ux), std::min(uy, 1.0f - uy));
        crtFrameMask = smooth(-0.002f, 0.018f + crtCurveAmount * 0.006f, aperture);
        sx = clampi((int)std::round(ux * (inW - 1)), 0, inW - 1);
        sy = clampi((int)std::round(uy * (inH - 1)), 0, inH - 1);
      }

      PF_Pixel p = inBase[sy * inStride + sx];
      float r = p.red / 255.0f, g = p.green / 255.0f, b = p.blue / 255.0f;

      if (s.continuous && strength > 0.0001f) {
        // Luma softness: spatial blur filter ONLY when softness slider > 0
        if (blurRadius > 0) {
          float rr = 0, gg = 0, bb = 0, weight = 0;
          for (int k = -blurRadius; k <= blurRadius; ++k) {
            const int xx = clampi(sx + k, 0, inW - 1);
            const float kw = (float)(blurRadius + 1 - std::abs(k));
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

        // Chroma separation & bleed
        const int cx = clampi(sx + (int)std::round(totalChromaX), 0, inW - 1);
        const int cy = clampi(sy + (int)std::round(totalChromaY), 0, inH - 1);
        const PF_Pixel cp = inBase[cy * inStride + cx];
        const float cbR = cp.red / 255.0f, cbG = cp.green / 255.0f, cbB = cp.blue / 255.0f;

        r = r * (1.0f - bleed) + cbR * bleed;
        g = g * (1.0f - bleed) + cbG * bleed;
        b = b * (1.0f - bleed) + cbB * bleed;

        // Saturation
        const float lum = 0.2126f * r + 0.7152f * g + 0.0722f * b;
        r = lum + (r - lum) * sat;
        g = lum + (g - lum) * sat;
        b = lum + (b - lum) * sat;

        // Camera profile color response & tonal curve
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

        const float black = (s.black * 0.35f + prof.pedestal * 100.0f * charScale) * std::min(2.0f, strength);
        r = clamp01((r - 0.5f) * (1.0f + black * 0.01f) + 0.5f - black * 0.005f);
        g = clamp01((g - 0.5f) * (1.0f + black * 0.01f) + 0.5f - black * 0.005f);
        b = clamp01((b - 0.5f) * (1.0f + black * 0.01f) + 0.5f - black * 0.005f);

        // Signal Grain
        if (grainAmount > 0.001f) {
          const float scale = std::max(1.0f, s.grainScale);
          const uint32_t motionFrame = (uint32_t)std::floor(timeSec * s.grainSpeed * fps);
          const uint32_t nseed = (uint32_t)s.seed ^ (motionFrame * 2246822519u) ^ (uint32_t)((x / (int)scale) * 73856093u) ^ (uint32_t)((y / (int)scale) * 19349663u);
          const float n = (rand01(nseed) - 0.5f) * grainAmount * 0.25f;
          r = clamp01(r + n * 1.15f);
          g = clamp01(g + n * 0.80f);
          b = clamp01(b + n * 1.30f);
        }
      }

      // Tracking bands
      if (s.continuous && effTracking > 0.001f) {
        const float phase = timeSec * (0.08f + 0.8f * s.trackFreq) * (1.0f + ci * 0.07f);
        const float bandY = std::fmod(phase, 1.0f);
        const float yn = (float)y / std::max(1, h);
        const float dist = std::fabs(yn - bandY);
        const float bw = std::max(0.002f, s.trackWidth * 0.08f);
        const float band = (1.0f - smooth(bw, bw * 2.5f, dist)) * effTracking;
        r = clamp01(r + band * 0.30f);
        g = clamp01(g + band * 0.42f);
        b = clamp01(b + band * 0.65f);
      }

      // Discrete glitch events
      if (eventEnv > 0.001f) {
        const uint32_t eh = (uint32_t)s.seed ^ (uint32_t)(eventSlot * 2654435761u) ^ (uint32_t)(y * 2246822519u);
        const float lineNoise = rand01(eh);
        if (lineNoise < 0.12f * eventEnv) { r = clamp01(r + 0.35f * eventEnv); g *= 1.0f - 0.5f * eventEnv; b = clamp01(b + 0.45f * eventEnv); }
        if (lineNoise < s.dropout * sigScale * eventEnv * 0.5f) { r *= 0.08f; g *= 0.08f; b *= 0.08f; }
        if (lineNoise < s.blocks * sigScale * eventEnv * 0.35f) { const int block = ((x / 16) + (y / 8) * 13 + eventSlot) % 5; if (block < 2) { r = rand01(eh + 7); g = rand01(eh + 11); b = rand01(eh + 19); } }
      }

      // Tape band effects
      if (tapeType > 0 && effTapeStrength > 0.001f) {
        const float speed = s.tapeSpeed;
        float pos = std::fmod(timeSec * speed * 0.12f + 0.17f, 1.0f);
        if (s.tapeDir == 1 || s.tapeDir == 3) pos = 1.0f - pos;
        const float yn = (float)y / std::max(1, h), xn = (float)x / std::max(1, w);
        float dist = 1.0f;
        if (tapeType == 1) dist = (s.tapeDir >= 2) ? std::fabs(xn - pos) : std::fabs(yn - pos);
        else if (tapeType == 2) dist = std::fabs(yn - (s.tapeDir == 0 ? s.tapeSize * 0.01f * 0.25f : 1.0f - s.tapeSize * 0.01f * 0.25f));
        else if (tapeType == 3 || tapeType == 4) dist = (s.tapeDir >= 2) ? std::fabs(yn - pos) : std::fabs(xn - pos);
        else if (tapeType == 5) dist = rand01((uint32_t)(x * 31 + y * 71 + frame * 997 + s.seed));

        const float width = s.tapeSize * 0.10f + 0.003f;
        const float mask = (tapeType == 5 ? clamp01(1.0f - dist) : 1.0f - smooth(width, width * 2.0f, dist));
        const float ts = clamp01(mask * effTapeStrength * (0.2f + 0.8f * clamp01(strength / 10.0f)));

        if (ts > 0.001f && (tapeType >= 1 && tapeType <= 4)) {
          int tx = sx, ty = sy;
          const float irregular = std::sin((float)y * 0.071f + timeSec * 8.3f) * 0.55f + std::sin((float)y * 0.019f - timeSec * 3.7f) * 0.45f;
          const int displacement = (int)std::round(irregular * ts * std::max(2, w / 24));
          if (tapeType == 1 || tapeType == 2) tx = clampi(sx + displacement, 0, inW - 1);
          else if (tapeType == 3) ty = clampi(sy + displacement, 0, inH - 1);
          else tx = clampi(sx + (displacement * (rand01((uint32_t)(y * 73 + s.seed + frame * 19)) > 0.5f ? 2 : -2)), 0, inW - 1);

          const PF_Pixel tp = inBase[ty * inStride + tx];
          const float tr = tp.red / 255.0f, tg = tp.green / 255.0f, tb = tp.blue / 255.0f;
          r = r * (1.0f - ts) + tr * ts;
          g = g * (1.0f - ts) + tg * ts;
          b = b * (1.0f - ts) + tb * ts;

          const float phase = std::sin((float)y * 0.31f + timeSec * 19.0f) * ts * 0.10f;
          if (tapeType == 2) { r *= 1.0f - ts * 0.25f; g *= 1.0f - ts * 0.35f; b *= 1.0f - ts * 0.18f; }
          if (tapeType == 4) { r = clamp01(r + phase + ts * 0.12f); g = clamp01(g - phase); b = clamp01(b - phase - ts * 0.08f); }
        } else if (tapeType == 5 && ts > 0.001f) {
          // Tape RF hiss is spatially correlated by scanline and coarse horizontal cell,
          // avoiding a generic full-frame white-noise overlay.
          const uint32_t lineKey=(uint32_t)(y/2 + frame*997 + s.seed);
          const float lineNoise=(rand01(lineKey)-0.5f)*ts*0.20f;
          const float chromaNoise=(rand01(lineKey+(uint32_t)(x/8*31))-0.5f)*ts*0.12f;
          r=clamp01(r+lineNoise+chromaNoise); g=clamp01(g+lineNoise-chromaNoise*0.25f); b=clamp01(b+lineNoise-chromaNoise);
        }
      }

      // Other VHS Engines
      if (otherEngine > 0 && otherStrength > 0.0001f) {
        const float xn = (float)x / std::max(1, w), yn = (float)y / std::max(1, h);
        const uint32_t oh = (uint32_t)s.seed ^ (uint32_t)(frame * 2246822519u) ^ (uint32_t)(x * 3266489917u) ^ (uint32_t)(y * 668265263u);
        if (otherEngine == 1) { // VHS Craft Reference
          const float trackPos = std::fmod(timeSec * 0.23f + 0.31f, 1.0f);
          const float track = 1.0f - smooth(0.008f, 0.025f, std::fabs(yn - trackPos));
          const float head = 1.0f - smooth(0.01f, 0.06f, std::fabs(yn - (1.0f - 0.025f)));
          const float n = rand01(oh) - 0.5f;
          const float t = otherStrength * (craftTracking * track + craftHead * head) * 0.006f;
          r = clamp01(r + t + n * craftSmear * otherStrength * 0.15f);
          g = clamp01(g - t * 0.25f + n * craftSmear * otherStrength * 0.15f);
          b = clamp01(b + t * 0.7f + craftChroma * otherStrength * 0.12f * n);
        } else if (otherEngine == 2) { // VHS Legend Reference
          const float line = (rand01((uint32_t)(y / 3 + frame * 41 + s.seed)) - 0.5f) * 2.0f;
          const bool tear = rand01((uint32_t)(y / 7 + x / 29 + frame * 17 + s.seed)) < legendTear * otherStrength * 0.08f;
          const bool loss = rand01((uint32_t)(x / 12 + y / 8 + frame * 73 + s.seed)) < legendLoss * otherStrength * 0.08f;
          r = clamp01(r + line * legendBreak * otherStrength * 0.35f);
          g = clamp01(g + line * legendBreak * otherStrength * 0.18f);
          b = clamp01(b - line * legendBreak * otherStrength * 0.28f);
          if (tear) { r = clamp01(r + 0.18f * otherStrength); g *= 0.55f; b = clamp01(b + 0.25f * otherStrength); }
          if (loss) { r *= 0.15f; g *= 0.15f; b *= 0.15f; }
          if (rand01(oh) < legendBlocks * otherStrength * 0.06f) { r = rand01(oh + 11); g = rand01(oh + 23); b = rand01(oh + 37); }
        } else if (otherEngine == 3) { // NTSC-RS Signal Model
          // Real ntsc-rs signal engine is invoked in post-pass
        } else if (otherEngine == 4) { // Signal Reference
          const float l = (rand01(oh + 3) - 0.5f) * signalLuma * signalStrength * otherStrength * 0.12f;
          const float c = (rand01(oh + 7) - 0.5f) * signalChroma * signalStrength * otherStrength * 0.18f;
          const bool error = rand01((uint32_t)(x / 8 + y / 4 + frame * 29 + s.seed)) < signalErrors * signalStrength * otherStrength * 0.08f;
          r = clamp01(r + l + c); g = clamp01(g + l - c * 0.25f); b = clamp01(b + l - c);
          if (error) { r *= 0.2f; g *= 0.2f; b *= 0.2f; }
        }
      }

      // Procedural Transparent Tape: translucent oxide wrinkle and local signal shear,
      // not a bright white stripe painted across the frame.
      if (transparentTapeEnabled && transparentTapeStrength > 0.001f) {
        const float yn=(float)y/(float)std::max(1,h-1);
        const float travel=std::fmod(timeSec*(0.08f+transparentTapeSpeed*0.006f),1.0f);
        const float wobble=0.010f*std::sin((float)x*0.021f+timeSec*4.7f)+0.005f*std::sin((float)x*0.067f-timeSec*2.9f);
        const float center=travel+wobble; const float halfWidth=0.002f+transparentTapeWidth*0.025f;
        const float edge=1.0f-smooth(halfWidth,halfWidth*1.9f,std::fabs(yn-center));
        const float broken=0.55f+0.45f*rand01((uint32_t)(x/11+y/3+(int)(timeSec*17.0f)+s.seed));
        const float a=clamp01(edge*broken*transparentTapeStrength*0.58f);
        if(a>0.001f){
          const int dx=(int)std::round(std::sin((float)y*0.13f+timeSec*8.0f)*a*std::max(1,w/80));
          const PF_Pixel q=inBase[sy*inStride+clampi(sx+dx,0,inW-1)];
          const float qR=q.red/255.0f,qG=q.green/255.0f,qB=q.blue/255.0f;
          const float l=0.2126f*r+0.7152f*g+0.0722f*b;
          r=clamp01(r*(1-a*0.42f)+qR*a*0.30f+l*a*0.12f);
          g=clamp01(g*(1-a*0.38f)+qG*a*0.26f+l*a*0.12f);
          b=clamp01(b*(1-a*0.44f)+qB*a*0.32f+l*a*0.12f);
        }
      }

      // Moving Tape Sweep
      if (tapeSweepEnabled && tapeSweepStrength > 0.001f) {
        const float slot = std::floor(timeSec / tapeSweepInterval);
        const float slotTime = timeSec - slot * tapeSweepInterval;
        const float sweepDuration = std::max(0.15f, tapeSweepInterval * 100.0f / std::max(1.0f, tapeSweepSpeed) * 0.25f);
        const float phase = clamp01(slotTime / sweepDuration);
        const float center = phase;
        const float yn = (float)y / (float)std::max(1, h - 1);
        const float wobble = 0.006f * std::sin((float)x * 0.031f + timeSec * 7.0f) + 0.004f * std::sin((float)x * 0.079f - timeSec * 5.0f);
        const float halfWidth = 0.004f + tapeSweepWidth * 0.075f;
        const float band = 1.0f - smooth(halfWidth, halfWidth * 1.8f, std::fabs(yn - center - wobble));
        const float envelope = clamp01(band * tapeSweepStrength * (slotTime < sweepDuration ? 1.0f : 0.0f));
        if (envelope > 0.001f) {
          const int dx = (int)std::round(std::sin((float)y * 0.11f + timeSec * 9.0f) * envelope * std::max(2, w / 18));
          const PF_Pixel moved = inBase[sy * inStride + clampi(sx + dx, 0, inW - 1)];
          const PF_Pixel redShift = inBase[sy * inStride + clampi(sx + dx + 2, 0, inW - 1)];
          const PF_Pixel blueShift = inBase[sy * inStride + clampi(sx + dx - 2, 0, inW - 1)];
          const float mr = moved.red / 255.0f, mg = moved.green / 255.0f, mb = moved.blue / 255.0f;
          r = clamp01(r * (1.0f - envelope) + mr * envelope + envelope * (redShift.red / 255.0f - mr) * 0.35f);
          g = clamp01(g * (1.0f - envelope) + mg * envelope);
          b = clamp01(b * (1.0f - envelope) + mb * envelope + envelope * (blueShift.blue / 255.0f - mb) * 0.35f);
          const float loss = envelope * (0.08f + 0.12f * rand01((uint32_t)(y / 2 + frame * 17 + s.seed)));
          r *= 1.0f - loss; g *= 1.0f - loss; b *= 1.0f - loss;
        }
      }

      // CRT TV Engine (Strictly opt-in via P_CRT_ENABLE)
      if (crtEnabled) {
        const float xn = (float)x / (float)std::max(1, w - 1), yn = (float)y / (float)std::max(1, h - 1);
        const float scanWave = 0.5f + 0.5f * std::cos((float)y * 3.14159265f);
        const float scanGain = 1.0f - crtScanlines * 0.30f * scanWave;
        const float dx = xn - 0.5f, dy = yn - 0.5f;
        const float edge = clamp01(1.0f - crtVignetteAmount * 1.35f * (dx * dx * 1.8f + dy * dy * 1.8f));
        const float flick = 1.0f - crtFlickerAmount * 0.035f * (0.5f + 0.5f * std::sin(timeSec * 37.0f));
        const float curved = 1.0f - crtCurveAmount * 0.16f * (dx * dx + dy * dy);
        const float l = 0.2126f * r + 0.7152f * g + 0.0722f * b;
        // Spatial phosphor bloom: bright neighboring source pixels contribute a
        // soft halo, rather than merely adding brightness to the bright pixel.
        float neighborLuma = 0.0f;
        const int gx0 = clampi(sx - 1, 0, inW - 1), gx1 = clampi(sx + 1, 0, inW - 1);
        const int gy0 = clampi(sy - 1, 0, inH - 1), gy1 = clampi(sy + 1, 0, inH - 1);
        const PF_Pixel glowL = inBase[sy * inStride + gx0];
        const PF_Pixel glowR = inBase[sy * inStride + gx1];
        const PF_Pixel glowU = inBase[gy0 * inStride + sx];
        const PF_Pixel glowD = inBase[gy1 * inStride + sx];
        neighborLuma = 0.25f * (
          0.2126f * (glowL.red + glowR.red + glowU.red + glowD.red) / 255.0f +
          0.7152f * (glowL.green + glowR.green + glowU.green + glowD.green) / 255.0f +
          0.0722f * (glowL.blue + glowR.blue + glowU.blue + glowD.blue) / 255.0f);
        const float bloomEnergy = std::max(0.0f, std::max(l, neighborLuma * 0.78f) - 0.58f);
        const float halo = bloomEnergy * crtGlowAmount * 0.22f;
        const float convergence = crtRgbAmount * 0.035f;
        float cr = r, cb = b;
        if (crtRgbAmount > 0.001f) {
          const int off = clampi((int)std::round(crtRgbAmount * 3.0f), 0, 3);
          const PF_Pixel rp = inBase[sy * inStride + clampi(sx + off, 0, inW - 1)];
          const PF_Pixel bp = inBase[sy * inStride + clampi(sx - off, 0, inW - 1)];
          cr = rp.red / 255.0f; cb = bp.blue / 255.0f;
        }
        // Fine RGB phosphor triad, kept deliberately subtle so it reads as a
        // display texture rather than colored square noise.
        const int triad = x % 3;
        const float triadGain = crtRgbAmount * 0.045f;
        const float redMask = triad == 0 ? 1.0f : 1.0f - triadGain;
        const float greenMask = triad == 1 ? 1.0f : 1.0f - triadGain;
        const float blueMask = triad == 2 ? 1.0f : 1.0f - triadGain;
        r = clamp01(((r * (1.0f - convergence) + cr * convergence + convergence * (g - b)) * scanGain * edge * flick * curved + halo) * redMask * crtFrameMask);
        g = clamp01((g * scanGain * edge * flick * curved + halo * 0.9f) * greenMask * crtFrameMask);
        b = clamp01(((b * (1.0f - convergence) + cb * convergence + convergence * (g - r)) * scanGain * edge * flick * curved + halo * 1.05f) * blueMask * crtFrameMask);
      }

  // Built-in VHS overlay effects. Subtle analog artifacts are generated directly
  // over the current image; no full-frame white noise, neon blocks, or static images.
  if (builtinOverlay > 0 && builtinOverlay <= 21 && builtinOverlayStrength > 0.0001f) {
    const float ov = clamp01(builtinOverlayStrength * builtinOverlayEventGain);
    const float ovPhase = timeSec * (0.18f + 2.8f * builtinOverlaySpeed);
    const float xn = (float)x / (float)std::max(1, w - 1);
    const float yn = (float)y / (float)std::max(1, h - 1);
    const uint32_t oh = (uint32_t)s.seed ^ (uint32_t)(frame * 2246822519u) ^ (uint32_t)(x * 3266489917u) ^ (uint32_t)(y * 668265263u) ^ (uint32_t)(builtinOverlay * 374761393u);
    const float n = rand01(oh) - 0.5f;
    const float roll = std::fmod(ovPhase * 0.11f + 0.37f, 1.0f);
    const float rollDist = std::fabs(yn - roll);
    switch (builtinOverlay) {
      case 1: { // Bad Signal Tear
        const float line = 1.0f - smooth(0.0015f, 0.018f, std::fabs(yn - std::fmod(roll + n * 0.06f, 1.0f)));
        const float a = ov * line * 0.85f;
        const int dx = (int)std::round((n + std::sin(y * 0.07f + ovPhase * 5.0f)) * a * std::max(2, w / 8));
        const PF_Pixel q = inBase[sy * inStride + clampi(sx + dx, 0, inW - 1)];
        r = clamp01(r * (1-a) + q.red / 255.0f * a); g = clamp01(g * (1-a) + q.green / 255.0f * a); b = clamp01(b * (1-a) + q.blue / 255.0f * a);
        if (builtinOverlayColor) { r = clamp01(r + a * 0.08f); b = clamp01(b - a * 0.04f); }
        break;
      }
      case 2: { // Moving Tracking Bar
        const float band = 1.0f - smooth(0.004f, 0.035f, std::fabs(yn-roll)); const float a = ov * band;
        const float l = 0.2126f*r + 0.7152f*g + 0.0722f*b;
        r=clamp01(r*(1-a*0.32f)+l*a*0.32f+(builtinOverlayColor?a*0.05f:0)); g=clamp01(g*(1-a*0.22f)+l*a*0.22f); b=clamp01(b*(1-a*0.28f)+l*a*0.28f+(builtinOverlayColor?a*0.06f:0));
        break;
      }
      case 3: { // Head-Switch Noise at bottom edge
        const float bottom=smooth(0.82f,0.98f,yn); const float line=rand01((uint32_t)(y/2+frame*37+s.seed)); const float a=ov*bottom*(line>0.32f?0.75f:0.12f);
        const float ink=builtinOverlayColor?0.38f:0.30f; r=clamp01(r*(1-a)+ink*a); g=clamp01(g*(1-a)+(ink*0.97f)*a); b=clamp01(b*(1-a)+(ink*0.90f)*a); break;
      }
      case 4: { // RF interference: restrained luma/chroma modulation
        const float wave=std::sin(y*0.19f+ovPhase*8.0f)*std::sin(x*0.013f-ovPhase*2.0f); const float a=ov*wave*0.12f;
        r=clamp01(r+a); g=clamp01(g+a*(builtinOverlayColor?-0.28f:0.0f)); b=clamp01(b-a*(builtinOverlayColor?0.72f:0.0f)); break;
      }
      case 5: { // Tape wrinkle: broken horizontal crease
        const float center=std::fmod(roll+0.025f*std::sin(x*0.021f+ovPhase*3.0f),1.0f); const float d=std::fabs(yn-center);
        const float crease=(1.0f-smooth(0.001f,0.006f,d))*(rand01((uint32_t)(x/17+y/2+frame*11+s.seed))>0.20f?1.0f:0.0f); const float a=ov*crease;
        r=clamp01(r*(1-a)+0.65f*a); g=clamp01(g*(1-a)+0.62f*a); b=clamp01(b*(1-a)+0.56f*a); break;
      }
      case 6: { // Signal Dropout
        const float streak=rand01((uint32_t)(y/2+builtinOverlaySlot*53+s.seed)); const float fragment=rand01((uint32_t)(x/9+y*31+builtinOverlaySlot*17+s.seed));
        const float a=ov*(1.0f-smooth(0.78f,0.98f,streak))*(fragment>0.18f?0.78f:0.0f); r*=1-a; g*=1-a; b*=1-a; break;
      }
      case 7: { // Chroma Bleed
        const int off=clampi((int)std::round(ov*(2.0f+8.0f*std::fabs(n))),0,10); const PF_Pixel rp=inBase[sy*inStride+clampi(sx-off,0,inW-1)]; const PF_Pixel bp=inBase[sy*inStride+clampi(sx+off,0,inW-1)]; const float a=ov*0.65f;
        r=clamp01(r*(1-a)+rp.red/255.0f*a); b=clamp01(b*(1-a)+bp.blue/255.0f*a); break;
      }
      case 8: { // Chroma Phase Drift
        const float phase=std::sin(ovPhase*1.7f+yn*7.0f)*ov*0.12f;
        if(builtinOverlayColor){r=clamp01(r+phase);g=clamp01(g-phase*0.25f);b=clamp01(b-phase);} else {const float l=0.2126f*r+0.7152f*g+0.0722f*b;r=clamp01(r*(1-ov*0.25f)+l*ov*0.25f);g=clamp01(g*(1-ov*0.25f)+l*ov*0.25f);b=clamp01(b*(1-ov*0.25f)+l*ov*0.25f);} break;
      }
      case 9: { // Vertical Roll boundary displacement
        const float band=1.0f-smooth(0.004f,0.045f,rollDist); const int dx=(int)std::round(std::sin(y*0.031f+ovPhase*4.0f)*ov*std::max(2,w/12)); const PF_Pixel q=inBase[sy*inStride+clampi(sx+dx,0,inW-1)]; const float a=ov*band;
        r=clamp01(r*(1-a)+q.red/255.0f*a);g=clamp01(g*(1-a)+q.green/255.0f*a);b=clamp01(b*(1-a)+q.blue/255.0f*a);break;
      }
      case 10: { // Frame Jitter: sparse scanline groups jump sideways
        const int group=y/3; const float gate=rand01((uint32_t)(group*313+frame*41+s.seed));
        if(gate<ov*0.55f){const int dx=(int)std::round((rand01(oh+7)-0.5f)*ov*std::max(3,w/6));const PF_Pixel q=inBase[sy*inStride+clampi(sx+dx,0,inW-1)];const float a=ov*0.8f;r=clamp01(r*(1-a)+q.red/255.0f*a);g=clamp01(g*(1-a)+q.green/255.0f*a);b=clamp01(b*(1-a)+q.blue/255.0f*a);}break;
      }
      case 11: { // Line Echo
        const int ey=clampi(sy-std::max(1,inH/180),0,inH-1);const PF_Pixel q=inBase[ey*inStride+sx];const float a=ov*0.42f;r=clamp01(r*(1-a)+q.red/255.0f*a);g=clamp01(g*(1-a)+q.green/255.0f*a);b=clamp01(b*(1-a)+q.blue/255.0f*a);break;
      }
      case 12: { // Capture Code telemetry ticks in corner
        const int margin=std::max(3,std::min(w,h)/28);const int hudW=std::max(24,w/8);const int hudH=std::max(5,h/45);
        if(x>=margin&&x<margin+hudW&&y>=margin&&y<margin+hudH){const int tick=(x-margin)/std::max(1,hudW/12);const bool lit=((tick+(int)(timeSec*builtinOverlaySpeed*2.0f))%3)!=0;const float a=ov*(lit?0.72f:0.22f);const float ink=(tick%4==0)?0.68f:0.90f;r=clamp01(r*(1-a)+ink*a);g=clamp01(g*(1-a)+ink*a);b=clamp01(b*(1-a)+ink*a);}break;
      }
      case 13: { // REC HUD with a blinking red recording lamp
        const int m=std::max(3,std::min(w,h)/32);const int hudW=std::max(18,w/10);const int hudH=std::max(6,h/38);
        if(x>=m&&x<m+hudW&&y>=m&&y<m+hudH){const bool lamp=((int)(timeSec*(2.0f+builtinOverlaySpeed*3.0f))&1)==0;const float a=ov*(lamp?0.95f:0.40f);r=clamp01(r*(1-a)+a);g=clamp01(g*(1-a)+0.10f*a);b=clamp01(b*(1-a)+0.08f*a);}break;
      }
      case 14: { // CRT Burn-In / phosphor falloff
        const float dx=xn-0.5f,dy=yn-0.5f;const float edge=clamp01(1.0f-(dx*dx*1.8f+dy*dy*1.8f)*ov*0.65f);const float scan=1.0f-ov*0.10f*(0.5f+0.5f*std::cos(y*3.14159265f));
        r=clamp01(r*edge*scan+ov*std::max(0.0f,r-0.65f)*0.08f);g=clamp01(g*edge*scan+ov*std::max(0.0f,g-0.65f)*0.08f);b=clamp01(b*edge*scan+ov*std::max(0.0f,b-0.65f)*0.08f);break;
      }
      case 15: { // Sparse, luma-aware tape dust rather than full-frame static
        const float threshold=0.9975f-ov*0.004f;if(rand01((uint32_t)(x/2+y*719+frame*997+s.seed))>threshold){const float a=ov*0.55f;const float l=0.2126f*r+0.7152f*g+0.0722f*b;const float ink=l>0.65f?0.12f:0.78f;r=clamp01(r*(1-a)+ink*a);g=clamp01(g*(1-a)+ink*a);b=clamp01(b*(1-a)+ink*a);}break;
      }
      case 16: { // Sync Loss: partial horizontal signal collapse
        const float band=1.0f-smooth(0.015f,0.09f,rollDist);const float a=ov*band*0.82f;r*=1-a;g*=1-a;b*=1-a;break;
      }
      case 17: { // Luma Pump / tape AGC instability
        const float pump=1.0f-ov*0.08f*(0.5f+0.5f*std::sin(ovPhase*2.2f));r=clamp01((r-0.5f)*pump+0.5f);g=clamp01((g-0.5f)*pump+0.5f);b=clamp01((b-0.5f)*pump+0.5f);break;
      }
      case 18: { // Edge Rainbow: restrained color fringe on actual luminance transitions
        const PF_Pixel left=inBase[sy*inStride+clampi(sx-1,0,inW-1)];const float lum=0.2126f*r+0.7152f*g+0.0722f*b;const float ll=0.2126f*left.red/255.0f+0.7152f*left.green/255.0f+0.0722f*left.blue/255.0f;const float edge=clamp01(std::fabs(lum-ll)*4.0f)*ov*0.22f;
        if(builtinOverlayColor){r=clamp01(r+edge);g=clamp01(g-edge*0.25f);b=clamp01(b-edge*0.85f);}break;
      }
      case 19: { // Composite Ghosting
        const int gx=clampi(sx-std::max(1,inW/90),0,inW-1);const PF_Pixel q=inBase[sy*inStride+gx];const float a=ov*0.28f;r=clamp01(r*(1-a)+q.red/255.0f*a);g=clamp01(g*(1-a)+q.green/255.0f*a);b=clamp01(b*(1-a)+q.blue/255.0f*a);break;
      }
      case 20: { // Bottom Tracking Noise
        const float bottom=smooth(0.90f,0.995f,yn);const float broken=rand01((uint32_t)(x/13+y/2+frame*31+s.seed));const float a=ov*bottom*(broken>0.18f?0.65f:0.08f);const float l=0.2126f*r+0.7152f*g+0.0722f*b;r=clamp01(r*(1-a)+l*a);g=clamp01(g*(1-a)+l*a);b=clamp01(b*(1-a)+l*a);break;
      }
      case 21: { // Analog Line Damage
        const float band=1.0f-smooth(0.001f,0.008f,std::fabs(yn-std::fmod(roll+n*0.03f,1.0f)));const float broken=rand01((uint32_t)(x/12+y/3+frame*13+s.seed));const float a=ov*band*(broken>0.25f?0.75f:0.0f);const float l=0.2126f*r+0.7152f*g+0.0722f*b;r=clamp01(r*(1-a)+l*a);g=clamp01(g*(1-a)+l*a);b=clamp01(b*(1-a)+l*a);break;
      }
      default: break;
    }
  }

  // Damage Overlay Layer
  if (overlayEnabled && overlayBase && overlayOpacity > 0.0f && overlayWorld->width > 0 && overlayWorld->height > 0) {
        const int ox = clampi((int)((x + 0.5f) * overlayWorld->width / std::max(1, w)), 0, overlayWorld->width - 1);
        const int oy = clampi((int)((y + 0.5f) * overlayWorld->height / std::max(1, h)), 0, overlayWorld->height - 1);
        const PF_Pixel op = overlayBase[oy * overlayStride + ox];
        float orr = op.red / 255.0f, og = op.green / 255.0f, ob = op.blue / 255.0f;
        float a = overlayOpacity * (op.alpha / 255.0f);
        if (overlayMode == 1) { // Blend
          r = clamp01(r + (orr - 0.5f) * a * 0.85f); g = clamp01(g + (og - 0.5f) * a * 0.85f); b = clamp01(b + (ob - 0.5f) * a * 0.85f);
        } else if (overlayMode == 2) { r = clamp01(r + orr * a); g = clamp01(g + og * a); b = clamp01(b + ob * a); }
        else if (overlayMode == 3) { r = 1.0f - (1.0f - r) * (1.0f - orr * a); g = 1.0f - (1.0f - g) * (1.0f - og * a); b = 1.0f - (1.0f - b) * (1.0f - ob * a); }
        else if (overlayMode == 4) { r = r * (1.0f - a) + r * orr * a; g = g * (1.0f - a) + g * og * a; b = b * (1.0f - a) + b * ob * a; }
        else if (overlayMode == 5) { const float key = 0.2126f * orr + 0.7152f * og + 0.0722f * ob; a *= smooth(0.08f, 0.45f, key); r = r * (1.0f - a) + orr * a; g = g * (1.0f - a) + og * a; b = b * (1.0f - a) + ob * a; }
        else { r = r * (1.0f - a) + orr * a; g = g * (1.0f - a) + og * a; b = b * (1.0f - a) + ob * a; }
      }

      // Fifteen separately tuned, deterministic Glitch Cam engines. These are native
      // procedural effects, not static screenshots/GIFs painted over the image.
      if (glitchPreset > 0 && glitchPresetStrength > 0.0001f) {
        const float phase = timeSec * (0.45f + 3.5f * glitchPresetSpeed);
        const int rowBand = (int)std::floor((float)y / (1.0f + (glitchPreset % 5)));
        const uint32_t gh = (uint32_t)s.seed ^ (uint32_t)((rowBand + (int)std::floor(phase * 9.0f)) * 2654435761u) ^ (uint32_t)(glitchPreset * 2246822519u);
        const float eventPeriodSec = glitchPresetRate > 0.01f ? 60.0f / glitchPresetRate : 1.0e6f;
        const int glitchSlot = (int)std::floor(timeSec / eventPeriodSec);
        const float eventGate = rand01((uint32_t)glitchSlot ^ (uint32_t)(s.seed * 33 + glitchPreset * 77));
        const float chance = (0.10f + 0.025f * (float)((glitchPreset + 2) % 5)) * glitchPresetStrength;
        const float eventOn = eventGate < chance ? 1.0f : 0.0f;
        const float local = rand01(gh + 71u);
        const float amount = glitchPresetStrength * eventOn * (0.25f + 0.75f * local);
        const float xn = (float)x / (float)std::max(1, w - 1);
        const float yn = (float)y / (float)std::max(1, h - 1);
        const int rowDx = (int)std::round(std::sin((float)y * (0.019f + 0.002f * glitchPreset) + phase * 3.0f) * amount * std::max(3, w / (8 + glitchPreset % 7)));
        const int sxGlitch = clampi((int)((clampi(x + rowDx, 0, w - 1) + 0.5f) * inW / std::max(1, w)), 0, inW - 1);
        const PF_Pixel moved = inBase[sy * inStride + sxGlitch];
        const float mr = moved.red / 255.0f, mg = moved.green / 255.0f, mb = moved.blue / 255.0f;
        const float mono = 0.2126f * mr + 0.7152f * mg + 0.0722f * mb;
        switch (glitchPreset) {
          case 1: { // horizontal RF tear
            const float band = 1.0f - smooth(0.002f, 0.018f, std::fabs(yn - std::fmod(phase * 0.13f + local, 1.0f)));
            const float a = amount * band; r = clamp01(r * (1-a) + mr*a); g = clamp01(g * (1-a) + mg*a); b = clamp01(b * (1-a) + mb*a); break;
          }
          case 2: { // RGB channel separation
            const int off = (int)std::round(amount * (2.0f + 12.0f * local));
            const PF_Pixel rp = inBase[sy*inStride+clampi(sx+off,0,inW-1)];
            const PF_Pixel bp = inBase[sy*inStride+clampi(sx-off,0,inW-1)];
            r = clamp01(r*(1-amount)+rp.red/255.0f*amount); b = clamp01(b*(1-amount)+bp.blue/255.0f*amount); break;
          }
          case 3: { // vertical sync roll / wavy tear
            const float wave = std::sin(yn * 19.0f + phase * 4.0f);
            r=clamp01(r+wave*amount*0.28f); g=clamp01(g-wave*amount*0.16f); b=clamp01(b+wave*amount*0.38f); break;
          }
          case 4: { // macroblock dropout: displaced source detail and luma collapse, never random neon squares
            const uint32_t blockKey=(uint32_t)(x/18 + (y/8)*313 + glitchSlot*19 + s.seed);
            if (rand01(blockKey) < amount*0.45f) {
              const int shift=(rand01(blockKey+13u)>0.5f?1:-1)*std::max(1,(int)(amount*std::max(2,w/80)));
              const PF_Pixel q=inBase[sy*inStride+clampi(sx+shift,0,inW-1)];
              const float a=amount*0.62f; const float ql=(0.2126f*q.red+0.7152f*q.green+0.0722f*q.blue)/255.0f;
              r=clamp01(r*(1-a)+q.red/255.0f*a-amount*0.05f); g=clamp01(g*(1-a)+q.green/255.0f*a-amount*0.05f); b=clamp01(b*(1-a)+q.blue/255.0f*a-amount*0.05f);
              if (ql < 0.18f) { r*=1.0f-amount*0.35f; g*=1.0f-amount*0.35f; b*=1.0f-amount*0.35f; }
            } break;
          }
          case 5: { // chroma smear without heavy luma loss
            const float a=amount*0.7f; r=clamp01(r*(1-a)+(0.65f*mr+0.35f*mono)*a); g=clamp01(g*(1-a)+(0.65f*mg+0.35f*mono)*a); b=clamp01(b*(1-a)+(0.65f*mb+0.35f*mono)*a); break;
          }
          case 6: { // intermittent signal loss band
            const float band=1.0f-smooth(0.004f,0.025f,std::fabs(yn-std::fmod(phase*0.09f+0.22f,1.0f)));
            const float a=amount*band*0.85f; r*=1-a; g*=1-a; b*=1-a; break;
          }
          case 7: { // correlated RF hiss: line-level chroma instability, not full-frame white noise
            const uint32_t lineKey=(uint32_t)(y/2 + (int)std::floor(phase*7.0f)*977 + s.seed);
            const float line=(rand01(lineKey)-0.5f)*amount*0.18f;
            const float fine=(rand01(gh+(uint32_t)(x/7*97))-0.5f)*amount*0.035f;
            r=clamp01(r+line+fine); g=clamp01(g+line*(glitchPresetColor?-0.28f:0.0f)+fine); b=clamp01(b-line*(glitchPresetColor?0.70f:0.0f)+fine); break;
          }
          case 8: { // discontinuous scanline jitter, no visible sinusoidal wave
            const int band=y/2; const uint32_t jk=(uint32_t)(band*313 + (int)std::floor(phase*12.0f)*911 + s.seed);
            const int dx=rand01(jk)<amount*0.65f ? (int)std::round((rand01(jk+17u)-0.5f)*amount*std::max(3,w/7)) : 0;
            const PF_Pixel q=inBase[sy*inStride+clampi(sx+dx,0,inW-1)]; const float a=amount*(dx!=0?0.82f:0.0f);
            r=clamp01(r*(1-a)+q.red/255.0f*a); g=clamp01(g*(1-a)+q.green/255.0f*a); b=clamp01(b*(1-a)+q.blue/255.0f*a); break;
          }
          case 9: { // thin vertical scratches
            const float scratch=rand01((uint32_t)(x/3 + glitchSlot*83 + s.seed));
            if (scratch < amount*0.16f) { r=clamp01(r+amount*0.65f); g=clamp01(g+amount*0.65f); b=clamp01(b+amount*0.65f); } break;
          }
          case 10: { // composite chroma dropout in short line segments; preserve source luma
            const float gate=rand01((uint32_t)(x/19 + y/3 + glitchSlot*11 + s.seed));
            if (eventOn>0 && gate < amount*0.32f) { const float l=0.2126f*r+0.7152f*g+0.0722f*b; const float a=amount*0.72f; r=clamp01(r*(1-a)+l*a); g=clamp01(g*(1-a)+l*a); b=clamp01(b*(1-a)+l*a); } break;
          }
          case 11: { // hard sync dropout / brief black line
            const float band=1.0f-smooth(0.001f,0.009f,std::fabs(yn-std::fmod(phase*0.16f+local,1.0f)));
            const float a=amount*band; r*=1-a; g*=1-a; b*=1-a; break;
          }
          case 12: { // repeated echo / line memory
            const int ey=clampi(sy-std::max(1,(int)(h*0.008f)),0,inH-1); const PF_Pixel q=inBase[ey*inStride+sx]; const float a=amount*0.55f;
            r=clamp01(r*(1-a)+q.red/255.0f*a); g=clamp01(g*(1-a)+q.green/255.0f*a); b=clamp01(b*(1-a)+q.blue/255.0f*a); break;
          }
          case 13: { // interlaced field jitter
            if (((y + frame + glitchPreset) & 1) == 0) { const float a=amount*0.65f; r=clamp01(r*(1-a)+mr*a); g=clamp01(g*(1-a)+mg*a); b=clamp01(b*(1-a)+mb*a); } break;
          }
          case 14: { // chroma tracking error: narrow phase slip with luma retained, not rainbow bars
            const float band=1.0f-smooth(0.004f,0.028f,std::fabs(yn-std::fmod(phase*0.11f+0.5f,1.0f)));
            const int off=(int)std::round(amount*band*(2.0f+8.0f*std::fabs(std::sin(phase*2.0f))));
            const PF_Pixel rp=inBase[sy*inStride+clampi(sx+off,0,inW-1)]; const PF_Pixel bp=inBase[sy*inStride+clampi(sx-off,0,inW-1)]; const float a=amount*band*0.65f;
            r=clamp01(r*(1-a)+rp.red/255.0f*a); b=clamp01(b*(1-a)+bp.blue/255.0f*a); break;
          }
          case 15: { // mixed analog tear + chroma split
            const float a=amount*0.75f; r=clamp01(r*(1-a)+mr*a+(glitchPresetColor?a*0.12f:0)); g=clamp01(g*(1-a)+mg*a); b=clamp01(b*(1-a)+mb*a+(glitchPresetColor?a*0.18f:0));
            if (rand01(gh+101u)<amount*0.06f) { r=mono; g=mono; b=mono; } break;
          }
          default: break;
        }
      }

      // Procedural camcorder OSD. Text layout is precomputed once per frame; per-pixel work is O(1).
      if (osdCharCount > 0 && osdOpacity > 0.001f && x >= osdLeft && y >= osdBaseY && x < osdLeft + osdCharCount*osdGlyphW && y < osdBaseY + osdGlyphH) {
        const int relX=x-osdLeft, relY=y-osdBaseY;
        const int glyphIndex=relX/osdGlyphW;
        const int gx=(relX%osdGlyphW)/osdScale, gy=relY/osdScale;
        if (glyphIndex>=0 && glyphIndex<osdCharCount && osdGlyphPixel(osdText[glyphIndex],gx,gy)) {
          const bool inRec = osdRec && glyphIndex >= (osdPlay ? 5 : 0) && glyphIndex < (osdPlay ? 8 : 3);
          const float ink = inRec ? 0.95f : 0.92f;
          r=r*(1.0f-osdOpacity)+ink*osdOpacity;
          g=g*(1.0f-osdOpacity)+ink*osdOpacity;
          b=b*(1.0f-osdOpacity)+ink*osdOpacity;
        }
      }

      dst[x].red = (uint8_t)std::round(clamp01(r) * 255.0f);
      dst[x].green = (uint8_t)std::round(clamp01(g) * 255.0f);
      dst[x].blue = (uint8_t)std::round(clamp01(b) * 255.0f);
      dst[x].alpha = p.alpha;
    }
  }

  // Real ntsc-rs signal processing engine pass
  if (otherEnabled && otherEngine == 3 && otherStrength > 0.0001f) {
    NtscrsCParams np;
    np.chroma_delay = ntscChroma * 100.0f;
    np.head_switching = ntscHead * 100.0f;
    np.edge_wave = ntscWave * 100.0f;
    np.composite_noise = ntscComposite * 100.0f;
    np.strength = otherStrength;
    np.random_seed = s.seed;
    np.frame_num = (uint32_t)frame;

    ntscrs_process_argb8(&np, (uint8_t*)outBase, (uint32_t)w, (uint32_t)h, (size_t)output->rowbytes);
  }

  if (overlayCheckedOut) PF_CHECKIN_PARAM(in_data, &overlayCheckout);
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
        // Quantize the signed waveform symmetrically. Do not clamp negative audio to silence.
        const float normalized = std::max(-1.0f, std::min(1.0f, wet));
        wet = std::round(normalized * levels) / levels;
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
