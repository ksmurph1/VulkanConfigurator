#include "VulkanMap.hpp"
#include "XmlProcessor.hpp"
configuration::VulkanMap::VulkanMap(XmlProcessor& processor, const char * filename) noexcept
{
   processor.process(filename);
   errorState=processor.getError();
   toStructs=std::addressof(processor.getStructs());
}