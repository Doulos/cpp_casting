#pragma once

// Debug aids

#include <iostream>
#include <iomanip>
#include <sstream>

template< typename T >
std::string ptr2str( T ptr ) {
  static std::ostringstream os;
  os.str( "" );
  if ( nullptr == ( ptr ) ) { os << "nullptr"; }
  else { os << "[" << ( static_cast<const void*>(ptr) ) << "] = " << *ptr; }
  return os.str();
}

#define GREEN "\033[92;1m"
#define BLUE  "\033[94;1m"
#define CYAN  "\033[96;1m"
#define NONE "\033[0m"
#define FUNC std::cout << GREEN HLINE << __func__ << NONE << std::endl
#define SHOW_EXPRESSION( var ) std::cout << std::setw(3) << __LINE__ << ": " << #var << " = " << (var) << '\n'
#define SHOW_FUNC_EXPR( var )  std::cout << std::setw(3) << __LINE__ << ": " << __func__ << "() " << #var << " = " << (var) << '\n'
#define SHOW_POINTER( ptr )    std::cout << std::setw(3) << __LINE__ << ": " << #ptr << " = " << ptr2str(ptr) << '\n'
#define SHOW_FUNC_PTR( ptr )   std::cout << std::setw(3) << __LINE__ << ": " << __func__ << "() " << #ptr << " = " << ptr2str(ptr) << '\n'
#define HLINE "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n"
#define HEADING( str ) std::cout << HLINE << str << '\n' << HLINE