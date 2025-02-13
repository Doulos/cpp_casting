//------------------------------------------------------------------------------
#include <cstdint>
#include <cmath>
#include <tuple>
#include <random>
#include <gsl/gsl>
using namespace std::literals;
#include "macros.hpp"
[[nodiscard]] int16_t movement() {
  // Small random movement
  std::random_device rd;
  static std::mt19937 gen{rd()};
  static constexpr std::initializer_list<double> normal = { 0.1, 1.7, 4.4, 9.2, 15.0, 19.1, 50.0, 19.1, 15.0, 9.2, 4.4, 1.7, 0.1 };
  //                                                     σ -3.0 -2.5 -2.0 -1.5  -1.0  -0.5   0.0,  0.5,  1.0, 1.5, 2.0, 2.5, 3.0
  //                                                     i   0    1    2    3     4     5     6     7     8    9   10   11   12
  static_assert( normal.size() & 1, "Distribution should be odd to ensure center is zero" );
  static std::discrete_distribution<int16_t> dist{normal};
  auto offset = dist(gen) - normal.size()/2;
  auto k = int16_t{3};
  return gsl::narrow_cast<int16_t>((k * offset)&0x0FFFF);
}

struct Gps {
  [[nodiscard,maybe_unused]] int16_t read_x() const { return m_x; }
  [[nodiscard,maybe_unused]] int16_t read_y() const { return m_y; }
  [[nodiscard,maybe_unused]] int16_t read_z() const { return m_z; }
  [[nodiscard,maybe_unused]] uint64_t read_xyz() const {
    return (static_cast<uint64_t>(m_x) << 32)
         | (static_cast<uint64_t>(m_y) << 16)
         | (static_cast<uint64_t>(m_z));
  }
  [[maybe_unused]]void write(volatile uint64_t xyz) {
    m_x = static_cast<volatile int16_t>( xyz >> 32 );
    m_y = static_cast<volatile int16_t>( (xyz >> 16) & 0xFFFFu );
    m_z = static_cast<volatile int16_t>( xyz & 0xFFFFu );
  }
  [[maybe_unused]]void write(volatile int16_t x, volatile int16_t y, volatile int16_t z) {
    std::tie(m_x,m_y,m_z) = std::tie(x,y,z);
  }
  [[maybe_unused]]void sample() {
    m_x += movement();
    m_y += movement();
    m_z += movement();
  }
private:
  volatile int16_t m_x{};
  volatile int16_t m_y{};
  volatile int16_t m_z{};
};

constexpr auto magic = 0x0100'2152'4110ull ;

#include "fixed.h"
void slide7_1() { // `T(expr)` explicit conversion
  FUNC;
  auto gps = Gps{};
  gps.write(magic); // lower 48 bits become three 16-bit fields (x,y,z)
  auto raw = gps.read_xyz(); // no casting required
  auto real = static_cast<double>(static_cast<int64_t>(raw>>24));
  auto scaled = numeric::fixed<16,16>{real}; //
  std::cout << "scaled=" << scaled << '\n';
}

// Background
struct Base {
  Base() = default;
  explicit Base(int i):i{i}{}
  explicit operator int() const { return i; }
  virtual explicit operator std::string() const
    { return "Base:"s + std::to_string(i); }
  virtual ~Base() = default;
  protected: int i{0};
};
struct Derived : Base {
  Derived() = default;
  explicit Derived(const Base& b):Base{int(b)}{}
  explicit operator std::string() const override
    { return "Derived:"s + std::to_string(i); }
  void inc10() { i += int(b); }
  Base b{10};
};

// Down-casting
void downcast1(Base* b) { // very unsafe
  static_cast<Derived*>(b)->inc10();
}
void downcast2(Base* b) { // possibly unsafe
  if ( int(b) >= 10 ) { //< valid assumption?
    static_cast<Derived&>(*b).inc10();
  }
}
void downcast3(Base* b) { // safe
  if ( auto d = dynamic_cast<Derived*>(b); d != nullptr )
    d->inc10();
  else
    printf("Not a Derived!\n");
}

//------------------------------------------------------------------------------
enum class Day { mon, tue, wed, thu, fri, sat, sun };
Day& operator++(Day& d) {
  return d = (d == Day::sun) ? Day::mon
                             : static_cast<Day>(static_cast<int>(d)+1);
}
std::string to_string(const Day& d) {
  return "mon tue wed thu fri sat sun"s.substr(4*static_cast<int>(d), 3);
}

void slide7_2() { // `static_cast<T>(expr)`
  FUNC;

  // Basic construction/conversion
  Base b{42};
  Derived d{};
  d.inc10();
  auto bp1 = &d; // implicit upcast
  auto bp2 = &d.b; // implicit downcast

  auto iPi = static_cast<int>(2*atan(1.0));
  auto day = Day::tue;
  for (auto n=9; n--;) {
    SHOW_EXPRESSION(to_string(day));
    ++day;
  }
  auto i8 = int8_t{-42};
  auto flt = static_cast<float>(i8)/-5.0f;
  auto u8 = static_cast<uint8_t>(flt);
  flt = 1.2e9f;
  auto i_ptr = (int*)(&flt); // "Programmer knows better"
  SHOW_EXPRESSION(*i_ptr);

  auto bp3 = static_cast<Base*>(&d); // Good: explicit upcast
  auto dp3 = static_cast<Derived*>(bp2); // Bad: do not use static_cast to downcast
  dp3->inc10(); // crashes if compiler fails discernment

  downcast1(&b); // likely crash
  downcast2(&b); // likely crash
  downcast3(&b); // should work

  SHOW_EXPRESSION(iPi);

  SHOW_EXPRESSION(std::string(d));
  SHOW_EXPRESSION(std::string(*bp1));
  SHOW_EXPRESSION(std::string(*bp2));
  SHOW_EXPRESSION(std::string(*bp3));
  //SHOW_EXPRESSION(dp3->to_string()); // crashes
  SHOW_EXPRESSION(int(i8));
  SHOW_EXPRESSION(flt);
  SHOW_EXPRESSION(int(u8));
}

//------------------------------------------------------------------------------
void slide7_3() { // `dynamic_cast<T>(ptr)`
  FUNC;
}

void g(const int& ref, const int* ptr = nullptr ) {
  std::cout << CYAN << HLINE << NONE;
  SHOW_FUNC_PTR(&ref);
  SHOW_FUNC_PTR(ptr);
  if( ptr != nullptr ) {
    *const_cast<int*>(ptr) += 666;
    SHOW_FUNC_PTR(ptr);
  }
  *const_cast<int*>(&ref) += 10;
  SHOW_FUNC_PTR(&ref);
  const_cast<int&>(ref) += 5;
  SHOW_FUNC_EXPR(ref);
}

//------------------------------------------------------------------------------
void slide8_1() { // The Bad
  FUNC;
  const int K = 1'000;
  SHOW_POINTER(&K);
  g(5);
  g( K, &K );
  auto var = 2*K;
  SHOW_EXPRESSION(var);
  SHOW_EXPRESSION(K);
  SHOW_POINTER(&K);
}

//------------------------------------------------------------------------------
void slide9_1() { // `reinterpret_cast<T>(expr)`
  FUNC;
  auto uart_tx = reinterpret_cast<volatile uint32_t* const>(0x4000'1C02); // Transmit register
  SHOW_EXPRESSION(uart_tx);
}

void do_slides() {
  slide7_1(); // `T(expr)` explicit conversion
  slide7_2(); // `static_cast<T>(expr)`
  slide7_3(); // `dynamic_cast<T>(ptr)`
  slide8_1(); // The Bad
  slide9_1(); // `reinterpret_cast<T>(expr)`
}
