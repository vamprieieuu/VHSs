# Binary Reference Inspection

This is static inspection only. None of the reference AEX files were executed by this audit environment.

| File | Size | SHA-256 | Static result |
|---|---:|---|---|
| `plugin work/ntsc-rs-ae.aex` | 1,881,600 | `8aab0fdc8e10567cbb0d82e88fef95dffd05e2c411b1ee63c8c2c5b826597f51` | /tmp/vhsstudy/VHS MAKER/plugin work/ntsc-rs-ae.aex: PE32+ executable for MS Windows 6.00 (DLL), x86-64, 6 sections |
| `plugin work/signal.aex` | 692,224 | `4180d0277eb6d12520e168a112743c6f4a81487dce4f2a351b5e1b2bbce5818f` | /tmp/vhsstudy/VHS MAKER/plugin work/signal.aex: PE32+ executable for MS Windows 6.00 (DLL), x86-64, 7 sections |
| `plugin work/VHSCraft.aex` | 2,011,136 | `28c8f13077bceb7095a5ba5716c74aef26364f6ded45f47947c2065ff89dc1e1` | /tmp/vhsstudy/VHS MAKER/plugin work/VHSCraft.aex: PE32+ executable for MS Windows 6.00 (DLL), x86-64, 6 sections |
| `plugin work/VHS-Legend-repo-FINAL/reference/VHS LEGEND.aex` | 1,164,288 | `78c8d09f54b5a44cd264b28f9aee442dd3a183662aa096cc64a8e15f5b790662` | /tmp/vhsstudy/VHS MAKER/plugin work/VHS-Legend-repo-FINAL/reference/VHS LEGEND.aex: PE32+ executable for MS Windows 6.00 (DLL), x86-64, 6 sections |

## `plugin work/ntsc-rs-ae.aex`

- Imported DLLs: api-ms-win-core-synch-l1-2-0.dll, bcryptprimitives.dll, kernel32.dll, ole32.dll, shell32.dll, ntdll.dll, VCRUNTIME140.dll, api-ms-win-crt-math-l1-1-0.dll, api-ms-win-crt-string-l1-1-0.dll, api-ms-win-crt-runtime-l1-1-0.dll, api-ms-win-crt-heap-l1-1-0.dll
- Relevant embedded strings: Tracking noise, Chroma delay, Head switching, Random seed
- Embedded strings identify separate signal-model controls including tracking noise, head switching, chroma delay, chroma vertical blend, edge wave, luma/chroma noise and VHS tape speed. The official project is open source and documents its algorithmic approach; this binary alone is not the authoritative source.

## `plugin work/signal.aex`

- Imported DLLs: KERNEL32.dll, USER32.dll, GDI32.dll, ADVAPI32.dll, SHELL32.dll, WININET.dll, WS2_32.dll, IPHLPAPI.DLL
- Relevant embedded strings: Tape Errors, Signal strength, Signal amplification, Online activation, Random seed
- Strings include `ADBE Signal`, `Signal strength`, `Signal amplification`, `Luma noise`, `Chroma noise`, `Hue Noise`, `Tape Errors`, `Chroma loss`, `Random seed`, and an online activation error message. Activation behavior should not be bypassed; the plugin is a behavioral reference.

## `plugin work/VHSCraft.aex`

- Imported DLLs: d3d12.dll, dxgi.dll, KERNEL32.dll, USER32.dll, GDI32.dll, ADVAPI32.dll, SHELL32.dll, MSVCP140.dll, WININET.dll, VCRUNTIME140_1.dll, VCRUNTIME140.dll, api-ms-win-crt-string-l1-1-0.dll, api-ms-win-crt-heap-l1-1-0.dll, api-ms-win-crt-convert-l1-1-0.dll, api-ms-win-crt-environment-l1-1-0.dll, api-ms-win-crt-math-l1-1-0.dll, api-ms-win-crt-runtime-l1-1-0.dll, api-ms-win-crt-stdio-l1-1-0.dll, api-ms-win-crt-utility-l1-1-0.dll, api-ms-win-crt-filesystem-l1-1-0.dll, api-ms-win-crt-time-l1-1-0.dll, WS2_32.dll, IPHLPAPI.DLL
- Relevant embedded strings: Moving Band Noise, Bottom Damage, Color Blotches, Color Recovery, Color Shift Speed, shader, CUDA, DirectX, Online activation
- The binary contains many shader-like/code-like strings naming separate computations such as tracking interference, RF amplitude, head-switching, dropouts, chroma smear/delay, grain/noise and color grading. This is evidence of an implementation with distinct processing stages, but does not establish their full runtime order or exact UI mapping.
- Imports include D3D12/DXGI and standard Windows networking/runtime libraries.

## `plugin work/VHS-Legend-repo-FINAL/reference/VHS LEGEND.aex`

- Imported DLLs: WINMM.dll, WS2_32.dll, nvcuda.dll, KERNEL32.dll, MSVCP140.dll, MSVCP140_ATOMIC_WAIT.dll, bcrypt.dll, VCRUNTIME140.dll, VCRUNTIME140_1.dll, api-ms-win-crt-string-l1-1-0.dll, api-ms-win-crt-heap-l1-1-0.dll, api-ms-win-crt-time-l1-1-0.dll, api-ms-win-crt-stdio-l1-1-0.dll, api-ms-win-crt-filesystem-l1-1-0.dll, api-ms-win-crt-runtime-l1-1-0.dll, api-ms-win-crt-math-l1-1-0.dll, api-ms-win-crt-environment-l1-1-0.dll, api-ms-win-crt-locale-l1-1-0.dll, api-ms-win-crt-convert-l1-1-0.dll
- Relevant embedded strings: Tracking Speed, Tracking Size, Tracking Distortion, Signal Blur, Tracking noise, Chroma delay, Crates VHS Master, CratesVHSMaster, shader, Vulkan, CUDA, DirectX
- Internal strings identify `Crates VHS Master` / `CratesVHSMaster`, `CratesVHSMaster_shaders`, ONNX model folders, Vulkan, CUDA and DirectX GPU paths. This is strong evidence that the file is not reliably identified as VHS Legend by its external filename; it appears internally associated with Crates VHS Master. It has GPU/runtime dependencies and cannot be assumed to be a portable source template.