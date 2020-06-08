#ifndef enumeration_wrapper_hpp
#define enumeration_wrapper_hpp
namespace configuration
{
template <typename T>
struct enumeration
{
   typedef T type;
};

template <typename T>
struct is_enumeration
{
   static constexpr bool value=false;
};
template<typename R>
struct is_enumeration<enumeration<R>>
{
   static constexpr bool value=true;
};
}
#endif