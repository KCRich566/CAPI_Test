# What is Application Binary Interface (ABI)

ABI (Application Binary Interface) is the binary-level contract between compiled programs.  
ABI（應用程式二進位介面）是已編譯程式之間的二進位層級契約。

An Application Binary Interface (ABI) is the low-level contract that defines how compiled programs interact at runtime.  
應用程式二進位介面（ABI）是定義已編譯程式在執行期如何互動的低階契約。

Unlike an API (which is source-level), an ABI is binary-level and depends on compiler, platform, and architecture.  
ABI 與 API 不同（API 是原始碼層級），ABI 是二進位層級，並依賴編譯器、平台與架構。

API is at source code level; ABI is at binary / machine code level.  
API 屬於原始碼層級；ABI 屬於二進位 / 機器碼層級。

---

## What ABI Covers

ABI covers calling convention, data layout, symbol naming, and platform-specific rules.  
ABI 涵蓋呼叫約定、資料版面、符號命名與平台特定規則。

An ABI includes rules for how programs communicate at the machine level:

- Function calling conventions  
- 函式呼叫約定  

- Register usage and stack layout  
- 暫存器使用方式與堆疊配置  

- Data type sizes and alignment  
- 資料型別大小與對齊方式  

- Name mangling rules (C++ only)  
- 名稱改編規則（僅 C++）  

- Binary format (ELF, PE, Mach-O)  
- 二進位格式（ELF、PE、Mach-O）  

- Exception handling mechanisms  
- 例外處理機制  

ABI applies at compile, link, and runtime stages.  
ABI 在編譯、連結與執行階段皆適用。

---

## ABI vs API

| Aspect | API | ABI |
|--------|-----|-----|
| Level | Source code | Binary |
| Stability | Easier to change | Hard to change |
| Affected by compiler | No | Yes |
| Language | Human-readable | Machine-level |

API is what you write.  
API 是你寫的程式碼介面。

ABI is what the compiled program executes.  
ABI 是編譯後實際執行的二進位行為。

---

## Example: Function Call

C++ function:

```cpp
int add(int a, int b);
```

ABI defines:

How a and b are passed (registers or stack)
a 和 b 如何傳遞（暫存器或堆疊）
Where return value is stored
回傳值存放位置
How the symbol name is encoded
符號名稱如何編碼
Platform Differences

Different platforms have different ABIs:

Windows x64: Microsoft x64 ABI
Windows x64：Microsoft x64 ABI
Linux x64: System V ABI
Linux x64：System V ABI
ARM64: AArch64 ABI
ARM64：AArch64 ABI

Even if code is identical, binaries may not be compatible across platforms.
即使程式碼相同，不同平台的二進位仍可能不相容。

Why ABI Matters

ABI compatibility is critical for:

DLL / shared library compatibility
動態函式庫相容性
Cross-language interop (C, C++, Python, C#)
跨語言互通（C、C++、Python、C#）
Plugin systems
外掛系統
Operating system interfaces
作業系統介面
Cross-Language Interop

C is commonly used as an ABI-neutral layer (no overloading, minimal mangling, predictable layout).
C 常被用作 ABI 中立層（無多載、最少改編、可預測的版面）。

For cross-language interop, use a stable C ABI boundary (extern "C").
為跨語言互通，常使用穩定的 C ABI 邊界（extern "C"）。

C# uses P/Invoke and Python uses ctypes/cffi; both must match the native library's ABI.
C# 使用 P/Invoke、Python 使用 ctypes/cffi；兩者都須匹配原生函式庫的 ABI。

Common ABI Problems

ABI mismatch causes crashes, memory corruption, or undefined behavior.
ABI 不匹配會導致崩潰、記憶體損壞或未定義行為。

Mixing compilers (MSVC vs GCC)
混用編譯器
Different struct packing/alignment
結構對齊不同
Mismatched calling conventions
呼叫約定不一致
C++ name mangling differences
C++ 名稱改編差異
Why C is Often Used

C is often chosen as an ABI-stable interface layer:

No function overloading
無函式多載
Simple symbol naming
簡單符號命名
Predictable memory layout
可預測記憶體版面

This makes it ideal for cross-language boundaries.
這使其非常適合跨語言邊界。

Key Insight

ABI is the invisible contract between compiled programs.
ABI 是編譯後程式之間「看不見的契約」。

If API is the design, ABI is the reality of execution.
API 是設計層，ABI 是執行現實。

A stable system design must respect ABI constraints explicitly.
穩定的系統設計必須明確遵守 ABI 約束。