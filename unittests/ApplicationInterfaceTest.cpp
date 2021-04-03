#include "ApplicationInterfaceTest.hpp"
#include "VulkanMap.inl"
#include "XmlProcessor.hpp"
TEST_F(AppInterfaceFixture, MapAccess)
{
    configuration::XmlProcessor processor;
    configuration::VulkanMap map(processor,filename);
    ASSERT_FALSE(map.getError());
    VkRenderPassCreateInfo strct=map.get<VkRenderPassCreateInfo>();

    ASSERT_EQ((strct.pAttachments)->finalLayout, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    ASSERT_EQ((strct.pDependencies)->dependencyFlags, VK_DEPENDENCY_BY_REGION_BIT);
}

