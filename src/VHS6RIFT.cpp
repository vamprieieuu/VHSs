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
  P_INPUT = 0, P_MASTER, P_CONTINUOUS, P_EVENTS, P_SEED,
  P_TOPIC_COLOR, P_SATURATION, P_BLACK, P_SOFTNESS, P_CHROMA_BLEED, P_CHROMA_X, P_CHROMA_Y, P_TOPIC_COLOR_END,
  P_TOPIC_GRAIN, P_GRAIN_AMOUNT, P_GRAIN_SCALE, P_GRAIN_SPEED, P_TOPIC_GRAIN_END,
  P_TOPIC_DAMAGE, P_TRACKING, P_TRACK_FREQ, P_TRACK_WIDTH, P_DROPOUT, P_BLOCK_GLITCH, P_EVENT_RATE, P_EVENT_DURATION, P_RECOVERY, P_TOPIC_DAMAGE_END,
  P_TOPIC_TAPE, P_TAPE_TYPE, P_TAPE_STRENGTH, P_TAPE_SIZE, P_TAPE_SPEED, P_TAPE_DIRECTION, P_TOPIC_TAPE_END,
  P_TOPIC_OVERLAY, P_OVERLAY_ENABLE, P_TOPIC_OVERLAY_END,
  P_TOPIC_OTHER, P_OTHER_STRENGTH,
  P_CRAFT_TOPIC, P_CRAFT_ENABLE, P_CRAFT_TRACKING, P_CRAFT_CHROMA, P_CRAFT_HEADSWITCH, P_CRAFT_SMEAR, P_CRAFT_END,
  P_LEGEND_TOPIC, P_LEGEND_ENABLE, P_LEGEND_BREAKUP, P_LEGEND_TEAR, P_LEGEND_SIGNAL, P_LEGEND_BLOCKS, P_LEGEND_END,
  P_NTSC_TOPIC, P_NTSC_ENABLE, P_NTSC_CHROMA_DELAY, P_NTSC_HEAD_SWITCH, P_NTSC_EDGE_WAVE, P_NTSC_COMPOSITE, P_NTSC_END,
  P_SIGNAL_TOPIC, P_SIGNAL_ENABLE, P_SIGNAL_STRENGTH, P_SIGNAL_LUMA_NOISE, P_SIGNAL_CHROMA_NOISE, P_SIGNAL_TAPE_ERRORS, P_SIGNAL_END,
  P_TOPIC_OTHER_END,
  P_TOPIC_AUDIO, P_AUDIO_ENABLE, P_AUDIO_EVENTS, P_AUDIO_MIX, P_AUDIO_EVENT_AMOUNT, P_AUDIO_CRACKLE, P_AUDIO_DROPOUT, P_AUDIO_SATURATION, P_AUDIO_LOFI, P_TOPIC_AUDIO_END,
  P_TOPIC_MODULES, P_GRAIN_ENABLE, P_TRACKING_ENABLE, P_TAPE_ENABLE, P_OTHER_ENABLE, P_TOPIC_MODULES_END,
  P_TOPIC_ADV_TAPE, P_TRANSPARENT_TAPE_ENABLE, P_TRANSPARENT_TAPE_STRENGTH, P_TRANSPARENT_TAPE_WIDTH, P_TRANSPARENT_TAPE_SPEED,
  P_TAPE_SWEEP_ENABLE, P_TAPE_SWEEP_STRENGTH, P_TAPE_SWEEP_WIDTH, P_TAPE_SWEEP_SPEED, P_TAPE_SWEEP_INTERVAL, P_TOPIC_ADV_TAPE_END,
  P_TOPIC_GLITCH_CAM, P_GLITCH_CAM_MODE, P_GLITCH_CAM_AMOUNT, P_GLITCH_CAM_CHROMA, P_GLITCH_CAM_BLOCKS, P_GLITCH_CAM_RATE, P_TOPIC_GLITCH_CAM_END,
  P_TOPIC_ADVANCED, P_ADV_GRAIN_SIZE, P_ADV_GRAIN_SOFTNESS, P_ADV_GRAIN_MODE, P_ADV_GRAIN_COLOR, P_ADV_GRAIN_RED, P_ADV_GRAIN_GREEN, P_ADV_GRAIN_BLUE,
  P_ADV_LOWPASS_TYPE, P_ADV_SHARPEN, P_ADV_CHROMA_DEMOD, P_ADV_VERTICAL_NOISE, P_ADV_DOT_NOISE, P_ADV_NOISE_COLOR,
  P_ADV_HEADSWITCH_POS, P_ADV_HEADSWITCH_HEIGHT, P_ADV_HEADSWITCH_MOTION, P_ADV_HEADSWITCH_RANDOM,
  P_ADV_OSD_ENABLE, P_ADV_OSD_MODE, P_ADV_OSD_OPACITY, P_ADV_OSD_COLOR, P_ADV_GLITCH_ENABLE, P_ADV_BUILTIN_OVERLAY_STYLE, P_ADV_BUILTIN_OVERLAY_OPACITY, P_ADV_BUILTIN_OVERLAY_SPEED, P_ADV_BUILTIN_OVERLAY_BLEND, P_ADV_OSD_POS_X, P_ADV_OSD_POS_Y, P_ADV_OSD_SCALE, P_ADV_GLITCH_COLOR, P_ADV_GLITCH_BLENDIN, P_TOPIC_ADVANCED_END, P_COUNT
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
  PF_ADD_FLOAT_SLIDERX("VHS Strength", 0, 2000, 0, 2000, 350, PF_Precision_TENTHS, 0, 0, P_MASTER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Continuous VHS Signal Treatment", "Enable", TRUE, 0, P_CONTINUOUS);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Legacy Event Toggle (unused)", "Reserved", FALSE, 0, P_EVENTS);
  AEFX_CLR_STRUCT(def);
  PF_ADD_SLIDER("Random Seed", 0, 9999, 0, 9999, 17, P_SEED);
  AEFX_CLR_STRUCT(def);

  PF_ADD_TOPIC("IMAGE — Color and Chroma", P_TOPIC_COLOR);
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

  PF_ADD_TOPIC("IMAGE — Moving Grain", P_TOPIC_GRAIN);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Colored Grain Amount", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_GRAIN_AMOUNT);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Grain Scale", 1, 100, 1, 100, 28, PF_Precision_TENTHS, 0, 0, P_GRAIN_SCALE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Grain Motion Speed", 0, 400, 0, 400, 100, PF_Precision_TENTHS, 0, 0, P_GRAIN_SPEED);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_GRAIN_END);

  AEFX_CLR_STRUCT(def);

  PF_ADD_TOPIC("SIGNAL — Tracking and Damage Events", P_TOPIC_DAMAGE);
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

  PF_ADD_TOPIC("TAPE — Tape Behavior", P_TOPIC_TAPE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Tape Behavior Model", 6, 1, "Off|Horizontal Band|Bottom Head-Switch|Vertical Roll|Side Tear|Signal Snow", P_TAPE_TYPE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Tape Strength", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_TAPE_STRENGTH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Tape Size", 1, 100, 1, 100, 18, PF_Precision_TENTHS, 0, 0, P_TAPE_SIZE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Tape Speed", 0, 300, 0, 300, 60, PF_Precision_TENTHS, 0, 0, P_TAPE_SPEED);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Tape Direction", 4, 1, "Top to Bottom|Bottom to Top|Left to Right|Right to Left", P_TAPE_DIRECTION);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_TAPE_END);

  AEFX_CLR_STRUCT(def);

  PF_ADD_TOPIC("OVERLAYS — Source-Footage VHS Overlays", P_TOPIC_OVERLAY);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable VHS Overlay on Source Footage", "Enable", FALSE, 0, P_OVERLAY_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_OVERLAY_END);

  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("SIGNAL ENGINES — Alternate Character", P_TOPIC_OTHER);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Signal Engines Strength", 0, 100, 0, 100, 65, PF_Precision_TENTHS, 0, 0, P_OTHER_STRENGTH);
  AEFX_CLR_STRUCT(def);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("VHS Craft — Signal Controls", P_CRAFT_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable VHS Craft Engine", "Enable", FALSE, 0, P_CRAFT_ENABLE);
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
  PF_ADD_TOPIC("VHS Legend — Signal Controls", P_LEGEND_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable VHS Legend Engine", "Enable", FALSE, 0, P_LEGEND_ENABLE);
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
  PF_ADD_TOPIC("NTSC-RS — Base Signal Controls", P_NTSC_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable NTSC-RS Signal Engine", "Enable", TRUE, 0, P_NTSC_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("NTSC Chroma Delay", 0, 100, 0, 100, 18, PF_Precision_TENTHS, 0, 0, P_NTSC_CHROMA_DELAY);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("NTSC Head Switching", 0, 100, 0, 100, 14, PF_Precision_TENTHS, 0, 0, P_NTSC_HEAD_SWITCH);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("NTSC Edge Wave", 0, 100, 0, 100, 12, PF_Precision_TENTHS, 0, 0, P_NTSC_EDGE_WAVE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("NTSC Composite Signal", 0, 100, 0, 100, 14, PF_Precision_TENTHS, 0, 0, P_NTSC_COMPOSITE);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_NTSC_END);
  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("Signal Reference — Controls", P_SIGNAL_TOPIC);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable Signal Reference Engine", "Enable", FALSE, 0, P_SIGNAL_ENABLE);
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
  PF_ADD_TOPIC("AUDIO — Tape Audio Degradation", P_TOPIC_AUDIO);
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
  PF_ADD_TOPIC("SYSTEM — Module Enables", P_TOPIC_MODULES);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable Grain / Noise", "Enable", TRUE, 0, P_GRAIN_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable Tracking Damage", "Enable", TRUE, 0, P_TRACKING_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable Tape Effects", "Enable", TRUE, 0, P_TAPE_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_CHECKBOX("Enable Other Signal Engines", "Enable", TRUE, 0, P_OTHER_ENABLE);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_MODULES_END);

  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("OVERLAYS — Source-Footage VHS Overlays", P_TOPIC_ADV_TAPE);
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
  PF_ADD_TOPIC("GLITCH CAM — Event Style and Timing", P_TOPIC_GLITCH_CAM);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Glitch Cam Style", 15, 1,
    "Sync Tear|Chroma Breakup|Block Corruption|Luma Dropout|Mixed Signal Burst|RGB Convergence Burst|Vertical Hold Roll|Color Smear|Head-Switch Noise|Horizontal Fold|Solarized Flash|Tape Stretch|Ghost Echo|Signal Crush|Chroma Bar Interference", P_GLITCH_CAM_MODE);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Glitch Cam Amount", 0, 200, 0, 200, 70, PF_Precision_TENTHS, 0, 0, P_GLITCH_CAM_AMOUNT);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Glitch Cam Chroma Split", 0, 12, 0, 12, 3, PF_Precision_TENTHS, 0, 0, P_GLITCH_CAM_CHROMA);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Glitch Cam Block Size", 2, 64, 2, 64, 16, PF_Precision_TENTHS, 0, 0, P_GLITCH_CAM_BLOCKS);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Glitch Cam Event Rate (events/minute)", 0, 60, 0, 60, 30, PF_Precision_TENTHS, 0, 0, P_GLITCH_CAM_RATE);
  AEFX_CLR_STRUCT(def);
  PF_END_TOPIC(P_TOPIC_GLITCH_CAM_END);

  AEFX_CLR_STRUCT(def);
  PF_ADD_TOPIC("ADVANCED — Grain, Signal Filters and OSD", P_TOPIC_ADVANCED);
  AEFX_CLR_STRUCT(def);
  PF_ADD_POPUP("Grain Size", 5, 3, "4mm (Soft / Low Detail)|8mm (Soft)|16mm (Balanced)|32mm (Fine)|64mm (Very Fine)", P_ADV_GRAIN_SIZE);
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
  PF_ADD_CHECKBOX("Enable Glitch Cam Events", "Enable", FALSE, 0, P_ADV_GLITCH_ENABLE);
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
  PF_ADD_POPUP("Glitch Cam Color Palette", 5, 1, "Original Signal|RGB Split|Cyan / Red|Magenta / Green|Amber / Blue", P_ADV_GLITCH_COLOR);
  AEFX_CLR_STRUCT(def);
  PF_ADD_FLOAT_SLIDERX("Glitch Cam Blend-in", 0, 100, 0, 100, 15, PF_Precision_TENTHS, 0, 0, P_ADV_GLITCH_BLENDIN);
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
  int seed=17, tapeType=0, tapeDir=0;
  float master=.35f, continuous=1, events=1, saturation=1, black=0, softness=.12f, bleed=.20f;
  float chromaX=2, chromaY=0, grain=.08f, grainScale=28, grainSpeed=1, tracking=.18f, trackFreq=.25f, trackWidth=.18f;
  float dropout=.10f, blocks=.05f, eventRate=8, eventDuration=.18f, recovery=.12f, tapeStrength=.35f, tapeSize=.18f, tapeSpeed=.6f;
};

static float getSlider(PF_ParamDef *params[], int id, float scale) { return (float)params[id]->u.fs_d.value / scale; }
static int getInt(PF_ParamDef *params[], int id) { return params[id]->u.sd.value; }
static bool getCheck(PF_ParamDef *params[], int id) { return params[id]->u.bd.value != 0; }
static Settings ReadSettings(PF_ParamDef *p[]) {
  Settings s;
  s.master = getSlider(p,P_MASTER,100.0f);
  s.continuous = getCheck(p,P_CONTINUOUS) ? 1.0f : 0.0f;
  s.events = getCheck(p,P_EVENTS) ? 1.0f : 0.0f; // Read for stable parameter compatibility; dedicated Glitch Cam no longer depends on it.
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
  return s;
}

struct VhsSignalProfile {
  float saturation, contrast, warm, green, rGain, gGain, bGain, gamma, pedestal, shoulder, chromaBleed, chromaOffsetX, chromaOffsetY, edgeRinging, noiseFloor;
};
// One fixed VHS transfer response. Camera selection was removed: this plugin is a VHS signal engine, not a camera-color LUT selector.
static const VhsSignalProfile kVhsSignalProfile = {1.0f, 1.08f, 0.015f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.015f, 0.18f, 0.38f, 1.0f, 0.0f, 0.12f, 0.06f};

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
  baseNtscParams.composite_noise = getSlider(params, P_NTSC_COMPOSITE, 1.0f);
  const bool hasNtscControls = getCheck(params, P_NTSC_ENABLE) && (baseNtscParams.chroma_delay > 0.001f || baseNtscParams.head_switching > 0.001f ||
    baseNtscParams.edge_wave > 0.001f || baseNtscParams.composite_noise > 0.001f);
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

  const VhsSignalProfile &prof = kVhsSignalProfile;

  // Master strength is 0..20 (UI 0..2000). Preserve the established default
  // at 3.5 (UI 350) as drive=1.0. Above the default, use a bounded linear
  // extension to drive=2.5 at UI 2000. This avoids the previous early ceiling
  // at drive=2.0 (UI 700), which made 700..2000 visually identical for many
  // drive-dependent stages. Values below the default retain proportional gain.
  const float drive = (strength <= 3.5f)
      ? (strength / 3.5f)
      : (1.0f + ((strength - 3.5f) / (20.0f - 3.5f)) * 1.5f);
  // A single neutral transfer response keeps the core VHS signal independent of camera-color presets.
  const float charScale = 1.0f;
  const float colorScale = 1.0f;
  const float sigScale = drive;

  // Color response & chroma behavior
  const float sat = s.saturation * prof.saturation * colorScale;
  const float bleed = clamp01(s.bleed * prof.chromaBleed * (0.3f + 0.7f * drive));
  const float totalChromaX = s.chromaX * drive + prof.chromaOffsetX * colorScale;
  const float totalChromaY = s.chromaY * drive + prof.chromaOffsetY * colorScale;

  // Luma Softness is strictly controlled by P_SOFTNESS slider (0..100).
  const float soft = clamp01(s.softness * 0.01f);
  const int lowpassType = std::max(0, params[P_ADV_LOWPASS_TYPE]->u.pd.value - 1);
  const int blurRadius = clampi((int)std::ceil(soft * (lowpassType == 0 ? 5.0f : 2.0f)), 0, 5);

  // Module checkboxes and enablers — strictly obeys checkbox state
  const bool grainEnabled = getCheck(params, P_GRAIN_ENABLE);
  const bool trackingEnabled = getCheck(params, P_TRACKING_ENABLE);
  const bool tapeEnabled = getCheck(params, P_TAPE_ENABLE);
  const bool overlayEnabled = getCheck(params, P_OVERLAY_ENABLE);
  const bool otherEnabled = getCheck(params, P_OTHER_ENABLE);

  const bool transparentTapeEnabled = getCheck(params, P_TRANSPARENT_TAPE_ENABLE);
  const bool tapeSweepEnabled = getCheck(params, P_TAPE_SWEEP_ENABLE);

  const float transparentTapeStrength = getSlider(params, P_TRANSPARENT_TAPE_STRENGTH, 100.0f);
  const float transparentTapeWidth = getSlider(params, P_TRANSPARENT_TAPE_WIDTH, 100.0f);
  const float transparentTapeSpeed = getSlider(params, P_TRANSPARENT_TAPE_SPEED, 100.0f);
  const float tapeSweepStrength = getSlider(params, P_TAPE_SWEEP_STRENGTH, 100.0f);
  const float tapeSweepWidth = getSlider(params, P_TAPE_SWEEP_WIDTH, 100.0f);
  const float tapeSweepSpeed = getSlider(params, P_TAPE_SWEEP_SPEED, 100.0f);
  const float tapeSweepInterval = std::max(0.1f, getSlider(params, P_TAPE_SWEEP_INTERVAL, 1.0f));


  // Moving grain is intentionally visible at the default and is spatially interpolated to avoid blocky cells.
  const float grainAmount = grainEnabled ? clamp01(std::max(s.grain, 0.35f)) : 0.0f;
  const int advGrainSize = clampi(params[P_ADV_GRAIN_SIZE]->u.pd.value - 1, 0, 4);
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
  const bool glitchCamEnabled = getCheck(params, P_ADV_GLITCH_ENABLE);
  const int osdMode = clampi(params[P_ADV_OSD_MODE]->u.pd.value - 1, 0, 3);
  const float osdOpacity = getSlider(params, P_ADV_OSD_OPACITY, 100.0f);
  const int osdColor = clampi(params[P_ADV_OSD_COLOR]->u.pd.value - 1, 0, 2);
  const float osdPosX = getSlider(params, P_ADV_OSD_POS_X, 100.0f);
  const float osdPosY = getSlider(params, P_ADV_OSD_POS_Y, 100.0f);
  const int osdScalePx = clampi((int)std::round(getSlider(params, P_ADV_OSD_SCALE, 100.0f)), 1, 3);
  const int glitchCamColor = clampi(params[P_ADV_GLITCH_COLOR]->u.pd.value - 1, 0, 4);
  const float glitchCamBlendIn = getSlider(params, P_ADV_GLITCH_BLENDIN, 100.0f);
  const bool builtinOverlayEnabled = overlayEnabled;
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
  const int glitchCamMode = std::max(0, params[P_GLITCH_CAM_MODE]->u.pd.value - 1);
  const float glitchCamAmount = getSlider(params, P_GLITCH_CAM_AMOUNT, 100.0f);
  const int glitchCamChroma = std::max(0, (int)getSlider(params, P_GLITCH_CAM_CHROMA, 1.0f));
  const int glitchCamBlockSize = std::max(2, (int)getSlider(params, P_GLITCH_CAM_BLOCKS, 1.0f));
  const float glitchCamRate = getSlider(params, P_GLITCH_CAM_RATE, 1.0f);
  const float effectiveEventRate = glitchCamRate > 0.001f ? glitchCamRate : s.eventRate;
  const float eventPeriod = effectiveEventRate > 0.001f ? 60.0f / effectiveEventRate : 1.0e9f;
  const int eventSlot = (int)std::floor(timeSec / eventPeriod);
  const float slotTime = timeSec - eventSlot * eventPeriod;
  // Use the same effective rate for both slot timing and event gating.
  // Previously the Glitch Cam Event Rate could change eventPeriod while the
  // gate still checked s.eventRate, so setting the base rate to zero silently
  // disabled all events even when the Glitch Cam rate slider was non-zero.
  const bool chosenEvent = effectiveEventRate > 0.0f && rand01((uint32_t)(eventSlot * 747796405u) ^ (uint32_t)s.seed) < 0.90f;
  float eventEnv = 0.0f;
  if (chosenEvent && glitchCamEnabled) {
    const float attack = 0.005f + 0.08f * glitchCamBlendIn;
    if (slotTime < attack) eventEnv = smooth(0.0f, attack, slotTime);
    else if (slotTime < s.eventDuration) eventEnv = 1.0f;
    else if (s.recovery > 0.0f) eventEnv = 1.0f - smooth(s.eventDuration, s.eventDuration + s.recovery, slotTime);
  }

  const float otherStrengthRaw = getSlider(params, P_OTHER_STRENGTH, 100.0f);
  const float otherStrength = otherStrengthRaw * drive;
  const bool craftEngineEnabled = getCheck(params, P_CRAFT_ENABLE) && otherEnabled;
  const bool legendEngineEnabled = getCheck(params, P_LEGEND_ENABLE) && otherEnabled;
  const bool signalReferenceEnabled = getCheck(params, P_SIGNAL_ENABLE) && otherEnabled;

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

  const int selTapeType = params[P_TAPE_TYPE]->u.pd.value - 1;
  // "Off" stays off even when Enable Tape Effects is checked. Users must
  // explicitly select a tape behavior before the tape module alters pixels.
  const int tapeType = selTapeType;
  const float tapeStrengthRaw = getSlider(params, P_TAPE_STRENGTH, 100.0f);
  // Zero means bypass; do not silently inject a 35% tape band.
  const float effTapeStrength = tapeEnabled ? tapeStrengthRaw : 0.0f;

  // Tracking amount is controlled by its slider; zero must disable the band.
  // sigScale already includes normalized master drive, so do not multiply by
  // raw master strength again (which over-amplified tracking at the default 3.5).
  const float effTracking = trackingEnabled ? clamp01(s.tracking * sigScale) : 0.0f;

  for (int y = 0; y < h; ++y) {
    PF_Pixel *dst = outBase + y * outStride;
    for (int x = 0; x < w; ++x) {
      int sx = clampi((int)((x + 0.5f) * inW / std::max(1, w)), 0, inW - 1);
      int sy = clampi((int)((y + 0.5f) * inH / std::max(1, h)), 0, inH - 1);

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
        const float black = (s.black * 0.35f + prof.pedestal * 100.0f * charScale) * drive;
        r = clamp01((r - 0.5f) * (1.0f + black * 0.01f) + 0.5f - black * 0.005f);
        g = clamp01((g - 0.5f) * (1.0f + black * 0.01f) + 0.5f - black * 0.005f);
        b = clamp01((b - 0.5f) * (1.0f + black * 0.01f) + 0.5f - black * 0.005f);

        // Signal Grain
        if (grainAmount > 0.001f) {
          static const float grainSizeScale[5] = { 5.0f, 3.4f, 2.0f, 1.0f, 0.5f };
          const float scale = std::max(1.0f, (1.0f + s.grainScale * 0.035f) * grainSizeScale[advGrainSize]);
          const uint32_t motionFrame = (uint32_t)std::floor(timeSec * std::max(0.05f, s.grainSpeed) * fps);
          const int gx = (int)std::floor((float)x / scale), gy = (int)std::floor((float)y / scale);
          const float fx = smooth(0.0f, 1.0f, std::fmod((float)x / scale, 1.0f));
          const float fy = smooth(0.0f, 1.0f, std::fmod((float)y / scale, 1.0f));
          auto grainCell = [&](int xx, int yy, uint32_t salt) {
            const uint32_t hs = (uint32_t)s.seed ^ (motionFrame * 2246822519u) ^ (uint32_t)(xx * 73856093u) ^ (uint32_t)(yy * 19349663u) ^ salt;
            return rand01(hs) - 0.5f;
          };
          const float grainTL = grainCell(gx, gy, 0u), grainTR = grainCell(gx + 1, gy, 0u);
          const float grainBL = grainCell(gx, gy + 1, 0u), grainBR = grainCell(gx + 1, gy + 1, 0u);
          const float top = grainTL + (grainTR - grainTL) * fx;
          const float bottom = grainBL + (grainBR - grainBL) * fx;
          const float rawN = top + (bottom - top) * fy;
          const float n = rawN * grainAmount * 1.05f * (1.0f - 0.65f * advGrainSoftness);
          const float chromaN = grainCell(gx, gy, 0xA511E9B3u) * grainAmount * 0.85f;
          const float lumaWeight = advGrainMode == 1 ? 0.0f : 1.0f;
          const float chromaWeight = advGrainMode == 0 ? 0.0f : 1.0f;
          const float softMix = advGrainSoftness;
          const float gn = n * (1.0f - 0.7f * softMix);
          if (advGrainColor == 1) {
            r = clamp01(r + gn); g = clamp01(g + gn); b = clamp01(b + gn);
          } else if (advGrainColor == 2) {
            r = clamp01(r + gn * 1.15f * advGrainR + chromaN * chromaWeight * noiseColor);
            g = clamp01(g + gn * 0.80f * advGrainG - chromaN * chromaWeight * 0.25f * noiseColor);
            b = clamp01(b + gn * 1.30f * advGrainB - chromaN * chromaWeight * noiseColor);
          } else {
            r = clamp01(r + gn * lumaWeight * 1.15f + chromaN * chromaWeight * 0.65f);
            g = clamp01(g + gn * lumaWeight * 0.80f - chromaN * chromaWeight * 0.15f);
            b = clamp01(b + gn * lumaWeight * 1.30f - chromaN * chromaWeight * 0.65f);
          }
        }
      }

      // Continuous tape dropouts (P_DROPOUT) are independent of discrete Glitch Cam events.
      // Use short horizontal streak segments instead of a field of square speckles.
      // Hashes depend only on frame/seed/segment, so rendering remains deterministic.
      if (s.continuous && s.dropout > 0.001f) {
        const float dropoutAmount = clamp01(s.dropout * sigScale);
        const int segmentW = std::max(12, w / 36);
        const int streakH = std::max(1, h / 360);
        const int segmentX = x / segmentW;
        const int streakY = y / streakH;
        // Hold each dropout pattern for three frames to resemble a brief tape
        // dropout instead of independent per-frame digital flicker. The frame
        // bucket remains a pure function of composition time, so scrubbing is stable.
        const int dropoutFrameBucket = frame >= 0 ? frame / 3 : (frame - 2) / 3;
        const uint32_t dh = (uint32_t)s.seed ^ (uint32_t)(dropoutFrameBucket * 747796405u)
          ^ (uint32_t)(segmentX * 2891336453u) ^ (uint32_t)(streakY * 1181783497u);
        // Zero is a true bypass; low values create sparse streaks, high values create
        // more frequent and deeper signal-loss marks.
        const float dropoutChance = 0.095f * dropoutAmount;
        if (rand01(dh) < dropoutChance) {
          const float dropoutMix = clamp01(dropoutAmount * (0.30f + 0.70f * rand01(dh + 17u)));
          const float residual = 0.03f + 0.10f * rand01(dh + 29u);
          r = clamp01(r * (1.0f - dropoutMix) + r * residual * dropoutMix);
          g = clamp01(g * (1.0f - dropoutMix) + g * residual * dropoutMix);
          b = clamp01(b * (1.0f - dropoutMix) + b * residual * dropoutMix);
        }
      }

      // Tracking bands
      if (s.continuous && effTracking > 0.001f) {
        const float phase = timeSec * (0.08f + 0.8f * s.trackFreq) * 1.0f;
        const float bandY = std::fmod(phase, 1.0f);
        const float yn = (float)y / std::max(1, h);
        const float dist = std::fabs(yn - bandY);
        const float bw = std::max(0.002f, s.trackWidth * 0.08f);
        const float band = (1.0f - smooth(bw, bw * 2.5f, dist)) * effTracking;
        r = clamp01(r + band * 0.30f);
        g = clamp01(g + band * 0.42f);
        b = clamp01(b + band * 0.65f);
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

      // Discrete glitch events: deterministic horizontal sync tears plus chroma breakup.
      // The same composition time/seed always yields the same frame; no history-dependent state.
      if (eventEnv > 0.001f) {
        const int bandHeight = std::max(2, h / 90);
        const int bandIndex = y / bandHeight;
        const uint32_t eh = (uint32_t)s.seed ^ (uint32_t)(eventSlot * 2654435761u) ^ (uint32_t)(bandIndex * 2246822519u);
        const float lineNoise = rand01(eh);
        const float amount = clamp01(glitchCamAmount * eventEnv * (0.75f + 0.25f * drive));
        const float tearChance = (glitchCamMode == 1 || glitchCamMode == 4 ? 0.26f : 0.035f) * amount;
        if ((glitchCamMode == 0 || glitchCamMode == 4) && lineNoise < tearChance) {
          const float signedShift = (rand01(eh + 31u) * 2.0f - 1.0f);
          const int maxShift = std::max(2, (int)(w / 12 * amount));
          const int shift = (int)std::round(signedShift * maxShift * eventEnv);
          const int tx = clampi(sx + shift, 0, inW - 1);
          const PF_Pixel tear = inBase[sy * inStride + tx];
          const PF_Pixel tearR = inBase[sy * inStride + clampi(tx + glitchCamChroma, 0, inW - 1)];
          const PF_Pixel tearB = inBase[sy * inStride + clampi(tx - glitchCamChroma, 0, inW - 1)];
          const float mix = clamp01(eventEnv * amount * (0.35f + 0.45f * rand01(eh + 37u)));
          const float tr = tear.red / 255.0f, tg = tear.green / 255.0f, tb = tear.blue / 255.0f;
          r = clamp01(r * (1.0f - mix) + tr * mix + (tearR.red / 255.0f - tr) * mix * 0.28f);
          g = clamp01(g * (1.0f - mix) + tg * mix);
          b = clamp01(b * (1.0f - mix) + tb * mix + (tearB.blue / 255.0f - tb) * mix * 0.28f);
        }
        if ((glitchCamMode == 1 || glitchCamMode == 4) && lineNoise < 0.22f * amount) {
          const int cx = clampi(sx + (rand01(eh + 41u) < 0.5f ? -glitchCamChroma : glitchCamChroma), 0, inW - 1);
          const PF_Pixel cp = inBase[sy * inStride + cx];
          const float cm = clamp01(0.6f * amount * eventEnv);
          r = clamp01(r * (1.0f - cm) + (cp.red / 255.0f) * cm);
          b = clamp01(b * (1.0f - cm) + (cp.blue / 255.0f) * cm);
        }
        if ((glitchCamMode == 2 || glitchCamMode == 4) && lineNoise < 0.35f * amount) {
          const int block = ((x / glitchCamBlockSize) + (y / std::max(2, glitchCamBlockSize / 2)) * 13 + eventSlot) % 5;
          if (block < 2) { r = clamp01(r * (1.0f - amount) + rand01(eh + 7u) * amount); g = clamp01(g * (1.0f - amount) + rand01(eh + 11u) * amount); b = clamp01(b * (1.0f - amount) + rand01(eh + 19u) * amount); }
        }
        if ((glitchCamMode == 3 || glitchCamMode == 4) && lineNoise < (0.20f + 0.25f * s.dropout * sigScale) * amount) {
          const float dropMix = clamp01(amount * eventEnv);
          r *= 1.0f - 0.92f * dropMix; g *= 1.0f - 0.92f * dropMix; b *= 1.0f - 0.92f * dropMix;
        }
        if (glitchCamMode == 4 && lineNoise < 0.08f * amount) { r = clamp01(r + 0.25f * amount); g *= 1.0f - 0.4f * amount; b = clamp01(b + 0.35f * amount); }

        // Additional reference-inspired Glitch Cam modes (indices 5..14).
        // Each mode has a different image operation; eventEnv makes them intermittent,
        // while coordinate/time/seed hashes keep frame renders deterministic.
        if (glitchCamMode == 5 && lineNoise < 0.30f * amount) { // RGB convergence burst
          const int d = std::max(1, (int)std::round((1.0f + glitchCamChroma) * amount));
          const PF_Pixel rp = inBase[sy * inStride + clampi(sx + d, 0, inW - 1)];
          const PF_Pixel bp = inBase[sy * inStride + clampi(sx - d, 0, inW - 1)];
          const float m = clamp01(amount * eventEnv * 0.85f);
          r = clamp01(r * (1.0f-m) + (rp.red/255.0f)*m);
          b = clamp01(b * (1.0f-m) + (bp.blue/255.0f)*m);
        } else if (glitchCamMode == 6 && lineNoise < 0.22f * amount) { // vertical hold roll
          const int roll = std::max(1, (int)std::round((h / 18.0f) * amount * (0.5f + rand01(eh+51u))));
          const int ry = clampi(sy + ((eh & 1u) ? roll : -roll), 0, inH - 1);
          const PF_Pixel rp = inBase[ry * inStride + sx];
          const float m = clamp01(0.75f * amount * eventEnv);
          r = clamp01(r*(1.0f-m)+(rp.red/255.0f)*m); g = clamp01(g*(1.0f-m)+(rp.green/255.0f)*m); b = clamp01(b*(1.0f-m)+(rp.blue/255.0f)*m);
        } else if (glitchCamMode == 7 && lineNoise < 0.28f * amount) { // color smear
          const int d = std::max(1, (int)std::round(glitchCamChroma + 2.0f * amount));
          const PF_Pixel cp = inBase[sy * inStride + clampi(sx-d, 0, inW-1)];
          const float m = clamp01(amount * eventEnv * 0.72f);
          r = clamp01(r*(1.0f-m)+(cp.red/255.0f)*m); b = clamp01(b*(1.0f-m)+(cp.blue/255.0f)*m);
        } else if (glitchCamMode == 8 && (float)y / (float)std::max(1,h) > 0.86f && lineNoise < 0.72f * amount) { // head-switch noise
          const float noise = rand01(eh + (uint32_t)(x/3));
          const float m = clamp01(amount * eventEnv * (0.35f + 0.65f*noise));
          r = clamp01(r*(1.0f-m)+noise*m); g = clamp01(g*(1.0f-m)+noise*m); b = clamp01(b*(1.0f-m)+noise*m);
        } else if (glitchCamMode == 9 && lineNoise < 0.24f * amount) { // horizontal fold
          const int fold = std::max(1, (int)std::round((w/10.0f)*amount*(rand01(eh+61u)*2.0f-1.0f)));
          const PF_Pixel fp = inBase[sy * inStride + clampi(sx+fold,0,inW-1)];
          const float m = clamp01(amount*eventEnv*0.9f);
          r=clamp01(r*(1.0f-m)+(fp.red/255.0f)*m); g=clamp01(g*(1.0f-m)+(fp.green/255.0f)*m); b=clamp01(b*(1.0f-m)+(fp.blue/255.0f)*m);
        } else if (glitchCamMode == 10 && lineNoise < 0.18f * amount) { // brief solarized flash
          const float l=0.2126f*r+0.7152f*g+0.0722f*b; const float inv=clamp01(1.0f-l);
          const float m=clamp01(amount*eventEnv*0.8f);
          r=clamp01(r*(1.0f-m)+inv*m); g=clamp01(g*(1.0f-m)+inv*m); b=clamp01(b*(1.0f-m)+inv*m);
        } else if (glitchCamMode == 11 && lineNoise < 0.20f * amount) { // tape stretch / horizontal smear
          const int d=std::max(1,(int)std::round((w/24.0f)*amount*(0.3f+rand01(eh+71u))));
          const PF_Pixel sp=inBase[sy*inStride+clampi(sx-d,0,inW-1)]; const float m=clamp01(amount*eventEnv*0.65f);
          r=clamp01(r*(1.0f-m)+(sp.red/255.0f)*m); g=clamp01(g*(1.0f-m)+(sp.green/255.0f)*m); b=clamp01(b*(1.0f-m)+(sp.blue/255.0f)*m);
        } else if (glitchCamMode == 12 && lineNoise < 0.26f * amount) { // short ghost echo
          const PF_Pixel gp=inBase[sy*inStride+clampi(sx-std::max(1,w/160),0,inW-1)]; const float m=clamp01(amount*eventEnv*0.45f);
          r=clamp01(r*(1.0f-m)+(gp.red/255.0f)*m); g=clamp01(g*(1.0f-m)+(gp.green/255.0f)*m); b=clamp01(b*(1.0f-m)+(gp.blue/255.0f)*m);
        } else if (glitchCamMode == 13 && lineNoise < 0.32f * amount) { // reduced-bit-depth signal crush
          const int levels=std::max(2, 12-(int)std::round(amount*8.0f));
          r=std::round(r*levels)/levels; g=std::round(g*levels)/levels; b=std::round(b*levels)/levels;
        } else if (glitchCamMode == 14 && lineNoise < 0.24f * amount) { // chroma bar interference
          const float phase=std::sin((float)y*0.43f+timeSec*18.0f+(float)(eh&255u)*0.02f);
          const float m=clamp01(amount*eventEnv*0.38f);
          r=clamp01(r+m*std::max(0.0f,phase)); g=clamp01(g-m*0.45f*std::fabs(phase)); b=clamp01(b+m*std::max(0.0f,-phase));
        }
        // Optional event palette: changes the chroma signature without replacing
        // the selected mode's geometry or turning the whole image into a tint.
        if (glitchCamColor > 0 && amount > 0.001f) {
          const float paletteMix = clamp01(amount * eventEnv * 0.18f);
          float pr = r, pg = g, pb = b;
          if (glitchCamColor == 1) { pr = clamp01(r + paletteMix * 0.35f); pb = clamp01(b + paletteMix * 0.35f); }
          else if (glitchCamColor == 2) { pr = clamp01(r + paletteMix * 0.3f); pg = clamp01(g + paletteMix * 0.2f); pb = clamp01(b + paletteMix * 0.4f); }
          else if (glitchCamColor == 3) { pr = clamp01(r + paletteMix * 0.35f); pg = clamp01(g - paletteMix * 0.2f); pb = clamp01(b + paletteMix * 0.35f); }
          else if (glitchCamColor == 4) { pr = clamp01(r + paletteMix * 0.35f); pg = clamp01(g + paletteMix * 0.08f); pb = clamp01(b - paletteMix * 0.25f); }
          r = clamp01(r * (1.0f - paletteMix) + pr * paletteMix);
          g = clamp01(g * (1.0f - paletteMix) + pg * paletteMix);
          b = clamp01(b * (1.0f - paletteMix) + pb * paletteMix);
        }
      }

      // Independent block corruption control (P_BLOCK_GLITCH).
      // This slider was read into Settings but previously never reached the pixel path,
      // making the UI control inert. Use stable per-cell/per-frame hashes so scrubbing
      // is deterministic and keep the stage bypassed at zero.
      if (s.continuous && s.blocks > 0.001f) {
        const float blockAmount = clamp01(s.blocks * sigScale);
        const int cellW = std::max(8, w / 48);
        const int cellH = std::max(4, h / 54);
        const int cellX = x / cellW;
        const int cellY = y / cellH;
        const uint32_t bh = (uint32_t)s.seed ^ (uint32_t)(frame * 2246822519u)
          ^ (uint32_t)(cellX * 3266489917u) ^ (uint32_t)(cellY * 668265263u);
        const float cellPick = rand01(bh);
        const float cellChance = 0.015f + 0.20f * blockAmount;
        if (cellPick < cellChance) {
          const int shiftLimit = std::max(1, (int)std::round((w / 80.0f) * blockAmount));
          const int shift = (int)std::round((rand01(bh + 13u) * 2.0f - 1.0f) * shiftLimit);
          const int bx = clampi(sx + shift, 0, inW - 1);
          const PF_Pixel bp = inBase[sy * inStride + bx];
          const float cellMix = clamp01(blockAmount * (0.25f + 0.65f * rand01(bh + 29u)));
          r = clamp01(r * (1.0f - cellMix) + (bp.red / 255.0f) * cellMix);
          g = clamp01(g * (1.0f - cellMix) + (bp.green / 255.0f) * cellMix);
          b = clamp01(b * (1.0f - cellMix) + (bp.blue / 255.0f) * cellMix);
        }
      }

      // Tape band effects. Direction chooses the travel axis and polarity for every
      // moving tape type; Size controls band thickness (or snow clump size), and
      // Speed controls temporal progression rather than being ignored by fixed bands.
      if (tapeType > 0 && effTapeStrength > 0.001f) {
        const float speed = s.tapeSpeed;
        const bool horizontalTravel = (s.tapeDir >= 2); // travel across X
        const bool reverseTravel = (s.tapeDir == 1 || s.tapeDir == 3);
        const float axis = horizontalTravel
          ? (float)x / std::max(1, w)
          : (float)y / std::max(1, h);
        float pos = std::fmod(timeSec * speed * 0.12f + 0.17f, 1.0f);
        if (reverseTravel) pos = 1.0f - pos;

        const float sizeNorm = clamp01(s.tapeSize);
        const float width = 0.0015f + sizeNorm * 0.095f;
        const float edgeJitter = (horizontalTravel
          ? (0.0045f * std::sin((float)y * 0.037f + timeSec * 5.7f))
          : (0.0045f * std::sin((float)x * 0.021f + timeSec * 4.3f)))
          + (rand01((uint32_t)(y * 4099u + frame * 131u + s.seed)) - 0.5f) * 0.004f;
        float dist = std::fabs(axis - pos - edgeJitter);
        if (tapeType == 2) {
          // Head-switching damage anchors to the selected edge, with Size defining
          // how far the damaged region reaches into the picture.
          const float edge = reverseTravel ? 1.0f : 0.0f;
          dist = std::fabs(axis - edge);
        }

        const uint32_t snowTick = (uint32_t)std::floor(std::max(0.0f, timeSec) * (0.5f + speed * 12.0f));
        const int snowCell = std::max(1, (int)std::round(1.0f + sizeNorm * 12.0f));
        const int snowX = x / snowCell, snowY = y / snowCell;
        const uint32_t snowHash = (uint32_t)s.seed ^ (uint32_t)(snowX * 73856093u)
          ^ (uint32_t)(snowY * 19349663u) ^ (snowTick * 83492791u);
        if (tapeType == 5) dist = rand01(snowHash);

        const float mask = (tapeType == 5)
          ? (rand01(snowHash + 17u) < (0.05f + 0.45f * sizeNorm) ? 1.0f : 0.0f)
          : 1.0f - smooth(width, width * 2.0f, dist);
        const float ts = clamp01(mask * effTapeStrength
          * (0.2f + 0.8f * clamp01(strength / 5.0f)));

        if (ts > 0.001f && (tapeType >= 1 && tapeType <= 4)) {
          int tx = sx, ty = sy;
          const float along = horizontalTravel ? (float)y : (float)x;
          const float irregular = std::sin(along * 0.071f + timeSec * (2.0f + speed * 10.0f)) * 0.55f
            + std::sin(along * 0.019f - timeSec * (1.0f + speed * 4.0f)) * 0.45f;
          int displacement = (int)std::round(irregular * ts * std::max(2, w / 24));
          if (tapeType == 1 || tapeType == 2) {
            tx = clampi(sx + displacement, 0, inW - 1);
          } else if (tapeType == 3) {
            // A real vertical-roll style moves the source vertically as the tape slips,
            // with a smaller local wobble layered over the frame displacement.
            const int roll = (int)std::round(std::sin(timeSec * (0.7f + speed * 3.2f)) * h * 0.025f);
            ty = clampi(sy + roll + (int)std::round(irregular * ts * std::max(1, h / 90)), 0, inH - 1);
          } else {
            // Side tear: abrupt, row-coherent horizontal displacement rather than per-pixel speckle.
            const uint32_t rowHash = (uint32_t)(y * 73u + s.seed + snowTick * 19u);
            const int tearDirection = rand01(rowHash) > 0.5f ? 1 : -1;
            displacement = (int)std::round(irregular * std::max(2, w / 16));
            tx = clampi(sx + displacement * tearDirection, 0, inW - 1);
          }

          const PF_Pixel tp = inBase[ty * inStride + tx];
          const PF_Pixel cpR = inBase[sy * inStride + clampi(tx + std::max(1, (int)std::round(1.0f + ts * 5.0f)), 0, inW - 1)];
          const PF_Pixel cpB = inBase[sy * inStride + clampi(tx - std::max(1, (int)std::round(1.0f + ts * 5.0f)), 0, inW - 1)];
          const float tr = tp.red / 255.0f, tg = tp.green / 255.0f, tb = tp.blue / 255.0f;
          r = r * (1.0f - ts) + tr * ts;
          g = g * (1.0f - ts) + tg * ts;
          b = b * (1.0f - ts) + tb * ts;

          // Tape instability combines luma loss, chroma separation, and irregular band-edge flutter.
          const float phase = std::sin(along * 0.31f + timeSec * (3.0f + speed * 25.0f)) * ts * 0.16f;
          const float flicker = 1.0f - ts * (0.06f + 0.12f * rand01(snowHash + 91u));
          r = clamp01(r * flicker + phase + ts * ((cpR.red - tp.red) / 255.0f) * 0.22f);
          g = clamp01(g * (1.0f - ts * 0.10f) - phase * 0.22f);
          b = clamp01(b * flicker - phase + ts * ((cpB.blue - tp.blue) / 255.0f) * 0.22f);
          if (tapeType == 2) { r *= 1.0f - ts * 0.30f; g *= 1.0f - ts * 0.42f; b *= 1.0f - ts * 0.24f; }
          if (tapeType == 4) { r = clamp01(r + ts * 0.08f + phase); g = clamp01(g - phase * 1.2f); b = clamp01(b - ts * 0.06f - phase); }
        } else if (tapeType == 5 && ts > 0.001f) {
          const float snow = (rand01(snowHash + 101u) - 0.5f) * ts * 0.8f;
          r = clamp01(r + snow); g = clamp01(g + snow); b = clamp01(b + snow);
        }
      }

      // Other VHS Engines
      if (otherStrength > 0.0001f) {
        const float xn = (float)x / std::max(1, w), yn = (float)y / std::max(1, h);
        const uint32_t oh = (uint32_t)s.seed ^ (uint32_t)(frame * 2246822519u) ^ (uint32_t)(x * 3266489917u) ^ (uint32_t)(y * 668265263u);
        if (craftEngineEnabled) { // VHS Craft signal engine
          const float trackPos = std::fmod(timeSec * 0.23f + 0.31f, 1.0f);
          const float track = 1.0f - smooth(0.008f, 0.025f, std::fabs(yn - trackPos));
          const float head = 1.0f - smooth(0.01f, 0.06f, std::fabs(yn - (1.0f - 0.025f)));
          const float n = rand01(oh) - 0.5f;
          const float t = otherStrength * (craftTracking * track + craftHead * head) * 0.08f;
          r = clamp01(r + t + n * craftSmear * otherStrength * 0.15f);
          g = clamp01(g - t * 0.25f + n * craftSmear * otherStrength * 0.15f);
          b = clamp01(b + t * 0.7f + craftChroma * otherStrength * 0.12f * n);
        }
        if (legendEngineEnabled) { // VHS Legend signal engine
          const float line = (rand01((uint32_t)(y / 3 + frame * 41 + s.seed)) - 0.5f) * 2.0f;
          const bool tear = rand01((uint32_t)(y / 7 + x / 29 + frame * 17 + s.seed)) < legendTear * otherStrength * 0.08f;
          const bool loss = rand01((uint32_t)(x / 12 + y / 8 + frame * 73 + s.seed)) < legendLoss * otherStrength * 0.08f;
          r = clamp01(r + line * legendBreak * otherStrength * 0.35f);
          g = clamp01(g + line * legendBreak * otherStrength * 0.18f);
          b = clamp01(b - line * legendBreak * otherStrength * 0.28f);
          if (tear) { r = clamp01(r + 0.18f * otherStrength); g *= 0.55f; b = clamp01(b + 0.25f * otherStrength); }
          if (loss) { r *= 0.15f; g *= 0.15f; b *= 0.15f; }
          if (rand01(oh) < legendBlocks * otherStrength * 0.06f) { r = rand01(oh + 11); g = rand01(oh + 23); b = rand01(oh + 37); }
        }
        if (signalReferenceEnabled) { // Signal Reference engine
          // ReadSettings normalizes each 0..100 slider to 0..1. Keep the
          // three controls independent so luma, chroma, and tape errors can be
          // dialed separately without an accidental second percent conversion.
          const float signalDrive = clamp01(signalStrength) * clamp01(otherStrength);
          const float lumaDrive = clamp01(signalLuma) * signalDrive;
          const float chromaDrive = clamp01(signalChroma) * signalDrive;
          const float errorDrive = clamp01(signalErrors) * signalDrive;
          const float l = (rand01(oh + 3) - 0.5f) * lumaDrive * 0.12f;
          const float c = (rand01(oh + 7) - 0.5f) * chromaDrive * 0.18f;
          // Tape errors are horizontal, segmented signal dropouts rather than
          // square pixel blocks. Keep the mask deterministic for a given frame
          // and seed, with error probability driven only by Signal Tape Errors.
          const uint32_t errorHash = (uint32_t)((x / 18) * 73856093u ^ (y / 2) * 19349663u ^ (frame * 29) ^ (s.seed * 83492791u));
          const bool error = rand01(errorHash) < errorDrive * 0.08f;
          r = clamp01(r + l + c); g = clamp01(g + l - c * 0.25f); b = clamp01(b + l - c);
          if (error) {
            const float loss = 0.55f + 0.35f * rand01(errorHash + 17u);
            r *= 1.0f - loss; g *= 1.0f - loss; b *= 1.0f - loss;
            // Brief chroma displacement accentuates a tape/sync error without
            // replacing the image with arbitrary RGB blocks.
            r = clamp01(r + chromaDrive * 0.035f);
            b = clamp01(b - chromaDrive * 0.025f);
          }
        }
      }

      // Procedural Transparent Tape
      if (transparentTapeEnabled && transparentTapeStrength > 0.001f) {
        const float yn = (float)y / (float)std::max(1, h - 1);
        const float travel = std::fmod(timeSec * (0.08f + transparentTapeSpeed * 0.006f), 1.0f);
        const float wobble = 0.012f * std::sin((float)x * 0.021f + timeSec * 4.7f) + 0.006f * std::sin((float)x * 0.067f - timeSec * 2.9f);
        const float center = travel + wobble;
        const float halfWidth = 0.002f + transparentTapeWidth * 0.045f;
        const float edge = 1.0f - smooth(halfWidth, halfWidth * 1.9f, std::fabs(yn - center));
        const float broken = 0.55f + 0.45f * rand01((uint32_t)(x / 11 + y / 3 + (int)(timeSec * 17.0f) + s.seed));
        const float a = clamp01(edge * broken * transparentTapeStrength * 0.72f);
        if (a > 0.001f) {
          const float l = 0.2126f * r + 0.7152f * g + 0.0722f * b;
          const float chromaPhase = std::sin((float)x * 0.047f + timeSec * 12.0f) * a * 0.16f;
          r = clamp01(r * (1.0f - a * 0.28f) + l * a * 0.28f + chromaPhase);
          g = clamp01(g * (1.0f - a * 0.20f) + l * a * 0.20f - chromaPhase * 0.35f);
          b = clamp01(b * (1.0f - a * 0.32f) + l * a * 0.32f + chromaPhase * 0.65f);
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
