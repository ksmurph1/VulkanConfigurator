//AUTO-GENERATED File, caution before modifying

#ifndef VULKANALIASLIST_HPP
 #define VULKANALIASLIST_HPP

#include "VulkanConfigurator.hpp"

namespace configuration
{

template <std::size_t ... I>

constexpr void Configurator::TypesFor<std::index_sequence<I...>>::setupAliases() noexcept
{

aliases.add("VkCommandPoolCreateFlags", strcpy("unsigned int",storage+storagePos,13));
storagePos+=13;
aliases.add("uint32_t", strcpy("unsigned int",storage+storagePos,13));
storagePos+=13;
aliases.add("const uint32_t *", strcpy("const unsigned int *",storage+storagePos,21));
storagePos+=21;
aliases.add("VkPipelineStageFlags", strcpy("unsigned int",storage+storagePos,13));
storagePos+=13;
aliases.add("VkAccessFlags", strcpy("unsigned int",storage+storagePos,13));
storagePos+=13;
aliases.add("VkDependencyFlags", strcpy("unsigned int",storage+storagePos,13));
storagePos+=13;
aliases.add("VkImageUsageFlags", strcpy("unsigned int",storage+storagePos,13));
storagePos+=13;
aliases.add("VkImageViewCreateFlags", strcpy("unsigned int",storage+storagePos,13));
storagePos+=13;
aliases.add("VkImageAspectFlags", strcpy("unsigned int",storage+storagePos,13));
storagePos+=13;
aliases.add("VkFenceCreateFlags", strcpy("unsigned int",storage+storagePos,13));
storagePos+=13;
aliases.add("VkPipelineRasterizationStateCreateFlags", strcpy("unsigned int",storage+storagePos,13));
storagePos+=13;
aliases.add("VkBool32", strcpy("unsigned int",storage+storagePos,13));
storagePos+=13;
aliases.add("VkPipelineViewportStateCreateFlags", strcpy("unsigned int",storage+storagePos,13));
storagePos+=13;
}
}
#endif
