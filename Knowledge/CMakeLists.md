# CMAKELists.md

CMake controls build configuration and targets.  
CMake 控制建置設定與目標。

This project uses C++20 and Ninja generator.  
此專案使用 C++20 與 Ninja 生成器。

---

## Core Build Setup

List source files explicitly and set visibility preset for shared libraries.  
明確列出來源檔並設定 shared library 的可見性預設。

add_library(SHARED) creates a dynamic library (.dll/.so); add_library(STATIC) creates a static archive (.lib/.a).  
add_library(SHARED) 建立動態函式庫（.dll/.so）；add_library(STATIC) 建立靜態檔案庫（.lib/.a）。

Use target_link_libraries to express dependencies between targets.  
使用 target_link_libraries 表達目標之間的相依關係。

---

## Include & Definitions

Use target_include_directories and target_compile_definitions for public headers and defines.  
使用 target_include_directories 與 target_compile_definitions 設定公開標頭與宏定義。

---

## Output Configuration

Set runtime and library output directories so executables and DLLs are colocated.  
設定執行檔與函式庫輸出目錄以便執行時能互相找到。

This ensures runtime dependencies can be resolved without manual copying.  
這可確保執行時相依性可被正確解析，無需手動複製檔案。

---

## Presets & Configuration Management

Presets simplify multi-architecture builds and consistent configuration.  
Presets 簡化多架構建置與一致性配置。

They help standardize builds across developers and CI systems.  
它們有助於在開發者與 CI 系統之間標準化建置流程。

---

## Shared Library Exporting

GenerateExportHeader module can auto-generate dllexport/dllimport macros for shared libraries.  
GenerateExportHeader 模組可為共享函式庫自動產生 dllexport/dllimport 巨集。

This improves portability between Windows DLL builds and cross-platform shared libraries.  
這能提升 Windows DLL 與跨平台共享函式庫的可移植性。

---

## Installation Rules

Use install() commands to define deployment rules for headers, libraries, and executables.  
使用 install() 命令定義標頭、函式庫與執行檔的部署規則。

This enables packaging and system-wide installation of built artifacts.  
這可支援打包與系統級安裝建置產物。