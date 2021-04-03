#include "StructDiffIdsPopulatedTest.hpp"
#include "ErrorState.hpp"
TEST_F(DiffIdsPopulatedFixture, 2TypeIds)
{
   bool result=process(filename);
   const char * structTypeName="VkRenderPassCreateInfo";
   ASSERT_TRUE(result);
   HashTypeMap typeMap; 
   auto map=getStructs();
   ASSERT_TRUE(map.find(structTypeName)!=map.end());

   ASSERT_GT(typeMap.getType(structTypeName).index(),0);
   VkRenderPassCreateInfo& strct=std::any_cast<VkRenderPassCreateInfo&>(map[structTypeName]);
      
   ASSERT_EQ((strct.pAttachments)->finalLayout, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
   ASSERT_EQ((strct.pDependencies)->dependencyFlags, VK_DEPENDENCY_BY_REGION_BIT);
}

TEST_F(DiffIdsPopulatedFixture, MatchIdValMissing)
{
   bool result=process(filenameMiss1);
   ASSERT_FALSE(result);
   ASSERT_TRUE(isError(
      configuration::ErrorState::ProcessError::FIELD_LEFT_UNASSIGNED));
   HashTypeMap typeMap; 
   auto map=getStructs();
   const char * const structName="VkRenderPassCreateInfo";
   ASSERT_TRUE(map.find(structName)!=map.end());

   ASSERT_GT(typeMap.getType(structName).index(),0);
   
   VkRenderPassCreateInfo& strct=std::any_cast<VkRenderPassCreateInfo&>(map[structName]);
      
   ASSERT_EQ((strct.pAttachments+1)->finalLayout, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL); 
}