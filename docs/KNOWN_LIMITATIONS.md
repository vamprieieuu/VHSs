# Known limitations and unverified areas

- The Windows x64 native build has not been run in this packaging environment. GitHub Actions is configured to set up stable Rust, validate project structure, run CMake/MSVC and collect one `VHS6RIFT.aex`.
- After Effects 23.2.1 has not been launched with this exact package; plugin loading, render output, host stability and audio callback behavior remain unverified.
- The selector now exposes nineteen parameterized camera/look profiles. The additional authored looks are not laboratory-measured hardware calibrations.
- The Python test suite is a model/scheduler check, not proof that every native C++ parameter changes rendered pixels correctly. The C++ harness has 15 test groups including all 19 camera profiles, a matched-baseline loop over 21 native overlay presets, and native audio callback pass-through/DSP checks; a local stub-linked run does not validate the real Rust engine.
- The NTSC-RS C ABI wrapper and Rust dependency are integrated in source/build configuration but have not been compiled here because this environment lacks Cargo and the Windows MSVC toolchain. The C++ bridge now sends control values using the 0–100 units expected by the Rust wrapper.
- CRT processing now includes barrel-warped aperture masking, scanlines, neighbor-sampled bloom, RGB convergence/triad modulation, flicker, and vignette. It is not claimed to be a physically complete CRT beam/phosphor simulation.
- Supplied animated overlay media must be imported into After Effects and selected as an overlay source layer. Separately, 21 procedural native overlay effects are available directly in the effect popup and do not require footage.
- The built-in OSD currently renders PLAY, REC, editable YYYY-MM-DD, and composition-time HH:MM:SS. Arbitrary user-defined text and the supplied animated OSD media still use the overlay layer.
- Audio is implemented in an AE audio callback path but requires a real export/render test for audible behavior and synchronization.
- No `.aex` binary is included as a deliverable from this Linux packaging step.


## Integrated feature scope (current source)

- The camera selector exposes 19 independently tuned profiles. Seven have direct user reference-folder labels; CPM35, Classic U, IR, FQS, CCDR, FXN, D Half, and five Retro VHS variants are additional authored profiles and should be treated as look profiles, not claims of measured hardware calibration.
- Fifteen Glitch Cam looks are native procedural pixel treatments. The original GIF/JPG/MP4 assets are retained in `references/` for manual AE overlay use; the plugin does not autonomously import files into an AE project.
- Built-in OSD renders PLAY, REC, editable YYYY-MM-DD and running HH:MM:SS using a bitmap font. Arbitrary custom strings and the original animated text overlays still use the overlay layer.
- The C++ translation unit passes a local g++ syntax-only check and the render harness passes 15/15 when linked to a temporary NTSC stub; these checks do not substitute for the Windows MSVC/Rust build or a real NTSC-RS render. AE host rendering has not been performed here.
