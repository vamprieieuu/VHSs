# Build and Debug Workflow — Adobe After Effects SDK Integration

## 1. Adobe AE SDK Requirement & Licensing

Adobe requires an Adobe Developer account and license acceptance to obtain the Adobe After Effects SDK (available from [Adobe Developer Console](https://developer.adobe.com/after-effects/)).
Because of Adobe's licensing restrictions, proprietary SDK headers (such as `AE_Effect.h`, `AE_EffectCB.h`) and tools (`PiPLtool.exe`) must not be committed to a public Git repository.

---

## 2. Supported SDK Directory Layouts

The build system (`CMakeLists.txt`, `build.ps1`, `tools/find_ae_sdk.ps1`) automatically discovers and supports three distinct SDK directory layouts:

### Layout 1: Standard Flat Layout
```
<AE_SDK_ROOT>/
├── Headers/
│   ├── AE_Effect.h
│   ├── AE_EffectCB.h
│   ├── AE_EffectCBSuites.h
│   ├── AE_EffectUI.h
│   ├── AE_Macros.h
│   └── Param_Utils.h
└── Resources/
    └── PiPLtool.exe
```

### Layout 2: SDK Examples Folder Layout
```
<AE_SDK_ROOT>/
└── Examples/
    ├── Headers/
    │   ├── AE_Effect.h
    │   └── ...
    └── Resources/
        └── PiPLtool.exe
```

### Layout 3: Nested Extraction Folder Layout
```
<AE_SDK_ROOT>/
└── Adobe After Effects 2023 SDK/
    ├── Headers/
    │   └── AE_Effect.h
    └── ...
```

---

## 3. Configuring the SDK for GitHub Actions CI

The GitHub Actions workflow (`.github/workflows/build.yml`) contains two separate jobs:
1. **`validate` Job:** Runs independently on standard runners without needing the proprietary SDK. It executes static architecture validation (`validate_project.py`), parameter wiring audits (`audit_parameter_wiring.py`), and the 14-gate functional test suite (`test_vhs6rift.py`).
2. **`build` Job:** Compiles the native `VHS6RIFT.aex` using MSVC 2022 on Windows x64.

To enable the `build` job on GitHub Actions, the repository owner can supply the licensed SDK using any of the following methods:

### Option A (Recommended): Base64 Repository Secret `AE_SDK_ZIP_BASE64`
1. Download the Adobe AE SDK zip from Adobe Developer.
2. In PowerShell, encode the zip file to a Base64 string:
   ```powershell
   $base64 = [Convert]::ToBase64String([IO.File]::ReadAllBytes("AfterEffectsSDK.zip"))
   $base64 | Set-Clipboard
   ```
3. In your GitHub repository:
   - Navigate to **Settings** -> **Secrets and variables** -> **Actions**
   - Click **New repository secret**
   - Name: `AE_SDK_ZIP_BASE64`
   - Value: Paste the copied Base64 string.
4. The workflow will automatically decode, cache, and extract the SDK into `extern/AfterEffectsSDK` on the runner.

### Option B: Private Download URL Secret `AE_SDK_URL`
If you store your licensed SDK in private cloud storage (Amazon S3, Google Cloud Storage, or Azure Blob Storage with a presigned URL):
- Name: `AE_SDK_URL`
- Value: `<Secure HTTPS URL to AfterEffectsSDK.zip>`

### Option C: Self-Hosted Runner Environment Variable
On a persistent self-hosted Windows runner, install the SDK and set:
```powershell
[System.Environment]::SetEnvironmentVariable('AE_SDK_ROOT', 'C:\SDKs\AfterEffectsSDK', 'Machine')
```

---

## 4. Local Windows Builds

To build locally on Windows:
```powershell
# Method 1: Using build.ps1 with auto-detection or parameter
.\build.ps1 -SdkRoot "C:\SDKs\AfterEffectsSDK"

# Method 2: Setting environment variable
$env:AE_SDK_ROOT = "C:\SDKs\AfterEffectsSDK"
.\build.ps1

# Method 3: Direct CMake invocation
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DAE_SDK_ROOT="C:\SDKs\AfterEffectsSDK"
cmake --build build --config Release --parallel
```
