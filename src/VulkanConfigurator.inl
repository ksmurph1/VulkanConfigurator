#ifndef VULKANCONFIG_INL
#define VULKANCONFIG_INL
#include "get_string_hash.hpp"
#include "VulkanAliasList.hpp"
      
template <std::size_t ... I>
 constexpr typename configuration::Configurator::WrapperVariant 
   configuration::Configurator::TypesFor<std::index_sequence<I...>>::getType(const char* typeName) const noexcept
   {
      const char* realTypeName=aliases.get(typeName).value_or(nullptr);
      if (!realTypeName)
         realTypeName=typeName;
           
      const WrapperVariant& wrapperVariant=typeForHash.get(realTypeName).value_or(WrapperVariant(
                                           boost::hana::basic_type<NullWrapper>{}));
      return wrapperVariant;
   }

template <std::size_t ... I>
   constexpr typename configuration::Configurator::WrapperVariant 
    configuration::Configurator::TypesFor<std::index_sequence<I...>>::getType(std::string_view typeName) const noexcept
   {
       const char* realTypeName=aliases.get(typeName.data()).value_or(nullptr);
      if (!realTypeName)
         realTypeName=typeName.data();

      const WrapperVariant& wrapperVariant=typeForHash.get(realTypeName).value_or(WrapperVariant(
                                           boost::hana::basic_type<NullWrapper>{}));
      return wrapperVariant;
   }
template <std::size_t ... I>
constexpr configuration::Configurator::TypesFor<std::index_sequence<I...>>::TypesFor() noexcept : aliases{} 
{
   ((typeNames[I]=strcpyEnum(boost::typeindex::ctti_type_index::type_id_with_cvr<
                    typename std::variant_alternative_t<I,WrapperVariant>::type>().pretty_name(),names[I],
                    MAX_CHAR_SIZE)),...);
   ((values[I]=std::variant_alternative_t<I,WrapperVariant>{}),...);
   typeForHash=decltype(typeForHash)::setup(typeNames,values);
   setupAliases();
}
#endif
