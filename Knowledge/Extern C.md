# Extern C.md

`extern "C"` tells the C++ compiler to use C-style linkage (disable name mangling).

C++ mangles symbol names for overloading; `extern "C"` ensures plain C-compatible names.

---

## Cross-Language Usage

Use `extern "C"` to expose C++ functions to C, C#, Python, or other languages.

Often combined with `__declspec(dllexport)` on Windows for DLL symbol exports.

---

## What It Does (and Does NOT Do)

`extern "C"` only affects symbol naming; it does not change calling convention or data layout.

It does not disable C++ features such as classes, templates, or exceptions internally.

---

## ABI Safety Note

extern "C" is necessary but not sufficient for full cross-language ABI safety.

You still must control:
- calling convention
- data layout
- memory ownership rules

---

## Practical Rule

Use `extern "C"` as a boundary layer, not as a full interoperability solution.
