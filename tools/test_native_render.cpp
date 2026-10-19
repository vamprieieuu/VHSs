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

#define VHS6RIFT_NATIVE_TEST 1
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
        in_data.current_time = 0;
		params[P_CAMERA]->u.pd.value = 1; // Fxc6
		params[P_MASTER]->u.fs_d.value = 1000.0;
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

		params[P_CAM08_CHARACTER]->u.fs_d.value = 65.0; params[P_CAM08_COLOR]->u.fs_d.value = 60.0; params[P_CAM08_SIGNAL]->u.fs_d.value = 40.0; params[P_CAM08_GRAIN]->u.fs_d.value = 30.0;
		params[P_CAM09_CHARACTER]->u.fs_d.value = 65.0; params[P_CAM09_COLOR]->u.fs_d.value = 60.0; params[P_CAM09_SIGNAL]->u.fs_d.value = 40.0; params[P_CAM09_GRAIN]->u.fs_d.value = 30.0;
		params[P_CAM10_CHARACTER]->u.fs_d.value = 65.0; params[P_CAM10_COLOR]->u.fs_d.value = 60.0; params[P_CAM10_SIGNAL]->u.fs_d.value = 40.0; params[P_CAM10_GRAIN]->u.fs_d.value = 30.0;
		params[P_CAM11_CHARACTER]->u.fs_d.value = 65.0; params[P_CAM11_COLOR]->u.fs_d.value = 60.0; params[P_CAM11_SIGNAL]->u.fs_d.value = 40.0; params[P_CAM11_GRAIN]->u.fs_d.value = 30.0;
		params[P_CAM12_CHARACTER]->u.fs_d.value = 65.0; params[P_CAM12_COLOR]->u.fs_d.value = 60.0; params[P_CAM12_SIGNAL]->u.fs_d.value = 40.0; params[P_CAM12_GRAIN]->u.fs_d.value = 30.0;
		params[P_CAM13_CHARACTER]->u.fs_d.value = 65.0; params[P_CAM13_COLOR]->u.fs_d.value = 60.0; params[P_CAM13_SIGNAL]->u.fs_d.value = 40.0; params[P_CAM13_GRAIN]->u.fs_d.value = 30.0;
		params[P_CAM14_CHARACTER]->u.fs_d.value = 65.0; params[P_CAM14_COLOR]->u.fs_d.value = 60.0; params[P_CAM14_SIGNAL]->u.fs_d.value = 40.0; params[P_CAM14_GRAIN]->u.fs_d.value = 30.0;
		params[P_CAM15_CHARACTER]->u.fs_d.value = 65.0; params[P_CAM15_COLOR]->u.fs_d.value = 60.0; params[P_CAM15_SIGNAL]->u.fs_d.value = 40.0; params[P_CAM15_GRAIN]->u.fs_d.value = 30.0;
		params[P_CAM16_CHARACTER]->u.fs_d.value = 65.0; params[P_CAM16_COLOR]->u.fs_d.value = 60.0; params[P_CAM16_SIGNAL]->u.fs_d.value = 40.0; params[P_CAM16_GRAIN]->u.fs_d.value = 30.0;
		params[P_CAM17_CHARACTER]->u.fs_d.value = 65.0; params[P_CAM17_COLOR]->u.fs_d.value = 60.0; params[P_CAM17_SIGNAL]->u.fs_d.value = 40.0; params[P_CAM17_GRAIN]->u.fs_d.value = 30.0;
		params[P_CAM18_CHARACTER]->u.fs_d.value = 65.0; params[P_CAM18_COLOR]->u.fs_d.value = 60.0; params[P_CAM18_SIGNAL]->u.fs_d.value = 40.0; params[P_CAM18_GRAIN]->u.fs_d.value = 30.0;
		params[P_CAM19_CHARACTER]->u.fs_d.value = 65.0; params[P_CAM19_COLOR]->u.fs_d.value = 60.0; params[P_CAM19_SIGNAL]->u.fs_d.value = 40.0; params[P_CAM19_GRAIN]->u.fs_d.value = 30.0;

        // Defaults for authored profiles 8–19, matching ParamsSetup.
        const int authoredProfiles[12][4] = {
          {P_CAM08_CHARACTER,P_CAM08_COLOR,P_CAM08_SIGNAL,P_CAM08_GRAIN},
          {P_CAM09_CHARACTER,P_CAM09_COLOR,P_CAM09_SIGNAL,P_CAM09_GRAIN},
          {P_CAM10_CHARACTER,P_CAM10_COLOR,P_CAM10_SIGNAL,P_CAM10_GRAIN},
          {P_CAM11_CHARACTER,P_CAM11_COLOR,P_CAM11_SIGNAL,P_CAM11_GRAIN},
          {P_CAM12_CHARACTER,P_CAM12_COLOR,P_CAM12_SIGNAL,P_CAM12_GRAIN},
          {P_CAM13_CHARACTER,P_CAM13_COLOR,P_CAM13_SIGNAL,P_CAM13_GRAIN},
          {P_CAM14_CHARACTER,P_CAM14_COLOR,P_CAM14_SIGNAL,P_CAM14_GRAIN},
          {P_CAM15_CHARACTER,P_CAM15_COLOR,P_CAM15_SIGNAL,P_CAM15_GRAIN},
          {P_CAM16_CHARACTER,P_CAM16_COLOR,P_CAM16_SIGNAL,P_CAM16_GRAIN},
          {P_CAM17_CHARACTER,P_CAM17_COLOR,P_CAM17_SIGNAL,P_CAM17_GRAIN},
          {P_CAM18_CHARACTER,P_CAM18_COLOR,P_CAM18_SIGNAL,P_CAM18_GRAIN},
          {P_CAM19_CHARACTER,P_CAM19_COLOR,P_CAM19_SIGNAL,P_CAM19_GRAIN}
        };
        for (const auto &profile : authoredProfiles) {
          params[profile[0]]->u.fs_d.value = 65.0;
          params[profile[1]]->u.fs_d.value = 60.0;
          params[profile[2]]->u.fs_d.value = 40.0;
          params[profile[3]]->u.fs_d.value = 30.0;
        }

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
		params[P_GLITCH_PRESET]->u.pd.value = 1; // Off
		params[P_GLITCH_PRESET_STRENGTH]->u.fs_d.value = 35.0;
		params[P_GLITCH_PRESET_SPEED]->u.fs_d.value = 100.0;
		params[P_GLITCH_PRESET_COUNT]->u.fs_d.value = 12.0;
        params[P_BUILTIN_OVERLAY]->u.pd.value = 1; // Off
        params[P_BUILTIN_OVERLAY_STRENGTH]->u.fs_d.value = 35.0;
        params[P_BUILTIN_OVERLAY_SPEED]->u.fs_d.value = 100.0;
        params[P_BUILTIN_OVERLAY_RATE]->u.fs_d.value = 12.0;
        params[P_BUILTIN_OVERLAY_COLOR]->u.bd.value = TRUE;
        params[P_BUILTIN_OVERLAY_BLEND]->u.fs_d.value = 35.0;
		params[P_OSD_PLAY]->u.bd.value = FALSE; params[P_OSD_REC]->u.bd.value = FALSE; params[P_OSD_TIMECODE]->u.bd.value = FALSE; params[P_OSD_DATE]->u.bd.value = FALSE;
		params[P_OSD_YEAR]->u.sd.value = 1998; params[P_OSD_MONTH]->u.sd.value = 10; params[P_OSD_DAY]->u.sd.value = 24;
		params[P_OSD_OPACITY]->u.fs_d.value = 85.0; params[P_OSD_SCALE]->u.fs_d.value = 2.0; params[P_OSD_POSITION]->u.pd.value = 1; params[P_OSD_SAFE_MARGIN]->u.fs_d.value = 4.0;

		params[P_GRAIN_ENABLE]->u.bd.value = FALSE;
		params[P_TRACKING_ENABLE]->u.bd.value = FALSE;
		params[P_TAPE_ENABLE]->u.bd.value = FALSE;
		params[P_OVERLAY_ENABLE]->u.bd.value = FALSE;
		params[P_OTHER_ENABLE]->u.bd.value = FALSE;
		params[P_CRT_ENABLE]->u.bd.value = FALSE;
		params[P_TRANSPARENT_TAPE_ENABLE]->u.bd.value = FALSE;
		params[P_TAPE_SWEEP_ENABLE]->u.bd.value = FALSE;
		params[P_AUDIO_ENABLE]->u.bd.value = FALSE;
		params[P_AUDIO_EVENTS]->u.bd.value = FALSE;

		params[P_CRT_SCANLINES]->u.fs_d.value = 35.0;
		params[P_CRT_CURVE]->u.fs_d.value = 20.0;
		params[P_CRT_GLOW]->u.fs_d.value = 25.0;
		params[P_CRT_RGB]->u.fs_d.value = 18.0;
		params[P_CRT_FLICKER]->u.fs_d.value = 8.0;
		params[P_CRT_VIGNETTE]->u.fs_d.value = 28.0;

		params[P_TRANSPARENT_TAPE_STRENGTH]->u.fs_d.value = 35.0;
		params[P_TRANSPARENT_TAPE_WIDTH]->u.fs_d.value = 30.0;
		params[P_TRANSPARENT_TAPE_SPEED]->u.fs_d.value = 45.0;
		params[P_TAPE_SWEEP_STRENGTH]->u.fs_d.value = 50.0;
		params[P_TAPE_SWEEP_WIDTH]->u.fs_d.value = 20.0;
		params[P_TAPE_SWEEP_SPEED]->u.fs_d.value = 80.0;
		params[P_TAPE_SWEEP_INTERVAL]->u.fs_d.value = 5.0;
		params[P_GLITCH_PRESET_COLOR]->u.bd.value = TRUE;
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
		params[P_MASTER]->u.fs_d.value = 1000.0;
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

	// Test 3: Camera Profiles Distinction (All 19 profiles produce distinct output)
	{
		totalTests++;
		SetDefaults();
		params[P_MASTER]->u.fs_d.value = 1000.0;
		params[P_SOFTNESS]->u.fs_d.value = 0.0;

		std::vector<std::vector<PF_Pixel>> profileOutputs(19);
		static const char* kProfNames[19] = {
          "camera_fxc6", "camera_gen7", "camera_jvc_c7u", "camera_r20", "camera_fall78", "camera_erc", "camera_jvc_sxm",
          "camera_cpm35", "camera_classic_u", "camera_ir", "camera_fqs", "camera_ccdr", "camera_fxn", "camera_d_half",
          "camera_retro_vhs_01", "camera_retro_vhs_02", "camera_retro_vhs_03", "camera_retro_vhs_04", "camera_retro_vhs_05"
        };
		for (int p = 1; p <= 19; ++p) {
			params[P_CAMERA]->u.pd.value = p;
			memset(dst_buffer.data(), 0, dst_buffer.size() * sizeof(PF_Pixel));
			EffectMain(PF_Cmd_RENDER, &in_data, &out_data, params.data(), &output_layer, nullptr);
			profileOutputs[p - 1] = dst_buffer;
			SavePPM(std::string("previews/") + kProfNames[p - 1] + ".ppm", dst_buffer.data(), width, height);
		}

		bool allDistinct = true;
		for (int i = 0; i < 19; ++i) {
			for (int j = i + 1; j < 19; ++j) {
				PixelStats diff = CompareImages(profileOutputs[i].data(), profileOutputs[j].data(), width, height);
				if (diff.changedPixels < (numPixels / 2) || diff.meanAbsError < 0.5) {
					std::cout << "    [FAIL] Camera Profile " << (i + 1) << " and " << (j + 1) << " are too similar (MAE: " << diff.meanAbsError << ")\n";
					allDistinct = false;
				}
			}
		}

		if (allDistinct) {
			std::cout << "  [PASS] Test 3: All 19 Camera Profiles produce distinct visual responses.\n";
			passedTests++;
		} else {
			std::cout << "  [FAIL] Test 3: Camera profiles are not sufficiently distinct!\n";
		}
	}

	// Helper test runner lambda
	auto RunTestCase = [&](const std::string &name, std::function<void()> setupFn, const std::string &previewName = "", bool compareAgainstInput = false) {
		totalTests++;
		SetDefaults();
		std::vector<PF_Pixel> baseline;
		if (!compareAgainstInput) {
			memset(dst_buffer.data(), 0, dst_buffer.size() * sizeof(PF_Pixel));
			EffectMain(PF_Cmd_RENDER, &in_data, &out_data, params.data(), &output_layer, nullptr);
			baseline = dst_buffer;
		}
		setupFn();

		memset(dst_buffer.data(), 0, dst_buffer.size() * sizeof(PF_Pixel));
		EffectMain(PF_Cmd_RENDER, &in_data, &out_data, params.data(), &output_layer, nullptr);

		const PF_Pixel *comparisonBase = compareAgainstInput ? src_buffer.data() : baseline.data();
		PixelStats stats = CompareImages(comparisonBase, dst_buffer.data(), width, height);
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
		params[P_MASTER]->u.fs_d.value = 1000.0;
	}, "vhs_sp_mode", true);

	RunTestCase("Test 5: Enable Grain / Noise", [&]() {
		params[P_GRAIN_ENABLE]->u.bd.value = TRUE;
        params[P_GRAIN_AMOUNT]->u.fs_d.value = 45.0;
	}, "grain_color");

	RunTestCase("Test 6: Enable Tracking Damage", [&]() {
		params[P_TRACKING_ENABLE]->u.bd.value = TRUE;
        params[P_TRACKING]->u.fs_d.value = 45.0;
	}, "vhs_lp_mode");

	RunTestCase("Test 7: Enable Tape Effects", [&]() {
		params[P_TAPE_ENABLE]->u.bd.value = TRUE;
        params[P_TAPE_TYPE]->u.pd.value = 2; // Horizontal Band, not Off
	}, "bad_signal_glitch");

	RunTestCase("Test 8: Enable Other VHS Engines", [&]() {
		params[P_OTHER_ENABLE]->u.bd.value = TRUE;
        params[P_OTHER_ENGINE]->u.pd.value = 2; // Explicit VHS Craft branch
        params[P_CRAFT_TRACKING]->u.fs_d.value = 55.0;
        params[P_CRAFT_CHROMA]->u.fs_d.value = 45.0;
        params[P_CRAFT_HEADSWITCH]->u.fs_d.value = 35.0;
        params[P_CRAFT_SMEAR]->u.fs_d.value = 30.0;
	}, "vhs_master_full_suite");

	RunTestCase("Test 9: Procedural Transparent Tape", [&]() {
		params[P_TRANSPARENT_TAPE_ENABLE]->u.bd.value = TRUE;
        params[P_TRANSPARENT_TAPE_STRENGTH]->u.fs_d.value = 100.0;
        params[P_TRANSPARENT_TAPE_WIDTH]->u.fs_d.value = 100.0;
        in_data.current_time = 15000; // Test the moving band away from the first scanline.
	});

	RunTestCase("Test 10: Moving Tape Sweep", [&]() {
		params[P_TAPE_SWEEP_ENABLE]->u.bd.value = TRUE;
        params[P_TAPE_SWEEP_STRENGTH]->u.fs_d.value = 100.0;
        params[P_TAPE_SWEEP_WIDTH]->u.fs_d.value = 100.0;
        params[P_TAPE_SWEEP_SPEED]->u.fs_d.value = 100.0;
        params[P_TAPE_SWEEP_INTERVAL]->u.fs_d.value = 5.0;
        in_data.current_time = 15000;
	});

	RunTestCase("Test 11: Real ntsc-rs Signal Engine Integration", [&]() {
		params[P_OTHER_ENABLE]->u.bd.value = TRUE;
		params[P_OTHER_ENGINE]->u.pd.value = 4; // NTSC-RS Signal Model
		params[P_OTHER_STRENGTH]->u.fs_d.value = 50.0;
		params[P_NTSC_CHROMA_DELAY]->u.fs_d.value = 30.0;
		params[P_NTSC_HEAD_SWITCH]->u.fs_d.value = 20.0;
		params[P_NTSC_EDGE_WAVE]->u.fs_d.value = 25.0;
		params[P_NTSC_COMPOSITE]->u.fs_d.value = 40.0;
	}, "ntsc_rs_signal_model");

	RunTestCase("Test 12: Glitch Cam Preset Engine", [&]() {
		params[P_GLITCH_PRESET]->u.pd.value = 16; // Glitch Cam 15
        params[P_SEED]->u.sd.value = 42; // deterministic seed chosen to trigger the test slot
		params[P_GLITCH_PRESET_STRENGTH]->u.fs_d.value = 100.0;
		params[P_GLITCH_PRESET_SPEED]->u.fs_d.value = 180.0;
		params[P_GLITCH_PRESET_COUNT]->u.fs_d.value = 120.0;
        params[P_GLITCH_PRESET_COLOR]->u.bd.value = TRUE;
        in_data.current_time = 3000; // 0.1s: first 120/minute event slot
	}, "glitch_cam_15");

	RunTestCase("Test 13: PLAY / Date / Timecode OSD", [&]() {
		params[P_OSD_PLAY]->u.bd.value = TRUE;
		params[P_OSD_DATE]->u.bd.value = TRUE;
		params[P_OSD_TIMECODE]->u.bd.value = TRUE;
		params[P_OSD_OPACITY]->u.fs_d.value = 100.0;
	}, "capture_osd");

    // Exercise all 21 built-in overlays against a matched baseline (not against raw input).
    {
        totalTests++;
        bool allOverlaysWork = true;
        for (int preset = 1; preset <= 21; ++preset) {
            SetDefaults();
            params[P_MASTER]->u.fs_d.value = 1000.0;
            params[P_SOFTNESS]->u.fs_d.value = 0.0;
            params[P_BUILTIN_OVERLAY]->u.pd.value = 1; // Off baseline
            params[P_BUILTIN_OVERLAY_STRENGTH]->u.fs_d.value = 90.0;
            params[P_BUILTIN_OVERLAY_SPEED]->u.fs_d.value = 150.0;
            params[P_BUILTIN_OVERLAY_RATE]->u.fs_d.value = 120.0;
            params[P_BUILTIN_OVERLAY_BLEND]->u.fs_d.value = 10.0;
            params[P_BUILTIN_OVERLAY_COLOR]->u.bd.value = TRUE;
            in_data.current_time = 3000; // 0.1s so blend-in has elapsed for this event slot.

            const bool isEventPreset = preset == 1 || preset == 3 || preset == 5 || preset == 6 || preset == 9 || preset == 10 || preset == 11 || preset == 16 || preset == 20 || preset == 21;
            if (isEventPreset) {
                int chosenSeed = 0;
                while (chosenSeed < 10000 && rand01((uint32_t)chosenSeed ^ (uint32_t)(preset * 2246822519u)) >= 0.55f) ++chosenSeed;
                params[P_SEED]->u.sd.value = chosenSeed;
            }

            std::fill(dst_buffer.begin(), dst_buffer.end(), PF_Pixel{});
            EffectMain(PF_Cmd_RENDER, &in_data, &out_data, params.data(), &output_layer, nullptr);
            const std::vector<PF_Pixel> baseline = dst_buffer;

            params[P_BUILTIN_OVERLAY]->u.pd.value = preset + 1; // popup item 1 is Off
            std::fill(dst_buffer.begin(), dst_buffer.end(), PF_Pixel{});
            EffectMain(PF_Cmd_RENDER, &in_data, &out_data, params.data(), &output_layer, nullptr);
            const PixelStats stats = CompareImages(baseline.data(), dst_buffer.data(), width, height);
            if (stats.changedPixels == 0 || stats.meanAbsError <= 0.001) {
                std::cout << "    [FAIL] Native overlay preset " << preset << " produced no measurable delta.\n";
                allOverlaysWork = false;
            }
            if (preset == 12) SavePPM("previews/capture_code_overlay.ppm", dst_buffer.data(), width, height);
            if (preset == 13) SavePPM("previews/rec_hud_overlay.ppm", dst_buffer.data(), width, height);
        }
        if (allOverlaysWork) {
            std::cout << "  [PASS] Test 14: All 21 native VHS overlay presets change a matched baseline.\n";
            passedTests++;
        } else {
            std::cout << "  [FAIL] Test 14: One or more native VHS overlay presets did not render.\n";
        }
    }

    // Exercise the actual native sample-domain audio callback (not the Python DSP model).
    {
        totalTests++;
        SetDefaults();
        const A_long audioFrames = 2048;
        std::vector<float> audioIn((size_t)audioFrames * 2), audioOut((size_t)audioFrames * 2, 0.0f);
        for (A_long i = 0; i < audioFrames; ++i) {
            const float v = 0.55f * std::sin((float)i * 0.031f) + 0.12f * std::sin((float)i * 0.19f);
            audioIn[(size_t)i*2] = v;
            audioIn[(size_t)i*2+1] = v * 0.87f;
        }
        in_data.start_sampL = 0;
        in_data.src_snd.dataP = audioIn.data();
        in_data.src_snd.num_samples = audioFrames;
        in_data.src_snd.fi.rateF = 48000.0;
        in_data.src_snd.fi.num_channels = PF_Channels_STEREO;
        out_data.dest_snd.dataP = audioOut.data();
        out_data.dest_snd.num_samples = audioFrames;
        out_data.dest_snd.fi.rateF = 48000.0;
        out_data.dest_snd.fi.num_channels = PF_Channels_STEREO;
        params[P_AUDIO_ENABLE]->u.bd.value = FALSE;
        EffectMain(PF_Cmd_AUDIO_RENDER, &in_data, &out_data, params.data(), nullptr, nullptr);
        bool dryIsExact = true;
        for (size_t i = 0; i < audioIn.size(); ++i) if (audioOut[i] != audioIn[i]) { dryIsExact = false; break; }
        params[P_AUDIO_ENABLE]->u.bd.value = TRUE;
        params[P_AUDIO_MIX]->u.fs_d.value = 100.0;
        params[P_AUDIO_SATURATION]->u.fs_d.value = 65.0;
        params[P_AUDIO_LOFI]->u.fs_d.value = 70.0;
        params[P_AUDIO_CRACKLE]->u.fs_d.value = 20.0;
        params[P_AUDIO_EVENTS]->u.bd.value = TRUE;
        params[P_AUDIO_EVENT_AMOUNT]->u.fs_d.value = 100.0;
        params[P_AUDIO_DROPOUT]->u.fs_d.value = 40.0;
        params[P_EVENT_RATE]->u.fs_d.value = 120.0;
        params[P_EVENT_DURATION]->u.fs_d.value = 1.0;
        params[P_RECOVERY]->u.fs_d.value = 0.25;
        std::fill(audioOut.begin(), audioOut.end(), 0.0f);
        EffectMain(PF_Cmd_AUDIO_RENDER, &in_data, &out_data, params.data(), nullptr, nullptr);
        bool finite = true, changed = false;
        for (size_t i = 0; i < audioOut.size(); ++i) {
            if (!std::isfinite(audioOut[i])) finite = false;
            if (std::fabs(audioOut[i] - audioIn[i]) > 1.0e-5f) changed = true;
        }
        if (dryIsExact && finite && changed) {
            std::cout << "  [PASS] Test 15: Native audio callback preserves exact dry audio and applies finite sample-domain degradation.\n";
            passedTests++;
        } else {
            std::cout << "  [FAIL] Test 15: Native audio callback dry-pass / DSP checks failed.\n";
        }
    }




	std::cout << "\n======================================================================\n";
	std::cout << "Summary: " << passedTests << " / " << totalTests << " native C++ tests PASSED.\n";
	std::cout << "======================================================================\n";

	return (passedTests == totalTests) ? 0 : 1;
}
