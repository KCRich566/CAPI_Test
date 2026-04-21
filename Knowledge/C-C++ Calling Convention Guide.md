# C/C++ Calling Convention Guide

C/C++ calling conventions define how functions are invoked at the binary level.

Always match calling convention and symbol linkage between caller and callee.

---

## Common Calling Conventions (Windows)

On Windows, common conventions include `__cdecl`, `__stdcall`, and `__fastcall`.

Variadic functions (e.g., `printf`) require `__cdecl` because only the caller knows the argument count.

Mismatched calling conventions between DLL export and consumer can cause stack corruption or crashes.

---

## Cross-Language Interoperability

Use `extern "C"` to avoid C++ name mangling for cross-language exports.

C# P/Invoke and Python `ctypes` must specify or assume the correct convention.

Prefer a clear, documented convention for cross-language DLLs (for example: `extern "C"` + `cdecl`).

---

## x64 Windows ABI

On x64 Windows, there is a single calling convention (Microsoft x64); keywords like `__cdecl` and `__stdcall` are ignored.

The first four integer/pointer arguments are passed in `RCX`, `RDX`, `R8`, `R9`; floating-point arguments in `XMM0`–`XMM3`.

---

## Key Takeaways

Calling conventions define stack usage, argument passing, and cleanup responsibility.

Incorrect conventions between modules lead to undefined behavior.

They are critical for DLL design, system programming, and language interoperability.