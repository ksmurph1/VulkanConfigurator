//
// Created by kerry on 3/28/21.
//
#include <boost/mpl/contains.hpp>
#include "ArenaContainers.hpp"
#include "string_hash.hpp"
#include "get_string_hash.hpp"
#include "TypeAllocators.hpp"

#ifndef VULKANMAP_HPP
#define VULKANMAP_HPP
namespace configuration
{
    class VulkanMap
    {
        friend class XmlProcessor;
    public:
        typedef aa::unordered_map_pool<aa::string, std::any, hash<aa::string>> ContainerType;
    private:
         ContainerType toStructs;
    protected:
        ContainerType& getStrucMap() noexcept
        {
            return toStructs;
        }
    public:
        VulkanMap() : toStructs()
        {}

        template<typename T,typename std::enable_if_t<boost::mpl::contains<Configurator::types,T>::value ,void*> =nullptr>
        T& get() noexcept;
    };
}
template<typename T,typename std::enable_if_t<boost::mpl::contains<configuration::Configurator::types,T>::value,void*> =nullptr>
T& configuration::VulkanMap::get() noexcept
{
    auto iter=toStructs.find(boost::typeindex::ctti_type_index::type_id_with_cvr<std::decay_t<T>>().pretty_name());
    if (iter != toStructs.end())
       return *std::any_cast<T>(std::addressof(iter->second));
    else
    {
       T* temp=new (TypeAllocators::TypeAlloc<T>.allocate(1)) T{};
       return *temp;
    }
}
#endif //VULKANMAP_HPP
