#include <gtest/gtest.h>
#include "ArenaContainersTest.hpp"
#include <climits>

class ArenaFixture : public ::testing::Test 
{
   constexpr inline static std::size_t arenaSize= 1024; //2^10 should be a power of two
    
 public:
   struct big_data_struct
   {
      uint64_t  d[sizeof(uint64_t)*sizeof(uint64_t)*CHAR_BIT];
   };   

 protected:
  template <typename T>
  using ThisAlloc=aatest::CxxAllocatorAdaptorArena<T>;

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
  virtual void SetUp() { }

  // You can define per-test tear-down logic as usual.
  virtual void TearDown() { }

  //should be heap allocated
  static folly::SysArena arena;
  static aatest::Function<big_data_struct (big_data_struct)> testBigData; 
  static ThisAlloc<big_data_struct (big_data_struct)> allocator;
  typedef big_data_struct (*Functor)(big_data_struct);
};