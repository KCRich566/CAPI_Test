# C++ Class

C++ class is a blueprint for objects, supporting encapsulation, inheritance, and polymorphism.  
C++ 的 class 是物件的藍圖，支援封裝、繼承與多型。

Constructor initializes objects; destructor cleans up resources.  
建構子（Constructor）用來初始化物件；解構子（Destructor）負責清理資源。

Encapsulation: use private to protect data and public getters/setters to access it.  
封裝（Encapsulation）：使用 private 保護資料，透過 public getter/setter 存取。

Inheritance lets derived classes extend base classes; polymorphism uses virtual functions.  
繼承（Inheritance）讓衍生類別擴展基底類別；多型（Polymorphism）使用虛擬函式。

Separate header (.h) and source (.cpp) to reduce compile-time coupling and hide implementation.  
將標頭（.h）與原始碼（.cpp）分離，以降低編譯耦合並隱藏實作。

Use RAII, smart pointers, and PIMPL for robust resource management and stable ABI.  
使用 RAII、智慧指標與 PIMPL 以達到穩健的資源管理與穩定的 ABI。

---

## Class Header Example (.h)

Header files declare the interface of a class without implementation details.  
標頭檔（.h）宣告類別的介面，而不包含實作細節。

```cpp
// Person.h
#ifndef PERSON_H
#define PERSON_H

#include <string>

class Person {
public:
    Person(const std::string& name);
    ~Person();

    std::string getName() const;
    void setName(const std::string& name);

private:
    std::string name_;
};

#endif

```

Class Implementation Example (.cpp)

Source files define the actual implementation of the class functions.
原始碼檔（.cpp）定義類別函式的實際實作。

```cpp
// Person.cpp
#include "Person.h"

Person::Person(const std::string& name) : name_(name) {}

Person::~Person() {}

std::string Person::getName() const {
    return name_;
}

void Person::setName(const std::string& name) {
    name_ = name;
}
```

Why Separate Header and Implementation

Separation reduces compile-time dependencies and speeds up builds.
分離可以降低編譯相依性並加快編譯速度。

Changes in implementation do not require recompiling all dependent files.
修改實作時，不需要重新編譯所有相依檔案。

It hides internal details and improves encapsulation.
它可以隱藏內部細節並提升封裝性。

It provides a clear interface for other developers to use.
它為其他開發者提供清晰的使用介面。

It supports large-scale project organization and maintainability.
它有助於大型專案的結構化與可維護性。




const after a member function means the function does not modify the object's state.
成員函式後面的 const 表示此函式不會修改物件的狀態。

Inside a const function, all member variables are treated as read-only.
在 const 函式內，所有成員變數都會被視為唯讀。

The compiler prevents calling non-const member functions inside a const function.
編譯器會禁止在 const 函式中呼叫非 const 的成員函式。

The this pointer becomes const Person* inside a const function.
在 const 函式中，this 指標會變成 const Person*。

This ensures logical constness and helps avoid unintended side effects.
這可以確保邏輯上的不變性，避免不小心修改資料。

搭配你的例子
std::string Person::getName() const {
    return name_;
}

This function promises not to modify name_ or any other member.
這個函式保證不會修改 name_ 或任何其他成員。

為什麼很重要（實務）

You can call const functions on const objects, but not non-const functions.
你可以在 const 物件上呼叫 const 函式，但不能呼叫非 const 函式。

const Person p("Kenzo");
p.getName();   // OK
// p.setName("A"); // Error

Const correctness improves code safety and readability.
const 正確性可以提升程式安全性與可讀性。


這裡的const表示不會修改到CalculatorContext物件的狀態，確保這個函式在被呼叫時不會改變任何成員變數的值。
```
double CalculatorContext::get_cur_value() const
```
