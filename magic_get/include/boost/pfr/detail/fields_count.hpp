// Copyright (c) 2016-2019 Antony Polukhin
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
// Kerry Murphy 01/20/2020 added support for arrays to be supported in structured binding.  Ex:  [a]=int[2] instead of [a,b]=int[2], no of elements need to be
// less.  arrays should not be expanded 
#ifndef BOOST_PFR_DETAIL_FIELDS_COUNT_HPP
#define BOOST_PFR_DETAIL_FIELDS_COUNT_HPP
#pragma once

#include "config.hpp"
#include "make_integer_sequence.hpp"

#include <climits>      // CHAR_BIT
#include <type_traits>
#include <utility>      // metaprogramming stuff

#ifdef __clang__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wmissing-braces"
#   pragma clang diagnostic ignored "-Wundefined-inline"
#   pragma clang diagnostic ignored "-Wundefined-internal"
#   pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif

namespace boost { namespace pfr { namespace detail {

///////////////////// General utility stuff
#define MAX_FIELD_COUNT std::numeric_limits<int8_t>::max()

static std::array<std::size_t,MAX_FIELD_COUNT> arrElemsToRemove{};
template <std::size_t Index>
using size_t_ = std::integral_constant<std::size_t, Index >;

template <std::size_t I>
struct ArrayIndexStored
{
   static std::size_t value;
};
template <std::size_t I>
std::size_t ArrayIndexStored<I>::value=0;
///////////////////// Structure that can be converted to reference to anything
struct ubiq_lref_constructor 
{
    const std::size_t ignore;

    template <class Type>
    constexpr operator Type&() const noexcept; // Undefined, allows initialization of reference fields (T& and const T&)
};

///////////////////// Structure that can be converted to rvalue reference to anything
struct ubiq_rref_constructor 
{
    const std::size_t ignore;

    template <class Type>
     constexpr operator Type&&() const noexcept; // Undefined, allows initialization of rvalue reference fields (T&&)
};

///////////////////// Structure that can be converted to reference to anything except reference to T
template <class T, bool IsCopyConstructible>
struct ubiq_constructor_except 
{
    template <class Type>
    constexpr operator std::enable_if_t<!std::is_same<T, Type>::value, Type&> () const noexcept; // Undefined

};

template <class T>
struct ubiq_constructor_except<T, false> 
{
    template <class Type> constexpr operator std::enable_if_t<!std::is_same<T, Type>::value, Type&&> () const noexcept; // Undefined

};

template<typename FieldType, std::size_t I>
constexpr bool storeArrayIdx()
{
   if constexpr (std::is_array_v<FieldType>)
   {
      ArrayIndexStored<I>::value=
         sizeof(FieldType) / sizeof(typename std::remove_all_extents<FieldType>::type);
      //return true;
   }
   else
   {
      arrElemsToRemove[I]=0;
      //ArrayIndexStored<I>::value=0;
      //return false;
   }
}

template<std::size_t I>
struct ubiq_lref_cons_arr
{
   template <typename Type,typename=std::enable_if_t<
     std::is_trivially_copy_constructible_v<Type> || std::is_copy_constructible_v<Type> ||
     std::is_default_constructible_v<Type>, Type&>>
   constexpr operator Type& () const noexcept // Undefined, allows initialization of reference fields (T& and const T&)
   {
      storeArrayIdx<Type,I>();
      //return Type{};
   }
};

template<std::size_t I>
struct ubiq_rref_cons_arr
{
   template <typename Type>
   constexpr operator std::enable_if_t</*storeArrayIdxAndRet<Type,I>() ||*/ // allow
   // fields to pass if not array
   !(std::is_copy_constructible_v<Type> ||
  std::is_trivially_copy_constructible_v<Type>), Type&&> () const noexcept; // Undefined, allows initialization of rvalue reference fields (T&&)
};

template <class T, std::size_t... I, typename /*Enable*/ = 
std::enable_if_t<std::is_copy_constructible_v<T> ||
      std::is_trivially_copy_constructible_v<T>>>
constexpr auto enable_if_cons_helper_arr(std::index_sequence<I...>) noexcept
    -> typename std::add_pointer<decltype(T{ ubiq_lref_cons_arr<I>{}... })>::type;

template <class T, std::size_t... I, typename /*Enable*/ =
      std::enable_if_t<!std::is_copy_constructible_v<T> || 
      std::is_trivially_copy_constructible_v<T>>>
constexpr auto enable_if_cons_helper_arr(std::index_sequence<I...>) noexcept
    -> typename std::add_pointer<decltype(T{ ubiq_rref_cons_arr<I>{}... })>::type;

template <class T, std::size_t... I, typename /*Enable*/ = decltype( enable_if_cons_helper_arr<T>(std::index_sequence<I...>) ) >
constexpr auto enable_if_cons_helper_arr_t(std::index_sequence<I...>) noexcept
->std::size_t;

///////////////////// Hand-made is_aggregate_initializable_n<T> trait

// `std::is_constructible<T, ubiq_constructor_except<T>>` consumes a lot of time, so we made a separate lazy trait for it.
template <std::size_t N, class T> struct is_single_field_and_aggregate_initializable: std::false_type {};
template <class T> struct is_single_field_and_aggregate_initializable<1, T>: std::integral_constant<
    bool, !std::is_constructible<T, ubiq_constructor_except<T, std::is_copy_constructible<T>::value>>::value
> {};

// Hand-made is_aggregate<T> trait:
// Aggregates could be constructed from `decltype(ubiq_?ref_constructor{I})...` but report that there's no constructor from `decltype(ubiq_?ref_constructor{I})...`
// Special case for N == 1: `std::is_constructible<T, ubiq_?ref_constructor>` returns true if N == 1 and T is copy/move constructible.
template <class T, std::size_t N>
struct is_aggregate_initializable_n {
    template <std::size_t ...I>
    static constexpr bool is_not_constructible_n(std::index_sequence<I...>) noexcept {
        return (!std::is_constructible<T, decltype(ubiq_lref_constructor{I})...>::value && !std::is_constructible<T, decltype(ubiq_rref_constructor{I})...>::value)
            || is_single_field_and_aggregate_initializable<N, T>::value
        ;
    }

    static constexpr bool value =
           std::is_empty<T>::value
        || std::is_array<T>::value
        #ifdef __cpp_lib_is_aggregate
        || std::is_aggregate<T>::value // Does not return `true` for build in types.
        #endif
        || std::is_standard_layout<T>::value
        || std::is_fundamental<T>::value
        || is_not_constructible_n(std::make_index_sequence<N>{})
    ;
};

///////////////////// Helper for SFINAE on fields count
template <class T, std::size_t... I, class /*Enable*/ = typename std::enable_if<std::is_copy_constructible<T>::value>::type>
constexpr auto enable_if_constructible_helper(std::index_sequence<I...>) noexcept
    -> typename std::add_pointer<decltype(T{ ubiq_lref_constructor{I}... })>::type;

template <class T, std::size_t... I, class /*Enable*/ = typename std::enable_if<!std::is_copy_constructible<T>::value>::type>
constexpr auto enable_if_constructible_helper(std::index_sequence<I...>) noexcept
    -> typename std::add_pointer<decltype(T{ ubiq_rref_constructor{I}... })>::type;

template <class T, std::size_t N, class /*Enable*/ = decltype( enable_if_constructible_helper<T>(std::make_index_sequence<N>()) ) >
using enable_if_constructible_helper_t=std::size_t;

///////////////////// Helpers for range size detection
template <std::size_t Begin, std::size_t Last>
using is_one_element_range = std::integral_constant<bool, Begin == Last>;

using multi_element_range = std::false_type;
using one_element_range = std::true_type;

///////////////////// Non greedy fields count search. Templates instantiation depth is log(sizeof(T)), templates instantiation count is log(sizeof(T)).
template <class T, std::size_t Begin, std::size_t Middle>
constexpr std::size_t detect_fields_count(detail::one_element_range, long) noexcept {
    static_assert(
        Begin == Middle,
        "====================> Boost.PFR: Internal logic error."
    );
    return Begin;
}

template <class T, std::size_t Begin, std::size_t Middle>
constexpr std::size_t detect_fields_count(detail::multi_element_range, int) noexcept;

template <class T, std::size_t Begin, std::size_t Middle>
constexpr auto detect_fields_count(detail::multi_element_range, long) noexcept
    -> detail::enable_if_constructible_helper_t<T, Middle>
{
    constexpr std::size_t next_v = Middle + (Middle - Begin + 1) / 2;
    return detail::detect_fields_count<T, Middle, next_v>(detail::is_one_element_range<Middle, next_v>{}, 1L);
}

template <class T, std::size_t Begin, std::size_t Middle>
constexpr std::size_t detect_fields_count(detail::multi_element_range, int) noexcept {
    constexpr std::size_t next_v = Begin + (Middle - Begin) / 2;
    return detail::detect_fields_count<T, Begin, next_v>(detail::is_one_element_range<Begin, next_v>{}, 1L);
}

///////////////////// Greedy search. Templates instantiation depth is log(sizeof(T)), templates instantiation count is log(sizeof(T))*T in worst case.
template <class T, std::size_t N>
constexpr auto detect_fields_count_greedy_remember(long) noexcept
    -> detail::enable_if_constructible_helper_t<T, N>
{
    return N;
}

template <class T, std::size_t N>
constexpr std::size_t detect_fields_count_greedy_remember(int) noexcept {
    return 0;
}

template <class T, std::size_t Begin, std::size_t Last>
constexpr std::size_t detect_fields_count_greedy(detail::one_element_range) noexcept {
    static_assert(
        Begin == Last,
        "====================> Boost.PFR: Internal logic error."
    );
    return detail::detect_fields_count_greedy_remember<T, Begin>(1L);
}

template <class T, std::size_t Begin, std::size_t Last>
constexpr std::size_t detect_fields_count_greedy(detail::multi_element_range) noexcept {
    constexpr std::size_t middle = Begin + (Last - Begin) / 2;
    constexpr std::size_t fields_count_big_range = detail::detect_fields_count_greedy<T, middle + 1, Last>(
        detail::is_one_element_range<middle + 1, Last>{}
    );

    constexpr std::size_t small_range_begin = (fields_count_big_range ? 0 : Begin);
    constexpr std::size_t small_range_last = (fields_count_big_range ? 0 : middle);
    constexpr std::size_t fields_count_small_range = detail::detect_fields_count_greedy<T, small_range_begin, small_range_last>(
        detail::is_one_element_range<small_range_begin, small_range_last>{}
    );
    return fields_count_big_range ? fields_count_big_range : fields_count_small_range;
}

///////////////////// Choosing between array size, greedy and non greedy search.
template <class T, std::size_t N>
constexpr auto detect_fields_count_dispatch(size_t_<N>, long, long) noexcept
    -> typename std::enable_if<std::is_array<T>::value, std::size_t>::type
{
    return sizeof(T) / sizeof(typename std::remove_all_extents<T>::type);
}

template <class T, std::size_t N>
constexpr auto detect_fields_count_dispatch(size_t_<N>, long, int) noexcept
    -> decltype(sizeof(T{}))
{
    constexpr std::size_t middle = N / 2 + 1;
    return detail::detect_fields_count<T, 0, middle>(detail::multi_element_range{}, 1L);
}

template <class T, std::size_t N>
constexpr std::size_t detect_fields_count_dispatch(size_t_<N>, int, int) noexcept {
    // T is not default aggregate initialzable. It means that at least one of the members is not default constructible,
    // so we have to check all the aggregate initializations for T up to N parameters and return the bigest succeeded
    // (we can not use binary search for detecting fields count).
    return detail::detect_fields_count_greedy<T, 0, N>(detail::multi_element_range{});
}

template <typename T,std::size_t... I, typename /*Enable*/ = enable_if_cons_helper_arr<T>(std::index_sequence<I...>) >
constexpr std::size_t getArrayElemsToRemove(std::index_sequence<I...>,long)
{
         //return (ArrayIndexStored<I>::value + ...);
         return (arrElemsToRemove[I] + ...);
}

template <typename T>
constexpr std::size_t getArrayElemsToRemove(std::index_sequence<>,long)
{
   return 0;
}
///////////////////// Returns non-flattened fields count
template <class T>
constexpr std::size_t fields_count() noexcept {
    using type = std::remove_cv_t<T>;

    static_assert(
        !std::is_reference<type>::value,
        "====================> Boost.PFR: Attempt to get fields count on a reference. This is not allowed because that could hide an issue and different library users expect different behavior in that case."
    );

    static_assert(
        std::is_copy_constructible<std::remove_all_extents_t<type>>::value || (
            std::is_move_constructible<std::remove_all_extents_t<type>>::value
            && std::is_move_assignable<std::remove_all_extents_t<type>>::value
        ),
        "====================> Boost.PFR: Type and each field in the type must be copy constructible (or move constructible and move assignable)."
    );

    static_assert(
        !std::is_polymorphic<type>::value,
        "====================> Boost.PFR: Type must have no virtual function, because otherwise it is not aggregate initializable."
    );

#ifdef __cpp_lib_is_aggregate
    static_assert(
        std::is_aggregate<type>::value             // Does not return `true` for build in types.
        || std::is_standard_layout<type>::value,   // Does not return `true` for structs that have non standard layout members.
        "====================> Boost.PFR: Type must be aggregate initializable."
    );
#endif

// Can't use the following. See the non_std_layout.cpp test.
//#if !BOOST_PFR_USE_CPP17
//    static_assert(
//        std::is_standard_layout<type>::value,   // Does not return `true` for structs that have non standard layout members.
//        "Type must be aggregate initializable."
//    );
//#endif

    constexpr std::size_t max_fields_count = (sizeof(type) * CHAR_BIT); // We multiply by CHAR_BIT because the type may have bitfields in T
    constexpr std::size_t interresult = detail::detect_fields_count_dispatch<type>(size_t_<max_fields_count>{}, 1L, 1L);
    constexpr std::size_t result=interresult - getArrayElemsToRemove<type>(
    std::make_index_sequence<interresult>(),1L);
    static_assert(
        is_aggregate_initializable_n<type, result>::value,
        "====================> Boost.PFR: Types with user specified constructors (non-aggregate initializable types) are not supported."
    );
    static_assert(
        result != 0 || std::is_empty<type>::value || std::is_fundamental<type>::value || std::is_reference<type>::value,
        "====================> Boost.PFR: If there's no other failed static asserts then something went wrong. Please report this issue to the github along with the structure you're reflecting."
    );

    return result;
}

}}} // namespace boost::pfr::detail

#ifdef __clang__
#   pragma clang diagnostic pop
#endif

#endif // BOOST_PFR_DETAIL_FIELDS_COUNT_HPP