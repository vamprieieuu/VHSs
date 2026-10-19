# Build and debug workflow

## First run

1. Add the Adobe SDK in `extern/AfterEffectsSDK/` using one of the layouts in the README.
2. Commit the SDK files that your workflow is allowed to access. If your SDK is stored privately, configure the repository so the runner can restore it; a plain GitHub runner does not already contain Adobe's SDK.
3. Push to `main` or manually start the workflow.
4. If the job fails, download `VHS6RIFT-build-logs` and send the complete `configure.log` and `build.log`, not only the last error line.
5. The next repair should be based on the first root-cause error. Don't delete the PiPL build pipeline, replace the SDK, or remove effect code to silence a downstream error.

## Common failure categories

- **SDK headers missing:** the runner cannot see `extern/AfterEffectsSDK/Headers/AE_Effect.h` or `extern/AfterEffectsSDK/Examples/Headers/AE_Effect.h`.
- **PiPLtool missing:** locate the exact file in the SDK; don't invent a substitute or copy a random executable from another version.
- **PiPL parse / wrong flags:** compare `resources/VHS6RIFTPiPL.r` with `GlobalSetup` flags and the actual SDK resource pipeline.
- **Parameter count mismatch:** preserve the order of enum IDs and all `PF_ADD_*` / `PF_END_TOPIC` calls. The static validator checks their ordering.
- **AEX compiles but AE refuses to load:** inspect the exact load error, PiPL metadata, exported `EffectMain`, target architecture, and SDK compatibility. A successful C++ link is not enough.
- **AE loads but preview is unchanged:** inspect the actual `PF_Cmd_RENDER` path and current parameter values. Do not mark UI-only controls as implemented.
- **Audio is silent or not processed:** inspect `PF_Cmd_AUDIO_RENDER`, float-only sample format, `PF_SoundWorld` sample counts/channels, and `PF_OutFlag_AUDIO_EFFECT_TOO`/`PF_OutFlag_AUDIO_FLOAT_ONLY` consistency. Then run a short export and verify actual waveform changes.
- **Overlay timing or loop is wrong:** inspect the custom-time `PF_CHECKOUT_PARAM` / `PF_CHECKIN_PARAM` path and compare its time base to the footage layer. The loop period is explicit and must match the source duration for seamless repetition.
