#include "constexpr_strcpy.hpp"
#include "get_string_hash.hpp"

template <std::size_t ... I>
char configuration::Configurator::TypesFor<std::index_sequence<I...>>::names[sizeof...(I)][MAX_CHAR_SIZE]{
   {}};

template <std::size_t ... I>
const char* configuration::Configurator::TypesFor<std::index_sequence<I...>>::typeNames[sizeof...(I)]=
            {strcpyEnum(boost::typeindex::ctti_type_index::type_id_with_cvr<
                    typename std::variant_alternative_t<I,WrapperVariant>::type>().pretty_name(),names[I],
                    MAX_CHAR_SIZE)...};

template <std::size_t ... I>                                                                          
configuration::StrHashTable<configuration::Configurator::typeMapBucketCount,
            typename configuration::Configurator::WrapperVariant>
            configuration::Configurator::TypesFor<std::index_sequence<I...>>::typeForHash(
                              decltype(typeForHash)::setup(typeNames,values));

template <std::size_t ... I>
 constexpr typename configuration::Configurator::WrapperVariant 
   configuration::Configurator::TypesFor<std::index_sequence<I...>>::getType(const char* typeName) const noexcept
   {
      const char* realTypeName=aliases[getStringHash(typeName,scaleFactor)];
      if (*realTypeName == '\0')
         realTypeName=typeName;
           
      const WrapperVariant& wrapperVariant=typeForHash.get(realTypeName).value_or(WrapperVariant(boost::hana::basic_type<NullWrapper>{}));
      return std::visit(
            [=](auto& arg) -> WrapperVariant 
            {
               return WrapperVariant(std::in_place_type<typename std::decay_t<decltype(arg)>>); 
            },
            wrapperVariant);
   }

template <std::size_t ... I>
   constexpr typename configuration::Configurator::WrapperVariant 
    configuration::Configurator::TypesFor<std::index_sequence<I...>>::getType(std::string_view typeName) const noexcept
   {
       const char* realTypeName=aliases[getStringHash(typeName,scaleFactor)];
      if (*realTypeName == '\0')
         realTypeName=typeName.data();

      const WrapperVariant& wrapperVariant=typeForHash.get(realTypeName).value_or(WrapperVariant(boost::hana::basic_type<NullWrapper>{}));
      return std::visit(
            [=](auto& arg) -> WrapperVariant 
            {
               return WrapperVariant(std::in_place_type<typename std::decay_t<decltype(arg)>>); 
            },
            wrapperVariant);
   }