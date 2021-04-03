#ifndef is_static_castable_hpp
#define is_static_castable_hpp
#include "enumeration_wrapper.hpp"
#include <type_traits>

namespace configuration
{
   template <typename F, typename T, typename = T>
   struct is_static_castable : public std::false_type
   {};

   template <typename F, typename T>
   struct is_static_castable<F, T, decltype(static_cast<T>(std::declval<F>()))> : public std::true_type
   {};

}
#endif

