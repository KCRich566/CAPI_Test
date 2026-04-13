# What is STL

STL is the C++ Standard Template Library: containers, algorithms, and iterators.  
STL 是 C++ 的標準模板庫：容器、演算法與迭代器。

Common containers include vector, list, map, and unordered_map.  
常見容器有 vector、list、map 與 unordered_map。

Common algorithms include sort, reverse, find, and count.  
常見演算法有 sort、reverse、find 與 count。

STL is header-only for templates and heavily optimized by the compiler.  
STL 為範本（template）頭文件式，由編譯器進行深度最佳化。

---

## Key Concepts

STL is based on generic programming using templates.  
STL 基於泛型程式設計（generic programming），使用 template 實作。

Iterators act like generalized pointers to traverse containers.  
迭代器（iterator）像泛化的指標，用來遍歷容器。

Algorithms are decoupled from containers and operate via iterators.  
演算法與容器解耦，透過迭代器進行操作。

---

## Important Notes

STL must not appear in public C API headers; C compilers cannot understand it.  
STL 不應出現在公開的 C API 標頭中；C 編譯器無法理解它。

STL is implementation-dependent; different compilers have different internal layouts.  
STL 是實作相依的；不同編譯器有不同的內部結構與實作方式。

Expose C APIs with opaque pointers when using STL internally to hide implementation.  
如果內部使用 STL，對外提供 C API 時應使用不透明指標以隱藏實作。

---

## Practical Insight

STL reduces development time by providing reusable, well-tested components.  
STL 透過提供可重用且經過測試的元件來降低開發時間。

It improves code readability and maintainability in modern C++.  
它提升現代 C++ 的可讀性與可維護性。

It is widely used in production systems and competitive programming.  
它廣泛用於生產系統與競程開發中。