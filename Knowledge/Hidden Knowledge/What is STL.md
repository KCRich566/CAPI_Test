# What is STL

The C++ Standard Template Library (STL) provides containers, algorithms, and iterators.

Common containers include `vector`, `list`, `map`, and `unordered_map`.

Common algorithms include `sort`, `reverse`, `find`, and `count`.

STL templates are header-only and optimized by the compiler.

---

## Key Concepts

STL is based on generic programming using templates.

Iterators behave like generalized pointers to traverse containers.

Algorithms are decoupled from containers and operate via iterators.

---

## Important Notes

Do not expose STL types in public C API headers; C compilers cannot parse C++ templates.

STL is implementation-dependent; internal details and layouts can vary between compilers.

When using STL internally but exposing a C API, prefer opaque pointers to hide implementation details.

---

## Practical Insights

STL provides reusable, well-tested components that reduce development time.

It improves code readability and maintainability in modern C++.

STL is widely used in production systems and competitive programming.
