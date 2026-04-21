# C++ Class

C++ class is a blueprint for objects, supporting encapsulation, inheritance, and polymorphism.  

Constructor initializes objects; destructor cleans up resources.

Encapsulation: use private to protect data and public getters/setters to access it.

Inheritance lets derived classes extend base classes; polymorphism uses virtual functions.

Separate header (.h) and source (.cpp) to reduce compile-time coupling and hide implementation.

Use RAII, smart pointers, and PIMPL for robust resource management and stable ABI.

---

## Class Header Example (.h)

Header files declare the interface of a class without implementation details.

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

Changes in implementation do not require recompiling all dependent files.

It hides internal details and improves encapsulation.

It provides a clear interface for other developers to use.

It supports large-scale project organization and maintainability.

It provides a clear interface for other developers to use.

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

這裡的const表示不會修改到CalculatorContext物件的狀態，確保這個函式在被呼叫時不會改變任何成員變數的值。
```
double CalculatorContext::get_cur_value() const
```
