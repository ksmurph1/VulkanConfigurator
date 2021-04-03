#include "StructFieldPopulatedTest.hpp"

TEST_F(FieldPopulatedFixture, EnumFieldPopulated)
{
   const char * name="VkCommandPoolCreateInfo";
   bool result=process(filename);

   if (result)
   {
      configuration::Configurator::HashTypeMap typeMap; 
      auto map=getStructs();
      ASSERT_TRUE(map.find(name)!=map.end());

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
   bool result=process(filename);
   const char * structTypeName="VkSwapchainCreateInfoKHR";
   if (result)
   {
      HashTypeMap typeMap; 
      auto map=getStructs();
      ASSERT_TRUE(map.find(structTypeName)!=map.end());

      ASSERT_GT(typeMap.getType(structTypeName).index(),0);
      VkSwapchainCreateInfoKHR& strct=std::any_cast<VkSwapchainCreateInfoKHR&>(map[structTypeName]);
      
      ASSERT_EQ(strct.imageArrayLayers, (decltype(strct.imageArrayLayers))1);
   }
   else
      FAIL();
   SUCCEED();    
}

TEST_F(FieldPopulatedFixture, CharPtrFieldPopulated)
{
   bool result=process(filename);
   const char * structTypeName="VkApplicationInfo";
   if (result)
   {
      HashTypeMap typeMap; 
      auto map=getStructs();
      ASSERT_TRUE(map.find(structTypeName)!=map.end());

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
   bool result=process(filename);
   const char * structTypeName="VkSwapchainCreateInfoKHR";
   if (result)
   {
      HashTypeMap typeMap; 
      auto map=getStructs();
      ASSERT_TRUE(map.find(structTypeName)!=map.end());

      ASSERT_GT(typeMap.getType(structTypeName).index(),0);
      VkSwapchainCreateInfoKHR& strct=std::any_cast<VkSwapchainCreateInfoKHR&>(map[structTypeName]);
      
      ASSERT_TRUE(strct.pNext==nullptr);
   }
   else
      FAIL();
   SUCCEED();    
}
TEST_F(FieldPopulatedFixture, OrEnumFieldPopulated)
{
   const char * name="VkImageCreateInfo";
   bool result=process(filenameOr);
   ASSERT_TRUE(result);
      configuration::Configurator::HashTypeMap typeMap; 
      auto map=getStructs();
      ASSERT_TRUE(map.find(name)!=map.end());

      ASSERT_GT(typeMap.getType(name).index(),0);

      VkImageCreateInfo& strct=std::any_cast<VkImageCreateInfo&>(map[name]);
      ASSERT_EQ(strct.usage,VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
}

TEST_F(FieldPopulatedFixture, AndEnumFieldPopulated)
{
   const char * name="VkSubpassDependency";
   bool result=process(filenameAnd);
   ASSERT_TRUE(result);
      configuration::Configurator::HashTypeMap typeMap; 
      auto map=getStructs();
      ASSERT_TRUE(map.find(name)!=map.end());

      ASSERT_GT(typeMap.getType(name).index(),0);

      VkSubpassDependency& strct=std::any_cast<VkSubpassDependency&>(map[name]);
      ASSERT_EQ(strct.dstAccessMask,VK_ACCESS_COLOR_ATTACHMENT_READ_BIT & VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
}