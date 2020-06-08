
/* Edit this file to add or remove types not in configuration file
Number of template parameters per template declaration is limitied to 256*/

#ifndef vulkan_type_list_hpp
#define vulkan_type_list_hpp

#include "boost_mpl_vector_range_ext/vector90.hpp"
#include "enumeration_wrapper.hpp"
namespace configuration
{

typedef boost::mpl::vector82<VkMemoryAllocateInfo,VkMappedMemoryRange,VkCommandBufferAllocateInfo,
VkCommandBufferBeginInfo, VkCommandBufferInheritanceInfo, VkCommandPoolCreateInfo, VkRenderPassBeginInfo,
VkRenderPassCreateInfo,VkImageMemoryBarrier,VkBufferMemoryBarrier, VkMemoryBarrier, VkImageCreateInfo,
VkSamplerCreateInfo, VkImageViewCreateInfo, VkViewport, VkRect2D, VkDescriptorImageInfo, VkSubmitInfo,
VkFramebufferCreateInfo,VkBufferCreateInfo,VkEventCreateInfo,VkDescriptorPoolCreateInfo,VkDescriptorPoolSize,
VkDescriptorSetLayoutCreateInfo,VkDescriptorSetAllocateInfo,VkWriteDescriptorSet,VkPipelineLayoutCreateInfo,
VkPipelineVertexInputStateCreateInfo,VkPipelineInputAssemblyStateCreateInfo,VkPipelineRasterizationStateCreateInfo,
VkPipelineColorBlendAttachmentState,VkPipelineDepthStencilStateCreateInfo,VkPipelineViewportStateCreateInfo,
VkVertexInputBindingDescription,VkVertexInputAttributeDescription,VkApplicationInfo,VkFenceCreateInfo,VkBool32,
VkInstanceCreateInfo, VkPipelineViewportStateCreateFlags,VkPipelineRasterizationStateCreateFlags,
VkXcbSurfaceCreateInfoKHR,VkSwapchainCreateInfoKHR,VkAttachmentDescription,VkSubpassDescription,VkAttachmentReference,
VkComponentMapping,VkImageSubresourceRange,VkSubpassDependency,VkDescriptorSetLayoutBinding,VkPipelineColorBlendStateCreateInfo,
VkPipelineCacheCreateInfo,VkCommandBufferLevel,VkCommandPoolCreateFlags,VkPipelineBindPoint,VkPipelineStageFlags,
VkAccessFlags,VkDependencyFlags,VkImageViewType,VkSampleCountFlagBits,VkImageTiling,VkImageUsageFlags,VkImageAspectFlags,
VkFenceCreateFlags,VkCompareOp,VkImageUsageFlagBits,VkSharingMode,VkComponentSwizzle,VkAttachmentLoadOp,VkAttachmentStoreOp,
VkImageLayout, enumeration<VkStructureType>, const VkSubpassDescription *, const VkSubpassDependency *, const VkAttachmentDescription *,
float, uint32_t, int32_t,const char *,const void *, const uint32_t *,const VkAttachmentReference *> TYPELIST;
}
#endif
