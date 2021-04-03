#ifndef VULKANCONFIGURATOR_HPP
#define VULKANCONFIGURATOR_HPP
#include <boost/hana/type.hpp>
#include "ctti_type_index.hpp"
#include "vulkan/vulkan.h"
#include "get_string_hash.hpp"
#include <xcb/xcb.h>
#include "vulkan/vulkan_xcb.h"
#include <boost/mpl/size.hpp>
#include <boost/mpl/begin.hpp>
#include <boost/mpl/advance.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/remove_if.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/set/set0.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/back_inserter.hpp>
#include "VulkanTypeList.hpp"
#include <tinyxml2.h>
#include "compile_time_string_hash.hpp"
#include <limits>
#include <array>
#include <variant>
#include <any>
#include <type_traits>
#include "ArenaContainers.hpp"
//#include "string_hash.hpp"
#include "StrHashTable.hpp"
#include "folly/src/portability/Constexpr.h"
#include "constexpr_strcpy.hpp"
//auto typeFor=boost::hana::make_map();
namespace configuration
{
   
class Configurator
{
constexpr inline static uint8_t typeMapBucketCount=std::numeric_limits<uint8_t>::max(); 
constexpr inline static uint8_t MAX_CHAR_SIZE=std::numeric_limits<uint8_t>::max();
//insert additional types
//typedef boost::mpl::copy<TYPELIST,boost::mpl::back_inserter<additionalTypes>>::type typeBag;

// remove duplicates
typedef typename boost::mpl::fold<TYPELIST
, boost::mpl::pair< boost::mpl::clear<TYPELIST>::type, boost::mpl::set0<> >
        , boost::mpl::if_<
              boost::mpl::contains< boost::mpl::second< boost::mpl::_1>,  boost::mpl::_2 >
            , boost::mpl::_1
            , boost::mpl::pair<
                   boost::mpl::push_back<  boost::mpl::first< boost::mpl::_1>,  boost::mpl::_2>
                ,  boost::mpl::insert<  boost::mpl::second< boost::mpl::_1>,  boost::mpl::_2 >
>
>
>::type::first uniqueTypes;
/*typedef boost::mpl::fold<
      typeBag
    , boost::mpl::set0<>
    , boost::mpl::insert<boost::mpl::_1,boost::mpl::_2>
    >::type uniqueTypes;*/

//remove all enum types
typedef boost::mpl::remove_if<uniqueTypes, std::is_enum<boost::mpl::_1>>::type types;
    /*using map_t=decltype(boost::hana::make_map(boost::hana::make_pair(boost::hana::size_c<stringview_hash(stringhash_uint32(
       boost::typeindex::ctti_type_index::type_id_with_cvr<void *>().pretty_name()))>,boost::hana::type_c<void *>)));*/
public:
   constexpr inline static float scaleFactor=(float)typeMapBucketCount/
                                 std::numeric_limits<stringhash_uint32::value_type>::max();




struct NullWrapper
   {
      typedef std::nullptr_t type;
      char dummy; // for visit_struct so no error is generated, must have at least one field
   };
private:
static inline StrHashTable<MAX_CHAR_SIZE,uint64_t> enumMap;
struct EnumMapInitializer
{
   constexpr EnumMapInitializer() noexcept;
   private:
      char key[1][MAX_CHAR_SIZE]={'\0'};
};
static EnumMapInitializer enumMapInit;

template <typename Iter, typename Seq>
struct MakeFromTypesAtIndices;

template <typename Iter,std::size_t ... I>
struct MakeFromTypesAtIndices<Iter, std::index_sequence<I...>>
{
   // insert null ptr at beginning so know if default constructed
   typedef std::variant<boost::hana::basic_type<NullWrapper>,boost::hana::basic_type<typename boost::mpl::deref<
   typename boost::mpl::advance<Iter, boost::mpl::integral_c<std::size_t,I>>::type>::type>...> variant;
                        
};
template <typename Types>
struct UnderlyingTypesVariant;

template <typename ... Args>
struct UnderlyingTypesVariant<std::variant<boost::hana::basic_type<Args> ...>>
{
    typedef std::variant<Args...> underlyingTypes;
};

template<typename Seq>
struct NumElements;
template <std::size_t ... I>
struct NumElements<std::index_sequence<I...>>
{
   constexpr static std::size_t value=
   std::numeric_limits<stringhash_uint32::value_type>::max()*scaleFactor;
};
 public:
   typedef MakeFromTypesAtIndices <boost::mpl::begin<types>::type,
           std::make_index_sequence <boost::mpl::size<types>::value>>::variant WrapperVariant;
   typedef UnderlyingTypesVariant <WrapperVariant>::underlyingTypes UnderlyingTypes;

private:
template <typename Seq>
struct TypesFor;
template <std::size_t ... I>
struct TypesFor<std::index_sequence<I...>>
{
   private:
      char storage[MAX_CHAR_SIZE];
      std::remove_const_t<decltype(MAX_CHAR_SIZE)> storagePos=0;
      char names[sizeof...(I)][MAX_CHAR_SIZE];
      WrapperVariant values[sizeof...(I)];
      const char*typeNames[sizeof...(I)];
      
      StrHashTable<MAX_CHAR_SIZE,WrapperVariant> typeForHash;
      StrHashTable<MAX_CHAR_SIZE,const char*> aliases;
      constexpr void setupAliases() noexcept;
      
public:
   constexpr TypesFor<std::index_sequence<I...>>() noexcept; 
   
   constexpr WrapperVariant getType(const char* typeName) const noexcept;
   
   constexpr WrapperVariant getType(std::string_view typeName) const noexcept;
   

   constexpr std::size_t size() const noexcept
   {
      return typeForHash.getSize();
   }
   
   
};
protected:
   using HashTypeMap=TypesFor<std::make_index_sequence <std::variant_size_v<WrapperVariant>>>;
private:
   static HashTypeMap typeMap;
   
   friend class XmlProcessor;
   friend class FieldExtractor;
   friend class ChildTypeParentFieldMatching;
   friend class ParentChildFieldLink;
   friend class VulkanMap;

public:

   Configurator() {}
  
};
}
#endif
