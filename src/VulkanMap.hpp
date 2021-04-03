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
    class XmlProcessor;
    struct VulkanMap
    {
        typedef aa::unordered_map_pool<aa::string, std::any, hash<aa::string>> ContainerType;
    private:
        ContainerType* toStructs;
        ErrorState::ProcessError errorState;
    public:
        VulkanMap(XmlProcessor&, const char*) noexcept;

        ErrorState::ProcessError getError() const noexcept
        {
            return errorState;
        }
        template<typename T,typename std::enable_if_t<boost::mpl::contains<Configurator::types,T>::value ,void*> =nullptr>
        T& get() noexcept;
    };
}
#endif //VULKANMAP_HPP
