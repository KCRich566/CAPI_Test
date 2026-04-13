# Single-Header Library

A single-header library is a library implemented entirely within one header file.  
單一標頭函式庫（single-header library）是將所有實作放在一個 .h 檔中的函式庫。

A single-header library packages interface and optional implementation in one header file.  
單一標頭函式庫將介面與（可選的）實作包在同一個標頭檔中。

It usually contains both declarations and implementations in the same file.  
它通常同時包含宣告與實作在同一個檔案中。

Users only need to include one file to use the library.  
使用者只需要 include 一個檔案即可使用該函式庫。

---

## Basic Example

Functions can be defined as `inline` to avoid multiple definition errors.  
函式通常會宣告為 `inline` 以避免重複定義錯誤。

```cpp
// math_utils.h
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

inline int add(int a, int b) {
    return a + b;
}

#endif


Implementation Control via Macro

Implementation is enabled by defining a macro (e.g., MYLIB_IMPLEMENTATION) in exactly one translation unit.
透過在恰好一個翻譯單元中定義巨集（例如 MYLIB_IMPLEMENTATION）來啟用實作。

Some libraries use a macro to separate declaration and implementation.
有些函式庫會用巨集來分離宣告與實作。




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
在其中一個 .cpp 檔中定義巨集後再 include 此標頭。

```cpp
#define MYLIB_IMPLEMENTATION
#include "mylib.h"
```

This avoids multiple definition while keeping a single-header distribution.
這樣可以避免重複定義，同時維持單一標頭的發佈方式。

Why Use Single-Header Libraries

Single-header libraries are easy to integrate (drop-in) and avoid linking steps.
單一標頭庫易於整合（drop-in），可避免額外的連結步驟。

It simplifies distribution since only one file is needed.
它簡化了發佈流程，因為只需要一個檔案。

It is suitable for tool libraries, small projects, and open-source distribution.
適合工具型函式庫、小型專案與開源發佈。

Trade-offs

Downsides: longer compile times, potential code bloat, and exposed implementation.
缺點：編譯時間變長、可能造成二進位膨脹，且實作被公開。

Large headers can increase compile time.
大型標頭檔可能會增加編譯時間。

Implementation changes require recompilation of all including files.
實作變更會導致所有 include 的檔案重新編譯。

Real-World Example

STB (stb_image) is a well-known example of a single-header library.
STB（stb_image）是單一標頭函式庫的知名範例。



