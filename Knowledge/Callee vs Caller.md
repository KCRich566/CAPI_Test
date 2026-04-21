# Callee vs Caller

The caller is the entity that initiates a function call; the callee is the function being called.

Argument passing, return value, and stack cleanup responsibilities depend on the calling convention.

---

## Stack Cleanup Rules

- `cdecl`: caller cleans the stack.
- `stdcall`: callee cleans the stack.
- `fastcall`: platform-dependent.

On Windows x64, the caller must reserve 32 bytes of shadow space on the stack for the callee.

---

## Registers and Return Values

Return values are placed in `RAX` (integer/pointer) or `XMM0` (floating-point).

Caller-saved registers (`RAX`, `RCX`, `RDX`, `R8`–`R11`) may be overwritten by the callee.

Callee-saved registers (`RBX`, `RBP`, `RDI`, `RSI`, `R12`–`R15`) must be preserved by the callee if used.

---

## ABI & Interoperability Rules

In FFI contexts, mismatched conventions or type layouts commonly cause crashes.

Use a C ABI boundary (`extern "C"`) and explicit memory ownership rules for safe interop.

On x64, calling convention keywords are mostly ignored; the platform ABI is fixed.

---

## Safe Cross-Language Design

- Use fixed-width types (`int32_t`, `int64_t`) for cross-platform consistency.
- Do not mix allocation and free across different runtimes; provide matching free functions.
- Avoid exposing exceptions, the C++ Standard Library (STL), or virtual functions across the ABI boundary.