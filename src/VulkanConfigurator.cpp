#include "XmlProcessor.hpp"
#include "VulkanConfigurator.hpp"


/*configuration::Configurator::WrapperVariant::TypeMap 
configuration::Configurator::WrapperVariant::value= configuration::Configurator::hanaTypeForStrHash=
HashTypeMap::getHanaTypeForHashMap();*/
configuration::Configurator::HashTypeMap configuration::Configurator::hanaTypeForStrHash;

#include "VulkanConfigurator.inl"                                                      
/*configuration::Configurator::MapBuilder::MapBuilder() :   allocator(
   MapType::allocator_type::inner_allocator_type(aa::ARENA))
  {
  }*/
  
 /* template <std::size_t ... I>
typename configuration::Configurator::TypesFor<std::index_sequence<I...>>::WrapperTypes
configuration::Configurator::TypesFor<std::index_sequence<I...>>::typeForHash[
   NumElements<std::make_index_sequence <typeMapBucketCount>>::value]=
   {boost::hana::basic_type<NullWrapper>{}};*/


