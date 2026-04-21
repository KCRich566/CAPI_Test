# Single-Header Library

A single-header library is a library implemented entirely within one header file.

A single-header library packages interface and optional implementation in one header file.

It usually contains both declarations and implementations in the same file.

Users only need to include one file to use the library.

---

## Basic Example

Functions can be defined as `inline` to avoid multiple definition errors.

```cpp
// math_utils.h
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

inline int add(int a, int b) {
    return a + b;
}

#endif
```

## Implementation Control via Macro

Implementation is enabled by defining a macro (e.g., `MYLIB_IMPLEMENTATION`) in exactly one translation unit.

Some libraries use a macro to separate declaration and implementation.

```cpp
// mylib.h
#ifndef MYLIB_H
#define MYLIB_H

int foo();

#ifdef MYLIB_IMPLEMENTATION
int foo() {
    return 42;
}
#endif

#endif
```

Define the macro in exactly one source file before including the header.

```cpp
#define MYLIB_IMPLEMENTATION
#include "mylib.h"
```

## Why Use Single-Header Libraries

Single-header libraries are easy to integrate (drop-in) and avoid linking steps.

They simplify distribution since only one file is needed.

They are suitable for tool libraries, small projects, and open-source distribution.

## Trade-offs

Downsides: longer compile times, potential code bloat, and exposed implementation.

Large headers can increase compile time.

Implementation changes require recompilation of all including files.

## Real-World Example

STB (stb_image) is a well-known example of a single-header library.



