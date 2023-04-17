/*
 * This code is from an article with the title **C++ Casting** found on the Doulos website under 
 * the KnowHow section.
 *
 * This C++ source illustrates the use of casting both Bad and Good. Be careful
 * not to infer these are recommended codings even though they do compile reasonably
 * well.
 *
 * We assume C++17, but only for a few features: [[nodiscard]], and string
 * literals (i.e., "Hello"s is equivalent to std::string{"Hello"})
 *
 */
static_assert( __cplusplus >= 201703L, "Requires C++17" );
#include <iostream>
#include <string>
#include <cmath>
using namespace std::literals;

#define SHOW_EXPRESSION(var) std::cout << __LINE__ << ": " << #var << " = " << (var) << '\n'
#define SHOW_POINTER(ptr)    std::cout << __LINE__ << ": " << #ptr << " = @";\
  if( nullptr == (ptr)) std::cout << "nullptr\n";\
  else                  std::cout << ((void*)(ptr)) << '\n'

void compiler_provided()
{
  double d1 = 3.14159, d2;
  float  f1;
  int i1 = 42, i2;
  d2 = i1; /* implicit conversion */ SHOW_EXPRESSION( d2);
  f1 = d1; /* implicit narrowing  */ SHOW_EXPRESSION( f1);
  i2 = d2; /* implicit conversion */ SHOW_EXPRESSION( i2);
  d2 = f1; /* implicit widening   */ SHOW_EXPRESSION( d2);

  // unwanted implicit conversion
  int buffer['z']{}; // an array of 122 integers -- bug?
  auto bufferDepth = sizeof(buffer)/sizeof(buffer[0]); SHOW_EXPRESSION( bufferDepth);
  SHOW_EXPRESSION( sizeof(buffer));
#ifdef ENABLE_BUGS
  buffer['z'] = 123; //< compiler emits error and warning!
#endif
  SHOW_EXPRESSION( buffer[int( 'z')]); //< suppressed warning, but error still there

  int i;
  float Pi(3.14159); SHOW_EXPRESSION( Pi);
  i = int(Pi); /* explicit conversion */ SHOW_EXPRESSION( i);

}

class Polar; //< forward declare
class Rect {
public:
  Rect(double x, double y) : m_x{x}, m_y{y} {}
  Rect() = default; // default constructor
  explicit Rect(double x) : Rect{x,0.0} {} // explicit constructor
  explicit Rect(const Polar& p); // explicit conversion
  explicit operator double() const { return m_x; } // explicit conversion
  [[nodiscard]] std::string string() const {
    return "R{ "s + std::to_string(m_x) + ", "s + std::to_string(m_y) + " }"s;
  }
  friend std::ostream& operator<<(std::ostream& os, const Rect& rhs) {
    return os << rhs.string();
  }
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
  [[nodiscard]] std::string string() const {
    return "P{ "s + std::to_string(m_r) + ", "s + std::to_string(m_a) + " }"s;
  }
  friend std::ostream& operator<<(std::ostream& os, Polar const& rhs) {
    return os << rhs.string();
  }
private:
  double m_r{}, m_a{};
};

Rect::Rect(const Polar& p) : Rect{ p.run(), p.rise() } {}

void use_polar_rect()
{
  Rect  r0{};                 SHOW_EXPRESSION( r0);
  Rect  r1{1.1};              SHOW_EXPRESSION( r1);
  Rect  r2{2.1,2.2};          SHOW_EXPRESSION( r2);
  Rect  r45{ 1.0, 1.0 };
  Polar p0{};                 SHOW_EXPRESSION( p0);
  Polar p1{1.2};              SHOW_EXPRESSION( p1);
  Polar p2{1.2, r45.angle()}; SHOW_EXPRESSION( p2);
  Polar p3{r2};               SHOW_EXPRESSION( p3);
  Rect  r3{p2};               SHOW_EXPRESSION( r3);
}

int main()
{
  compiler_provided();
  use_polar_rect();

  // C-style casting is dangerous
  const char *pcChar = "HelloWorld"; SHOW_POINTER( &pcChar); SHOW_EXPRESSION( pcChar);
  float *pFloat{nullptr}; SHOW_POINTER( pFloat);
  char  *pChar{nullptr}; SHOW_POINTER( pChar);
  pFloat = (float*)pcChar; //< DANGEROUS cast - do you really know the internal format of floats?
  SHOW_EXPRESSION( pFloat);
  int i{42}; SHOW_EXPRESSION( i);
  int *pInt{ &i }; SHOW_POINTER( pInt);
  short *pShort{nullptr};
  pShort = (short*)&i; //< DANGEROUS cast - narrowing (endianness dependent)

  // Uses of cast
  pChar = const_cast<char*>(pcChar); //< DANGER: Assigning to *pChar is undefined behaviour
  // *pChar = 'h'; SHOW_EXPRESSION(pChar); // CRASH!!

  return 0;
}
