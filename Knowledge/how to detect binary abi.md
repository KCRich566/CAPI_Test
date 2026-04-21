# How to Detect Binary ABI

Binary ABI is the runtime contract: calling convention, name mangling, data layout, architecture, and platform rules.

---

## 1. Identify Platform and Architecture

Detect platform and architecture first (x86 / x64 / ARM64).

ABI rules differ significantly across architectures and OS platforms.

On x64, only one ABI exists per platform (Microsoft x64 or System V); no cdecl vs stdcall difference.

Commands (examples):

- On Linux / WSL: detect machine arch

```bash
uname -m           # x86_64, aarch64, i686
file out/build/linux-x64-debug/lib/libCalculatorDLL.so
readelf -h out/build/linux-x64-debug/lib/libCalculatorDLL.so | grep Class
```

- On Windows (PowerShell in Developer Command Prompt):

```powershell
dumpbin /headers out\build\windows-x64-debug\bin\CalculatorDLL.dll | findstr /i "machine"
```

---

## 2. Inspect Symbols

Inspect exported symbols (dumpbin / nm / objdump) to check for name mangling.

Clean names indicate C ABI; mangled names (e.g., ?add@@YAHHH@Z) indicate C++ ABI.

C++ name mangling encodes function signature, namespace, and class information.

Useful commands:

- Windows (Developer Command Prompt):

```powershell
# list exported functions (clean names indicate C linkage)
dumpbin /exports out\build\windows-x64-debug\bin\CalculatorDLL.dll

# list symbols (mangled names visible)
dumpbin /symbols out\build\windows-x64-debug\bin\CalculatorDLL.dll
```

- Linux / WSL:

```bash
# dynamic symbols (exports)
nm -D --defined-only out/build/linux-x64-debug/lib/libCalculatorDLL.so

# show demangled names
nm -D --defined-only out/build/linux-x64-debug/lib/libCalculatorDLL.so | c++filt

# dynamic symbol table (objdump)
objdump -T out/build/linux-x64-debug/lib/libCalculatorDLL.so
```

---

## 3. Calling Convention Clues (x86 Windows)

On Windows x86, symbol decoration reveals calling convention (e.g., _add@8 for stdcall).

cdecl typically has no suffix decoration.

fastcall may encode register usage in symbol decoration depending on compiler.

Check symbol decorations (x86 only):

```powershell
dumpbin /symbols path\to\your.dll | findstr "_yourfunc@"    # stdcall decoration _func@<bytes>
```

On Linux/x64 this is not applicable — x64 has one platform ABI.

---

## 4. Runtime & Dependency Inspection

Check dependencies to infer the CRT / runtime used by the binary.

Different MSVC runtime versions may affect ABI compatibility.

Commands to inspect dependencies:

- Windows:

```powershell
dumpbin /dependents out\build\windows-x64-debug\bin\CalculatorDLL.dll
```

- Linux / WSL:

```bash
ldd out/build/linux-x64-debug/lib/libCalculatorDLL.so
readelf -d out/build/linux-x64-debug/lib/libCalculatorDLL.so
```

---

## 5. Data Layout Inference

Use headers and documentation to infer struct layouts and memory ownership rules.

Mismatched struct packing or alignment leads to memory corruption.

Windows uses LLP64 data model; Linux/macOS use LP64 — `long` is 4 bytes on Windows but 8 bytes on Linux.

Quick way to confirm sizes for your toolchain: compile and run a tiny C program that prints sizeof for key types.

```c
// sizeof_test.c
#include <stdio.h>
int main(void) {
    printf("sizeof(void*)=%zu\n", sizeof(void*));
    printf("sizeof(long)=%zu\n", sizeof(long));
    printf("sizeof(size_t)=%zu\n", sizeof(size_t));
    return 0;
}
```

Compile & run:

- Linux / WSL:

```bash
gcc -std=c11 sizeof_test.c -o sizeof_test && ./sizeof_test
```

- Windows (MSVC Developer Command Prompt):

```powershell
cl /Febuild\sizeof_test.exe sizeof_test.c
build\sizeof_test.exe
```

---

## 6. Practical ABI Checklist

- Architecture (x86 / x64 / ARM64)
- Name mangling (C vs C++)
- Calling convention (x86 only meaningful)
- Data model (LP64 / LLP64)
- Runtime / CRT version

Practical flow for this repository (examples):

- Build (Windows x64, using CMake presets / Ninja):

```powershell
cmake --preset windows-x64-debug
cmake --build --preset windows-x64-debug
```

Then inspect exports:

```powershell
dumpbin /exports out\build\windows-x64-debug\bin\CalculatorDLL.dll
```

- Build (Linux x64, WSL):

```bash
cmake --preset linux-x64-debug
cmake --build out/build/linux-x64-debug -j$(nproc)
nm -D --defined-only out/build/linux-x64-debug/lib/libCalculatorDLL.so | c++filt
ldd out/build/linux-x64-debug/lib/libCalculatorDLL.so
```

Tips:
- Use `c++filt` to demangle C++ names on Linux: `nm -D ... | c++filt`.
- On Windows, run `dumpbin` from a Developer Command Prompt (Visual Studio) so environment vars are set.

---

## Key Insight

ABI is not one thing—it is a combination of multiple binary-level rules.

Breaking any one of them can cause crashes or undefined behavior.