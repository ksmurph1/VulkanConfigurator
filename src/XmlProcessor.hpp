#pragma once
#include "VulkanConfigurator.hpp"
#include "folly/src/Function.h"
#include "StructFieldExtractor.hpp"
#include "get_string_hash.hpp"
#include "pair_hash.hpp"
#include "pair_equalto.hpp"
#include "is_static_castable.hpp"
#include "ErrorState.hpp"
#include "char_array_limits.hpp"
#include "ParentChildFieldLink.hpp"
#include "UtilityTypes.hpp"
#include "VulkanMap.hpp"

//#include "StrHashTable.hpp"
namespace configuration
{
   class ChildTypeParentFieldMatching;
class XmlProcessor : public tinyxml2::XMLVisitor
{
   friend class FieldExtractor;
   friend class EnumBitOpSplitterBase;
   template <typename T, typename R>
   friend class EnumBitOpSplitter;
   
      constexpr inline static float scaleFactor=1.0f/  // scale result down so fits in array
                                                // like container-make sure we have twice space
               (std::numeric_limits<stringhash_uint32::value_type>::max()/MAX_CHAR_SIZE);

   tinyxml2::XMLDocument doc;
   
   aa::string valForAttrName[MAX_CHAR_SIZE]={""};

   // key->id number:type name as in typelist,val->pointer to child struct of type
   aa::unordered_map_pool<aa::string,FieldExtractor*,configuration::hash<aa::string>> childTypes;

   FieldExtractor* extractorList[MAX_CHAR_SIZE]={nullptr};
   uint8_t extCount=0;

   bool idTagFound=false;

   FieldExtractor* extractor;
   
   bool inField=false;
   bool inStruct=false;
   ErrorState error;
   public:

      bool processParentChildFieldInteraction(const aa::string&,const char*,const std::tuple<FieldExtractor*,aa::string,aa::string>&, ChildTypeParentFieldMatching&);
   private:
   void addChildStructDeleteIndicator(const UtilityTypes::KeyValType::first_type&) noexcept;

   void permStoreNewContainer(const FieldExtractor*) noexcept;
   void permReplaceContainer(const FieldExtractor*) noexcept;

   bool saveStructAsFieldValue(const std::tuple<FieldExtractor*,aa::string,aa::string>&, const char*, ChildTypeParentFieldMatching&);


   //void cleanUpParentMatchIter(const KeyValType*);

   void cleanup(FieldExtractor*);

   template <typename FieldType>
   bool processFieldInStruct(const char*, const std::any&) noexcept;
   
   template <typename FieldType,typename ArgType,
             typename std::enable_if_t<configuration::is_static_castable<ArgType,FieldType>::value,void*> = nullptr>
   bool processFieldInStruct(const char*, const ArgType&) noexcept;
   

   inline constexpr void pushExtList(FieldExtractor* extractor) noexcept
   {
      extractorList[extCount++]=extractor;
   }
   inline bool hasAttribute(const char * tag) const noexcept
   {
      return !valForAttrName[getStringHash(tag,scaleFactor)].empty();
   }
   inline const aa::string& 
   getAttribute(const char* tag) const noexcept
   {
      return valForAttrName[getStringHash(tag,scaleFactor)];
   }
   
  
   void deleteExtractorAlloc(FieldExtractor*) const ;
   void clearAttrMap() noexcept;
   ParentChildFieldLink parChildLink;
    VulkanMap::ContainerType& structs;

protected:

    void populateAttrMap(const tinyxml2::XMLAttribute *) noexcept;
  
   std::tuple<const std::remove_extent_t<decltype(valForAttrName)> * const,std::size_t>  
   getAttribMap() const noexcept
   {
      return std::make_tuple(valForAttrName,sizeof(valForAttrName)/sizeof(valForAttrName[0]));
   }

   constexpr bool checkValidType(const Configurator::WrapperVariant&) const noexcept;
   template <typename ... Args>
   constexpr bool isError(const std::variant<Args...>&, ErrorState::ProcessError) const;
   template <typename ... Args>
   constexpr bool isError(const std::variant<boost::hana::basic_type<Args>...>&,ErrorState::ProcessError) const;
   template <typename T>
   constexpr bool isError(const boost::hana::basic_type<T>&, ErrorState::ProcessError) noexcept;
    
   bool parseConfig(const char *const);
       constexpr void setError(const ErrorState::ProcessError) noexcept;
    VulkanMap::ContainerType& getStructs() const noexcept
       {
           return structs;
       }
    public:
       XmlProcessor();
       bool VisitEnter (const tinyxml2::XMLElement &, const tinyxml2::XMLAttribute *) override;
       bool VisitExit  (const tinyxml2::XMLElement &) override;
       inline tinyxml2::XMLDocument& getXmlDoc()
       {
          return doc;
       }
       bool process(const char* const);
     
       constexpr bool isError(const ErrorState::ProcessError error) const noexcept
       {
          return (this->error).isError(error);
       }


   ~XmlProcessor();
  
};
}
