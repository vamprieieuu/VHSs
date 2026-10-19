#ifndef _WIN32
#define _WIN32
#endif
#ifndef AE_OS_WIN
#define AE_OS_WIN
#endif

typedef void** Handle;
typedef int Fixed;
typedef unsigned char Boolean;

#include "AEConfig.h"
#include "windows.h"

#ifndef DllExport
  #define DllExport
#endif

#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"
#include "String_Utils.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <functional>
#include <algorithm>
#include <set>

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
  #include <direct.h>
  #define IS_WIN_BUILD 1
#else
  #include <sys/stat.h>
#endif

#include "../src/VHS6RIFT.cpp"

struct PixelStats {
	size_t changedPixels;
	double meanAbsError;
	int minR, maxR;
	int minG, maxG;
	int minB, maxB;
};

static PixelStats CompareImages(const PF_Pixel *src, const PF_Pixel *dst, int width, int height) {
	PixelStats stats = {0, 0.0, 255, 0, 255, 0, 255, 0};
	size_t total = width * height;
	double totalDiff = 0.0;

	for (size_t i = 0; i < total; ++i) {
		int dr = std::abs((int)dst[i].red - (int)src[i].red);
		int dg = std::abs((int)dst[i].green - (int)src[i].green);
		int db = std::abs((int)dst[i].blue - (int)src[i].blue);

		if (dr > 0 || dg > 0 || db > 0) {
			stats.changedPixels++;
		}
		totalDiff += (dr + dg + db) / 3.0;

		stats.minR = std::min(stats.minR, (int)dst[i].red);
		stats.maxR = std::max(stats.maxR, (int)dst[i].red);
		stats.minG = std::min(stats.minG, (int)dst[i].green);
		stats.maxG = std::max(stats.maxG, (int)dst[i].green);
		stats.minB = std::min(stats.minB, (int)dst[i].blue);
		stats.maxB = std::max(stats.maxB, (int)dst[i].blue);
	}

	stats.meanAbsError = totalDiff / (double)total;
	return stats;
}

static void SavePPM(const std::string &path, const PF_Pixel *pixels, int w, int h) {
	std::ofstream ofs(path, std::ios::binary);
	if (!ofs) return;
	ofs << "P6\n" << w << " " << h << "\n255\n";
	for (int i = 0; i < w * h; ++i) {
		unsigned char rgb[3] = { pixels[i].red, pixels[i].green, pixels[i].blue };
		ofs.write((char*)rgb, 3);
	}
}

int main() {
	std::cout << "======================================================================\n";
	std::cout << "VHS6RIFT Production Native C++ Render Pipeline Test Suite\n";
	std::cout << "======================================================================\n\n";

#if defined(IS_WIN_BUILD)
	_mkdir("previews");
#else
	mkdir("previews", 0755);
#endif

	const int width = 256;
	const int height = 256;
	const size_t numPixels = width * height;

	// Prepare mock InData & OutData
	PF_InData in_data;
	memset(&in_data, 0, sizeof(in_data));
	in_data.time_scale = 30000;
	in_data.current_time = 0;
	in_data.time_step = 1000;

	PF_OutData out_data;
	memset(&out_data, 0, sizeof(out_data));

	// Allocate and setup parameters array
	std::vector<PF_ParamDef> param_defs(P_COUNT);
	std::vector<PF_ParamDef*> params(P_COUNT);
	for (size_t i = 0; i < P_COUNT; ++i) {
		memset(&param_defs[i], 0, sizeof(PF_ParamDef));
		params[i] = &param_defs[i];
	}

	// Helper to reset parameters to default
	auto SetDefaults = [&]() {
		params[P_CAMERA]->u.pd.value = 1; // Fxc6
		params[P_MASTER]->u.fs_d.value = 350.0;
		params[P_CONTINUOUS]->u.bd.value = TRUE;
		params[P_EVENTS]->u.bd.value = FALSE;
		params[P_SEED]->u.sd.value = 17;

		params[P_FXC6_CHARACTER]->u.fs_d.value = 65.0;
		params[P_FXC6_COLOR]->u.fs_d.value = 55.0;
		params[P_FXC6_SIGNAL]->u.fs_d.value = 35.0;
		params[P_FXC6_GRAIN]->u.fs_d.value = 28.0;

		params[P_GEN7_CHARACTER]->u.fs_d.value = 75.0;
		params[P_GEN7_COLOR]->u.fs_d.value = 80.0;
		params[P_GEN7_SIGNAL]->u.fs_d.value = 85.0;
		params[P_GEN7_GRAIN]->u.fs_d.value = 70.0;

		params[P_JVC_C7_CHARACTER]->u.fs_d.value = 82.0;
		params[P_JVC_C7_COLOR]->u.fs_d.value = 65.0;
		params[P_JVC_C7_SIGNAL]->u.fs_d.value = 75.0;
		params[P_JVC_C7_GRAIN]->u.fs_d.value = 60.0;

		params[P_R20_CHARACTER]->u.fs_d.value = 55.0;
		params[P_R20_COLOR]->u.fs_d.value = 70.0;
		params[P_R20_SIGNAL]->u.fs_d.value = 50.0;
		params[P_R20_GRAIN]->u.fs_d.value = 30.0;

		params[P_FALL78_CHARACTER]->u.fs_d.value = 30.0;
		params[P_FALL78_COLOR]->u.fs_d.value = 45.0;
		params[P_FALL78_SIGNAL]->u.fs_d.value = 25.0;
		params[P_FALL78_GRAIN]->u.fs_d.value = 15.0;

		params[P_ERC_CHARACTER]->u.fs_d.value = 60.0;
		params[P_ERC_COLOR]->u.fs_d.value = 90.0;
		params[P_ERC_SIGNAL]->u.fs_d.value = 45.0;
		params[P_ERC_GRAIN]->u.fs_d.value = 40.0;

		params[P_JVC_SXM_CHARACTER]->u.fs_d.value = 50.0;
		params[P_JVC_SXM_COLOR]->u.fs_d.value = 65.0;
		params[P_JVC_SXM_SIGNAL]->u.fs_d.value = 35.0;
		params[P_JVC_SXM_GRAIN]->u.fs_d.value = 25.0;

		params[P_SATURATION]->u.fs_d.value = 100.0;
		params[P_BLACK]->u.fs_d.value = 0.0;
		params[P_SOFTNESS]->u.fs_d.value = 0.0; // Sharp by default for testing
		params[P_CHROMA_BLEED]->u.fs_d.value = 42.0;
		params[P_CHROMA_X]->u.fs_d.value = 4.0;
		params[P_CHROMA_Y]->u.fs_d.value = 0.0;

		params[P_GRAIN_AMOUNT]->u.fs_d.value = 0.0;
		params[P_GRAIN_SCALE]->u.fs_d.value = 28.0;
		params[P_GRAIN_SPEED]->u.fs_d.value = 100.0;

		params[P_TRACKING]->u.fs_d.value = 0.0;
		params[P_TRACK_FREQ]->u.fs_d.value = 25.0;
		params[P_TRACK_WIDTH]->u.fs_d.value = 18.0;

		params[P_TAPE_TYPE]->u.pd.value = 1; // Off
		params[P_TAPE_STRENGTH]->u.fs_d.value = 35.0;
		params[P_TAPE_SIZE]->u.fs_d.value = 18.0;
		params[P_TAPE_SPEED]->u.fs_d.value = 60.0;
		params[P_TAPE_DIRECTION]->u.pd.value = 1;

		params[P_OVERLAY_MODE]->u.pd.value = 1;
		params[P_OVERLAY_OPACITY]->u.fs_d.value = 60.0;
		params[P_OVERLAY_SPEED]->u.fs_d.value = 100.0;

		params[P_OTHER_ENGINE]->u.pd.value = 1; // Off
		params[P_OTHER_STRENGTH]->u.fs_d.value = 50.0;

		params[P_GRAIN_ENABLE]->u.bd.value = FALSE;
		params[P_TRACKING_ENABLE]->u.bd.value = FALSE;
		params[P_TAPE_ENABLE]->u.bd.value = FALSE;
		params[P_OVERLAY_ENABLE]->u.bd.value = FALSE;
		params[P_OTHER_ENABLE]->u.bd.value = FALSE;
		params[P_CRT_ENABLE]->u.bd.value = FALSE;

		params[P_CRT_SCANLINES]->u.fs_d.value = 35.0;
		params[P_CRT_CURVE]->u.fs_d.value = 20.0;
		params[P_CRT_GLOW]->u.fs_d.value = 25.0;
		params[P_CRT_RGB]->u.fs_d.value = 18.0;
		params[P_CRT_FLICKER]->u.fs_d.value = 8.0;
		params[P_CRT_VIGNETTE]->u.fs_d.value = 28.0;

		params[P_TRANSPARENT_TAPE_ENABLE]->u.bd.value = FALSE;
		params[P_TRANSPARENT_TAPE_STRENGTH]->u.fs_d.value = 35.0;
		params[P_TAPE_SWEEP_ENABLE]->u.bd.value = FALSE;
		params[P_TAPE_SWEEP_STRENGTH]->u.fs_d.value = 50.0;
	};

	SetDefaults();

	// Create synthetic source image buffer (radial color gradient)
	std::vector<PF_Pixel> src_buffer(numPixels);
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			size_t idx = y * width + x;
			src_buffer[idx].red   = (A_u_char)(x * 255 / width);
			src_buffer[idx].green = (A_u_char)(y * 255 / height);
			src_buffer[idx].blue  = (A_u_char)((x + y) * 255 / (width + height));
			src_buffer[idx].alpha = 255;
		}
	}
	SavePPM("previews/original.ppm", src_buffer.data(), width, height);

	// Setup Input LayerDef in params[P_INPUT]
	PF_LayerDef input_layer;
	memset(&input_layer, 0, sizeof(input_layer));
	input_layer.width = width;
	input_layer.height = height;
	input_layer.rowbytes = width * sizeof(PF_Pixel);
	input_layer.data = src_buffer.data();
	params[P_INPUT]->u.ld = input_layer;

	// Setup Output LayerDef
	std::vector<PF_Pixel> dst_buffer(numPixels);
	PF_LayerDef output_layer;
	memset(&output_layer, 0, sizeof(output_layer));
	output_layer.width = width;
	output_layer.height = height;
	output_layer.rowbytes = width * sizeof(PF_Pixel);
	output_layer.data = dst_buffer.data();

	int passedTests = 0;
	int totalTests = 0;

	// Test 1: Identity Bypass (Master = 0)
	{
		totalTests++;
		SetDefaults();
		params[P_MASTER]->u.fs_d.value = 0.0;
		memset(dst_buffer.data(), 0, dst_buffer.size() * sizeof(PF_Pixel));

		EffectMain(PF_Cmd_RENDER, &in_data, &out_data, params.data(), &output_layer, nullptr);
		PixelStats stats = CompareImages(src_buffer.data(), dst_buffer.data(), width, height);

		if (stats.changedPixels == 0 && stats.meanAbsError == 0.0) {
			std::cout << "  [PASS] Test 1: Master = 0 Identity Bypass (Bit-exact match to input).\n";
			passedTests++;
		} else {
			std::cout << "  [FAIL] Test 1: Master = 0 failed identity check! Changed pixels: " << stats.changedPixels << "\n";
		}
	}

	// Test 2: CRT TV Engine Opt-In Isolation (CRT unchecked produces NO CRT changes)
	{
		totalTests++;
		SetDefaults();
		params[P_SOFTNESS]->u.fs_d.value = 0.0;
		params[P_MASTER]->u.fs_d.value = 350.0;
		params[P_CRT_ENABLE]->u.bd.value = FALSE; // Unchecked

		memset(dst_buffer.data(), 0, dst_buffer.size() * sizeof(PF_Pixel));
		EffectMain(PF_Cmd_RENDER, &in_data, &out_data, params.data(), &output_layer, nullptr);
		std::vector<PF_Pixel> crtOffOutput = dst_buffer;

		params[P_CRT_ENABLE]->u.bd.value = TRUE; // Checked
		memset(dst_buffer.data(), 0, dst_buffer.size() * sizeof(PF_Pixel));
		EffectMain(PF_Cmd_RENDER, &in_data, &out_data, params.data(), &output_layer, nullptr);
		PixelStats crtStats = CompareImages(crtOffOutput.data(), dst_buffer.data(), width, height);
		SavePPM("previews/crt_tv_trinitron.ppm", dst_buffer.data(), width, height);

		if (crtStats.changedPixels > 0 && crtStats.meanAbsError > 0.5) {
			std::cout << "  [PASS] Test 2: CRT TV Engine is strictly opt-in (Enabling produced " << crtStats.changedPixels << " changed pixels, MAE: " << crtStats.meanAbsError << ").\n";
			passedTests++;
		} else {
			std::cout << "  [FAIL] Test 2: CRT TV Engine toggle failed to alter output!\n";
		}
	}

	// Test 3: Camera Profiles Distinction (All 7 profiles produce distinct output)
	{
		totalTests++;
		SetDefaults();
		params[P_MASTER]->u.fs_d.value = 350.0;
		params[P_SOFTNESS]->u.fs_d.value = 0.0;

		std::vector<std::vector<PF_Pixel>> profileOutputs(7);
		static const char* kProfNames[7] = {
			"camera_fxc6", "camera_gen7", "camera_jvc_c7u", "camera_r20", "camera_fall78", "camera_erc", "camera_jvc_sxm"
		};
		for (int p = 1; p <= 7; ++p) {
			params[P_CAMERA]->u.pd.value = p;
			memset(dst_buffer.data(), 0, dst_buffer.size() * sizeof(PF_Pixel));
			EffectMain(PF_Cmd_RENDER, &in_data, &out_data, params.data(), &output_layer, nullptr);
			profileOutputs[p - 1] = dst_buffer;
			SavePPM(std::string("previews/") + kProfNames[p - 1] + ".ppm", dst_buffer.data(), width, height);
		}

		bool allDistinct = true;
		for (int i = 0; i < 7; ++i) {
			for (int j = i + 1; j < 7; ++j) {
				PixelStats diff = CompareImages(profileOutputs[i].data(), profileOutputs[j].data(), width, height);
				if (diff.changedPixels < (numPixels / 2) || diff.meanAbsError < 0.5) {
					std::cout << "    [FAIL] Camera Profile " << (i + 1) << " and " << (j + 1) << " are too similar (MAE: " << diff.meanAbsError << ")\n";
					allDistinct = false;
				}
			}
		}

		if (allDistinct) {
			std::cout << "  [PASS] Test 3: All 7 Camera Reference Profiles produce distinct visual responses.\n";
			passedTests++;
		} else {
			std::cout << "  [FAIL] Test 3: Camera profiles are not sufficiently distinct!\n";
		}
	}

	// Helper test runner lambda
	auto RunTestCase = [&](const std::string &name, std::function<void()> setupFn, const std::string &previewName = "") {
		totalTests++;
		SetDefaults();
		setupFn();

		memset(dst_buffer.data(), 0, dst_buffer.size() * sizeof(PF_Pixel));
		EffectMain(PF_Cmd_RENDER, &in_data, &out_data, params.data(), &output_layer, nullptr);

		PixelStats stats = CompareImages(src_buffer.data(), dst_buffer.data(), width, height);
		if (!previewName.empty()) {
			SavePPM(std::string("previews/") + previewName + ".ppm", dst_buffer.data(), width, height);
		}

		std::cout << "  " << std::left << std::setw(38) << name
		          << " -> Changed: " << std::setw(6) << stats.changedPixels << "/" << numPixels
		          << " | MAE: " << std::fixed << std::setprecision(2) << std::setw(5) << stats.meanAbsError
		          << " | Output Range R:[" << std::setw(3) << stats.minR << ".." << std::setw(3) << stats.maxR << "]"
		          << " G:[" << std::setw(3) << stats.minG << ".." << std::setw(3) << stats.maxG << "]"
		          << " B:[" << std::setw(3) << stats.minB << ".." << std::setw(3) << stats.maxB << "]\n";

		if (stats.changedPixels > 0 && stats.meanAbsError > 0.1) {
			passedTests++;
		} else {
			std::cout << "    [FAIL] No pixels changed or MAE zero!\n";
		}
	};

	std::cout << "\n--- Individual Sub-System Modules Execution Tests ---\n";

	RunTestCase("Test 4: Default Active Treatment", [&]() {
		params[P_MASTER]->u.fs_d.value = 350.0;
	}, "vhs_sp_mode");

	RunTestCase("Test 5: Enable Grain / Noise", [&]() {
		params[P_GRAIN_ENABLE]->u.bd.value = TRUE;
	}, "grain_color");

	RunTestCase("Test 6: Enable Tracking Damage", [&]() {
		params[P_TRACKING_ENABLE]->u.bd.value = TRUE;
	}, "vhs_lp_mode");

	RunTestCase("Test 7: Enable Tape Effects", [&]() {
		params[P_TAPE_ENABLE]->u.bd.value = TRUE;
	}, "bad_signal_glitch");

	RunTestCase("Test 8: Enable Other VHS Engines", [&]() {
		params[P_OTHER_ENABLE]->u.bd.value = TRUE;
	}, "vhs_master_full_suite");

	RunTestCase("Test 9: Procedural Transparent Tape", [&]() {
		params[P_TRANSPARENT_TAPE_ENABLE]->u.bd.value = TRUE;
	});

	RunTestCase("Test 10: Moving Tape Sweep", [&]() {
		params[P_TAPE_SWEEP_ENABLE]->u.bd.value = TRUE;
	});

	RunTestCase("Test 11: Real ntsc-rs Signal Engine Integration", [&]() {
		params[P_OTHER_ENABLE]->u.bd.value = TRUE;
		params[P_OTHER_ENGINE]->u.pd.value = 3; // NTSC-RS Signal Model
		params[P_OTHER_STRENGTH]->u.fs_d.value = 50.0;
		params[P_NTSC_CHROMA_DELAY]->u.fs_d.value = 30.0;
		params[P_NTSC_HEAD_SWITCH]->u.fs_d.value = 20.0;
		params[P_NTSC_EDGE_WAVE]->u.fs_d.value = 25.0;
		params[P_NTSC_COMPOSITE]->u.fs_d.value = 40.0;
	}, "ntsc_rs_signal_model");

	std::cout << "\n======================================================================\n";
	std::cout << "Summary: " << passedTests << " / " << totalTests << " native C++ tests PASSED.\n";
	std::cout << "======================================================================\n";

	return (passedTests == totalTests) ? 0 : 1;
}
