<#
.SYNOPSIS
    Builds the VHS6RIFT native Adobe After Effects plug-in on Windows x64.
.DESCRIPTION
    Validates project architecture, locates and validates the Adobe AE SDK,
    configures CMake with Visual Studio 2022, and builds the Release VHS6RIFT.aex.
.PARAMETER SdkRoot
    Optional path to the Adobe After Effects SDK.
    If omitted, checks $env:AE_SDK_ROOT, $env:AE_SDK_PATH, and extern/AfterEffectsSDK.
#>
param(
    [string]$SdkRoot = ""
)

$ErrorActionPreference = 'Stop'

Write-Host "================================================================================" -ForegroundColor Cyan
Write-Host "  VHS6RIFT NATIVE BUILD SYSTEM — WINDOWS x64 / AFTER EFFECTS 23.x" -ForegroundColor Cyan
Write-Host "================================================================================" -ForegroundColor Cyan

# Step 1: Run static project validator and functional tests
Write-Host "`n[1/4] Running Static Architecture & Parameter Wiring Validation..." -ForegroundColor Yellow
python tools/validate_project.py
if ($LASTEXITCODE -ne 0) { throw 'Static project validation failed.' }

python tools/audit_parameter_wiring.py
if ($LASTEXITCODE -ne 0) { throw 'Parameter wiring audit failed.' }

python tools/test_vhs6rift.py
if ($LASTEXITCODE -ne 0) { throw 'Functional test suite failed.' }

# Step 2: Discover and validate Adobe After Effects SDK
Write-Host "`n[2/4] Detecting and Validating Adobe After Effects SDK..." -ForegroundColor Yellow
$FindSdkScript = Join-Path $PSScriptRoot "tools\find_ae_sdk.ps1"
$SdkInfo = & $FindSdkScript -CandidatePath $SdkRoot

if (-not $SdkInfo -or -not $SdkInfo.IsValid) {
    throw "Adobe AE SDK detection failed. See diagnostic instructions above or refer to README.md."
}

$ResolvedSdkRoot = $SdkInfo.SdkRoot
Write-Host "Using Verified SDK Root: $ResolvedSdkRoot" -ForegroundColor Green

# Step 3: Configure CMake with Visual Studio 2022
Write-Host "`n[3/4] Configuring CMake (MSVC 2022 x64)..." -ForegroundColor Yellow
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 "-DAE_SDK_ROOT=$ResolvedSdkRoot"
if ($LASTEXITCODE -ne 0) { throw 'CMake configure failed.' }

# Step 4: Build Release Binary
Write-Host "`n[4/4] Building Release VHS6RIFT.aex..." -ForegroundColor Yellow
cmake --build build --config Release --parallel
if ($LASTEXITCODE -ne 0) { throw 'MSBuild compilation failed.' }

$files = @(Get-ChildItem build -Recurse -Filter "VHS6RIFT.aex")
if ($files.Count -ne 1) {
    throw "Expected exactly one VHS6RIFT.aex build artifact, found $($files.Count)."
}

Write-Host "`n================================================================================" -ForegroundColor Green
Write-Host "  BUILD SUCCESSFUL: $($files[0].FullName)" -ForegroundColor Green
Write-Host "  File Size: $([math]::Round($files[0].Length / 1KB, 1)) KB" -ForegroundColor Green
Write-Host "================================================================================" -ForegroundColor Green
