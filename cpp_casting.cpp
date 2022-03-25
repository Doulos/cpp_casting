/*
 * This C++ source illustrates the use of casting both Bad and Good. Be careful
 * not to infer these are recommended codings even though they do compile reasonably
 * well.
 *
 * We assume C++17, but only for a few features: [[nodiscard]]
 *
 */
static_assert( __cplusplus >= 201703L );
#include <iostream>
#include <string>
#include <cstdint>
#include <cmath>
using namespace std::literals;

#define SHOW(var)    std::cout << __LINE__ << ": " << #var << " = " << (var) << '\n'
#define SHOWPTR(ptr) std::cout << __LINE__ << ": " << #ptr << " = @";\
  if( nullptr == (ptr)) std::cout << "nullptr\n";\
  else                  std::cout << ((void*)(ptr)) << '\n'

void compiler_provided()
{
  double d1 = 3.14159, d2;
  float  f1;
  int i1 = 42, i2;
  d2 = i1; /* implicit conversion */ SHOW(d2);
  f1 = d1; /* implicit narrowing  */ SHOW(f1);
  i2 = d2; /* implicit conversion */ SHOW(i2);
  d2 = f1; /* implicit widening   */ SHOW(d2);

  // unwanted implicit conversion
  int buffer['z']{}; // an array of 122 integers -- bug?
  auto bufferDepth = sizeof(buffer)/sizeof(buffer[0]); SHOW(bufferDepth);
  SHOW(sizeof(buffer));
  buffer['z'] = 123; //< compiler emits error and warning!
  SHOW(buffer[int('z')]); //< suppressed warning, but error still there

  int i;
  float Pi(3.14159); SHOW(Pi);
  i = int(Pi); /* explicit conversion */ SHOW(i);

}

class Polar; //< forward declare
class Rect {
public:
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

void use_polar_rect()
{
  Rect  r0{};                 SHOW(r0);
  Rect  r1{1.1};              SHOW(r1);
  Rect  r2{2.1,2.2};          SHOW(r2);
  Rect  r45{ 1.0, 1.0 };
  Polar p0{};                 SHOW(p0);
  Polar p1{1.2};              SHOW(p1);
  Polar p2{1.2, r45.angle()}; SHOW(p2);
  Polar p3{r2};               SHOW(p3);
  Rect  r3{p2};               SHOW(r3);
}

int main()
{
  compiler_provided();
  use_polar_rect();

  // C-style casting is dangerous
  const char *pcChar = "HelloWorld"; SHOWPTR(&pcChar); SHOW(pcChar);
  float *pFloat{nullptr}; SHOWPTR(pFloat);
  char  *pChar{nullptr}; SHOWPTR(pChar);
  pFloat = (float*)pcChar; //< DANGEROUS cast - do you really know the internal format of floats?
  SHOW(pFloat);
  int i{42}; SHOW(i);
  int *pInt{ &i }; SHOWPTR(pInt);
  short *pShort{nullptr};
  pShort = (short*)&i; //< DANGEROUS cast - narrowing (endianess dependent)

  // Uses of cast
  pChar = const_cast<char*>(pcChar); //< DANGER: Assigning to *pChar is undefined behaviour
  // *pChar = 'h'; SHOW(pChar); // CRASH!!

  return 0;
}