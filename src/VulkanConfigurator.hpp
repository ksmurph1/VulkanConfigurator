#pragma once
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
#include "vulkan_type_list.hpp"
#include <tinyxml2.h>
#include "compile_time_string_hash.hpp"
#include <limits>
#include <array>
#include <variant>
#include <any>
#include <type_traits>
#include "ArenaContainers.hpp"
#include "string_hash.hpp"
#include "StrHashTable.hpp"
#include "char_array_limits.hpp"
#include "folly/src/portability/Constexpr.h"
#include "constexpr_strcpy.hpp"
//auto typeFor=boost::hana::make_map();
namespace configuration
{
   
class Configurator
{
     

       aa::Alloc<char> stringAlloc;
constexpr inline static uint16_t typeMapBucketCount=std::numeric_limits<uint8_t>::max();
constexpr inline static float scaleFactor=(float)typeMapBucketCount/
                                 std::numeric_limits<stringhash_uint32::value_type>::max();
      


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
   template <typename T>
   using  UnderlyingType=typename std::decay_t<T>::type;

   template <typename T>
   using WrapperType=boost::hana::basic_type<T>;

/*struct MapBuilder
{
   typedef aa::unordered_map<aa::string,std::any> MapType;
   private:
   typename MapType::allocator_type allocator;
   public:
      MapBuilder();// : allocator(MapType::allocator_type::inner_allocator_type(aa::ARENA)) {}
      MapBuilder(const MapBuilder&)=delete;
      MapType getExtractor() const
      {
                return MapType(boost::mpl::size<types>::value,allocator); 
      }
};*/

struct NullWrapper
   {
      typedef std::nullptr_t type;
      char dummy; // for visit_struct so no error is generated, must have at least one field
   };
private:
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

   //typedef WrapperTypes (&TypeMap) [NumElements<std::make_index_sequence <typeMapBucketCount>>::value];
   private:
      
      static char names[sizeof...(I)][MAX_CHAR_SIZE];
      constexpr static WrapperVariant values[sizeof...(I)]={std::variant_alternative_t<I,WrapperVariant>{}...};
      static const char*typeNames[sizeof...(I)];
      
      static StrHashTable<typeMapBucketCount,WrapperVariant> typeForHash;
      char aliases[typeMapBucketCount][MAX_CHAR_SIZE]={};
      
public:
      constexpr TypesFor<std::index_sequence<I...>>() noexcept 
      {}
   /*constexpr TypeMap getHanaTypeForHashMap()
   {
       return typeForHash;
   }*/
   /*template <std::size_t N>
   constexpr auto getType(const char (&typeName)[N]) const noexcept
   {
      return typename std::variant_alternative_t<typeForHash[getStringHash(typeName,scaleFactor)
      ].index(),WrapperVariant>::type{};
   }*/
   constexpr WrapperVariant getType(const char* typeName) const noexcept;
   
   constexpr WrapperVariant getType(std::string_view typeName) const noexcept;
   

   constexpr static std::size_t size() noexcept
   {
      return decltype(typeForHash)::getSize();
   }
   /*template <typename ...Types, std::size_t ... Sizes>
   constexpr void setAliases(const std::pair<const char (&) [Sizes],
      boost::hana::basic_type<Types>> ... pairs)
   {
        // do group assignment to array hopefully at compile-time for strings to type map
         ((typeForHash[getStringHash(pairs.first,scaleFactor)]=pairs.second),...);    
   }*/
   constexpr TypesFor<std::index_sequence<I...>> setAlias(const char *first, const char * second) noexcept
   {
      strcpy(second, aliases[getStringHash(first,scaleFactor)],folly::constexpr_strlen(second));
      return *this;    
   }
   
   /*template <std::size_t HASH>
   using  BaseType=typename std::variant_alternative_t<typeForHash[HASH].index(),WrapperTypes>::type;*/
   /*constexpr static TypeMap value=[] ()
   {
      variant thisMap[NumElements<std::make_index_sequence <typeMapBucketCount>>::value]{};
    ((thisMap[getStringHash(
       boost::typeindex::ctti_type_index::type_id_with_cvr<typename std::variant_alternative_t<
       I,variant>::type>().pretty_name())]=std::move(variant(std::in_place_index<I>))),...);
       //variant(std::in_place_index<I>)),...);
    return thisMap;
   }();*/
//   {variant(std::in_place_index_t<I>())...};
/*
   public:
      constexpr static TypeMap getHanaTypeForHashMap()
      {
         TypeMap typeForHash={};
         // do group assignment to array hopefully at compile-time for strings to type map
         ((typeForHash[stringview_hash(stringhash_uint32(
       boost::typeindex::ctti_type_index::type_id_with_cvr<typename std::variant_alternative<I,TypeMap::value_type>::type::type
       >().pretty_name()))]=type_selector[I]),...);

       return typeForHash;
      }*/
};
protected:
   using HashTypeMap=TypesFor<std::make_index_sequence <std::variant_size_v<WrapperVariant>>>;

private:
   static HashTypeMap hanaTypeForStrHash;
   // convert into struct type
  // static HashTypeMap::TypeMap hanaTypeForStrHash;
   
   friend class XmlProcessor;
   friend class StructureTypes;
   friend class FieldExtractor;

public:

   explicit Configurator() : stringAlloc()
   {}
   //void parseConfig(const char *const);
  
};
}

//std::puts(boost::typeindex::ctti_type_index::type_id<boost::hana::type<int>>().pretty_name().data());
