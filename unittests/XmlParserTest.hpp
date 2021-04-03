#include <gtest/gtest.h>
#include "../src/XmlProcessor.hpp"

class XmlProcFixture : public ::testing::Test, public configuration::XmlProcessor
{
 protected:
   // if change in XmlProcessor must change here
   constexpr inline static uint8_t limit=std::numeric_limits<u_int8_t>::max();
   constexpr inline static float scaleFactor=1.0f/  // scale result down so fits in array
                                                // like container-make sure we have twice space
               (std::numeric_limits<stringhash_uint32::value_type>::max()/limit);
  constexpr inline static const char * filename1="vulkan_settings_testxml.xml";
  constexpr inline static const char * filename2="not_found.xml";
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
  bool VisitEnter (const tinyxml2::XMLElement & element,
                 const tinyxml2::XMLAttribute * attribute) override;

};