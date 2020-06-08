#pragma once

#include "VulkanConfigurator.hpp"
#include "folly/src/Function.h"
#include "FieldExtractor.hpp"
#include "get_string_hash.hpp"
//#include "StrHashTable.hpp"
namespace configuration
{
class XmlProcessor : public tinyxml2::XMLVisitor
{
  public:
   enum ProcessError : uint8_t
   {
      SUCCESS=0,
      TYPE_MISSING_LIST,
      PROCESSING_OUT_OF_ORDER,
      FIELD_HASH_INVALID,
      ENUM_TYPE,
      STRUCT_META_MISSING,
      VALUE_MISMATCH_CONFIG,
      FILE_LOAD_ERR
   };
   
   private:
      aa::string curTypeName;
      const static uint8_t numAttribs=std::numeric_limits<uint8_t>::max()/2;
      constexpr inline static float scaleFactor=1.0f/  // scale result down so fits in array
                                                // like container-make sure we have twice space
               (std::numeric_limits<stringhash_uint32::value_type>::max()/numAttribs);
   ProcessError errorState=ProcessError::SUCCESS;
   tinyxml2::XMLDocument doc;
   Configurator::WrapperVariant parentType;
   aa::string valForAttrName[numAttribs]={""};

   FieldExtractor* extractor=nullptr;
   aa::Alloc<FieldExtractor> extractorAlloc;

   aa::StringAlloc stringAlloc;
   aa::Alloc<char> charAlloc;
   aa::unordered_map_pool<aa::string,std::any> toStructs;

   inline bool hasAttribute(const char * tag) const noexcept
   {
      return !valForAttrName[getStringHash(tag,scaleFactor)].empty();
   }
   inline const aa::string& 
   getAttribute(const char* tag) const noexcept
   {
      return valForAttrName[getStringHash(tag,scaleFactor)];
   }
   inline void setError (ProcessError flag)
   {
      errorState=flag;
      doc.Clear(); // reset doc so processing stops
   }
  
  
   void clearAttrMap() noexcept;
  protected:
  void populateAttrMap(const tinyxml2::XMLAttribute *) noexcept;
  
   std::tuple<const std::remove_extent_t<decltype(valForAttrName)> * const,std::size_t>  
   getAttribMap() const noexcept
   {
      return std::make_tuple(valForAttrName,sizeof(valForAttrName)/sizeof(valForAttrName[0]));
   }
   template <typename ... Args>
   constexpr bool isError(const std::variant<Args...>&, ProcessError) const;
   template <typename ... Args>
   constexpr bool isError(const std::variant<boost::hana::basic_type<Args>...>&,ProcessError) const;
   template <typename T>
   constexpr bool isError(const boost::hana::basic_type<T>&, ProcessError) noexcept;
    
   bool parseConfig(const char *const);
    public:
       XmlProcessor() : doc(),stringAlloc(),parentType(),extractorAlloc(),charAlloc(),
       toStructs(std::variant_size_v<Configurator::WrapperVariant>),curTypeName()
       {
          Configurator::hanaTypeForStrHash.setAlias("int32_t","int").setAlias(
             "const uint32_t *","const unsigned int *");
       }

       bool VisitEnter (const tinyxml2::XMLElement &, const tinyxml2::XMLAttribute *) override;
       bool VisitExit  (const tinyxml2::XMLElement &) override;
       inline tinyxml2::XMLDocument& getXmlDoc()
       {
          return doc;
       }
       bool process(const char* const filename)
       {
          return parseConfig(filename);        
       }
       inline ProcessError getErrorState() const noexcept
       {
          return errorState;
       }
  
       const decltype(toStructs) getStructsMap() const noexcept
       {
          return toStructs;
       }
   ~XmlProcessor()
   {
      if (extractor)
      {
         extractor->~FieldExtractor();
         extractorAlloc.deallocate(extractor,1);
      }
   }
  
};
}
