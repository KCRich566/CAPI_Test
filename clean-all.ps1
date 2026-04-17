#!/usr/bin/env pwsh

# command
## powershell.exe -ExecutionPolicy Bypass -File .\clean-all.ps1
## Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
<#
.SYNOPSIS
    Clean all build artifacts (MSBuild, CMake, dotnet) and restore a pristine state.

.DESCRIPTION
    Deletes the following directories/files:
      - MSBuild C++ outputs:  ARM64\, x64\, x86\, Debug\, Release\
      - CMake outputs:        out\build\
      - C# outputs:           CSharpTest\bin\, CSharpTest\obj\
      - Runtime artifacts:    history.txt, sobel_output.png, *.ilk, *.pdb (root directory)
    Does NOT delete source code, thirdparty, image.jpg, etc.

.EXAMPLE
    .\clean-all.ps1          # Default mode, actually deletes files
    .\clean-all.ps1 -DryRun  # Show what would be deleted without removing anything
#>

param(
    [switch]$DryRun
)

$ErrorActionPreference = 'SilentlyContinue'
$root = $PSScriptRoot
if (-not $root) { $root = Get-Location }
Push-Location $root

function Remove-Path {
    param([string]$Path, [string]$Label)
    if (Test-Path $Path) {
        if ($DryRun) {
            Write-Host "[DryRun] Would remove: $Label ($Path)" -ForegroundColor Yellow
        } else {
            Remove-Item $Path -Recurse -Force
            Write-Host "Removed: $Label" -ForegroundColor Green
        }
    }
}

Write-Host "=== Cleaning all build artifacts ===" -ForegroundColor Cyan
Write-Host ""

# --- MSBuild C++ outputs (solution build: <SolutionDir>\<Platform>\<Configuration>) ---
Write-Host "[MSBuild C++ Outputs]" -ForegroundColor Magenta
foreach ($platform in @("ARM64", "x64", "x86", "Win32")) {
    Remove-Path "$root\$platform" "MSBuild output: $platform\"
}
foreach ($config in @("Debug", "Release")) {
    Remove-Path "$root\$config" "MSBuild output: $config\"
}

# --- CMake outputs ---
Write-Host "[CMake Outputs]" -ForegroundColor Magenta
Remove-Path "$root\out\build" "CMake build: out\build\"
# Remove out\ if empty
if ((Test-Path "$root\out") -and -not (Get-ChildItem "$root\out" -ErrorAction SilentlyContinue)) {
    Remove-Path "$root\out" "CMake empty directory: out\"
}

# --- project outputs ---
Write-Host "[project Outputs]" -ForegroundColor Magenta
foreach ($platform in @("ARM64", "x64", "x86","WIN32")) {
    Remove-Path "$root\CSharpTest\$platform" "MSBuild output: CSharpTest\$platform\"
    Remove-Path "$root\CPPTest\$platform" "MSBuild output: CPPTest\$platform\"
    Remove-Path "$root\SobelDLL\$platform" "MSBuild output: SobelDLL\$platform\"
    Remove-Path "$root\CalculatorDLL\$platform" "MSBuild output: CalculatorDLL\$platform\"
}

Write-Host "[Runtime Artifacts]" -ForegroundColor Magenta
Remove-Path "$root\CSharpTest\bin"      "CSharpTest\bin"
Remove-Path "$root\CSharpTest\obj"      "CSharpTest\obj"

# --- Runtime generated files ---
Write-Host "[Runtime Artifacts]" -ForegroundColor Magenta
Remove-Path "$root\history.txt"      "history.txt"
Remove-Path "$root\sobel_output.png" "sobel_output.png"

# --- Remove generated folder
Write-Host "[Runtime Artifacts]" -ForegroundColor Magenta
Remove-Path "$root\generated"      "generated\"


Write-Host ""
if ($DryRun) {
    Write-Host "=== DryRun complete, nothing was deleted ===" -ForegroundColor Yellow
} else {
    Write-Host "=== Clean complete ===" -ForegroundColor Green
}

Pop-Location