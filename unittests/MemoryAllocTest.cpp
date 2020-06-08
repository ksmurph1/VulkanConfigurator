#include "MemoryAllocTest.hpp"
#include <typeinfo>
#include <list>

folly::SysArena ArenaFixture::arena(ArenaFixture::arenaSize);
ArenaFixture::ThisAlloc<ArenaFixture::big_data_struct (ArenaFixture::big_data_struct)> 
ArenaFixture::allocator(ArenaFixture::arena);  

aatest::Function<std::allocator_traits<decltype(ArenaFixture::allocator)>::value_type>
ArenaFixture::testBigData([](big_data_struct bg)->big_data_struct { return big_data_struct();},
allocator);
TEST_F(ArenaFixture, ContainsArenaAllocTest)
{
    ASSERT_STREQ(typeid(std::allocator_traits<decltype(allocator)>::allocator_type).name(),
    typeid(ThisAlloc<big_data_struct (big_data_struct)>).name());
}

TEST_F(ArenaFixture, IsHeapAllocTest)
{
  ASSERT_EQ(allocator.inner_allocator().bytesUsed(),0); 
}

TEST_F(ArenaFixture, AllocationOccurred)
{
  ThisAlloc<Functor> a(arena);
  std::scoped_allocator_adaptor<ThisAlloc<Functor>> ar(a);
  std::list<std::vector<Functor,decltype(a)>,decltype(ar)> l(sizeof(big_data_struct::d), ar);
  ASSERT_GT(ar.outer_allocator().inner_allocator().bytesUsed(),0);
}