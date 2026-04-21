# Compiler

CMake is the high-level build tool used to manage builds.

# Common Build Tools

# High-level (modern)

Describe what to build.

## 1) CMake

CMake is widely used and cross-platform.

Features:
- Cross-platform
- Generates project files: `.vcxproj`, `Makefile`, `build.ninja`

## 2) Meson

- Clean syntax
- Often paired with Ninja

## 3) Bazel

- Strong dependency management
- Suitable for very large projects

# Low-level (execution)

Describes how to build.

## 4) Make

Classic build tool.

## 5) Ninja

Fast executor, commonly used with CMake.

## 6) MSBuild

Used on Windows for Visual Studio projects.

# C / C++ common compilers

## GCC

- Widely used on Linux
- Open source
- Typical commands:

```
gcc main.c -o main
g++ main.cpp -o main
```

## Clang

- Based on LLVM
- Clear diagnostics

## MSVC

- Windows compiler integrated with Visual Studio

# C# / .NET compiler

## Roslyn

C# compiler used by .NET

# Python

CPython executes .py files by compiling to bytecode (.pyc) and running an interpreter.