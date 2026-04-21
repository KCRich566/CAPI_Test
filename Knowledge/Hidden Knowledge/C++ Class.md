# C++ Class

C++ classes are blueprints for objects, supporting encapsulation, inheritance, and polymorphism.

Constructors initialize objects; destructors clean up resources.

Encapsulation: use `private` to protect data and public getters/setters to access it.

Inheritance lets derived classes extend base classes; polymorphism uses virtual functions.

Separate header (`.h`) and source (`.cpp`) to reduce compile-time coupling and hide implementation.

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

## Class Implementation Example (.cpp)

Source files define the actual implementation of class functions.

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

## Why Separate Header and Implementation

Separation reduces compile-time dependencies and speeds up builds.

Changes in implementation do not require recompiling all dependent files.

It hides internal details and improves encapsulation.

It provides a clear interface for other developers to use and supports large-scale project maintainability.

## Const Member Functions

A `const` after a member function means the function does not modify the object's state.

Inside a `const` function, member variables are treated as read-only; the compiler prevents calling non-`const` members.

The `this` pointer becomes `const Person*` inside a `const` member function.

Const correctness improves code safety and readability.

Example:

```cpp
std::string Person::getName() const {
    return name_;
}
```

This function promises not to modify `name_` or any other member.

Const methods can be invoked on const objects but not non-const methods.

```cpp
const Person p("Kenzo");
 p.getName();   // OK
 // p.setName("A"); // Error
```

This const behavior ensures functions do not accidentally modify object state.
