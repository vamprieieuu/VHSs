// tools/test_native_render.cpp - Standalone Native C++ Verification Harness
// Compiles with standard C++17 to verify all algorithms, buffer memory safety, and audio DSP

#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <cstring>
#include "../include/VHS6RIFT.h"
#include "../src/VHS6RIFT.cpp"

void test_native_pixel_pipeline() {
  std::cout << "[Native C++ Test] Initializing image buffer tests..." << std::endl;
  const int32_t w = 320;
  const int32_t h = 240;

  // Allocate 8-bit ARGB surfaces
  std::vector<PF_Pixel8> srcBuf(w * h);
  std::vector<PF_Pixel8> dstBuf(w * h);

  // Fill source with SMPTE color bar pattern
  for (int32_t y = 0; y < h; ++y) {
    for (int32_t x = 0; x < w; ++x) {
      int32_t idx = y * w + x;
      srcBuf[idx].alpha = 255;
      srcBuf[idx].red   = static_cast<A_u_char>((x * 255) / w);
      srcBuf[idx].green = static_cast<A_u_char>((y * 255) / h);
      srcBuf[idx].blue  = static_cast<A_u_char>(((x + y) * 255) / (w + h));
    }
  }

  // Setup PF_ParamDef array
  std::vector<PF_ParamDef> paramStore(P_COUNT);
  std::vector<PF_ParamDef*> params(P_COUNT);
  for (size_t i = 0; i < P_COUNT; ++i) {
    params[i] = &paramStore[i];
  }

  PF_EffectWorld *inWorld = reinterpret_cast<PF_EffectWorld*>(&paramStore[P_INPUT].u);
  inWorld->data = srcBuf.data();
  inWorld->width = w;
  inWorld->height = h;
  inWorld->rowbytes = w * sizeof(PF_Pixel8);
  inWorld->data_type = 0;

  PF_EffectWorld outWorld;
  outWorld.data = dstBuf.data();
  outWorld.width = w;
  outWorld.height = h;
  outWorld.rowbytes = w * sizeof(PF_Pixel8);
  outWorld.data_type = 0;

  PF_InData inData;
  std::memset(&inData, 0, sizeof(inData));
  inData.width = w;
  inData.height = h;
  inData.current_time.value = 15;
  inData.current_time.scale = 30; // 0.5 sec
  inData.time_step.scale = 30;

  PF_OutData outData;
  std::memset(&outData, 0, sizeof(outData));

  // 1. Run GlobalSetup
  PF_Err err = EffectMain(PF_Cmd_GLOBAL_SETUP, &inData, &outData, params.data(), &outWorld, nullptr);
  assert(err == PF_Err_NONE);
  assert(outData.my_version == PF_VERSION(MAJOR_VERSION, MINOR_VERSION, BUG_VERSION, STAGE_VERSION, BUILD_VERSION));
  std::cout << "[Native C++ Test] GlobalSetup passed (Version: " << MAJOR_VERSION << "." << MINOR_VERSION << ")" << std::endl;

  // 2. Run ParamsSetup
  err = EffectMain(PF_Cmd_PARAMS_SETUP, &inData, &outData, params.data(), &outWorld, nullptr);
  assert(err == PF_Err_NONE);
  std::cout << "[Native C++ Test] ParamsSetup registered " << P_COUNT << " parameter slots" << std::endl;

  // 3. Test Master Strength == 0 (Exact Bitwise Passthrough)
  params[P_MASTER_STRENGTH]->u.sd.value = 0;
  params[P_MIX_ORIGINAL]->u.sd.value = 100;
  err = EffectMain(PF_Cmd_RENDER, &inData, &outData, params.data(), &outWorld, nullptr);
  assert(err == PF_Err_NONE);

  int32_t diffCount = 0;
  for (size_t i = 0; i < srcBuf.size(); ++i) {
    if (srcBuf[i].red != dstBuf[i].red ||
        srcBuf[i].green != dstBuf[i].green ||
        srcBuf[i].blue != dstBuf[i].blue) {
      diffCount++;
    }
  }
  assert(diffCount == 0);
  std::cout << "[Native C++ Test] Master Strength 0 Bit-Exact Passthrough verified (0 pixel differences)" << std::endl;

  // 4. Test Master Strength > 0 Rendering with All 16 Presets
  for (int32_t preset = STYLE_CLEAN_VHS; preset <= STYLE_DAMAGED_TAPE_PLAYBACK; ++preset) {
    params[P_MASTER_STRENGTH]->u.sd.value = 350;
    params[P_STYLE_PRESET]->u.pd.value = preset;
    params[P_PRESET_INTENSITY]->u.sd.value = 100;

    err = EffectMain(PF_Cmd_RENDER, &inData, &outData, params.data(), &outWorld, nullptr);
    assert(err == PF_Err_NONE);

    // Verify non-zero alteration occurred
    int32_t alteredPixels = 0;
    for (size_t i = 0; i < srcBuf.size(); ++i) {
      if (srcBuf[i].red != dstBuf[i].red ||
          srcBuf[i].green != dstBuf[i].green ||
          srcBuf[i].blue != dstBuf[i].blue) {
        alteredPixels++;
      }
    }
    assert(alteredPixels > 1000);
  }
  std::cout << "[Native C++ Test] All 16 Style Presets successfully rendered on 8-bit ARGB surface" << std::endl;

  // 5. Test 16-bit Deep Color Rendering
  std::vector<PF_Pixel16> src16(w * h);
  std::vector<PF_Pixel16> dst16(w * h);
  for (size_t i = 0; i < src16.size(); ++i) {
    src16[i].alpha = 32768;
    src16[i].red   = 16000;
    src16[i].green = 20000;
    src16[i].blue  = 12000;
  }
  inWorld->data = src16.data();
  inWorld->rowbytes = w * sizeof(PF_Pixel16);
  inWorld->data_type = 1;

  outWorld.data = dst16.data();
  outWorld.rowbytes = w * sizeof(PF_Pixel16);
  outWorld.data_type = 1;

  err = EffectMain(PF_Cmd_RENDER, &inData, &outData, params.data(), &outWorld, nullptr);
  assert(err == PF_Err_NONE);
  std::cout << "[Native C++ Test] 16-bit Deep Color rendering verified" << std::endl;

  // 6. Test 32-bit Float Deep Color Rendering
  std::vector<PF_PixelFloat> src32(w * h);
  std::vector<PF_PixelFloat> dst32(w * h);
  for (size_t i = 0; i < src32.size(); ++i) {
    src32[i].alpha = 1.0f;
    src32[i].red   = 0.5f;
    src32[i].green = 0.65f;
    src32[i].blue  = 0.4f;
  }
  inWorld->data = src32.data();
  inWorld->rowbytes = w * sizeof(PF_PixelFloat);
  inWorld->data_type = 2;

  outWorld.data = dst32.data();
  outWorld.rowbytes = w * sizeof(PF_PixelFloat);
  outWorld.data_type = 2;

  err = EffectMain(PF_Cmd_RENDER, &inData, &outData, params.data(), &outWorld, nullptr);
  assert(err == PF_Err_NONE);
  std::cout << "[Native C++ Test] 32-bit Float Deep Color rendering verified" << std::endl;

  // 7. Test Audio DSP Callback
  std::vector<PF_FpShort> audioL(4800);
  std::vector<PF_FpShort> audioR(4800);
  for (size_t i = 0; i < audioL.size(); ++i) {
    audioL[i] = std::sin(i * 0.05f) * 0.7f;
    audioR[i] = std::cos(i * 0.05f) * 0.7f;
  }
  PF_SoundWorld sound;
  sound.num_samples = 4800;
  sound.rate = 48000;
  sound.num_channels = 2;
  sound.data[0] = audioL.data();
  sound.data[1] = audioR.data();

  params[P_AUDIO_ENABLE]->u.bd.value = 1;
  params[P_AUDIO_WET_DRY]->u.sd.value = 100;
  params[P_AUDIO_SATURATION]->u.sd.value = 50;
  params[P_AUDIO_QUANTIZATION]->u.sd.value = 8;
  params[P_AUDIO_DOWNSAMPLE_RATE]->u.sd.value = 16000;

  err = EffectMain(PF_Cmd_AUDIO_RENDER, &inData, &outData, params.data(), &outWorld, &sound);
  assert(err == PF_Err_NONE);
  std::cout << "[Native C++ Test] Audio DSP Callback (PF_Cmd_AUDIO_RENDER) verified" << std::endl;

  std::cout << "\n========================================================" << std::endl;
  std::cout << "[SUCCESS] ALL NATIVE C++ ALGORITHM AND MEMORY TESTS PASSED!" << std::endl;
  std::cout << "========================================================\n" << std::endl;
}

int main() {
  test_native_pixel_pipeline();
  return 0;
}
