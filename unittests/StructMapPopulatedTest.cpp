#include "StructMapPopulatedTest.hpp"

TEST_F(MapPopulatedFixture, TypeMapPopulated)
{
   bool result=process(filename);

   if (result)
   {
      auto map=getStructs();
      ASSERT_TRUE(map.find("VkFramebufferCreateInfo")!=map.end());
      ASSERT_TRUE(map.find("VkBufferCreateInfo")!=map.end());
      ASSERT_TRUE(map.find("VkFenceCreateInfo")!=map.end());
   }
   else
      FAIL();
   SUCCEED();    
}