#ifndef _WIN32
#define _WIN32
#endif
#ifndef AE_OS_WIN
#define AE_OS_WIN
#endif
#ifndef DllExport
#define DllExport
#endif
#include "AEConfig.h"
#include "windows.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"
#include "String_Utils.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>
#include "../src/VHS6RIFT.cpp"

static size_t changed(const std::vector<PF_Pixel>& a, const std::vector<PF_Pixel>& b) {
  size_t n = 0;
  for (size_t i = 0; i < a.size(); ++i)
    if (a[i].red != b[i].red || a[i].green != b[i].green || a[i].blue != b[i].blue) ++n;
  return n;
}

int main() {
  constexpr int W = 192, H = 128;
  const size_t N = (size_t)W * H;
  std::vector<PF_Pixel> src(N), dst(N), baseline(N);
  for (int y = 0; y < H; ++y) for (int x = 0; x < W; ++x) {
    auto &p = src[(size_t)y * W + x];
    p.red = (A_u_char)(x * 255 / W); p.green = (A_u_char)(y * 255 / H);
    p.blue = (A_u_char)((x + y) * 255 / (W + H)); p.alpha = 255;
  }
  std::vector<PF_ParamDef> defs(P_COUNT);
  std::vector<PF_ParamDef*> params(P_COUNT);
  for (int i = 0; i < P_COUNT; ++i) { std::memset(&defs[i], 0, sizeof(PF_ParamDef)); params[i] = &defs[i]; }
  PF_LayerDef input{}; input.width = W; input.height = H; input.rowbytes = W * sizeof(PF_Pixel); input.data = src.data();
  PF_LayerDef output{}; output.width = W; output.height = H; output.rowbytes = W * sizeof(PF_Pixel); output.data = dst.data();
  params[P_INPUT]->u.ld = input;
  PF_InData in{}; in.time_scale = 30000; in.time_step = 1000; in.current_time = 0;

  auto defaults = [&]() {
    for (int i = 0; i < P_COUNT; ++i) std::memset(&defs[i], 0, sizeof(PF_ParamDef));
    params[P_INPUT]->u.ld = input;
    params[P_MAIN_ENABLE]->u.bd.value = TRUE;
    params[P_MASTER]->u.fs_d.value = 350.0;
    params[P_CONTINUOUS]->u.bd.value = TRUE;
    params[P_SEED]->u.sd.value = 17;
    params[P_SATURATION]->u.fs_d.value = 100.0;
    params[P_SOFTNESS]->u.fs_d.value = 12.0;
    params[P_CHROMA_BLEED]->u.fs_d.value = 40.0;
    params[P_GRAIN_AMOUNT]->u.fs_d.value = 0.0;
    params[P_GRAIN_LINES_ENABLE]->u.bd.value = FALSE;
    params[P_GRAIN_SCALE]->u.fs_d.value = 28.0;
    params[P_GRAIN_SPEED]->u.fs_d.value = 100.0;
    params[P_NTSC_ENABLE]->u.bd.value = TRUE;
    params[P_NTSC_CHROMA_DELAY]->u.fs_d.value = 18.0;
    params[P_NTSC_HEAD_SWITCH]->u.fs_d.value = 0.0;
    params[P_NTSC_EDGE_WAVE]->u.fs_d.value = 0.0;
    params[P_GRAIN_PRESET]->u.pd.value = 2;
    params[P_ADV_GRAIN_SOFTNESS]->u.fs_d.value = 35.0;
    params[P_ADV_GRAIN_MODE]->u.pd.value = 1;
    params[P_ADV_GRAIN_COLOR]->u.pd.value = 1;
    params[P_OTHER_STRENGTH]->u.fs_d.value = 65.0;
    params[P_OTHER_ENABLE]->u.bd.value = TRUE;
    params[P_CAMERA]->u.pd.value = 1;
  };
  auto render = [&]() { std::fill(dst.begin(), dst.end(), PF_Pixel{}); output.data = dst.data(); return Render(&in, params.data(), &output); };

  defaults(); params[P_MASTER]->u.fs_d.value = 0.0; render();
  if (changed(src, dst) != 0) { std::cerr << "FAIL: Master=0 must be identity.\n"; return 1; }
  std::cout << "PASS: Master=0 identity bypass.\n";

  defaults(); params[P_MAIN_ENABLE]->u.bd.value = FALSE; render();
  if (changed(src, dst) != 0) { std::cerr << "FAIL: Main VHS disabled must be identity.\n"; return 2; }
  std::cout << "PASS: Main VHS disabled identity bypass.\n";

  defaults(); in.current_time = 1000; render();
  const size_t baseChanged = changed(src, dst); baseline = dst;
  if (baseChanged < N / 3) { std::cerr << "FAIL: default VHS/NTSC signal treatment is too weak. changed=" << baseChanged << "\n"; return 3; }
  std::cout << "PASS: default VHS/NTSC signal path changes " << baseChanged << " pixels.\n";

  defaults(); in.current_time = 1000; params[P_TRACKING_ENABLE]->u.bd.value = TRUE; params[P_TRACKING]->u.fs_d.value = 100.0; params[P_TRACK_WIDTH]->u.fs_d.value = 100.0; render();
  if (changed(baseline, dst) < N / 20) { std::cerr << "FAIL: tracking damage did not materially affect the frame.\n"; return 4; }
  std::cout << "PASS: wide tracking damage materially affects the frame.\n";

  defaults(); params[P_MASTER]->u.fs_d.value = 350.0; params[P_GRAIN_AMOUNT]->u.fs_d.value = 100.0; in.current_time = 1000; render(); const auto frame1 = dst;
  in.current_time = 2000; render();
  if (changed(frame1, dst) < N / 20) { std::cerr << "FAIL: moving grain did not vary across frames.\n"; return 5; }
  std::cout << "PASS: temporal grain varies between frames.\n";

  defaults(); in.current_time = 1000; params[P_GRAIN_LINES_ENABLE]->u.bd.value = TRUE;
  params[P_GRAIN_LINES_AMOUNT]->u.fs_d.value = 100.0; params[P_GRAIN_LINES_SIZE]->u.fs_d.value = 50.0; params[P_GRAIN_LINES_SPEED]->u.fs_d.value = 3.0; render();
  if (changed(baseline, dst) < N / 100) { std::cerr << "FAIL: native NTSC-RS composite signal noise is not visible.\n"; return 6; }
  std::cout << "PASS: native NTSC-RS composite signal noise changes the image.\n";

  defaults(); in.current_time = 1000; params[P_CAMERA]->u.pd.value = 1; render(); const auto camera1 = dst;
  params[P_CAMERA]->u.pd.value = 5; render();
  if (changed(camera1, dst) < N / 200) { std::cerr << "FAIL: camera engines are not visibly distinct.\n"; return 7; }
  std::cout << "PASS: selected camera engines have distinct signal responses.\n";
  std::cout << "7 / 7 native C++ tests PASSED.\n";
  return 0;
}
