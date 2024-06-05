# C++特性

- `constexpr`关键字：
    - 修饰变量的时候，可以把 `constexpr` 对象当作加强版的 `const` 对象：`const` 对象表明值不会改变，但不一定能够在编译期取得结果；`constexpr` 对象不仅值不会改变，而且保证能够在编译期取得结果。如果一个 const 变量能够在编译期求值，将其改为 `constexpr` 能够让代码更清晰易读。
    - `constexpr` 函数可以把运行期计算迁移至编译期，使得程序运行更快（但会增加编译时间）。但如果 `constexpr` 函数中存在无法在编译期求值的参数，则 `constexpr` 函数和普通函数一样在运行时求值，此时的返回值不是常量表达式

- C++中，表示大小或基于内存的索引时通常推荐使用`std::size_t`。这是因为`std::size_t`是一个无符号整数类型，其大小是为了能够安全地表示对象的大小，以及对象最大可能的索引，这样可以增强代码的可移植性和安全性。

- 使用单引号（'）作为数字分隔符，这个特性自 C++14 起被引入

- `std::copy` 是标准库算法，用于复制一个范围内的元素到另一个范围。

- `std::optional<T>`是C++17的新特性，是一个模板类，要么包含一个为T的值，要么包含nothing，即`nullopt`，注意这个不等价于`nullptr`
    - `Nullptr`: an object that can be converted to a value of any pointer type
    - `Nullopt`: an object that can be converted to a value of any optional type
    - 在很多场景下，这种类型特别有用，特别是当函数需要返回一个值，但在某些情况下可能没有合适的值可返回时，比如：

```cpp
#include <iostream>
#include <optional>

std::optional<int> divide(int numerator, int denominator) {
  if (denominator != 0) {
    
    return numerator / denominator;
  } else {
    // 用nullopt处理这种异常情况，更安全
    return std::nullopt;
  }
}

int main() {
  int a = 10;
  int b = 2;

  std::optional<int> result = divide(a, b);

  if (result) {
    std::cout << "Result: " << result.value() << std::endl;
  } else {
    std::cout << "Division by zero occurred." << std::endl;
  }

  result = divide(10, 0);

  if (result) {
    std::cout << "Result: " << result.value() << std::endl;
  } else {
    std::cout << "Division by zero occurred." << std::endl;
  }

  return 0;
}
```

- `[[nodiscard]]` 是 C++17 引入的一个属性，用于指示函数的返回值不应被忽略。当一个函数被标记为 [[nodiscard]]，如果调用该函数但不使用其返回值，编译器将发出警告或错误。这对于那些返回值表示状态、错误或对程序逻辑至关重要的函数特别有用。

1. 错误状态检查：对于那些返回错误码或状态的函数，使用 `[[nodiscard]]` 可以确保调用者检查这些返回值，从而避免错误被忽视
```cpp
#include <iostream>
#include <vector>

[[nodiscard]] bool insertIntoDatabase(const std::vector<int>& data) {
    if (data.empty()) {
        std::cerr << "Error: Data is empty." << std::endl;
        return false;
    }
    // 插入数据到数据库...
    return true;
}

int main() {
    std::vector<int> myData;

    // 假设忽略了函数返回值
    insertIntoDatabase(myData); // 编译器将发出警告

    // 正确的做法
    if (!insertIntoDatabase(myData)) {
        // 处理错误
    }

    return 0;
}
```

2. 重要的返回值
```cpp
#include <memory>

class MyClass {};

[[nodiscard]] std::unique_ptr<MyClass> createMyClass() {
    return std::make_unique<MyClass>();
}

int main() {
    createMyClass(); // 这将导致编译器警告，因为创建的对象没有被使用

    auto myObject = createMyClass(); // 正确使用

    return 0;
}
```

## 测试结果
![](assets/2024-06-05-23-15-56.png)