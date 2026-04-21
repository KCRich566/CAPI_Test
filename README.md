# CAPI_Test

C API calculator with temporary memory and persistent history (keeps last 10 results), supporting add, sub, mul, div.


Relationship between CMakePresets.json, CMakeLists.txt and Visual Studio IDE:
When you save changes, Visual Studio detects modifications to CMakePresets.json and automatically reloads the project configuration.
This means editing presets (for example: generator, build type, platform) will cause Visual Studio to refresh the project settings accordingly.
If no CMakePresets.json is present, Visual Studio will fall back to default CMake behavior. With presets, you gain finer control over the configuration and Visual Studio adapts automatically.
If you prefer to disable automatic reload, adjust the CMake-related option in Visual Studio settings: "Automatically reload project when CMakePresets.json changes".
Use the CMake Targets View to switch presets and build configurations without manually editing CMakePresets.json.
## Project Structure

| Path | Description |
|---|---|
| `CalculatorDLL/calculator.h` | Pure-C public header: export macro, error codes, opaque-handle C API |
| `CalculatorDLL/calculator.cpp` | C++ internal implementation + C API wrappers |
| `CalculatorTest/CalculatorTest.cpp` | C++ test / demo executable (uses C API) |
| `CalculatorCSharp/` | C# P/Invoke example (calls the native DLL) |

## Build & Run

### Prerequisites

- CMake ≥ 3.16
- A C++20-capable compiler (MSVC, GCC, or Clang)
- Ninja (recommended generator)
- .NET 10 SDK (optional, for the C# example)

### Available Presets

| Preset | Platform | Arch | Build Type |
|---|---|---|---|
| `windows-x64-debug` | Windows | x64 | Debug |
| `windows-x64-release` | Windows | x64 | Release |
| `windows-x86-debug` | Windows | x86 | Debug |
| `windows-x86-release` | Windows | x86 | Release |
| `windows-arm64-debug` | Windows | ARM64 | Debug |
| `windows-arm64-release` | Windows | ARM64 | Release |
| `wsl-x64-debug` | WSL (Linux) | x64 | Debug |
| `wsl-x64-release` | WSL (Linux) | x64 | Release |
| `linux-x64-debug` | Linux | x64 | Debug |
| `linux-x64-release` | Linux | x64 | Release |

> All Windows presets use `"strategy": "external"` — the MSVC toolchain must be
> activated for the target architecture **before** running CMake (see below).

### Windows

The Ninja generator requires the correct MSVC toolchain in `PATH`.
Use `vcvarsall.bat` to select the architecture, then configure & build:

```powershell
# ── ARM64 ──
# (from a plain PowerShell / cmd — not Developer Command Prompt)
cmd /c "`"C:\Program Files\Microsoft Visual Studio\<VS_VERSION>\Community\VC\Auxiliary\Build\vcvarsall.bat`" arm64 && cmake --preset windows-arm64-debug && cmake --build out/build/windows-arm64-debug"
.\out\build\windows-arm64-debug\bin\CalculatorTest.exe

# ── x64 ──
cmd /c "`"C:\Program Files\Microsoft Visual Studio\<VS_VERSION>\Community\VC\Auxiliary\Build\vcvarsall.bat`" x64 && cmake --preset windows-x64-debug && cmake --build out/build/windows-x64-debug"
.\out\build\windows-x64-debug\bin\CalculatorTest.exe

# ── x86 ──
cmd /c "`"C:\Program Files\Microsoft Visual Studio\<VS_VERSION>\Community\VC\Auxiliary\Build\vcvarsall.bat`" x86 && cmake --preset windows-x86-debug && cmake --build out/build/windows-x86-debug"
.\out\build\windows-x86-debug\bin\CalculatorTest.exe
```

> **Tip:** Replace `<VS_VERSION>` with your Visual Studio version number
> (e.g. `2022`, `2026` / `18`). If you're already inside a
> *Developer Command Prompt for VS (ARM64/x64/x86)*, you can skip `vcvarsall`
> and run `cmake --preset ... && cmake --build ...` directly.

### Linux (native)

```bash
cmake --preset linux-x64-debug
cmake --build out/build/linux-x64-debug

./out/build/linux-x64-debug/bin/CalculatorTest
```

# WSL (from Windows host)

```powershell
# Build inside WSL
wsl bash -c "cd /mnt/c/Users/$env:USERNAME/Desktop/Code/CAPI_Test && cmake --preset linux-x64-debug && cmake --build out/build/linux-x64-debug"

# Run inside WSL
wsl bash -c "cd /mnt/c/Users/$env:USERNAME/Desktop/Code/CAPI_Test && ./out/build/linux-x64-debug/bin/CalculatorTest"
```

### C# Example

The C# project automatically copies the native DLL/SO from `NativeBuildDir`.
Default: `windows-arm64-debug` on Windows, `linux-x64-debug` on Linux.
Override with `-p:NativeBuildDir=...` for a different preset.

```powershell
# ── Windows (default preset) ──
dotnet run --project CalculatorCSharp

# ── Windows (different architecture — override NativeBuildDir) ──
dotnet run --project CalculatorCSharp -p:NativeBuildDir=out\build\windows-x64-debug

# ── Linux / WSL ──
# (run from inside WSL or a native Linux shell)
dotnet run --project CalculatorCSharp

# ── WSL from Windows host ──
wsl bash -c "cd /mnt/c/Users/$env:USERNAME/Desktop/Code/CAPI_Test && dotnet run --project CalculatorCSharp"
```

> **Important:** The native DLL architecture must match the .NET runtime architecture.
> For example, an ARM64 DLL cannot be loaded by an x64 .NET process
> (`BadImageFormatException`). Use `dotnet --info` to check your runtime architecture.

## Inspecting the Native Library

### Windows — `dumpbin` (Visual Studio Developer Command Prompt)

```bash
# List exported functions
dumpbin /exports out\build\windows-arm64-debug\bin\CalculatorDLL.dll

# Check target architecture (ARM64 / x64 / x86)
dumpbin /headers out\build\windows-arm64-debug\bin\CalculatorDLL.dll | findstr "machine"

# List all symbols (including internal)
dumpbin /symbols out\build\windows-arm64-debug\bin\CalculatorDLL.dll

# Show DLL dependencies
dumpbin /dependents out\build\windows-arm64-debug\bin\CalculatorDLL.dll
```

### Linux / WSL

```bash
# List exported symbols (T = text/code, D = data)
nm -D --defined-only out/build/linux-x64-debug/lib/libCalculatorDLL.so

# Check target architecture and file type
file out/build/linux-x64-debug/lib/libCalculatorDLL.so

# Detailed ELF header info (architecture, type, entry point)
readelf -h out/build/linux-x64-debug/lib/libCalculatorDLL.so

# List dynamic symbol table (similar to nm -D)
objdump -T out/build/linux-x64-debug/lib/libCalculatorDLL.so

# Show shared library dependencies (like dumpbin /dependents)
ldd out/build/linux-x64-debug/lib/libCalculatorDLL.so
```

### Quick Reference

| Task | Windows (`dumpbin`) | Linux |
|---|---|---|
| Exported functions | `dumpbin /exports lib.dll` | `nm -D --defined-only lib.so` |
| Target architecture | `dumpbin /headers lib.dll \| findstr machine` | `file lib.so` or `readelf -h lib.so` |
| All symbols | `dumpbin /symbols lib.dll` | `nm lib.so` |
| Dependencies | `dumpbin /dependents lib.dll` | `ldd lib.so` |
| Disassembly | `dumpbin /disasm lib.dll` | `objdump -d lib.so` |

## Possible Future Extensions

- **Undo / Redo** — expose history navigation through the C API.
- **Expression parser** — accept string expressions (e.g. `"3 + 4 * 2"`) and evaluate them.
- **Thread safety** — add a mutex to `CalculatorContext` so the handle can be shared across threads.
- **Callback / event API** — let callers register a callback that fires after every operation (useful for logging or UI updates).
- **Additional math operations** — power, square root, modulus, etc.
- **Versioned API** — add `calculator_api_version()` so consumers can check compatibility at runtime.
