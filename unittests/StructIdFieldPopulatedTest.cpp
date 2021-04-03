#include "StructIdFieldPopulatedTest.hpp"

TEST_F(IdFieldPopulatedFixture, MatchIdToType)
{
   bool result=process(filename);
   if (result)
   {
      const char * structTypeName="VkRenderPassCreateInfo";
      configuration::Configurator::HashTypeMap typeMap; 
      auto map=getStructs();
      ASSERT_TRUE(map.find(structTypeName)!=map.end());

      ASSERT_GT(typeMap.getType(structTypeName).index(),0);
      VkRenderPassCreateInfo& strct=std::any_cast<VkRenderPassCreateInfo&>(map[structTypeName]);
      
      ASSERT_EQ((strct.pAttachments)->finalLayout,VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
   }
   else
      FAIL();
   SUCCEED();
}

TEST_F(IdFieldPopulatedFixture, Match2IdsToType)
{
   bool result=process(filename2);
   if (result)
   {
      const char * structTypeName="VkRenderPassCreateInfo";
      configuration::Configurator::HashTypeMap typeMap; 
      auto map=getStructs();
      ASSERT_TRUE(map.find(structTypeName)!=map.end());

      ASSERT_GT(typeMap.getType(structTypeName).index(),0);
      VkRenderPassCreateInfo& strct=std::any_cast<VkRenderPassCreateInfo&>(map[structTypeName]);
      
      ASSERT_EQ((strct.pAttachments[1]).finalLayout, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
      ASSERT_EQ((strct.pAttachments[0]).stencilLoadOp, VK_ATTACHMENT_LOAD_OP_DONT_CARE);
   }
   else
      FAIL();
   SUCCEED();
}