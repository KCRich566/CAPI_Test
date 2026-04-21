# CMAKELists.md

CMake controls build configuration and targets.

This project uses C++20 and the Ninja generator.

---

## Core Build Setup

List source files explicitly and set the visibility preset for shared libraries.

add_library(SHARED) creates a dynamic library (.dll/.so); add_library(STATIC) creates a static archive (.lib/.a).

Use target_link_libraries to express dependencies between targets.

---

## Include & Definitions

Use target_include_directories and target_compile_definitions for public headers and compile-time definitions.

---

## Output Configuration

Set runtime and library output directories so executables and DLLs are colocated.

This ensures runtime dependencies can be resolved without manual copying.

---

## Presets & Configuration Management

Presets simplify multi-architecture builds and provide consistent configuration.

They help standardize builds across developers and CI systems.

---

## Shared Library Exporting

The GenerateExportHeader module can auto-generate dllexport/dllimport macros for shared libraries.

This improves portability between Windows DLL builds and cross-platform shared libraries.

---

## Installation Rules

Use install() commands to define deployment rules for headers, libraries, and executables.

This enables packaging and system-wide installation of built artifacts.
