# Callee vs Caller

Caller is the entity that initiates a function call; Callee is the function being called.  
呼叫者（Caller）是發起函式呼叫的實體；被呼叫者（Callee）是被呼叫的函式。

Argument passing, return value, and stack cleanup responsibilities depend on the calling convention.  
參數傳遞、回傳值與堆疊清理責任取決於呼叫約定。

---

## Stack Cleanup Rules

cdecl: caller cleans stack; stdcall: callee cleans stack; fastcall: platform-dependent.  
cdecl：呼叫者清理堆疊；stdcall：被呼叫者清理堆疊；fastcall：視平台而定。

On Windows x64, the caller must reserve 32 bytes of shadow space on the stack for the callee.  
在 Windows x64 上，呼叫者須在堆疊預留 32 位元組的陰影空間（shadow space）供被呼叫者使用。

---

## Registers and Return Values

Return values are placed in RAX (integer/pointer) or XMM0 (floating-point).  
回傳值放置於 RAX（整數/指標）或 XMM0（浮點數）。

Caller-saved registers (RAX, RCX, RDX, R8–R11) may be overwritten by the callee at any time.  
呼叫者保存的暫存器（RAX、RCX、RDX、R8–R11）可能在任何時候被被呼叫者覆寫。

Callee-saved registers (RBX, RBP, RDI, RSI, R12–R15) must be preserved by the callee if used.  
被呼叫者保存的暫存器（RBX、RBP、RDI、RSI、R12–R15）若被使用，被呼叫者須負責保存與還原。

---

## ABI & Interoperability Rules

In FFI contexts, mismatched conventions or type layouts commonly cause crashes.  
在 FFI（跨語言呼叫）情境中，呼叫約定或型別版面不符常會導致崩潰。

Use a C ABI boundary (`extern "C"`) and explicit memory ownership rules for safe interop.  
使用 C ABI 邊界（`extern "C"`）與明確的記憶體所有權規則以確保安全的互操作。

On x64, calling convention keywords are mostly ignored; the ABI is fixed per platform.  
在 x64 上，呼叫約定關鍵字大多被忽略；ABI 由平台固定。

---

## Safe Cross-Language Design

Use fixed-width types (int32_t, int64_t) for cross-platform consistency.  
使用固定寬度型別（int32_t、int64_t）以確保跨平台一致性。

Do not mix allocation and free across different runtimes; provide matching free functions.  
不要跨不同執行環境混用配置與釋放；需提供對應的釋放函式。

Avoid exposing exceptions, STL, or virtual functions across the ABI boundary.  
避免將例外、STL 或虛擬函式暴露至 ABI 邊界之外。