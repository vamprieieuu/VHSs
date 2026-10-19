# VHS MAKER — Reference Audit

Generated from the uploaded archive with `ffprobe`, `file`, `objdump`, `strings`, and frame sampling. The audit records measurable properties separately from visual interpretation.

## Archive overview

- Video files inspected: **22**
- Total duration across video files (not unique footage): **157.66 seconds**
- Files with audio streams: **3**
- Files with a video pixel format carrying alpha: **0**
- Video files were sampled at start, midpoint, and end; the long 63.3-second overlay demo was additionally sampled every 2 seconds. See the contact sheets.

## Per-file observations

### `Vhs cams /Erc-v2s/ScreenRecording_10-14-2026 18-24-52_1.mov`

- **Measured:** 12.348s; 662×814; hevc; nominal 60/1 fps; average 373800/7409 fps; 623 frames; audio: no; pixel format: yuvj420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** One 12.35-second HEVC recording, 662×814, nominal 60 fps, no audio stream. User notes state that it contains four named modes/filters within one recording. The video includes an iOS control overlay near the end, so not all frames are usable as camera-reference material. Four modes must be segmented from the recording rather than represented as one camera treatment.

### `Vhs cams /Fall-78 /ScreenRecording_10-14-2026 18-31-08_1.mov`

- **Measured:** 11.347s; 772×394; hevc; nominal 30/1 fps; average 25575/851 fps; 341 frames; audio: no; pixel format: yuvj420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** One 11.35-second HEVC recording, 772×394, nominal 30 fps, no audio. Sampled frames show archival-looking footage with a warm/yellow cast, low detail, visible blur/noise and baked-in title text. This is a visual reference, not sufficient evidence for a specific VHS camera model.

### `Vhs cams /Fxc6/ScreenRecording_10-14-2026 18-22-43_1.mov`

- **Measured:** 6.733s; 828×1050; hevc; nominal 60/1 fps; average 3870/101 fps; 258 frames; audio: no; pixel format: yuvj420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** Two screen recordings of the same road footage in a side-by-side comparison layout. Both are HEVC, approximately 6.7–6.8 s, 828 px wide and 1050/1068 px tall, nominal 60 fps, no audio stream, no alpha. The visible split allows comparison but the two recordings are not guaranteed frame-aligned or identical in crop.

### `Vhs cams /Fxc6/ScreenRecording_10-14-2026 18-23-02_1.mov`

- **Measured:** 6.800s; 828×1068; hevc; nominal 60/1 fps; average 630/17 fps; 252 frames; audio: no; pixel format: yuvj420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** Two screen recordings of the same road footage in a side-by-side comparison layout. Both are HEVC, approximately 6.7–6.8 s, 828 px wide and 1050/1068 px tall, nominal 60 fps, no audio stream, no alpha. The visible split allows comparison but the two recordings are not guaranteed frame-aligned or identical in crop.

### `Vhs cams /Gen 7/After vhs /ScreenRecording_10-14-2026 17-30-46_1.mov`

- **Measured:** 8.960s; 828×1078; hevc; nominal 60/1 fps; average 59800/1789 fps; 299 frames; audio: yes; pixel format: yuvj420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** Source `IMG_3621.MP4` is 1.52 s, 828×1086, HEVC, nominal 30 fps, no audio. The processed screen recording is 8.96 s, 828×1078, HEVC nominal 60 fps, with stereo AAC audio. It becomes extremely noisy/near-static in the middle/end. Since source and processed recordings differ in duration and framing, they are not a frame-exact A/B pair. User notes specifically request studying the color and audio degradation as strength increases.

### `Vhs cams /Gen 7/before vhs/IMG_3621.MP4`

- **Measured:** 1.517s; 828×1086; hevc; nominal 30/1 fps; average 2700/91 fps; 45 frames; audio: no; pixel format: yuvj420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** Source `IMG_3621.MP4` is 1.52 s, 828×1086, HEVC, nominal 30 fps, no audio. The processed screen recording is 8.96 s, 828×1078, HEVC nominal 60 fps, with stereo AAC audio. It becomes extremely noisy/near-static in the middle/end. Since source and processed recordings differ in duration and framing, they are not a frame-exact A/B pair. User notes specifically request studying the color and audio degradation as strength increases.

### `Vhs cams /Jvc  gr-sxm740u/ScreenRecording_10-14-2026 17-39-15_1.mov`

- **Measured:** 9.203s; 828×1060; hevc; nominal 60/1 fps; average 165300/2761 fps; 551 frames; audio: no; pixel format: yuvj420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** A 9.20-second HEVC screen recording, 828×1060 nominal 60 fps, no audio stream. Sampled frames show indoor retail/camcorder demonstration footage. The screen recording includes interface/crop elements, so its displayed image should not be treated as an unaltered camera sensor capture.

### `Vhs cams /Jvc gr-c7u ( 1986)/Filters /Capture codes /ScreenRecording_10-14-2026 17-38-11_1 2.mov`

- **Measured:** 2.460s; 828×1092; hevc; nominal 60/1 fps; average 3650/123 fps; 73 frames; audio: no; pixel format: yuvj420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** The ordinary VHS-v1 recording is 8.22 s at 828×1328 and 60 fps nominal, HEVC, no audio. Six filter comparison recordings are 2.06–4.22 s at 828×1092. Folder notes identify `Vhs-v1` as the ordinary camera reference, and `Filters/Capture codes` and `Filters/CRT-TV` as two different filter families, each with three recordings. Capture codes footage includes a `Capture Card` label; CRT-TV versions visibly add a blue/cyan cast, rounded screen edges/vignette, and more monitor-like texture. The clips are not equal duration, so only qualitative comparison is valid.

### `Vhs cams /Jvc gr-c7u ( 1986)/Filters /Capture codes /ScreenRecording_10-14-2026 17-38-11_1 3.mov`

- **Measured:** 3.170s; 828×1092; hevc; nominal 60/1 fps; average 9800/317 fps; 98 frames; audio: no; pixel format: yuvj420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** The ordinary VHS-v1 recording is 8.22 s at 828×1328 and 60 fps nominal, HEVC, no audio. Six filter comparison recordings are 2.06–4.22 s at 828×1092. Folder notes identify `Vhs-v1` as the ordinary camera reference, and `Filters/Capture codes` and `Filters/CRT-TV` as two different filter families, each with three recordings. Capture codes footage includes a `Capture Card` label; CRT-TV versions visibly add a blue/cyan cast, rounded screen edges/vignette, and more monitor-like texture. The clips are not equal duration, so only qualitative comparison is valid.

### `Vhs cams /Jvc gr-c7u ( 1986)/Filters /Capture codes /ScreenRecording_10-14-2026 17-38-11_1.mov`

- **Measured:** 2.250s; 828×1092; hevc; nominal 60/1 fps; average 1065/34 fps; 71 frames; audio: no; pixel format: yuvj420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** The ordinary VHS-v1 recording is 8.22 s at 828×1328 and 60 fps nominal, HEVC, no audio. Six filter comparison recordings are 2.06–4.22 s at 828×1092. Folder notes identify `Vhs-v1` as the ordinary camera reference, and `Filters/Capture codes` and `Filters/CRT-TV` as two different filter families, each with three recordings. Capture codes footage includes a `Capture Card` label; CRT-TV versions visibly add a blue/cyan cast, rounded screen edges/vignette, and more monitor-like texture. The clips are not equal duration, so only qualitative comparison is valid.

### `Vhs cams /Jvc gr-c7u ( 1986)/Filters /Crt-Tv /ScreenRecording_10-14-2026 17-38-11_1 2.mov`

- **Measured:** 2.693s; 828×1092; hevc; nominal 60/1 fps; average 6225/202 fps; 83 frames; audio: no; pixel format: yuvj420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** The ordinary VHS-v1 recording is 8.22 s at 828×1328 and 60 fps nominal, HEVC, no audio. Six filter comparison recordings are 2.06–4.22 s at 828×1092. Folder notes identify `Vhs-v1` as the ordinary camera reference, and `Filters/Capture codes` and `Filters/CRT-TV` as two different filter families, each with three recordings. Capture codes footage includes a `Capture Card` label; CRT-TV versions visibly add a blue/cyan cast, rounded screen edges/vignette, and more monitor-like texture. The clips are not equal duration, so only qualitative comparison is valid.

### `Vhs cams /Jvc gr-c7u ( 1986)/Filters /Crt-Tv /ScreenRecording_10-14-2026 17-38-11_1 3.mov`

- **Measured:** 4.220s; 828×1092; hevc; nominal 60/1 fps; average 6350/211 fps; 127 frames; audio: no; pixel format: yuvj420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** The ordinary VHS-v1 recording is 8.22 s at 828×1328 and 60 fps nominal, HEVC, no audio. Six filter comparison recordings are 2.06–4.22 s at 828×1092. Folder notes identify `Vhs-v1` as the ordinary camera reference, and `Filters/Capture codes` and `Filters/CRT-TV` as two different filter families, each with three recordings. Capture codes footage includes a `Capture Card` label; CRT-TV versions visibly add a blue/cyan cast, rounded screen edges/vignette, and more monitor-like texture. The clips are not equal duration, so only qualitative comparison is valid.

### `Vhs cams /Jvc gr-c7u ( 1986)/Filters /Crt-Tv /ScreenRecording_10-14-2026 17-38-11_1.mov`

- **Measured:** 2.062s; 828×1092; hevc; nominal 60/1 fps; average 37200/1247 fps; 62 frames; audio: no; pixel format: yuvj420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** The ordinary VHS-v1 recording is 8.22 s at 828×1328 and 60 fps nominal, HEVC, no audio. Six filter comparison recordings are 2.06–4.22 s at 828×1092. Folder notes identify `Vhs-v1` as the ordinary camera reference, and `Filters/Capture codes` and `Filters/CRT-TV` as two different filter families, each with three recordings. Capture codes footage includes a `Capture Card` label; CRT-TV versions visibly add a blue/cyan cast, rounded screen edges/vignette, and more monitor-like texture. The clips are not equal duration, so only qualitative comparison is valid.

### `Vhs cams /Jvc gr-c7u ( 1986)/Vhs-v1/ScreenRecording_10-14-2026 17-37-39_1.mov`

- **Measured:** 8.217s; 828×1328; hevc; nominal 60/1 fps; average 14820/493 fps; 247 frames; audio: no; pixel format: yuvj420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** The ordinary VHS-v1 recording is 8.22 s at 828×1328 and 60 fps nominal, HEVC, no audio. Six filter comparison recordings are 2.06–4.22 s at 828×1092. Folder notes identify `Vhs-v1` as the ordinary camera reference, and `Filters/Capture codes` and `Filters/CRT-TV` as two different filter families, each with three recordings. Capture codes footage includes a `Capture Card` label; CRT-TV versions visibly add a blue/cyan cast, rounded screen edges/vignette, and more monitor-like texture. The clips are not equal duration, so only qualitative comparison is valid.

### `Vhs cams /vhs-r20/After/ScreenRecording_10-14-2026 17-34-29_1.mov`

- **Measured:** 4.450s; 828×1084; hevc; nominal 60/1 fps; average 1920/67 fps; 128 frames; audio: yes; pixel format: yuvj420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** The Before clip is 1.02 s, 828×1578, HEVC 60 fps, no audio. The After clip is 4.45 s, 828×1084, HEVC 60 fps, stereo AAC. The after sample looks slightly softened/warmer with stronger analog noise/texture. The input and output have different dimensions and durations; they cannot support frame-by-frame quantitative claims. User notes request separate audio controls and stronger noise/muffling behavior, but the recording alone does not prove which sound changes are from the effect versus the source capture.

### `Vhs cams /vhs-r20/Before/ScreenRecording_10-14-2026 17-34-14_1.mov`

- **Measured:** 1.017s; 828×1578; hevc; nominal 60/1 fps; average 1800/61 fps; 30 frames; audio: no; pixel format: yuvj420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** The Before clip is 1.02 s, 828×1578, HEVC 60 fps, no audio. The After clip is 4.45 s, 828×1084, HEVC 60 fps, stereo AAC. The after sample looks slightly softened/warmer with stronger analog noise/texture. The input and output have different dimensions and durations; they cannot support frame-by-frame quantitative claims. User notes request separate audio controls and stronger noise/muffling behavior, but the recording alone does not prove which sound changes are from the effect versus the source capture.

### `overlays as effects/Model 1 .mp4`

- **Measured:** 1.528s; 848×476; h264; nominal 100/3 fps; average 30600/917 fps; 51 frames; audio: no; pixel format: yuv420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** 1.528 s, H.264, 848×476, 51 frames, no audio, no alpha. Frames show black/dark field with white horizontal tracking/static and colored chroma noise, plus `EP`/timecode-like OSD in a portion. Best treated as a tape-signal texture; normal mode must preserve black.

### `overlays as effects/Model 2 .mp4`

- **Measured:** 0.150s; 848×636; h264; nominal 20/1 fps; average 20/1 fps; 3 frames; audio: no; pixel format: yuv420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** 0.150 s, H.264, 848×636, only 3 frames, no audio, no alpha. Very short saturated red/green/blue/magenta tearing/static. A loop may stutter unless its 3-frame duration is intentional; not enough frames to prove a seamless loop.

### `overlays as effects/Model 3 .mp4`

- **Measured:** 1.758s; 848×652; h264; nominal 25/2 fps; average 2640/211 fps; 22 frames; audio: no; pixel format: yuv420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** 1.758 s, H.264, 848×652, 22 frames, no audio, no alpha. Strong horizontal colored tracking bands, a moving/wavy vertical tear and saturated signal breakup. Loop continuity needs a separate test.

### `overlays as effects/Model 4 .mp4`

- **Measured:** 1.798s; 848×476; h264; nominal 100/3 fps; average 36000/1079 fps; 60 frames; audio: no; pixel format: yuv420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** 1.798 s, H.264, 848×476, 60 frames, no audio, no alpha. Mostly black background with intermittent broad horizontal corruption, white noise, and colored fragments. Suitable for keyed/Screen-like integration only as an option; Normal must keep black.

### `overlays as effects/Model 5 .mp4`

- **Measured:** 1.678s; 848×634; h264; nominal 25/1 fps; average 25200/1007 fps; 42 frames; audio: no; pixel format: yuv420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** 1.678 s, H.264, 848×634, 42 frames, no audio, no alpha. Blocky multicolor bars and a distorted silhouette/subject, mixing digital corruption with analog-looking signal damage. Different from line-only noise.

### `overlays as effects/More than 12+ modesl for vhs effect .mov`

- **Measured:** 63.300s; 1280×720; h264; nominal 30/1 fps; average 30/1 fps; 1899 frames; audio: yes; pixel format: yuv420p; alpha: not present in reported pixel format.
- **Observation / interpretation:** 63.30 s, H.264, 1280×720, 30 fps, stereo AAC, no alpha. Sampled every 2 s: approx 0–16 s dark PLAY display and bottom tracking; 18–26 s fast-forward/FF with horizontal noise; 28–34 s REWIND display and increasing snow; 36–42 s dense monochrome snow; 44–50 s horizontal interference/color bars/strong waveform distortion; 52–56 s near-black signal loss with sparse horizontal bursts; 58–60 s fine static; around 62–63 s prominent bottom tracking/bands. It contains a visible stock watermark, so it should remain a reference and must not be redistributed as a bundled production asset.

## Important limitations of the evidence

- Screen recordings include UI overlays, crop changes, and sometimes split-screen comparisons. Those can affect apparent color, framing, timing, and sound.
- A codec/pixel format without alpha means there is no encoded alpha channel in these files; black backgrounds are opaque pixels and must not be removed automatically.
- File duration, codec, frame rate and audio-stream presence are measured. Camera model characteristics and the cause of audio changes are interpretations unless independently verified.
- The long overlay demo has a visible stock watermark and is reference-only. Do not package it as a redistributable plugin asset.
- The supplied AEX binaries are x64 Windows DLLs. Binary strings/imports can reveal controls, dependencies and embedded shader/source-like strings, but do not prove complete runtime behavior or recover original source.
