#ifndef APPLICATIONINTERFACEFIXTURE_HPP
#define APPLICATIONINTERFACEFIXTURE_HPP
#include <gtest/gtest.h>

class AppInterfaceFixture : public ::testing::Test
{
protected:

    constexpr inline static const char * filename="vulkan_settings_field2ids.xml";

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
#endif //APPLICATIONINTERFACEFIXTURE_HPP
