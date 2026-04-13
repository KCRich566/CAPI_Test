# C-C++ Calling Convention Guide

C / C++ calling conventions define how functions are invoked at the binary level.  
C / C++ 的呼叫約定定義了在二進位層級上如何呼叫函式。

Always match calling convention and symbol linkage between caller and callee.  
呼叫端與被呼叫端之間務必匹配呼叫約定與符號連結方式。

---

## Common Calling Conventions (Windows)

On Windows, common conventions include __cdecl, __stdcall, and __fastcall.  
在 Windows 上，常見約定包括 __cdecl、__stdcall 和 __fastcall。

Variadic functions (e.g., printf) always require __cdecl because only the caller knows the argument count.  
可變參數函式（如 printf）一律需要 __cdecl，因為只有呼叫端才知道參數數量。

Mismatched calling conventions between DLL export and consumer cause stack corruption or crashes.  
DLL 匯出端與使用端的呼叫約定不匹配會導致堆疊損壞或崩潰。

---

## Cross-Language Interoperability

Use `extern "C"` to avoid C++ name mangling for cross-language exports.  
對於跨語言匯出，使用 `extern "C"` 可避免 C++ 名稱改編。

C# P/Invoke and Python ctypes must specify or assume the correct convention.  
C# 的 P/Invoke 與 Python 的 ctypes 必須指定或假設正確的呼叫約定。

Prefer a clear, documented convention for cross-language DLLs (e.g., extern "C" + cdecl).  
跨語言 DLL 建議採用明確且文件化的約定（例如 extern "C" + cdecl）。

---

## x64 Windows ABI

On x64 Windows, only one calling convention exists (Microsoft x64); __cdecl, __stdcall, etc. are ignored.  
在 x64 Windows 上，只有一種呼叫約定（Microsoft x64）；__cdecl、__stdcall 等關鍵字會被忽略。

The first four integer/pointer arguments are passed in RCX, RDX, R8, R9; floating-point in XMM0–XMM3.  
前四個整數/指標參數透過 RCX、RDX、R8、R9 傳遞；浮點數透過 XMM0–XMM3 傳遞。

---

## Key Takeaways

Calling conventions define stack usage, argument passing, and cleanup responsibility.  
呼叫約定定義堆疊使用方式、參數傳遞方式與清理責任。

Incorrect conventions between modules lead to undefined behavior.  
模組間呼叫約定錯誤會導致未定義行為。

They are critical for DLL design, system programming, and language interop.  
它們對 DLL 設計、系統程式設計與語言互通性非常重要。