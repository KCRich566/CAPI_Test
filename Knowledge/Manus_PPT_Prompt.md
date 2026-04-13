# Manus PPT Prompt — CAPI_Test 簡報製作

> 以下提供兩個版本的 Prompt：**版本 A（10 分鐘精簡版）** 與 **版本 B（完整版，超過 10 分鐘）**。
> 請依據需要選擇其中一個複製給 Manus。

---

## 版本 A：10 分鐘精簡版 Prompt

```
請幫我製作一份 10 分鐘以內的技術簡報 PPT，主題為：

「OS-Free C API Design — 跨平台 C/C++ DLL 設計與 C#/Python 整合」

簡報基於我的開源專案 CAPI_Test（https://github.com/KCRich566/CAPI_Calculator），
這是一個以純 C 為公開介面、C++ 為內部實作的跨平台共享函式庫（DLL/SO），
示範 Opaque Handle Pattern 與跨語言整合。

═══════════════════════════════════════
■ 簡報大綱（建議 8–10 張投影片）
═══════════════════════════════════════

【投影片 1】封面
- 標題：OS-Free C API Design — 跨平台 DLL 設計與 C#/Python 整合
- 副標：以 CAPI_Test 專案為例
- 作者與日期

【投影片 2】為什麼需要 C API？（Why C API?）
- 問題：C++ ABI 不穩定（name mangling、編譯器不相容）
- 解法：extern "C" + 純 C header = 穩定 ABI
- 好處：跨平台（Windows/Linux）、跨語言（C#/Python/C++）
- 關鍵字：OS-Free = 不使用任何 OS 特定 API（如 Windows.h）

【投影片 3】核心設計模式 — Opaque Handle
- 展示 calculator.h 的關鍵程式碼：
  ```c
  typedef void* CalculatorHandle;  // 使用者只看到一個不透明指標
  
  CALCULATORDLL_API CalculatorHandle calculator_create(const char* historyFile);
  CALCULATORDLL_API void calculator_destroy(CalculatorHandle handle);
  CALCULATORDLL_API CalculatorError calculator_add(CalculatorHandle handle, double r, double* result);
  ```
- 另一種方式（SobelDLL 的前向宣告型別安全版）：
  ```c
  typedef struct SobelContext SobelContext;
  typedef SobelContext* SobelHandle;
  ```
- 說明：void* vs forward-declared struct* 各自優缺點

【投影片 4】Functional Requirements 與 Error Handling
- Lifecycle: create → operate → destroy
- Error codes（不使用 C++ exception 跨越 API 邊界）：
  ```c
  typedef enum CalculatorErrorCode {
      CALC_SUCCESS = 0,
      CALC_ERROR_DIVIDE_BY_ZERO = -1,
      CALC_ERROR_FILE_IO = -3,
      CALC_ERROR_UNKNOWN = -99
  } CalculatorError;
  ```
- 每個 C API wrapper 都用 try/catch 包住 C++ 實作，轉為 error code 回傳
- 記憶體所有權規則：
  - Library-owned: calculator_get_history_file() → 不可 free
  - Caller-owned: calculator_dup_history_file() → 必須用 calculator_free() 釋放

【投影片 5】跨平台編譯 Export Macro + CMake Pipeline
- Export macro（Windows vs Linux）：
  ```c
  #if defined(_WIN32)
    #ifdef CALCULATORDLL_EXPORTS
      #define CALCULATORDLL_API __declspec(dllexport)
    #else
      #define CALCULATORDLL_API __declspec(dllimport)
    #endif
  #else
    #define CALCULATORDLL_API __attribute__((visibility("default")))
  #endif
  ```
- CMake 流程圖：
  CMakeLists.txt → add_library(SHARED) → .dll (Windows) / .so (Linux)
  CMakePresets.json → 支援 x64/x86/ARM64/WSL 多平台
- 編譯指令範例：
  Windows: cmake --preset windows-x64-debug && cmake --build --preset windows-x64-debug
  Linux:   cmake --preset linux-x64-debug && cmake --build --preset linux-x64-debug

【投影片 6】ABI 檢查方法
- Windows（dumpbin）：
  dumpbin /exports CalculatorDLL.dll      → 查看匯出函式
  dumpbin /headers CalculatorDLL.dll | findstr machine  → 查看架構
- Linux（nm / readelf / file）：
  nm -D --defined-only libCalculatorDLL.so  → 查看匯出符號
  file libCalculatorDLL.so                  → 查看架構
  readelf -h libCalculatorDLL.so            → 詳細 ELF header
  ldd libCalculatorDLL.so                   → 查看相依性
- 表格對照：
  | 任務 | Windows | Linux |
  |------|---------|-------|
  | 匯出函式 | dumpbin /exports | nm -D |
  | 架構 | dumpbin /headers | file / readelf -h |
  | 相依性 | dumpbin /dependents | ldd |
  | 所有符號 | dumpbin /symbols | nm |

【投影片 7】C# 整合 — P/Invoke
- 展示 CalculatorNativeMethods.cs 核心程式碼：
  ```csharp
  [DllImport("CalculatorDLL", CallingConvention = CallingConvention.Cdecl)]
  internal static extern IntPtr calculator_create(string historyFile);
  
  [DllImport("CalculatorDLL", CallingConvention = CallingConvention.Cdecl)]
  internal static extern int calculator_add(IntPtr handle, double r, out double result);
  ```
- 展示 Calculator wrapper class（IDisposable pattern）：
  ```csharp
  public sealed class Calculator : IDisposable {
      private IntPtr handle;
      public Calculator(string historyFile) {
          handle = CalculatorNativeMethods.calculator_create(historyFile);
      }
      public void Dispose() { calculator_destroy(handle); }
  }
  ```
- 執行指令：dotnet run --project CSharpTest

【投影片 8】Python 整合 — ctypes
- 展示 Python 呼叫範例（概念程式碼）：
  ```python
  import ctypes
  
  # Windows
  lib = ctypes.CDLL("./CalculatorDLL.dll")
  # Linux
  # lib = ctypes.CDLL("./libCalculatorDLL.so")
  
  lib.calculator_create.restype = ctypes.c_void_p
  lib.calculator_create.argtypes = [ctypes.c_char_p]
  
  handle = lib.calculator_create(b"history.txt")
  
  result = ctypes.c_double()
  lib.calculator_add(handle, ctypes.c_double(3.0), ctypes.byref(result))
  print(f"Result: {result.value}")
  
  lib.calculator_destroy(handle)
  ```
- 說明：ctypes.CDLL 對應 CallingConvention.Cdecl

【投影片 9】完整 Pipeline 流程圖
- 用流程圖呈現從原始碼到執行的完整工作流：
  ```
  ┌─────────────────────────────────────────────────────────────┐
  │  Source Code                                                 │
  │  calculator.h (Pure C Header)                                │
  │  calculator.cpp (C++ Implementation)                         │
  └──────────────┬──────────────────────────────────────────────┘
                 │ CMake + Compiler
                 ▼
  ┌──────────────────────────────┐  ┌────────────────────────────┐
  │  Windows                      │  │  Linux                      │
  │  CalculatorDLL.dll            │  │  libCalculatorDLL.so        │
  │  CalculatorDLL.lib            │  │                              │
  └──────────────┬───────────────┘  └──────────────┬─────────────┘
                 │                                  │
       ┌─────────┼─────────┐              ┌────────┼────────┐
       ▼         ▼         ▼              ▼        ▼        ▼
    C++ exe   C# P/Invoke  Python      C++ exe  C# P/Invoke Python
    (link)    (DllImport)  (ctypes)    (link)   (DllImport) (ctypes)
  ```
- ABI 檢查點：在產出 .dll/.so 之後用 dumpbin/nm 驗證

【投影片 10】總結與未來擴展
- 重點回顧：
  ✅ 純 C Header（extern "C" + export macro）
  ✅ C++ 內部實作（class + STL，完全隱藏）
  ✅ Opaque Handle（void* 或 forward-declared struct*）
  ✅ Error Code（不讓 C++ exception 跨 API）
  ✅ OS-Free（無 OS 特定 API，可跨平台）
  ✅ 跨語言：C++ / C# / Python 皆可呼叫
- 未來方向：Undo/Redo、Expression Parser、Thread Safety、Callback API、Versioned API

═══════════════════════════════════════
■ 設計要求
═══════════════════════════════════════
- 風格：專業技術簡報，簡潔但有足夠程式碼範例
- 語言：中英雙語（標題英文，內文中英並列）
- 配色：深色科技風格或乾淨白底技術風
- 程式碼：使用等寬字體，語法高亮（或至少底色區分）
- 流程圖：Pipeline 用清晰的流程圖呈現
- 表格：ABI 工具對照用表格
- 時間控制：10 分鐘內，每張投影片約 1 分鐘
```

---

## 版本 B：完整版 Prompt（超過 10 分鐘）

```
請幫我製作一份完整的技術簡報 PPT（約 15–20 分鐘），主題為：

「OS-Free C API Design — 跨平台 C/C++ DLL 設計與 C#/Python 整合」

簡報基於我的開源專案 CAPI_Calculator（https://github.com/KCRich566/CAPI_Calculator），
這是一個以純 C 為公開介面、C++ 為內部實作的跨平台共享函式庫（DLL/SO），
示範 Opaque Handle Pattern 與跨語言整合。
專案包含兩個 DLL：CalculatorDLL（計算機）和 SobelDLL（Sobel 邊緣檢測），
以及 C++ Test、C# Test 執行檔。

═══════════════════════════════════════
■ 簡報大綱（建議 18–22 張投影片）
═══════════════════════════════════════

【Part 1：背景與動機（3 張）】

投影片 1 — 封面
- 標題：OS-Free C API Design — 跨平台 DLL 設計與 C#/Python 整合
- 副標：以 CAPI_Calculator 與 SobelDLL 為例
- 作者與日期

投影片 2 — 問題意識（Why?）
- C++ ABI 不穩定：name mangling、編譯器版本不同就不相容
- C++ exception 不能跨 DLL 邊界安全傳遞
- OS-specific API（如 Windows.h）限制可移植性
- 目標：設計一個 OS-Free、跨平台、跨語言的穩定 C API

投影片 3 — 什麼是 ABI？
- ABI vs API 對照表：
  | 面向 | API | ABI |
  |------|-----|-----|
  | 層級 | 原始碼 | 二進位 / 機器碼 |
  | 關注 | 函式簽名、型別 | 呼叫約定、資料版面、符號命名 |
  | 相容性 | 原始碼改了重編就好 | 二進位必須相容才能載入 |
- ABI 涵蓋：calling convention、data layout、name mangling、binary format（PE/ELF）
- 重點：extern "C" 只解決 name mangling，不解決全部 ABI 問題

【Part 2：核心設計模式（4 張）】

投影片 4 — extern "C" 與 Export Macro
- extern "C" 的作用：停用 C++ name mangling
  ```c
  #ifdef __cplusplus
  extern "C" {
  #endif
    CALCULATORDLL_API CalculatorHandle calculator_create(const char* historyFile);
  #ifdef __cplusplus
  }
  #endif
  ```
- Export macro（跨平台）：
  ```c
  #if defined(_WIN32) || defined(_WIN64)
    #ifdef CALCULATORDLL_EXPORTS
      #define CALCULATORDLL_API __declspec(dllexport)
    #else
      #define CALCULATORDLL_API __declspec(dllimport)
    #endif
  #else
    #define CALCULATORDLL_API __attribute__((visibility("default")))
  #endif
  ```
- 搭配 CMake：
  target_compile_definitions(CalculatorDLL PRIVATE CALCULATORDLL_EXPORTS)
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)

投影片 5 — Opaque Handle Pattern（不透明句柄）
- 核心概念：使用者只看到指標，看不到內部結構
- 方式一（void*）— CalculatorDLL：
  ```c
  typedef void* CalculatorHandle;
  ```
  優點：最簡單、完全黑箱
  缺點：無型別安全，任何 void* 都能傳入
  
- 方式二（forward-declared struct*）— SobelDLL：
  ```c
  typedef struct SobelContext SobelContext;
  typedef SobelContext* SobelHandle;
  ```
  優點：型別安全，編譯器能區分不同 handle
  缺點：稍微多一點 header 聲明

- 內部實作（完全隱藏，只在 .cpp 中）：
  ```cpp
  // calculator.cpp
  class CalculatorContext {
      double cur_value_;
      std::vector<double> history_;
      std::string history_file_;
  };
  ```

投影片 6 — Error Handling 設計
- 原則：絕不讓 C++ exception 跨越 API 邊界
- Error Code enum：
  ```c
  typedef enum CalculatorErrorCode {
      CALC_SUCCESS = 0,
      CALC_ERROR_DIVIDE_BY_ZERO = -1,
      CALC_ERROR_INVALID_OPERATION = -2,
      CALC_ERROR_FILE_IO = -3,
      CALC_ERROR_UNKNOWN = -99
  } CalculatorError;
  ```
- C API wrapper 中的 try/catch 模式（以 SobelDLL 為例）：
  ```cpp
  SOBELDLL_API SobelError sobel_load(SobelHandle handle, const char* inputFile)
  {
      if (!handle || !handle->impl || !inputFile) return SOBEL_ERROR_INVALID_ARG;
      try { return handle->impl->load(inputFile); }
      catch (...) { return SOBEL_ERROR_UNKNOWN; }
  }
  ```

投影片 7 — 記憶體所有權（Ownership）
- Library-owned（借用）：
  ```c
  // 回傳內部指標，caller 不可 free
  const char* calculator_get_history_file(CalculatorHandle handle);
  ```
- Caller-owned（轉移）：
  ```c
  // 回傳新配置的記憶體，caller 必須用 calculator_free() 釋放
  char* calculator_dup_history_file(CalculatorHandle handle);
  void calculator_free(void* ptr);
  ```
- 為何需要 calculator_free()？
  - DLL 與 caller 可能使用不同 CRT（尤其 Windows MSVC）
  - 必須由同一個 allocator 配置和釋放

【Part 3：完整 Pipeline（4 張）】

投影片 8 — 專案結構總覽
- 專案目錄結構：
  ```
  CAPI_Calculator/
  ├── CalculatorDLL/
  │   ├── calculator.h          ← 純 C 公開 header
  │   └── calculator.cpp        ← C++ 內部實作 + C API wrapper
  ├── SobelDLL/
  │   ├── sobel.h               ← 純 C 公開 header（Sobel 邊緣檢測）
  │   └── sobel.cpp             ← C++ 實作 + stb_image（single-header lib）
  ├── CPPTest/
  │   └── CPPTest.cpp           ← C++ 測試程式（直接 link DLL）
  ├── CSharpTest/
  │   ├── CalculatorNativeMethods.cs  ← P/Invoke 宣告
  │   ├── SobelNativeMethods.cs       ← P/Invoke 宣告
  │   └── Program.cs                  ← C# 主程式
  ├── thirdparty/stb/           ← stb_image single-header（SobelDLL 用）
  ├── CMakeLists.txt            ← 跨平台建置腳本
  ├── CMakePresets.json         ← 多平台 preset（x64/x86/ARM64/WSL）
  └── build_wsl.sh              ← WSL 建置腳本
  ```

投影片 9 — CMake 編譯 Pipeline
- 完整流程圖：
  ```
  原始碼 (.h + .cpp)
       │
       ▼
  CMakeLists.txt
  ┌─────────────────────────────────┐
  │ add_library(CalculatorDLL SHARED)│
  │ add_library(SobelDLL SHARED)     │
  │ add_executable(CPPTest)          │
  │ target_link_libraries(...)       │
  └─────────────┬───────────────────┘
                │
       CMakePresets.json
       （選擇 platform + arch + build type）
                │
       ┌────────┴────────┐
       ▼                 ▼
   Windows (MSVC)    Linux (GCC/Clang)
   CalculatorDLL.dll  libCalculatorDLL.so
   SobelDLL.dll       libSobelDLL.so
   CPPTest.exe        CPPTest
  ```
- 編譯指令：
  Windows: cmake --preset windows-x64-debug && cmake --build --preset windows-x64-debug
  Linux:   cmake --preset linux-x64-debug && cmake --build --preset linux-x64-debug
  WSL:     wsl bash -c "cmake --preset wsl-x64-debug && cmake --build --preset wsl-x64-debug"

投影片 10 — ABI 驗證方法
- 產出二進位檔後，必須驗證 ABI 正確性
- Windows 工具（dumpbin，需在 Developer Command Prompt 中）：
  ```
  dumpbin /exports CalculatorDLL.dll          → 匯出函式清單
  dumpbin /headers CalculatorDLL.dll | findstr machine  → 目標架構
  dumpbin /symbols CalculatorDLL.dll          → 所有符號（含內部）
  dumpbin /dependents CalculatorDLL.dll       → DLL 相依性
  ```
- Linux 工具：
  ```
  nm -D --defined-only libCalculatorDLL.so    → 匯出符號
  file libCalculatorDLL.so                    → 架構與檔案類型
  readelf -h libCalculatorDLL.so              → 詳細 ELF header
  ldd libCalculatorDLL.so                     → 共享函式庫相依性
  objdump -T libCalculatorDLL.so              → 動態符號表
  ```
- 對照表格：
  | 檢查項目 | Windows (dumpbin) | Linux |
  |---------|-------------------|-------|
  | 匯出函式 | /exports | nm -D --defined-only |
  | 目標架構 | /headers \| findstr machine | file / readelf -h |
  | 所有符號 | /symbols | nm |
  | 相依性 | /dependents | ldd |
  | 反組譯 | /disasm | objdump -d |
- 驗證重點：
  - 確認匯出符號是 clean name（非 mangled）
  - 確認架構匹配（x64 DLL 給 x64 process）
  - 確認無意外的 C++ 符號洩漏

投影片 11 — 端到端 Pipeline 全貌
- 完整流程圖（從原始碼到各語言執行）：
  ```
  ┌────────────────────────────────────────────────────────────────┐
  │                     Source Code Layer                           │
  │  calculator.h / sobel.h      (Pure C Header — 公開介面)         │
  │  calculator.cpp / sobel.cpp  (C++ Implementation — 隱藏實作)    │
  └────────────────────┬───────────────────────────────────────────┘
                       │ ① CMake Configure + Build
                       ▼
  ┌────────────────────────────────────────────────────────────────┐
  │                  Binary Artifacts Layer                         │
  │  Windows: .dll + .lib    │    Linux: .so                        │
  └────────────────────┬───────────────────────────────────────────┘
                       │ ② ABI 驗證 (dumpbin / nm / readelf)
                       ▼
  ┌────────────────────────────────────────────────────────────────┐
  │                   Consumer Layer                                │
  │                                                                 │
  │  ┌──────────┐  ┌───────────────┐  ┌──────────────────┐         │
  │  │ C++ App  │  │ C# App        │  │ Python Script    │         │
  │  │ (link)   │  │ (P/Invoke     │  │ (ctypes.CDLL)    │         │
  │  │          │  │  DllImport)   │  │                  │         │
  │  └──────────┘  └───────────────┘  └──────────────────┘         │
  └────────────────────────────────────────────────────────────────┘
  ```

【Part 4：跨語言整合（4 張）】

投影片 12 — C++ 呼叫端
- CPPTest.cpp 核心程式碼：
  ```cpp
  CalculatorHandle calc = calculator_create("history.txt");
  double result;
  calculator_add(calc, 3, &result);
  printf("Result: %f\n", result);
  calculator_destroy(calc);
  ```
- 直接 link DLL，最直接的使用方式
- target_link_libraries(CPPTest PRIVATE CalculatorDLL SobelDLL)

投影片 13 — C# 整合 — P/Invoke
- Step 1: P/Invoke 宣告（CalculatorNativeMethods.cs）：
  ```csharp
  private const string DllName = "CalculatorDLL";  // 不加副檔名 → .NET 自動解析
  
  [DllImport(DllName, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
  internal static extern IntPtr calculator_create(string historyFile);
  
  [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
  internal static extern int calculator_add(IntPtr handle, double r, out double result);
  
  [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
  internal static extern void calculator_destroy(IntPtr handle);
  ```
- Step 2: Managed Wrapper（IDisposable）：
  ```csharp
  public sealed class Calculator : IDisposable {
      private IntPtr handle;
      public Calculator(string historyFile) {
          handle = CalculatorNativeMethods.calculator_create(historyFile);
          if (handle == IntPtr.Zero) throw new InvalidOperationException("Failed to create handle.");
      }
      public CalculatorError Add(double r, out double result)
          => (CalculatorError)CalculatorNativeMethods.calculator_add(handle, r, out result);
      public void Dispose() {
          if (handle != IntPtr.Zero) {
              CalculatorNativeMethods.calculator_destroy(handle);
              handle = IntPtr.Zero;
          }
          GC.SuppressFinalize(this);
      }
      ~Calculator() => Dispose();
  }
  ```
- Step 3: 使用：
  ```csharp
  using var calc = new Calculator("history.txt");
  calc.Add(3, out double result);
  Console.WriteLine($"Result: {result}");
  ```
- 重要：DLL 架構必須匹配 .NET runtime 架構（x64 DLL → x64 .NET）

投影片 14 — Python 整合 — ctypes
- Python 呼叫範例：
  ```python
  import ctypes
  import os
  
  # 載入 DLL/SO
  if os.name == 'nt':
      lib = ctypes.CDLL("./CalculatorDLL.dll")
  else:
      lib = ctypes.CDLL("./libCalculatorDLL.so")
  
  # 設定函式簽名
  lib.calculator_create.restype = ctypes.c_void_p
  lib.calculator_create.argtypes = [ctypes.c_char_p]
  
  lib.calculator_add.restype = ctypes.c_int
  lib.calculator_add.argtypes = [ctypes.c_void_p, ctypes.c_double, ctypes.POINTER(ctypes.c_double)]
  
  lib.calculator_destroy.restype = None
  lib.calculator_destroy.argtypes = [ctypes.c_void_p]
  
  lib.calculator_get_cur_value.restype = ctypes.c_double
  lib.calculator_get_cur_value.argtypes = [ctypes.c_void_p]
  
  # 使用
  handle = lib.calculator_create(b"history.txt")
  
  result = ctypes.c_double()
  err = lib.calculator_add(handle, ctypes.c_double(3.0), ctypes.byref(result))
  print(f"Error code: {err}, Result: {result.value}")
  
  err = lib.calculator_add(handle, ctypes.c_double(7.0), ctypes.byref(result))
  print(f"Error code: {err}, Result: {result.value}")
  
  print(f"Current value: {lib.calculator_get_cur_value(handle)}")
  
  lib.calculator_destroy(handle)
  ```
- 說明：
  - ctypes.CDLL → 對應 cdecl calling convention
  - 必須手動設定 restype 和 argtypes 以確保正確的資料類型轉換
  - b"history.txt" → bytes 字串，對應 C 的 const char*

投影片 15 — 跨語言整合對照表
  | 面向 | C++ | C# | Python |
  |------|-----|-----|--------|
  | 載入方式 | 編譯期 link | DllImport (P/Invoke) | ctypes.CDLL |
  | Handle 型別 | CalculatorHandle (void*) | IntPtr | c_void_p |
  | 字串傳遞 | const char* | string (auto marshal) | bytes (c_char_p) |
  | Error 處理 | 檢查回傳值 | cast to enum | 檢查 int 回傳值 |
  | 資源釋放 | calculator_destroy() | IDisposable.Dispose() | calculator_destroy() |
  | Calling Convention | cdecl (default) | CallingConvention.Cdecl | CDLL (cdecl) |

【Part 5：SobelDLL 案例研究（2 張）】

投影片 16 — SobelDLL：更複雜的實際範例
- 展示另一種 Opaque Handle（forward-declared struct）：
  ```c
  typedef struct SobelContext SobelContext;
  typedef SobelContext* SobelHandle;
  
  SobelHandle sobel_create(void);
  SobelError  sobel_load(SobelHandle handle, const char* inputFile);
  SobelError  sobel_apply(SobelHandle handle);
  SobelError  sobel_save(SobelHandle handle, const char* outputFile);
  void        sobel_destroy(SobelHandle handle);
  ```
- 內部架構（雙層隱藏）：
  ```
  SobelHandle → SobelContext (struct) → ImageInternal (C++ class)
                  ↑ header 中宣告           ↑ 完全隱藏在 .cpp
  ```
- 使用 stb_image single-header library（第三方, platform-independent）
- Lifecycle: create → load → apply → save → destroy

投影片 17 — SobelDLL C# 整合
- SobelNativeMethods.cs + SobelFilter wrapper：
  ```csharp
  using var sobel = new SobelFilter();
  var err = sobel.Load("image.jpg");
  Console.WriteLine($"Image: {sobel.Width} x {sobel.Height}");
  sobel.Apply();
  sobel.Save("sobel_output.png");
  ```

【Part 6：Design Trade-offs 與總結（3 張）】

投影片 18 — Technical Requirements 總結
- ✅ Public interface 必須是純 C（header file）
- ✅ Internal implementation 用 C++（class, STL, exceptions internally）
- ✅ Platform-independent code（無 OS-specific API）
- ✅ Focus on API design and robustness
- ✅ Error codes 而非 exceptions 跨越 API 邊界
- ✅ 明確的記憶體所有權（library-owned vs caller-owned）

投影片 19 — Design Trade-offs
- 優點：
  - ABI 穩定（不受編譯器版本影響）
  - 跨平台（Windows/Linux/macOS）
  - 跨語言（C/C++/C#/Python/Rust/Go...）
  - 封裝良好（內部變更不影響使用者）
- 代價：
  - 額外的指標間接存取（pointer indirection）→ 微小效能開銷
  - Heap allocation → 記憶體碎片風險
  - 除錯困難（內部狀態被隱藏）
  - C API 設計較繁瑣（每個操作都要 handle + error code）

投影片 20 — 總結與未來擴展
- 核心觀點：
  「Opaque patterns 將 API 轉變為嚴格契約，而非資料暴露」
  「C API 是跨語言互通的最大公約數」
- 本專案展示了完整的 pipeline：
  設計 C API → C++ 實作 → CMake 跨平台編譯 → ABI 驗證 → C++/C#/Python 整合
- 未來方向：
  - Undo/Redo history navigation
  - Expression parser（字串運算式求值）
  - Thread safety（mutex 保護 handle）
  - Callback/Event API（操作後通知）
  - Versioned API（calculator_api_version() 執行期版本檢查）
  - Python wheel 打包（pip install）

投影片 21 — Q&A
- 問答時間
- 附上 GitHub 連結：https://github.com/KCRich566/CAPI_Calculator

═══════════════════════════════════════
■ 設計要求
═══════════════════════════════════════
- 風格：專業技術簡報，簡潔但有足夠程式碼範例
- 語言：中英雙語（標題英文，內文中英並列）
- 配色：深色科技風格或乾淨白底技術風
- 程式碼：使用等寬字體，語法高亮（或至少底色區分）
- 流程圖：Pipeline 用清晰的流程圖呈現（多處都需要）
- 表格：ABI 工具對照、跨語言對照等用表格
- 動畫：程式碼區塊可用淡入效果
- 頁碼：每張投影片加上頁碼
- 時間估算：約 18–22 張，每張約 1 分鐘，共 18–22 分鐘

═══════════════════════════════════════
■ 程式碼來源聲明
═══════════════════════════════════════
所有程式碼皆來自 CAPI_Test 專案：
- calculator.h / calculator.cpp — CalculatorDLL
- sobel.h / sobel.cpp — SobelDLL
- CalculatorNativeMethods.cs / SobelNativeMethods.cs / Program.cs — CSharpTest
- CMakeLists.txt / CMakePresets.json — 建置系統
- Python 範例為概念程式碼，展示 ctypes 用法
```
