# VHS6RIFT — Known Limitations and Verification Boundaries

- This deliverable is source-only; no `.aex` binary is included or claimed as built.
- Cargo/Rust and the complete licensed Adobe After Effects SDK headers plus `PiPLtool.exe` were not available in the local verification environment. The Rust `ntscrs_c` library and native C++ render harness have therefore not been compiled or executed here.
- Static tests validate parameter registration and source wiring, not perceived image quality, frame-to-frame behavior, alpha correctness, performance, or successful plugin loading.
- The built-in overlay library is procedural. It does not embed or play the user's reference GIF files as actual overlay footage.
- VHS Craft-style, VHS Legend-style, and Signal Reference are separate original processing paths, not wrappers around third-party AEX binaries. Visual similarity to reference plugins is not yet validated.
- The CRT TV system was removed from the source/UI as requested.
- Audio processing is present but has not been verified during AE RAM preview or final audio export.
- The updated native render test source needs to be run on a Windows environment with the complete SDK and Rust toolchain before the code can be treated as build-verified.
