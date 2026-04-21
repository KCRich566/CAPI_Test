# Opaque Patterns in C/C++ — Types, Pointers, and API Design

This document summarizes common opaque-type patterns in C and C++, the rationale behind them, and practical guidance.

---

## Key Benefits

- Encapsulation — hide implementation details and reduce coupling.
- API stability — internal changes are less likely to break clients (ABI stability).
- Controlled memory management — centralize allocation and deallocation responsibilities.
- Safety — prevent external code from accidentally accessing internal state.

These benefits make the opaque pattern widely used in C APIs, cross-language bindings, and projects requiring binary compatibility.

---

## Core Idea

The library exposes only a forward-declared type or an opaque handle (e.g. `void*` or `struct Foo*`) so callers cannot inspect or depend on the internal layout.

---

## Common Patterns

1) Forward declaration + pointer (C)

```c
// header: public API
typedef struct MyStruct MyStruct;
MyStruct* MyStruct_create(void);
void MyStruct_destroy(MyStruct* s);
void MyStruct_doSomething(MyStruct* s);
```

```c
// implementation: full definition hidden from users
struct MyStruct { int value; char* name; };
```

2) Handle type (`void*`) — simplest black box

```c
typedef void* OpaqueHandle;
OpaqueHandle openResource(void);
void closeResource(OpaqueHandle h);
```

3) PIMPL in C++ (Pointer-to-Implementation)

```cpp
// public header
class Widget { public: Widget(); ~Widget(); void set(int); int get() const;
private: struct Impl; Impl* pImpl; };

// cpp: define Impl with STL, third-party headers, etc.
```

PIMPL reduces compile-time dependencies, controls symbol exports, and helps maintain ABI.

---

## API Design Rules

- Ownership: who allocates and who frees resources (e.g. `calculator_dup_history_file()` returns a caller-owned string that must be freed with `calculator_free()`).
- Lifetimes: document when returned pointers remain valid or become invalid.
- Error handling: do not let C++ exceptions cross the C API boundary; use error codes.
- Thread-safety: if not thread-safe, state that callers must synchronize.

---

## Cross-language ABI Guidance

- Use `extern "C"` for C++ → C linkage to avoid name mangling.
- Export symbols portably (Windows `__declspec(dllexport)` vs GCC/Clang `__attribute__((visibility("default")))`).
- Prefer plain C types in the public header (no STL, no exceptions, stable fundamental types).

Example export macro:

```c
#if defined(_WIN32) || defined(_WIN64)
#ifdef MYLIB_EXPORTS
#define MYLIB_API __declspec(dllexport)
#else
#define MYLIB_API __declspec(dllimport)
#endif
#else
#if defined(__GNUC__) && __GNUC__ >= 4
#define MYLIB_API __attribute__((visibility("default")))
#else
#define MYLIB_API
#endif
#endif
```

---

## Common Trade-offs

- Extra indirection (pointer) adds a small runtime overhead.
- Heap allocation increases complexity and potential fragmentation.
- Debugging is harder because internal state is hidden.

Trade-off: gain encapsulation and ABI stability at the cost of reduced transparency and minor performance overhead.

---

## Practical Recommendations

- Document the API thoroughly: ownership, lifetimes, thread-safety, and error semantics.
- Keep the public header pure C (no C++ standard library types).
- Wrap C++ implementation with C-style wrappers that catch exceptions and return error codes.
- Provide both library-owned and caller-owned accessor variants when returning pointers to internal data (and a matching free function).
- Verify ABI after build (tools: `dumpbin` on Windows; `nm`, `readelf`, `file`, `ldd` on Linux).

---

## Summary

Opaque types are a simple, robust pattern for hiding implementation details, preserving ABI stability, and enabling safe cross-language interop. Use forward declarations, `void*` handles, or the PIMPL pattern as appropriate and document ownership and error semantics clearly.
