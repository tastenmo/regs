
#include <cassert>

#define ESCAD_ASSERT(EXPR, ...) assert(EXPR)
//  #define ESCAD_ASSERT(EXPR, MSG) std::cout << "Assertion-Beschreibung: " MSG
//  << std::endl; assert(EXPR)
// #define ESCAD_ASSERT(EXPR, ...) throw EXPR;