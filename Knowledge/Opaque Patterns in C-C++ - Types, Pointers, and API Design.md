# Opaque Patterns in C/C++ — Types, Pointers, and API Design

此文件整理了在 C 與 C++ 中實作不透明類型（opaque type）的常見做法、設計理由與注意事項，並以雙語（中 / 英）呈現要點與範例，方便用於教學或簡報。

This document summarizes common opaque-type patterns in C and C++, the rationale behind them, and practical guidance. Key points are presented in both Chinese and English.

---

## Key Benefits / 核心優勢

- Encapsulation — 隱藏實作細節，減少耦合。
- API stability — 修改內部實作不易破壞使用者程式（ABI 穩定性）。
- Controlled memory management — 凝聚分配與釋放責任。
- Safety — 避免外部直接存取內部狀態造成誤用。

這些優勢使不透明模式廣泛用於 C API、跨語言綁定與二進位相容性需求的專案。

---

## Core Idea / 核心概念

使用者只看到一個前向宣告的型別或一個 opaque handle（例如 `void*` 或 `struct Foo*`），而無法得知其內部欄位或實作細節。

The library exposes only a forward-declared type or an opaque handle (e.g. `void*` or `struct Foo*`) so callers cannot inspect or depend on the internal layout.

---

## Common Patterns 範例

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

PIMPL 減少編譯依賴、控制符號匯出，並有助於維持 ABI。

---

## API Design Rules / 設計規則（要明確記載）

- Ownership / 所有權：誰負責配置與釋放（e.g. `calculator_dup_history_file()` 回傳 caller-owned 字串，必須呼叫 `calculator_free()` 釋放）。
- Lifetimes / 生命週期：回傳的指標在何種情況下仍有效或會變得無效。
- Error handling / 錯誤處理：不要讓 C++ exception 跨越 C API 邊界；使用 error codes 回傳錯誤。
- Thread-safety / 執行緒安全：若非 thread-safe 必須在文件中明確說明。

---

## Cross-language ABI Guidance / 跨語言 ABI 建議

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

## Common Trade-offs / 設計取捨

- Extra indirection (pointer) adds small runtime overhead.
- Heap allocation increases complexity and potential fragmentation.
- Debugging is harder because internal state is hidden.

考量：以封裝與 ABI 穩定性換取較少透明度與微小效能成本。

---

## Practical Recommendations / 實務建議

- Document the API thoroughly: ownership, lifetimes, thread-safety, and error semantics.
- Keep the public header pure C (no C++ standard library types).
- Wrap C++ implementation with C-style wrappers that catch exceptions and return error codes.
- Provide both library-owned and caller-owned accessor variants when returning pointers to internal data (and a matching free function).
- Verify ABI after build (tools: `dumpbin` on Windows; `nm`, `readelf`, `file`, `ldd` on Linux).

---

## Summary / 總結

Opaque types are a simple, robust pattern for hiding implementation details, preserving ABI stability, and enabling safe cross-language interop. Use forward declarations, `void*` handles, or the PIMPL pattern as appropriate and document ownership and error semantics clearly.

不透明型別是隱藏實作、維持 ABI 穩定並簡化跨語言呼叫的有效方法。請搭配清楚的文件說明（所有權、生命週期、錯誤語義）以確保接口穩定與安全。






# Opaque Patterns in C/C++ — Types, Pointers, and API Design

## What is "opaque"?

In C and C++, an opaque type is a type whose internal structure is intentionally hidden from API users. The user can hold and pass values (usually pointers) that represent the object but cannot inspect or modify its internal fields directly.

Think of it as a black box: the interface is visible, the implementation is not.

## Why use opaque types?

- Encapsulation: hide implementation details.
- API stability: internal changes are less likely to break clients.
- Controlled memory management: the library controls allocation and deallocation.
- Safety: prevents accidental misuse of internal state.

## Common opaque patterns

### 1) Forward declaration + pointer (C)

Declare the type in the header without defining its fields, and expose functions that operate on pointers to that type.

```c
// mylib.h
typedef struct MyStruct MyStruct;

// Compared with MyStruct* MyStruct_create(), the "void" means that the function takes no parameters explicitly.
MyStruct* MyStruct_create(void);
void MyStruct_doSomething(MyStruct* obj);
void MyStruct_destroy(MyStruct* obj);
```

```c
// mylib.c
#include "mylib.h"
#include <stdlib.h>

struct MyStruct {
    int value;
    char* name;
};

MyStruct* MyStruct_create(void) {
    MyStruct* obj = malloc(sizeof(*obj));
    if (!obj) return NULL;
    obj->value = 0;
    obj->name = NULL;
    return obj;
}

void MyStruct_doSomething(MyStruct* obj) {
    if (!obj) return;
    obj->value += 1;
}

void MyStruct_destroy(MyStruct* obj) {
    if (!obj) return;
    free(obj->name);
    free(obj);
}
```

Clients only see `MyStruct*`; they cannot access or depend on the layout of the structure.

### 2) Handle types (void*)

Expose a generic handle type, typically `void*`, that points to an internal object. This is common for C APIs and cross-language bindings.

```c
typedef void* OpaqueHandle;

OpaqueHandle openResource(void);
void useResource(OpaqueHandle h);
void closeResource(OpaqueHandle h);
```

Internally the handle is cast to the real struct type or class; externally it is opaque.

### 3) Object-like API in C

Use opaque pointers to create a C-style object-oriented API.

```c
typedef struct Widget Widget;

Widget* Widget_new(void);
void Widget_setValue(Widget* w, int v);
int Widget_getValue(const Widget* w);
void Widget_free(Widget* w);
```

This pattern keeps state private while providing a clear lifecycle and operations.

### 4) PIMPL (Pointer to Implementation) in C++

PIMPL hides implementation details from the public header by moving them into a private implementation struct.

```cpp
// Widget.h
class Widget {
public:
    Widget();
    ~Widget();
    void setValue(int v);
    int getValue() const;

private:
    struct Impl;
    Impl* pImpl;
};
```

```cpp
// Widget.cpp
struct Widget::Impl { int value; };

Widget::Widget() : pImpl(new Impl{0}) {}
Widget::~Widget() { delete pImpl; }
void Widget::setValue(int v) { pImpl->value = v; }
int Widget::getValue() const { return pImpl->value; }
```

PIMPL reduces compile-time dependencies and helps preserve ABI when implementation changes.

## Practical notes and recommendations

- Document ownership: clearly state which functions allocate and which free resources.
- Lifecycle: document pointer lifetimes and whether returned pointers remain valid after other calls.
- Thread-safety: either make operations thread-safe internally or document that callers must synchronize.
- String/ptr returns: avoid returning pointers to internal buffers unless the lifetime is documented; provide copy-and-free helpers when needed.
- Cross-language interop: prefer `extern "C"` functions and error codes for stable ABI.

## Summary

Opaque types are a simple and effective way to hide implementation details, improve API stability, and enforce controlled resource management. Common techniques include forward declarations, `void*` handles, and C++ PIMPL. When designing an opaque API, be explicit about ownership, lifetimes, and thread-safety.
