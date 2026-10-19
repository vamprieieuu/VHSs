# Build report — initial handoff

- **Target:** After Effects 23.2.1, Windows 11 x64, Visual Studio 2022/MSVC.
- **Source implementation:** native C++ effect source and PiPL resource are present.
- **SDK available in uploaded VHS MAKER archive:** no. The prior `VHS-Legend-repo-FINAL` project had `src/PLACE_SOURCE_HERE.txt`, an SDK placeholder, and no compilable plugin source. Its CMake file alone was not a complete plugin.
- **PiPL pipeline:** CMake looks for `PiPLtool.exe` in both common SDK layouts, preprocesses the `.r` file, runs PiPLtool, then generates the Windows `.rc` resource.
- **Static source checks:** `tools/validate_project.py` passes; the C++ source also passed a syntax-only check against temporary minimal API stubs. These checks are not an Adobe SDK compile.
- **Build status:** not run. This environment does not contain the user's Adobe SDK or Windows MSVC toolchain. No `.aex` success is claimed.
- **AE load status:** not tested.
- **Visual processing status:** source implementation present; output not verified in AE.
- **Audio processing status:** sample-domain callback implemented in source using the SDK audio selectors and float-only audio flag; not compiled or tested in AE.
- **Overlay timing:** custom-time layer checkout and explicit loop-period mapping implemented in source; not yet verified against AE 23.2.1 layer checkout semantics.

## Required next evidence

1. Successful Windows Actions build log and exactly one `VHS6RIFT.aex` artifact.
2. AE 23.2.1 load screenshot or log without PiPL/version/flag errors.
3. Rendered comparison frames for each camera and major engine.
4. An exported audio test and measured visual/audio onset offset.

Update this report after every build attempt. Never replace “not tested” with “passed” without evidence.


## 2026-10-15 source review patch

- Increased Master VHS Strength UI range from 0–200 to 0–2000 and expanded the internal intensity ceiling. This changes the response mapping; extreme values can be destructive by design and need visual calibration in After Effects.
- Changed Event Glitches, Audio Glitch Enable, and Audio Event Glitches to disabled defaults. Set colored grain, tracking noise, dropouts, and block corruption defaults to zero so applying the effect does not immediately add optional damage.
- Added reference-guided per-camera warm/cool crossover, contrast response, and highlight shoulder in addition to existing softness/chroma/noise parameters. These are approximate creative profiles, not laboratory-calibrated camera emulations.
- Updated PiPL global outflags to 0x48000006 to match GlobalSetup's audio flags and updated the static validator accordingly.
- Validation performed: source consistency script only. No Windows SDK build or After Effects runtime test was available in this environment; the code/PiPL version warning must still be checked in AE.


## 2026-10-15 implementation revision

- Aligned the declared code version to `PF_VERSION(1, 1, 0, PF_Stage_RELEASE, 1)` and PiPL `AE_Effect_Version { 557057 }` (1.1). PiPL global outflags remain `0x48000006`, matching `PF_OutFlag_AUDIO_EFFECT_TOO | PF_OutFlag_AUDIO_FLOAT_ONLY`.
- Removed the unavailable `entry.h` include from the supplied SDK snapshot and declared the exported Windows entry point directly, avoiding the previously reported missing-header failure.
- Appended new parameter IDs without reordering existing IDs; added independent optional-module toggles for grain/noise, tracking, tape, overlays, other signal engines, and CRT.
- Added a CRT stage with optional source-coordinate barrel warp, scanline modulation, edge falloff, flicker, highlight glow, and RGB convergence controls.
- Added separate procedural Transparent Tape and Moving Tape Sweep controls. Both are disabled by default; the sweep performs time-driven source displacement and chroma/luma disturbance instead of painting a plain white bar.
- Reworked the tape band path to displace actual source samples within the band, with distinct behavior for head switching, vertical roll, side tear, and signal snow.
- Revised camera response with per-profile channel gains, gamma, color crossover, contrast, highlight shoulder, softness, and chroma response. These remain reference-guided approximations and are not calibrated sensor models.
- Corrected softness and grain intensity mapping to avoid out-of-range blending and clipping at high Master Strength values. Master Strength is mapped to a 0–20 internal drive from the 0–2000 UI range.
- Validation: `tools/validate_project.py` passes after adding checks for parameter order, module controls, Master Strength range, version/outflags declarations, and the absent `entry.h` dependency.
- Not performed: Windows/MSVC build and After Effects 23.2.1 runtime/render verification. Those are intentionally left to the existing GitHub Actions/host workflow; no successful AEX artifact is claimed.


## 2026-10-15 targeted handoff audit

- Audited the uploaded repository's camera popup and render profile table. The seven existing choices are Fxc6 Reference, Gen 7 Reference, JVC GR-C7U (1986), VHS-R20 Reference, Fall-78 Reference, ERC-V2S Reference, and JVC GR-SXM740U. These are VHS/camcorder-oriented reference profiles; no still-photography camera models were present in this current source list, so none were removed.
- Renamed the UI label to `VHS Camcorder / Reference Profile` to make the scope explicit. Updated the About text to report version 1.1 instead of the stale 1.0 string. No parameter IDs, enum order, PiPL values, CMake configuration, or workflow steps were changed by this targeted patch.
- The source contains a legacy `PF_Cmd_RENDER` path and a `PF_Cmd_AUDIO_RENDER` path. Automated Python tests in this repository include a separate numerical model; they are not proof that the compiled C++ renderer changes pixels in After Effects.
- Validation run in this environment: `python tools/validate_project.py` and `python tools/test_vhs6rift.py`. The first is a static consistency check; the second exercises its Python test implementation. Neither compiles the native plugin nor validates runtime rendering in AE.
- Not run here: Windows/MSVC + full Adobe SDK build, loading the resulting `.aex` in AE 23.2.1, or actual frame-by-frame visual comparisons inside AE. Preserve the existing build workflow and SDK paths; do not treat this handoff as a confirmed successful build.


## 2026-10-15 visible-default correction

- Increased the default Master VHS Strength from 70 to 350 and increased default Luma Softness (12 to 28), Chroma Bleed (20 to 42), and Chroma Offset X (2 to 4) so applying the effect has a more obvious image-space result without enabling optional damage modules.
- Incremented the PiPL effect version from 1.1 (557057) to 1.2 (559105) to make this build distinguishable from a previously installed 1.1 binary and reduce stale-plugin ambiguity. Updated C++ About and GlobalSetup to report version 1.2, matching the PiPL value.
- No Windows/MSVC build or After Effects render was available in this environment. This patch is not represented as verified in AE.
