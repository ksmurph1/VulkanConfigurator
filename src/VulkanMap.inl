#ifndef VULKANMAP_INL
#define VULKANMAP_INL
#include "VulkanMap.hpp"
    template<typename T,typename std::enable_if_t<boost::mpl::contains<configuration::Configurator::types,T>::value,void*> =nullptr>
    T& configuration::VulkanMap::get() noexcept
    {
        auto iter=toStructs->find(boost::typeindex::ctti_type_index::type_id_with_cvr<std::decay_t<T>>().pretty_name());
        if (iter != toStructs->end())
            return *std::any_cast<T>(std::addressof(iter->second));
        else
        {
            T* temp=new (TypeAllocators::TypeAlloc<T>.allocate(1)) T{};
            return *temp;
        }
    }

#endif