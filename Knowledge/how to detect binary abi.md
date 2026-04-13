# How to Detect Binary ABI

Binary ABI is the runtime contract: calling convention, name mangling, data layout, architecture, and platform rules.  
二進位 ABI 是執行期契約：呼叫約定、名稱改編、資料版面、架構與平台規則。

---

## 1. Identify Platform and Architecture

Detect platform and architecture first (x86 / x64 / ARM64).  
首先偵測平台與架構（x86 / x64 / ARM64）。

ABI rules differ significantly across architectures and OS platforms.  
不同架構與作業系統之間的 ABI 規則差異很大。

On x64, only one ABI exists per platform (Microsoft x64 or System V); no cdecl vs stdcall difference.  
在 x64 上，每個平台只有一種 ABI（Microsoft x64 或 System V）；無 cdecl 與 stdcall 之分。

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
檢查輸出符號（dumpbin / nm / objdump）以判斷名稱有無被改編。

Clean names indicate C ABI; mangled names (e.g., ?add@@YAHHH@Z) indicate C++ ABI.  
乾淨的名稱代表 C ABI；改編名稱（例如 ?add@@YAHHH@Z）代表 C++ ABI。

C++ name mangling encodes function signature, namespace, and class information.  
C++ 名稱改編會編碼函式簽名、命名空間與類別資訊。

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
在 Windows x86 上，符號裝飾可揭示呼叫約定（例如 _add@8 代表 stdcall）。

cdecl typically has no suffix decoration.  
cdecl 通常沒有額外符號後綴。

fastcall may encode register usage in symbol decoration depending on compiler.  
fastcall 可能會在符號中編碼暫存器使用資訊（依編譯器而異）。

Check symbol decorations (x86 only):

```powershell
dumpbin /symbols path\to\your.dll | findstr "_yourfunc@"    # stdcall decoration _func@<bytes>
```

On Linux/x64 this is not applicable — x64 has one platform ABI.

---

## 4. Runtime & Dependency Inspection

Check dependencies to infer the CRT / runtime used by the binary.  
檢查相依性以推斷該二進位使用的 CRT / 執行環境。

Different MSVC runtime versions may affect ABI compatibility.  
不同 MSVC runtime 版本可能影響 ABI 相容性。

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
使用標頭與文件推斷結構版面與記憶體所有權規則。

Mismatched struct packing or alignment leads to memory corruption.  
結構體對齊或打包不一致會導致記憶體損壞。

Windows uses LLP64 data model; Linux/macOS use LP64 -- long is 4 bytes on Windows but 8 bytes on Linux.  
Windows 使用 LLP64 資料模型；Linux/macOS 使用 LP64 —— long 在 Windows 為 4 位元組，在 Linux 為 8 位元組。

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
  架構（x86 / x64 / ARM64）

- Name mangling (C vs C++)  
  名稱改編（C vs C++）

- Calling convention (x86 only meaningful)  
  呼叫約定（僅 x86 有實際差異）

- Data model (LP64 / LLP64)  
  資料模型（LP64 / LLP64）

- Runtime / CRT version  
  執行環境 / CRT 版本

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
ABI 不是單一概念，而是多種二進位規則的組合。

Breaking any one of them can cause crashes or undefined behavior.  
任何一項不一致都可能導致崩潰或未定義行為。