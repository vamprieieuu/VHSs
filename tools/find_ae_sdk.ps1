<#
.SYNOPSIS
    Discovers, inspects, and validates the Adobe After Effects SDK installation.
.DESCRIPTION
    Searches candidate paths, environment variables, and nested directory layouts
    for required Adobe AE SDK headers and PiPLtool.exe.
    Provides detailed error diagnostics and CI guidance when missing or incomplete.
#>
param(
    [string]$CandidatePath = "",
    [switch]$FailFast = $false,
    [switch]$Quiet = $false
)

$ErrorActionPreference = "Continue"

$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path

# 1. Compile list of candidate directories to search
$SearchList = [System.Collections.Generic.List[string]]::new()

if (-not [string]::IsNullOrWhiteSpace($CandidatePath)) {
    $SearchList.Add($CandidatePath)
}
if (-not [string]::IsNullOrWhiteSpace($env:AE_SDK_ROOT)) {
    $SearchList.Add($env:AE_SDK_ROOT)
}
if (-not [string]::IsNullOrWhiteSpace($env:AE_SDK_PATH)) {
    $SearchList.Add($env:AE_SDK_PATH)
}

# Standard repository conventions
$SearchList.Add((Join-Path $RepoRoot "extern\AfterEffectsSDK"))
$SearchList.Add((Join-Path $RepoRoot "AfterEffectsSDK"))

# Search for any unzipped SDK folders under extern
$ExternDir = Join-Path $RepoRoot "extern"
if (Test-Path $ExternDir) {
    Get-ChildItem -Path $ExternDir -Directory -ErrorAction SilentlyContinue | ForEach-Object {
        if ($_.Name -like "*After*Effects*" -or $_.Name -like "*SDK*") {
            $SearchList.Add($_.FullName)
            Get-ChildItem -Path $_.FullName -Directory -ErrorAction SilentlyContinue | ForEach-Object {
                $SearchList.Add($_.FullName)
            }
        }
    }
}

$RequiredHeaders = @(
    "AE_Effect.h",
    "AE_EffectCB.h",
    "AE_EffectCBSuites.h",
    "AE_EffectUI.h",
    "AE_Macros.h",
    "Param_Utils.h"
)

$CheckedLocations = [System.Collections.Generic.List[string]]::new()
$DiscoveredSdk = $null

foreach ($cand in $SearchList) {
    if ([string]::IsNullOrWhiteSpace($cand)) { continue }
    $norm = [System.IO.Path]::GetFullPath($cand)
    if ($CheckedLocations.Contains($norm)) { continue }
    $CheckedLocations.Add($norm)

    if (-not (Test-Path $norm)) {
        continue
    }

    # Locate AE_Effect.h: direct, Examples/Headers, or recursive
    $effectHeader = $null
    $direct = Join-Path $norm "Headers\AE_Effect.h"
    $examples = Join-Path $norm "Examples\Headers\AE_Effect.h"

    if (Test-Path $direct) {
        $effectHeader = Get-Item $direct
    } elseif (Test-Path $examples) {
        $effectHeader = Get-Item $examples
    } else {
        # Search recursively up to 4 levels deep for nested zip extractions
        $effectHeader = Get-ChildItem -Path $norm -Filter "AE_Effect.h" -File -Recurse -Depth 4 -ErrorAction SilentlyContinue | Select-Object -First 1
    }

    if ($effectHeader) {
        $headersDir = $effectHeader.Directory.FullName
        $includeRoot = $effectHeader.Directory.Parent.FullName

        if ($effectHeader.Directory.Parent.Name -eq "Examples") {
            $sdkRoot = $effectHeader.Directory.Parent.Parent.FullName
        } else {
            $sdkRoot = $includeRoot
        }

        # Check required headers
        $missingHeaders = @()
        foreach ($h in $RequiredHeaders) {
            $hPath = Join-Path $headersDir $h
            if (-not (Test-Path $hPath)) {
                $missingHeaders += $h
            }
        }

        # Locate PiPLtool.exe
        $piplCandidate = $null
        $piplLocations = @(
            (Join-Path $sdkRoot "Resources\PiPLtool.exe"),
            (Join-Path $includeRoot "Resources\PiPLtool.exe"),
            (Join-Path $sdkRoot "Examples\Resources\PiPLtool.exe")
        )
        foreach ($loc in $piplLocations) {
            if (Test-Path $loc) {
                $piplCandidate = (Get-Item $loc).FullName
                break
            }
        }

        if (-not $piplCandidate) {
            $foundPipl = Get-ChildItem -Path $sdkRoot -Filter "PiPLtool.exe" -File -Recurse -Depth 5 -ErrorAction SilentlyContinue | Select-Object -First 1
            if ($foundPipl) {
                $piplCandidate = $foundPipl.FullName
            }
        }

        if (-not $piplCandidate -and $norm -ne $sdkRoot) {
            $foundPipl = Get-ChildItem -Path $norm -Filter "PiPLtool.exe" -File -Recurse -Depth 5 -ErrorAction SilentlyContinue | Select-Object -First 1
            if ($foundPipl) {
                $piplCandidate = $foundPipl.FullName
            }
        }

        $isValid = ($missingHeaders.Count -eq 0) -and ($null -ne $piplCandidate)

        $DiscoveredSdk = [PSCustomObject]@{
            IsValid         = $isValid
            SdkRoot         = $sdkRoot
            IncludeRoot     = $includeRoot
            HeadersDir      = $headersDir
            PiPLtool        = $piplCandidate
            MissingHeaders  = $missingHeaders
            MissingPiPLtool = ($null -eq $piplCandidate)
        }

        if ($isValid) {
            break
        }
    }
}

if ($DiscoveredSdk -and $DiscoveredSdk.IsValid) {
    if (-not $Quiet) {
        Write-Host "================================================================================" -ForegroundColor Green
        Write-Host "  ADOBE AFTER EFFECTS SDK DETECTED & VALIDATED" -ForegroundColor Green
        Write-Host "================================================================================" -ForegroundColor Green
        Write-Host "  SDK Root:     $($DiscoveredSdk.SdkRoot)"
        Write-Host "  Headers:      $($DiscoveredSdk.HeadersDir)"
        Write-Host "  PiPLtool:     $($DiscoveredSdk.PiPLtool)"
        Write-Host "  Status:       Ready for MSVC / CMake Windows Build"
        Write-Host "================================================================================"
    }
    # Export environment variables for downstream CMake and MSBuild steps
    $env:AE_SDK_ROOT = $DiscoveredSdk.SdkRoot
    $env:AE_INCLUDE_ROOT = $DiscoveredSdk.IncludeRoot
    $env:AE_HEADERS = $DiscoveredSdk.HeadersDir
    $env:PIPL_TOOL = $DiscoveredSdk.PiPLtool

    return $DiscoveredSdk
} else {
    # Generate actionable error diagnosis
    $errBanner = @"
================================================================================
[ERROR] ADOBE AFTER EFFECTS SDK IS UNAVAILABLE OR INCOMPLETE
================================================================================
The VHS6RIFT native plugin build requires the licensed Adobe After Effects SDK
to compile the native .aex binary and process PiPL resources.

CHECKED DIRECTORIES:
"@
    foreach ($p in $CheckedLocations) {
        $existsStr = if (Test-Path $p) { "EXISTS (headers missing)" } else { "NOT FOUND" }
        $errBanner += "`n  - $p [$existsStr]"
    }

    if ($DiscoveredSdk) {
        if ($DiscoveredSdk.MissingHeaders.Count -gt 0) {
            $errBanner += "`n`nMISSING REQUIRED HEADERS in $($DiscoveredSdk.HeadersDir):"
            foreach ($mh in $DiscoveredSdk.MissingHeaders) {
                $errBanner += "`n  - $mh"
            }
        }
        if ($DiscoveredSdk.MissingPiPLtool) {
            $errBanner += "`n`nMISSING REQUIRED TOOL:"
            $errBanner += "`n  - PiPLtool.exe (searched under $($DiscoveredSdk.SdkRoot))"
        }
    }

    $errBanner += @"

================================================================================
HOW TO CONFIGURE THE ADOBE AE SDK:
================================================================================
Adobe licensing terms prohibit distributing proprietary SDK headers (AE_Effect.h)
and binaries (PiPLtool.exe) in a public git repository.

A. FOR GITHUB ACTIONS WORKFLOWS (Choose one of the following):

  Option 1 (Recommended): Repository Secret 'AE_SDK_ZIP_BASE64'
    1. Download the Adobe After Effects SDK (2022/2023/2024) from Adobe Developer:
       https://developer.adobe.com/after-effects/
    2. Zip the SDK and convert it to a Base64 string in PowerShell:
       `$base64 = [Convert]::ToBase64String([IO.File]::ReadAllBytes('AfterEffectsSDK.zip'))
       `$base64 | Set-Clipboard
    3. In your GitHub repository:
       Go to Settings -> Secrets and variables -> Actions -> New repository secret
       Name:  AE_SDK_ZIP_BASE64
       Value: <Paste the base64 string>
    The CI workflow will automatically decode, cache, and extract the SDK!

  Option 2: Private Secure URL Secret 'AE_SDK_URL'
    If you host your licensed SDK in private cloud storage (S3/GCS/Azure),
    add a repository secret:
       Name:  AE_SDK_URL
       Value: <Private signed URL to AfterEffectsSDK.zip>

  Option 3: Self-Hosted Runner Environment Variable
    Configure a runner with the SDK and set system environment variable:
       AE_SDK_ROOT = C:\SDKs\AfterEffectsSDK

B. FOR LOCAL WINDOWS / POWERSHELL BUILDS:
  Run build.ps1 with an explicit SDK path or set the environment variable:
    `$env:AE_SDK_ROOT = "C:\Path\To\AfterEffectsSDK"
    .\build.ps1 -SdkRoot "C:\Path\To\AfterEffectsSDK"

SUPPORTED SDK DIRECTORY LAYOUTS:
  Layout 1 (Standard):
    <AE_SDK_ROOT>\Headers\AE_Effect.h
    <AE_SDK_ROOT>\Resources\PiPLtool.exe

  Layout 2 (SDK Examples Folder):
    <AE_SDK_ROOT>\Examples\Headers\AE_Effect.h
    <AE_SDK_ROOT>\Examples\Resources\PiPLtool.exe

  Layout 3 (Nested Extraction):
    <AE_SDK_ROOT>\Adobe After Effects * SDK\Headers\AE_Effect.h
================================================================================
"@

    Write-Host $errBanner -ForegroundColor Red

    if ($FailFast) {
        throw "Adobe AE SDK validation failed. See diagnosis above."
    }

    return $null
}
