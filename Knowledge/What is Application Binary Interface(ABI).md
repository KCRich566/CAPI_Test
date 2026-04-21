# What is Application Binary Interface (ABI)

ABI (Application Binary Interface) is the binary-level contract between compiled programs.

An Application Binary Interface (ABI) is the low-level contract that defines how compiled programs interact at runtime.

Unlike an API (which is source-level), an ABI is binary-level and depends on compiler, platform, and architecture.

API is at source code level; ABI is at binary / machine code level.

---

## What ABI Covers

ABI covers calling convention, data layout, symbol naming, and platform-specific rules.

An ABI includes rules for how programs communicate at the machine level:

- Function calling conventions
- Register usage and stack layout
- Data type sizes and alignment
- Name mangling rules (C++ only)
- Binary format (ELF, PE, Mach-O)
- Exception handling mechanisms

ABI applies at compile, link, and runtime stages.

---

## ABI vs API

| Aspect | API | ABI |
|--------|-----|-----|
| Level | Source code | Binary |
| Stability | Easier to change | Hard to change |
| Affected by compiler | No | Yes |
| Language | Human-readable | Machine-level |

API is what you write. API is the source-level interface. ABI is the binary reality of execution.

---

## Example: Function Call

C++ function:

```cpp
int add(int a, int b);
```

ABI defines:

- How `a` and `b` are passed (registers or stack)
- Where return value is stored
- How the symbol name is encoded
- Platform differences

Different platforms have different ABIs:

- Windows x64: Microsoft x64 ABI
- Linux x64: System V ABI
- ARM64: AArch64 ABI

Even if code is identical, binaries may not be compatible across platforms.

---

## Why ABI Matters

ABI compatibility is critical for:

- DLL / shared library compatibility
- Cross-language interop (C, C++, Python, C#)
- Plugin systems
- Operating system interfaces

Cross-language interop

C is commonly used as an ABI-neutral layer (no overloading, minimal mangling, predictable layout).

For cross-language interop, use a stable C ABI boundary (`extern "C`).

C# uses P/Invoke and Python uses ctypes/cffi; both must match the native library's ABI.

---

## Common ABI Problems

ABI mismatch causes crashes, memory corruption, or undefined behavior.

Common causes:

- Mixing compilers (MSVC vs GCC)
- Different struct packing/alignment
- Mismatched calling conventions
- C++ name mangling differences

## Why C is Often Used

C is often chosen as an ABI-stable interface layer:

- No function overloading
- Simple symbol naming
- Predictable memory layout

This makes it ideal for cross-language boundaries.

---

## Key Insight

ABI is the invisible contract between compiled programs.

If API is the design, ABI is the reality of execution.

A stable system design must respect ABI constraints explicitly.