# generate_version_h.ps1
# Generates generated/version.h from version.txt for MSBuild builds.
# CMake builds use configure_file() instead; both output to the same file.
#
# 從 version.txt 產生 generated/version.h（給 MSBuild 建置使用）。
# CMake 建置改用 configure_file()，兩者輸出同一個檔案。

param(
    [Parameter(Mandatory)][string]$VersionFile,
    [Parameter(Mandatory)][string]$OutputFile
)

$version = (Get-Content $VersionFile -Raw).Trim()
$parts   = $version.Split('.')
$major   = $parts[0]
$minor   = $parts[1]
$patch   = $parts[2]

$content = @"
#pragma once

/*
 * version.h  (auto-generated from version.txt — do NOT edit)
 *
 * Shared version macros for all native DLLs in this project.
 * 由建置系統從 version.txt 自動產生，請勿手動編輯。
 */

#define CAPI_VERSION_MAJOR  $major
#define CAPI_VERSION_MINOR  $minor
#define CAPI_VERSION_PATCH  $patch
#define CAPI_VERSION_STRING "$version"
"@

$dir = Split-Path $OutputFile -Parent
if (!(Test-Path $dir)) { New-Item -ItemType Directory -Path $dir -Force | Out-Null }

# Write UTF-8 without BOM (C++ compilers prefer no BOM)
[System.IO.File]::WriteAllText($OutputFile, $content, [System.Text.UTF8Encoding]::new($false))

Write-Host "Generated $OutputFile (version $version)"
