#pragma once
#include <gtest/gtest.h>
#include "../src/XmlProcessor.hpp"

class FieldPopulatedFixture : public ::testing::Test, public configuration::Configurator
{
 protected:
   
  constexpr inline static const char * filename="vulkan_settings_fieldtest.xml";

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

};