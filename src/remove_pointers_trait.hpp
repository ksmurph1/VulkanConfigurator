#ifndef remove_pointers_trait_hpp
#define remove_pointers_trait_hpp
#include <type_traits>
namespace configuration
{
template <typename T>
struct identity
{
    using type = T;
};

template<typename T>
struct remove_all_ptrs : std::conditional_t<
    std::is_pointer_v<T>,
    remove_all_ptrs<
        std::remove_pointer_t<T>
    >,
    identity<T>
>
{};
template<typename T>
using remove_all_ptrs_t = typename remove_all_ptrs<T>::type;
}
#endif