#include "StructFieldPopulatedTest.hpp"

TEST_F(FieldPopulatedFixture, EnumFieldPopulated)
{
   const char * name="VkCommandPoolCreateInfo";
   configuration::XmlProcessor processor;
   bool result=processor.process(filename);

   if (result)
   {
      HashTypeMap typeMap; 
      auto map=processor.getStructsMap();
      EXPECT_TRUE(map.find(name)!=map.end());

      ASSERT_GT(typeMap.getType(name).index(),0);

      VkCommandPoolCreateInfo& strct=std::any_cast<VkCommandPoolCreateInfo&>(map[name]);
      ASSERT_EQ(strct.sType,VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO);
   }
   else
      FAIL();
   SUCCEED();    
}

TEST_F(FieldPopulatedFixture, UInt32AliasFieldPopulated)
{
   configuration::XmlProcessor processor;
   bool result=processor.process(filename);
   const char * structTypeName="VkSwapchainCreateInfoKHR";
   if (result)
   {
      HashTypeMap typeMap; 
      auto map=processor.getStructsMap();
      EXPECT_TRUE(map.find(structTypeName)!=map.end());

      ASSERT_GT(typeMap.getType(structTypeName).index(),0);
      VkSwapchainCreateInfoKHR& strct=std::any_cast<VkSwapchainCreateInfoKHR&>(map[structTypeName]);
      
      ASSERT_EQ(strct.pQueueFamilyIndices, (decltype(strct.pQueueFamilyIndices))1);
   }
   else
      FAIL();
   SUCCEED();    
}

TEST_F(FieldPopulatedFixture, CharPtrFieldPopulated)
{
   configuration::XmlProcessor processor;
   bool result=processor.process(filename);
   const char * structTypeName="VkApplicationInfo";
   if (result)
   {
      HashTypeMap typeMap; 
      auto map=processor.getStructsMap();
      EXPECT_TRUE(map.find(structTypeName)!=map.end());

      ASSERT_GT(typeMap.getType(structTypeName).index(),0);
      VkApplicationInfo& strct=std::any_cast<VkApplicationInfo&>(map[structTypeName]);
      
      ASSERT_STREQ(strct.pEngineName,"vulkanExample");
   }
   else
      FAIL();
   SUCCEED();    
}

TEST_F(FieldPopulatedFixture, NullptrFieldPopulated)
{
   configuration::XmlProcessor processor;
   bool result=processor.process(filename);
   const char * structTypeName="VkSwapchainCreateInfoKHR";
   if (result)
   {
      HashTypeMap typeMap; 
      auto map=processor.getStructsMap();
      EXPECT_TRUE(map.find(structTypeName)!=map.end());

      ASSERT_GT(typeMap.getType(structTypeName).index(),0);
      VkSwapchainCreateInfoKHR& strct=std::any_cast<VkSwapchainCreateInfoKHR&>(map[structTypeName]);
      
      ASSERT_TRUE(strct.pNext==nullptr);
   }
   else
      FAIL();
   SUCCEED();    
}