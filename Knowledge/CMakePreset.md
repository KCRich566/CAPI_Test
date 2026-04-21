# CMakePreset.md

CMakePresets.json defines reusable configure/build presets.

Presets include generator, binaryDir, architecture, and cacheVariables.

---

## Purpose

Use presets for consistent and repeatable builds across environments.  
使用 presets 可在不同環境中達到一致且可重現的建置流程。

They reduce manual CMake configuration errors.  
它們可減少手動設定 CMake 時的錯誤。

---

## Common Platform Presets

Use presets for common platforms: windows-x64, windows-x86, windows-arm64, wsl, linux.

Each preset defines architecture-specific compiler and build settings.

---

## Generator

Generator "Ninja" is used for fast lightweight builds in this repo.

Ninja improves incremental build speed compared to traditional generators.

---

## Typical Fields

Presets may include:

- generator
- binaryDir
- architecture
- cacheVariables

---

## Benefits

Presets standardize build environments across teams and CI pipelines.

They make switching between platforms fast and reliable.

They reduce the need for long manual CMake command lines.
