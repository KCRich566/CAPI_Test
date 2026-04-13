# CMakePreset.md

CMakePresets.json defines reusable configure/build presets.  
CMakePresets.json 定義可重用的 configure/build presets。

Presets include generator, binaryDir, architecture, and cacheVariables.  
Presets 包含生成器、輸出目錄、架構與快取變數。

---

## Purpose

Use presets for consistent and repeatable builds across environments.  
使用 presets 可在不同環境中達到一致且可重現的建置流程。

They reduce manual CMake configuration errors.  
它們可減少手動設定 CMake 時的錯誤。

---

## Common Platform Presets

Use presets for common platforms: windows-x64, windows-x86, windows-arm64, wsl, linux.  
為常見平台使用 presets：windows-x64、windows-x86、windows-arm64、wsl、linux。

Each preset defines architecture-specific compiler and build settings.  
每個 preset 都會定義架構專屬的編譯器與建置設定。

---

## Generator

Generator "Ninja" is used for fast lightweight builds in this repo.  
在此倉庫中使用 "Ninja" 作為生成器以達到快速輕量的建置。

Ninja improves incremental build speed compared to traditional generators.  
與傳統生成器相比，Ninja 可提升增量建置速度。

---

## Typical Fields

Presets may include:

- generator  
- binaryDir  
- architecture  
- cacheVariables  

Presets 可能包含：

- 生成器  
- 輸出目錄  
- 架構設定  
- 快取變數  

---

## Benefits

Presets standardize build environments across teams and CI pipelines.  
Presets 可標準化團隊與 CI pipeline 的建置環境。

They make switching between platforms fast and reliable.  
它們能讓不同平台之間的切換快速且可靠。

They reduce the need for long manual CMake command lines.  
它們減少了手動輸入冗長 CMake 指令的需求。