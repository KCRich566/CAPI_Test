# Manus PPT Prompt — CAPI_Test Presentation Prompt

Two prompt variants are provided below: Variant A (10-minute summary) and Variant B (full version, longer than 10 minutes). Choose the one you need and copy it to Manus.

---

## Variant A: 10-minute Summary Prompt

```
Please create a technical presentation (PPT) under 10 minutes on the topic:

"OS-Free C API Design — Cross-platform C/C++ DLL Design and C#/Python Integration"

The presentation should be based on the open-source project CAPI_Test (https://github.com/KCRich566/CAPI_Calculator).
This project exposes a pure C public API with a C++ internal implementation and demonstrates the opaque handle pattern and cross-language integration.

Suggested slide outline (8–10 slides):

[Slide 1] Title
- Title: OS-Free C API Design — Cross-platform DLL Design and C#/Python Integration
- Subtitle: Example: CAPI_Test
- Author and date

[Slide 2] Why a C API?
- Problem: C++ ABI instability (name mangling, compiler incompatibility)
- Solution: extern "C" + pure C header for a stable ABI
- Benefits: cross-platform (Windows/Linux), cross-language (C#/Python/C++)
- Note: OS-Free means avoiding OS-specific APIs like `Windows.h`

[Slide 3] Core Pattern — Opaque Handle
- Show key excerpts from `calculator.h`:
```c
typedef void* CalculatorHandle;
CALCULATORDLL_API CalculatorHandle calculator_create(const char* historyFile);
CALCULATORDLL_API void calculator_destroy(CalculatorHandle handle);
CALCULATORDLL_API CalculatorError calculator_add(CalculatorHandle handle, double r, double* result);
```
- Alternative: forward-declared struct pointer (`typedef struct SobelContext SobelContext; typedef SobelContext* SobelHandle;`)
- Discuss tradeoffs: `void*` vs `struct*`

[Slide 4] Functional Requirements and Error Handling
- Lifecycle: create → operate → destroy
- Error codes (do not propagate C++ exceptions across the C API):
```c
typedef enum CalculatorErrorCode {
    CALC_SUCCESS = 0,
    CALC_ERROR_DIVIDE_BY_ZERO = -1,
    CALC_ERROR_FILE_IO = -3,
    CALC_ERROR_UNKNOWN = -99
} CalculatorError;
```
- Each C wrapper catches exceptions and returns an error code
- Memory ownership rules:
  - Library-owned: `calculator_get_history_data_from_file()` — DO NOT free
  - Caller-owned: `calculator_dup_history_data_from_file()` — free with `calculator_free()`

[Slide 5] Export Macro and CMake Pipeline
- Export macro (Windows vs Linux):
```c
#if defined(_WIN32)
#  ifdef CALCULATORDLL_EXPORTS
#    define CALCULATORDLL_API __declspec(dllexport)
#  else
#    define CALCULATORDLL_API __declspec(dllimport)
#  endif
#else
#  define CALCULATORDLL_API __attribute__((visibility("default")))
#endif
```
- Build flow: `CMakeLists.txt` → `add_library(SHARED)` → `.dll` / `.so`
- Use `CMakePresets.json` to support multi-arch builds (x64/x86/ARM64/WSL)

[Slide 6] ABI Inspection
- Windows (dumpbin): `dumpbin /exports CalculatorDLL.dll`, `dumpbin /headers CalculatorDLL.dll | findstr machine`
- Linux (nm / readelf / file): `nm -D --defined-only libCalculatorDLL.so`, `file libCalculatorDLL.so`, `readelf -h libCalculatorDLL.so`, `ldd libCalculatorDLL.so`
- Quick comparison table for tools (Windows vs Linux)

[Slide 7] C# Integration — P/Invoke
- Show `CalculatorNativeMethods.cs` excerpts:
```csharp
[DllImport("CalculatorDLL", CallingConvention = CallingConvention.Cdecl)]
internal static extern IntPtr calculator_create(string historyFile);
[DllImport("CalculatorDLL", CallingConvention = CallingConvention.Cdecl)]
internal static extern int calculator_add(IntPtr handle, double r, out double result);
```
- Show `Calculator` wrapper (IDisposable pattern) and usage: `dotnet run --project CSharpTest`

[Slide 8] Python Integration — ctypes
- Example:
```python
import ctypes
lib = ctypes.CDLL("./CalculatorDLL.dll")  # or ./libCalculatorDLL.so
lib.calculator_create.restype = ctypes.c_void_p
lib.calculator_create.argtypes = [ctypes.c_char_p]
handle = lib.calculator_create(b"history.txt")
result = ctypes.c_double()
lib.calculator_add(handle, ctypes.c_double(3.0), ctypes.byref(result))
print(f"Result: {result.value}")
lib.calculator_destroy(handle)
```

[Slide 9] Pipeline Diagram
- Show flow from source to artifacts (.dll/.so) and how consumers (C++, C#, Python) interact

[Slide 10] Summary and Future Work
- Key points recap:
  - Pure C header (`extern "C"` + export macro)
  - C++ internal implementation (class + STL) fully hidden
  - Opaque handle pattern
  - Error codes (no exceptions across API)
  - OS-Free and cross-language interoperability
- Future directions: Undo/Redo, expression parser, thread safety, callback API, versioned API

Design requirements:
- Style: professional technical slides with concise code examples
- Language: English content (titles and body in English)
- Color: dark tech theme or clean light theme
- Use monospace font for code and syntax highlighting
- Provide clear pipeline diagrams and comparison tables
- Keep the presentation within 10 minutes (about 1 minute per slide)
```

---

## Variant B: Full Version Prompt (15–20 minutes)

```
Please create a full technical presentation (~15–20 minutes) on:

"OS-Free C API Design — Cross-platform C/C++ DLL Design and C#/Python Integration"

Base the presentation on the open-source project CAPI_Test (https://github.com/KCRich566/CAPI_Calculator). This project provides a pure C public interface with C++ internal implementation and demonstrates the opaque handle pattern and cross-language integration. The project includes two DLLs: `CalculatorDLL` and `SobelDLL`, along with C++ and C# test executables.

Suggested detailed outline (18–22 slides):

Part 1 — Background and Motivation (3 slides)
- Slide 1: Title
- Slide 2: Why a stable C ABI? (C++ ABI instability, exceptions, OS-specific APIs)
- Slide 3: What is ABI? (compare API vs ABI, coverage: calling convention, data layout, name mangling, binary formats)

Part 2 — Core Design Patterns (4 slides)
- Slide 4: `extern "C"` and export macro (explain usage and show examples)
```c
#ifdef __cplusplus
extern "C" {
#endif
CALCULATORDLL_API CalculatorHandle calculator_create(const char* historyFile);
#ifdef __cplusplus
}
#endif
```
- Slide 5: Opaque handle pattern and ownership rules
- Slide 6: Error handling strategy (error codes, wrapper try/catch, no exceptions across C API)
- Slide 7: Build pipeline and CMake presets

... (rest of the full prompt continues with detailed slides and examples)
