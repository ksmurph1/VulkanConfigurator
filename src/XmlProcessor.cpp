#include "XmlProcessor.hpp"
#include "vulkan_structs_meta.hpp"
#include "magic_enum.hpp"

#include "FieldExtractor.inl"
#include "VulkanConfigurator.inl"
bool configuration::XmlProcessor::parseConfig(const char *const filename)
{
   bool result=true;
   tinyxml2::XMLError error=doc.LoadFile(filename);
   if (error)
   {
      setError(ProcessError::FILE_LOAD_ERR);
      result=false;
   }
   else
      doc.Accept(static_cast<tinyxml2::XMLVisitor*>(this));
   return result;
}

template <typename ... Args>
constexpr bool configuration::XmlProcessor::isError(const 
                       std::variant<boost::hana::basic_type<Args>...>& variant, ProcessError checkError) const
{
   switch (checkError)
   {
      case ProcessError::TYPE_MISSING_LIST: return std::holds_alternative<boost::hana::basic_type<Configurator::NullWrapper>> (variant);
      case ProcessError::STRUCT_META_MISSING: return extractor == nullptr || !std::visit([this](const auto& arg)-> bool {return 
      extractor->isVisitable<Configurator::UnderlyingType<decltype(arg)>>();}, variant);
      case ProcessError::ENUM_TYPE: return std::visit([](const auto& arg)-> bool{ return std::is_enum_v<Configurator::UnderlyingType<
      decltype(arg)>>;}, variant);
      default: return false;
   }
}
template <typename ... Args>
constexpr bool configuration::XmlProcessor::isError(const std::variant<Args...>& variant, ProcessError checkError) const
{
   switch (checkError)
   {
      case ProcessError::TYPE_MISSING_LIST: return std::holds_alternative<Configurator::NullWrapper>(variant);
      case ProcessError::STRUCT_META_MISSING: return extractor == nullptr || !std::visit([this](const auto& arg){return 
      extractor->isVisitable<std::decay_t<decltype(arg)>>();}, variant);
      case ProcessError::ENUM_TYPE: return std::visit([](const auto& arg){ return std::is_enum_v<std::decay_t<decltype(arg)>>;}, variant);
      default: return false;
   }
}
template <typename T>
constexpr bool configuration::XmlProcessor::isError(const boost::hana::basic_type<T>& type, ProcessError checkError) noexcept
{
   switch (checkError)
   {
      case ProcessError::TYPE_MISSING_LIST: return std::is_same_v<T,Configurator::NullWrapper>;
      case ProcessError::STRUCT_META_MISSING: return extractor == nullptr || !extractor->isVisitable<T>();
      case ProcessError::ENUM_TYPE: return std::is_enum_v<T>;
      case ProcessError::FIELD_HASH_INVALID: return extractor==nullptr || !extractor->isFieldInStruct(type);
      default: return false;
   }
}

void configuration::XmlProcessor::populateAttrMap(const tinyxml2::XMLAttribute * attribute) noexcept
{
   do
   {
      valForAttrName[configuration::getStringHash(attribute->Name(),scaleFactor)]=attribute->Value();
   }
   while (attribute=attribute->Next());   
}
void configuration::XmlProcessor::clearAttrMap() noexcept
{
     std::generate_n(valForAttrName,sizeof(valForAttrName)/sizeof(valForAttrName[0]),[]()->const char *{return "";});    
}

bool configuration::XmlProcessor::VisitEnter (const tinyxml2::XMLElement & element,
  const tinyxml2::XMLAttribute * attribute)
{
   bool result=true;
   if (attribute != nullptr)
   {
      populateAttrMap(attribute);
      if (hasAttribute("type") && !strcmp(element.Name(),"struct"))
      {
         parentType=
         Configurator::hanaTypeForStrHash.getType(getAttribute("type").data());
          std::visit([this](const auto& arg)
            {
               extractor=new (extractorAlloc.allocate(1)) FieldExtractor(decltype(arg){});
            },parentType);
         if (isError(parentType,ProcessError::TYPE_MISSING_LIST))
         {
            setError(ProcessError::TYPE_MISSING_LIST);
            result=false;
         }
         else if (isError(parentType,ProcessError::ENUM_TYPE))
         {
            setError(ProcessError::ENUM_TYPE);
            result=false;
         }
         else if (isError(parentType,ProcessError::STRUCT_META_MISSING))
         {
            setError(ProcessError::STRUCT_META_MISSING);
            result=false;
         }
         else
         {
            curTypeName=getAttribute("type");
            std::visit([this](const auto& arg)
            {
               extractor->iterateFieldPositions(decltype(arg){});
            },parentType);
         }     
      }
      else if (hasAttribute("type") && !strcmp(element.Name(),"field"))
      {
         if (extractor == nullptr || !extractor->getContainer().has_value())
         {
            setError(ProcessError::PROCESSING_OUT_OF_ORDER);
            result=false;
         }
         else
         {
            std::any fieldVal=extractor->convertToFieldValue(getAttribute("value").data(), getAttribute("type").data());
            if (fieldVal.has_value())
            {
            std::visit([this,&fieldVal,&result](const auto& arg)
            {
               using noQaulType=std::remove_const_t<std::remove_reference_t<decltype(arg)>>;
               
               if (!extractor->isVisitable<Configurator::UnderlyingType<noQaulType>>())
               {
                  setError(ProcessError::STRUCT_META_MISSING);
                  result=false;
               }
               else if (isError(decltype(arg){},ProcessError::FIELD_HASH_INVALID))
               {
                  setError(ProcessError::FIELD_HASH_INVALID);
                  result=false;
               }
               else   
                  extractor->setFieldForStruct<noQaulType>(getAttribute("name").data(),fieldVal);
              
            },parentType);
            }
            else
            {
               setError(ProcessError::VALUE_MISMATCH_CONFIG);
               result=false;
            }
         }
      }
          clearAttrMap();
   }
          return result;
}

bool configuration::XmlProcessor::VisitExit (const tinyxml2::XMLElement & element)
{
   bool result=true;
   if (!curTypeName.empty())
   {
      if (extractor == nullptr || !extractor->getContainer().has_value())
      {
         setError(ProcessError::PROCESSING_OUT_OF_ORDER);
         result=false;
      }
      else
      {
         toStructs.try_emplace(curTypeName,extractor->getContainer());
      }
   }
   curTypeName.clear();
   return result;
}
