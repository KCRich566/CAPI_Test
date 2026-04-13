# Extern C.md

`extern "C"` tells the C++ compiler to use C-style linkage (disable name mangling).  
`extern "C"` 指示 C++ 編譯器使用 C 樣式的連結（停用名稱改編）。

C++ mangles symbol names for overloading; `extern "C"` ensures plain C-compatible names.  
C++ 為了支援多載會改編符號名稱；`extern "C"` 確保產生純 C 相容的名稱。

---

## Cross-Language Usage

Use `extern "C"` to expose C++ functions to C, C#, Python, or other languages.  
使用 `extern "C"` 將 C++ 函式暴露給 C、C#、Python 或其他語言。

Often combined with `__declspec(dllexport)` on Windows for DLL symbol exports.  
在 Windows 上常與 `__declspec(dllexport)` 搭配使用以匯出 DLL 符號。

---

## What It Does (and Does NOT Do)

`extern "C"` only affects symbol naming; it does not change calling convention or data layout.  
`extern "C"` 只影響符號命名；不會改變呼叫約定或資料版面。

It does not disable C++ features such as classes, templates, or exceptions internally.  
它不會關閉 C++ 功能，例如 class、template 或例外機制。

---

## ABI Safety Note

extern "C" is necessary but not sufficient for full cross-language ABI safety.  
extern "C" 是必要但不足以完全保證跨語言 ABI 的安全。

You still must control:
- calling convention  
- data layout  
- memory ownership rules  

仍然需要控制：
- 呼叫約定  
- 資料版面  
- 記憶體所有權規則  

---

## Practical Rule

Use `extern "C"` as a boundary layer, not as a full interoperability solution.  
將 `extern "C"` 當作邊界層，而不是完整的互操作解決方案。