#pragma once
#include <gtest/gtest.h>
#include "../src/VulkanConfigurator.hpp"
#include "ArenaContainersTest.hpp"

class TypeHashFixture : public ::testing::Test, public configuration::Configurator
{

  template<std::size_t...Is>
  constexpr std::tuple< std::integral_constant<std::size_t,Is>... > make_indexes(std::index_sequence<Is...>)
  {
    return std::make_tuple(std::integral_constant<std::size_t,Is>{}...);
  }
  protected:
  using hashType= decltype(configuration::getStringHash(0,0.0));
  
  constexpr inline static float scaleFactor=((float)HashTypeMap::size())/ 
  // scale result down so fits in array like container
               std::numeric_limits<stringhash_uint32::value_type>::max();
  
  HashTypeMap typeMap;

  template<std::size_t ...Is>
  constexpr  std::tuple<std::integral_constant<std::size_t,Is>... >  indexing_tuple(std::index_sequence<Is...>)
  { 
           return make_indexes(std::make_index_sequence<sizeof...(Is)>{});
  }
  template<std::size_t...Is, class F, class T>
  constexpr auto tuple_foreach( std::index_sequence<Is...>, T&& tup, F&& f )
  {
  ( f( std::get<Is>( std::forward<T>(tup) ) ), ... );
  }

  template<class F, class T>
  constexpr auto tuple_foreach( T&& tup,F&& f )
  {
    auto indexes = std::make_index_sequence< std::tuple_size_v< std::decay_t<T> > >{};
    return tuple_foreach( indexes, std::forward<T>(tup), std::forward<F>(f));
  }
  TypeHashFixture() : typeMap() 
  {

  }
  // Per-test-suite set-up.
  // Called before the first test in this test suite.
  // Can be omitted if not needed.
  static void SetUpTestSuite() 
  {
  }

  // Per-test-suite tear-down.
  // Called after the last test in this test suite.
  // Can be omitted if not needed.
  static void TearDownTestSuite()
  {
  }

  // You can define per-test set-up logic as usual.
  virtual void SetUp() 
  {

  }

  // You can define per-test tear-down logic as usual.
  virtual void TearDown() { }

};