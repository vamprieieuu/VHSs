param(
  [string]$SdkRoot = (Join-Path $PSScriptRoot 'extern\AfterEffectsSDK')
)
$ErrorActionPreference = 'Stop'
if (-not (Test-Path (Join-Path $SdkRoot 'Headers\AE_Effect.h')) -and -not (Test-Path (Join-Path $SdkRoot 'Examples\Headers\AE_Effect.h'))) {
  throw "Adobe AE SDK headers not found under '$SdkRoot'. See README.md."
}
python tools/validate_project.py
if ($LASTEXITCODE -ne 0) { throw 'Static project validation failed.' }
cmake -S . -B build -G 'Visual Studio 17 2022' -A x64 "-DAE_SDK_ROOT=$SdkRoot"
if ($LASTEXITCODE -ne 0) { throw 'CMake configure failed.' }
cmake --build build --config Release --parallel
if ($LASTEXITCODE -ne 0) { throw 'MSBuild failed.' }
$files = Get-ChildItem build -Recurse -Filter VHS6RIFT.aex
if ($files.Count -ne 1) { throw "Expected exactly one VHS6RIFT.aex, found $($files.Count)." }
Write-Host "Build output: $($files[0].FullName)"
