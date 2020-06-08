/*#include <boost/hana/type.hpp>
#include <boost/hana/map.hpp>
#include <boost/hana/pair.hpp>
#include "ctti_type_index.hpp"
#include <xcb/xcb.h>*/
#include "XmlProcessor.hpp"
#include <cstdio>
#include <new>
#include "vulkan/vulkan.h"
#include "boost/mpl/vector.hpp"
#include "ctti_type_index.hpp"

template <typename T>
    struct enumerator 
    {
       typedef T type;
    };

typedef boost::mpl::vector<enumerator<VkFenceCreateFlags>,VkCompareOp,VkImageUsageFlagBits,VkSharingMode,VkComponentSwizzle,
VkAttachmentLoadOp,VkAttachmentStoreOp,VkImageLayout,const VkSubpassDescription *, const VkSubpassDependency *, 
const VkAttachmentDescription *,float, uint32_t, int32_t,const char *,const void *, const uint32_t *,const VkAttachmentReference *> TYPELIST;
 
int main()
{    
    std::printf("%s\n", boost::typeindex::ctti_type_index::type_id_with_cvr<VkImageUsageFlagBits>().pretty_name());
   // extractor.parseConfig("vulkan_settings.xml");
}
