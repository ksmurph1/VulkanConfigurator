#include "StructMapPopulatedTest.hpp"

TEST_F(MapPopulatedFixture, TypeMapPopulated)
{
   configuration::XmlProcessor processor;
   bool result=processor.process(filename1);

   if (result)
   {
      auto map=processor.getStructsMap();
      ASSERT_TRUE(map.find("VkFramebufferCreateInfo")!=map.end());
      ASSERT_TRUE(map.find("VkBufferCreateInfo")!=map.end());
      ASSERT_TRUE(map.find("VkFenceCreateInfo")!=map.end());
   }
   else
      FAIL();
   SUCCEED();    
}