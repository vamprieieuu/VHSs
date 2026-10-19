# VHS6RIFT manual verification plan

Record AE version, OS, project dimensions, frame rate, bit depth, source clip, settings, and outcome for every test. Use a short test clip with fine text, high-contrast edges, skin tones, and dark areas; also test a clip with audio.

1. **Load / PiPL:** install only the built `VHS6RIFT.aex`; verify AE 23.2.1 lists VHS6RIFT and applies it without version, flags, or reinstall errors.
2. **Neutral bypass:** set Master Strength to 0, disable Continuous Treatment and Event Glitches, set Tape Type Off, Overlay Opacity 0, Other VHS Engine Off, Audio Glitch Enable off. Compare output to source.
3. **Camera profiles:** render the same frame for all seven profiles. Record differences in color response, softness, grain and signal behavior; confirm profile sliders affect only their selected profile.
4. **Continuous-only:** enable Continuous Treatment and disable Event Glitches. Verify color/chroma/grain/tracking treatment without sudden event bursts.
5. **Events-only:** disable Continuous Treatment and enable Event Glitches. Verify short, irregular events separated by quiet frames. Change rate, duration, recovery and seed one at a time.
6. **Repeatability:** render frame N, scrub away and back, render again. Repeat with same settings and seed. Compare images. Then change only Seed and confirm the event pattern changes.
7. **Color controls:** individually test Saturation, Black Level, Luma Softness, Chroma Bleed, Chroma Offset X and Y. Verify each control has a visible and isolated effect.
8. **Grain:** vary Amount, Scale and Motion Speed; compare frames at different times and render the same time twice.
9. **Tape types:** test every Tape Type, Strength, Size, Speed and Direction. Confirm Off disables the tape band and each other type has distinct behavior.
10. **Overlay:** select an actual overlay footage layer in the comp. Test Normal with black background retained; then Add, Screen, Multiply and explicit Luma Key. Test opacity, speed 0/50/100/200%, negative/positive offset, loop off/on and several loop periods. Inspect end-to-start discontinuities.
11. **Other VHS:** test each engine separately and move one control at a time. Confirm only the selected engine's bank affects its output.
12. **Audio independent toggles:** apply the effect to a layer containing audio. Toggle Audio Glitch Enable; test Audio Event Glitches with visual Event Glitches off and vice versa. Test Wet/Dry, Event Amount, Crackle, Dropouts, Saturation and Lo-Fi Quantization individually.
13. **Audio sync:** render a 5–10 second segment containing audible transients. Compare waveform/sample timestamps to visual event onsets. Repeat after scrubbing and in final render. Record measured offset in milliseconds.
14. **Stability:** test 8-bpc and 16-bpc comps; this initial renderer advertises no deep-color flag, so AE's actual conversion path must be recorded. Test overlay layer empty/unassigned, missing source, layer start/end, negative offset and loop boundary.
15. **Performance:** test 720p and 1080p; record preview and render time. CPU processing is expected in this baseline.

## Results log template

| Test | Settings / source | Expected | Actual | Status | Evidence |
|---|---|---|---|---|---|
| AE load | | Loads without errors | | Not run | |
| Neutral bypass | | Matches source | | Not run | |
| Camera profiles | | Each has distinct signal/color behavior | | Not run | |
| Audio sync | | Measured offset documented | | Not run | |
