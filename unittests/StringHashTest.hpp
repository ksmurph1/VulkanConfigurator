#include <gtest/gtest.h>
#include "../src/compile_time_string_hash.hpp"
#include "ArenaContainersTest.hpp"
#include <climits>
#include <random>
class StringHashFixture : public ::testing::Test
{
   std::random_device rd;  //Will be used to obtain a seed for the random number engine
   std::mt19937 gen;   //Standard mersenne_twister_engine seeded with rd()
   std::uniform_real_distribution<float> dis;
 protected:
   constexpr inline static uint8_t limit=std::numeric_limits<u_int8_t>::max();
   constexpr inline static float scaleFactor=1.0f/  // scale result down so fits in array
                                                // like container-make sure we have twice space
               (std::numeric_limits<stringhash_uint32::value_type>::max()/limit);
               
  static aatest::string largeString;
  static aatest::StringAlloc allocator;
  

  StringHashFixture() : gen(rd()),dis(0.0, 1.001) {}
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
  char getRandomChar() noexcept
  {
     return (char)(dis(gen)*CHAR_MAX);
  }

};
