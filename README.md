## Introduction to C++ casting issues ##
[Version 1.4]

There are many times when calling subroutines or assigning results from returns, the situation arises that types don't match, and C++ strict typing appears to get in the way. To solve this problem, there are several solutions including implict, explict conversions, and casting. Some of these are more dangerous than others. This article attempts to clarify and guide you to make good decisions.

Some general comments are appropriate before proceeding.

First, good C++ programmers avoid casting whenever possible.

Second, these same programmers rightfully take a dim view of any casting they see during code reviews.

Third, whenever you see or feel the need for casting, be afraid.

## Conversions ##

Conversions come in two varieties. Implicit conversions include things like `int` to `float` and visa versa. 

```cpp
    double d1 = 3.14159, d2;
    int i1 = 42, i2;
    d2 = i1; // implicit conversion
    i2 = f1; // implicit conversion
```
Unfortunately, *implicit* conversions can get you in trouble. For instance, `char` is considered to be an 8-bit integer. Consider the following legal, but highly likely bug:

```cpp
    int mem['t']{}; // an array of 116 integers - silently compiles -- bug?
```

To avoid this problem, you can use explicit conversions using the target type as an operator function. For instance:

```cpp
    int i;
    float pi(3.14159);
    i = int(pi); // explicit conversion
```

When creating classes, you can create your own conversions. Constructors act as conversions to a class. You can use `operator` definitions to define conversions to other classes. With modern C++ you also can require their explicit use by adding the `explict` keyword. In the following, omit the keyword `explicit` in the operators for C++ prior to C++11.

~~~cpp
class Polar; //< forward declare
class Rect {
public:
  using std::string;
  Rect(double x, double y) : m_x{x}, m_y{y} {}
  Rect() = default; // default constructor
  explicit Rect(double x) : Rect{x,0.0} {} // explicit constructor
  explicit Rect(const Polar& p); // explicit conversion
  explicit operator double() const { return m_x; } // explicit conversion
  [[nodiscard]] std::string string() const { return "R{ "s + std::to_string(m_x) + ", "s + std::to_string(m_y) + " }"s; }
  friend std::ostream& operator<<(std::ostream& os, const Rect& rhs) { os << rhs.string(); return os; }
  [[nodiscard]] double magnitude() const { return std::sqrt(m_x*m_x+m_y*m_y); }
  [[nodiscard]] double angle() const { return std::atan2(m_y,m_x); }
private:
  double m_x{}, m_y{};
};

class Polar {
public:
  Polar(double r, double a): m_r{r}, m_a{a} {}
  Polar() = default; // default constructor
  explicit Polar(double r) : Polar{r,0.0} {}
  explicit Polar(const Rect& r) : Polar{r.magnitude(), r.angle()} {}
  explicit operator double() const { return m_r; } // explicit conversion
  [[nodiscard]] double run() const { return m_r * std::sin(m_a); }
  [[nodiscard]] double rise() const { return m_r * std::cos(m_a); }
  [[nodiscard]] std::string string() const { return "P{ "s + std::to_string(m_r) + ", "s + std::to_string(m_a) + " }"s; }
  friend std::ostream& operator<<(std::ostream& os, Polar const& rhs) { os << rhs.string(); return os; }
private:
  double m_r{}, m_a{};
};

Rect::Rect(const Polar& p) : Rect{ p.run(), p.rise() } {}
~~~

Sometimes, a conversion does not exist, but you believe you know more than the compiler. In these situations, it may be appropriate to use a cast. For these situations, C++ supplies four types of casting with various levels of safety.

## C-style casts ##

C provides a cast of the form `(TYPE)VALUE`, but this is _extremely dangerous_! It says, I know more than the compiler, and because the syntax is terse (few characters), it is easy to overlook. Consider the following disaster:

```cpp
  char  *pc = "HelloWorld";
  float *pf;
  pf = (float*)pc; //< DANGEROUS cast - do you really know the internal format of floats?
```

Smart C++ programmers NEVER use C-style casts. There is another section on this ahead. Read on…

## Static cast ##

`static_cast<T>` is the first cast you should attempt to use. It is called static because the C++ standard requires compilers to validate `static_cast`s at compile-time.  If the compiler cannot resolve the type conversion as valid, then it won't compile. The `T` is a type name such as `int`, a class name or struct.

`static_cast<T>` does things like implicit conversions between compatible types (such as `int` to `float`, or pointer `to void*`), and it can also call explicit conversion functions (or implicit ones).

In many cases, explicitly stating `static_cast<T>` isn't necessary, but it's important to note that the `T(something)` syntax may be equivalent to `(T)something` (see ISO-IEC-14882-2011 section 5.2.3) and should be avoided (more on that later). A `T(something, something_else)` (i.e. two or more arguments) is safe, and guaranteed to call a constructor.

`static_cast<T>` can also cast through inheritance hierarchies. It is unnecessary when casting upwards (towards a base class), but when casting downwards it can be used as long as it doesn't cast through `virtual` inheritance, which requires use of `dynamic_cast`. It does not do run-time checking, however, and it is undefined behavior to `static_cast<T>` down a hierarchy to a type that isn't actually the type of the object. Thus `static_cast<>` should not generally be used for down casting.

## Const cast ##

`const_cast<T>` can be used to remove or add `const` to a variable and no other C++ cast is capable of removing it (not even `reinterpret_cast`). It is important to note that using it is only undefined if the original variable is `const`; if you use it to take the `const` off a reference to something that wasn't declared with `const`, it is safe. This can be useful when overloading member functions based on `const`, for instance. It can also be used to add `const` to an object, such as to call a member function overload. Although, there are occasionally good reasons to use `const_cast<T>`, many experts suggest it is dangerous. The danger comes because you can remove the `const` property from something that should never be changed. This will create undefined behavior.

You **should not** use the `const_cast<T>` operator to directly override a constant variable's constant status.

`const_cast` also works similarly on `volatile`, though that's less common.

## Dynamic cast ##

`dynamic_cast<T>` is almost exclusively used for handling polymorphism. You can cast a pointer or reference to any polymorphic type to any other class type (a polymorphic type has at least one `virtual` function, declared or inherited). You don't have to use it to cast downwards, you can cast sideways or even up another chain. The `dynamic_cast` will seek out the desired object and return it if possible. If it can't, it will return `nullptr` in the case of a pointer, or throw `std::bad_cast` in the case of a reference.

If type-id is not `void*`, a run-time check is made to see if the object pointed to by expression can be converted to the type pointed to by `T`.

`dynamic_cast` has some limitations, though. It doesn't work if there are multiple objects of the same type in the inheritance hierarchy (the so-called *dreaded diamond*) and you aren't using `virtual` inheritance. It also can only go through `public` inheritance - it will always fail to travel through `protected` or `private` inheritance. This is rarely an issue, however, as such forms of inheritance are rare.

Although `dynamic_cast` conversions are safer that `static_cast`, `dynamic_cast` only works on pointers or references, and the run-time type check is an overhead.

## Reinterpret cast ##

`reinterpret_cast<T>` is the most severe cast, and should be used very sparingly. It turns one type directly into another - such as casting the value from one pointer to another, or storing a pointer in an `int`, or all sorts of other nasty things. Largely, the only guarantee you get with `reinterpret_cast` is that if you cast the result back to the original type, you will get the exact same value. There are a number of conversions that `reinterpret_cast` cannot do, too. It's used primarily for particularly weird conversions and bit manipulations, like turning a raw data stream into actual data, or storing data in the low bits of an `aligned` pointer. Simply put, it is `int32_t herp = 1337; float* derp = (float*)&herp; float magic = *derp;` This is essentially how the [fast inverse square root](http://en.wikipedia.org/wiki/Fast_inverse_square_root) works.

## C-style casts ##

**C-style casts** are casts using `(type)object` or `type(object)`; unless, a C++ conversion operator or constructor of the form `type(object)` precludes the latter. A C-style cast is defined as the first of the following which succeeds:

```cpp
    const_cast<T>
    static_cast<T>
    static_cast, then const_cast<T>
    reinterpret_cast<T>
    reinterpret_cast<T>, then const_cast<T>
```

It can therefore be used as a replacement for other casts in some instances, but can be extremely dangerous because of the ability to devolve into a `reinterpret_cast`, and the latter should be preferred when `explicit` casting is needed, unless you are sure `static_cast` will succeed or `reinterpret_cast` will fail. Even then, consider the longer, more explicit option.

C-style casts also ignore access control when performing a `static_cast`, which means that they have the ability to perform an operation that no other cast can. This is mostly a kludge, though, and in my mind is just another reason to **avoid C-style casts**.

## Guidelines ##

- Whenever possible avoid using any type of casting. 
- During code reviews, be especially suspicious of casts and require documentation demonstrating the need.
- If casting is unavoidable, then justify the decision with a well written comment block next to every cast or group of casts.
- Use `dynamic_cast` for converting pointers/references within an inheritance hierarchy. Runtime overhead is insignificant compared to the bugs it may avert.
- Use `static_cast` for ordinary type conversions.
- Use `reinterpret_cast` for low-level reinterpretation of bit patterns. Use with **extreme caution**. This type of casting is often non-portable due to endianess issues.
- Use `const_cast` for casting away const/volatile. Avoid this unless you are stuck using a const-incorrect API.
- Use conversion operators (e.g. `operator int()`) or constructors (e.g. `T2(T1)`) when possible, but be sure they are conversions and not devolved C-style casts.
- Avoid C-style casts.

## References ##

0. [`Origin of this article`](http://stackoverflow.com/questions/332030/when-should-static-cast-dynamic-cast-and-reinterpret-cast-be-used)
1. [`Discussion of static_cast`](http://msdn.microsoft.com/en-us/library/c36yw7x9%28v=vs.80%29.aspx)
1. [Wikipedia `static_cast`](http://en.wikipedia.org/wiki/Static_cast)
2. [Wikipedia `dynamic_cast`](http://en.wikipedia.org/wiki/Dynamic_cast)
3. [Wikipedia `const_cast`](http://en.wikipedia.org/wiki/Const_cast)
4. [Wikipedia `reinterpret_cast`](http://en.wikipedia.org/wiki/Reinterpret_cast)

### The end

Written by David C Black, Senior Member Technical Staff at Doulos.<br/>
This article is Copyright (C) 2018-2022 by Doulos. All rights are reserved.

[<img src="https://doulos.com/media/1009/doulos-logo-header.svg" width=80 style="vertical-align:bottom"/>**<big>/knowhow</big>**]( http://www.doulos.com/knowhow/) 

Search [Doulos KnowHow](https://www.doulos.com/knowhow) for the very latest information.
