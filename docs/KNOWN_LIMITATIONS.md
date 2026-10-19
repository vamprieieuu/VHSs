# VHS6RIFT — Known Limitations and Verification Boundaries

- The current deliverable is the **source repository only**. No `.aex` binary is included or claimed as built.
- This environment did not have Cargo installed, so the Rust `ntscrs_c` static library and native C++ render harness could not be built or executed here.
- The repository archive does not include the full licensed Adobe After Effects SDK headers and `PiPLtool.exe`; supply the licensed SDK at `extern/AfterEffectsSDK` before running the Windows 2022 / MSVC build workflow.
- The added rendering stages have source-level wiring and static validation, but require Windows MSVC compilation and actual AE 23.x visual checks to verify behavior, performance, alpha handling, and temporal appearance.
- OSD uses a compact built-in 5x7 bitmap glyph set and preset display modes; it is not a general-purpose custom text/font loader.
- Built-in overlay styles are generated procedurally. They are editable animated signal graphics inspired by analog-video artifact families, not copies of reference clips or proprietary plugin algorithms.
- Audio code is present but needs verification during After Effects RAM preview and audio export, because static image rendering does not exercise audio selectors.
