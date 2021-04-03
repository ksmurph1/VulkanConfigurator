#include "ApplicationInterfaceTest.hpp"
#include "XmlProcessor.hpp"
#include "VulkanMap.inl"
TEST_F(AppInterfaceFixture, MapAccess)
{
    configuration::XmlProcessor processor;
    bool result=processor.process(filename);
    ASSERT_TRUE(result);
    VkRenderPassCreateInfo strct=configuration::map.get<VkRenderPassCreateInfo>();

    ASSERT_EQ((strct.pAttachments)->finalLayout, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    ASSERT_EQ((strct.pDependencies)->dependencyFlags, VK_DEPENDENCY_BY_REGION_BIT);
}

