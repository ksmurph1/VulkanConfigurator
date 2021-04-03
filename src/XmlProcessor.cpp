#include "XmlProcessor.hpp"
#include "vulkan_structs_meta.hpp"
#include "magic_enum.hpp"
#include "constexpr_stoi.hpp"
#include "StructFieldExtractor.hpp"
#include "FieldExtractor.inl"
#include "StructFieldExtractor.inl"
#include "VulkanConfigurator.inl"
#include "ChildTypeParentFieldMatching.hpp"
#include "ChildTypeParentFieldMatching.inl"
#include "ParentChildFieldLink.hpp"

configuration::XmlProcessor::XmlProcessor() : doc(),error(),
       childTypes(),extractor(nullptr),parChildLink(),structs()
       {}
constexpr void configuration::XmlProcessor::setError(const ErrorState::ProcessError error) noexcept
{
   (this->error).setError(error);
}
bool configuration::XmlProcessor::parseConfig(const char *const filename)
{
   bool result=true;
   tinyxml2::XMLError error=doc.LoadFile(filename);
   if (error)
   {
      setError(ErrorState::ProcessError::FILE_LOAD_ERR);
      result=false;
   }
   else
      doc.Accept(static_cast<tinyxml2::XMLVisitor*>(this));
   return result;
}
constexpr bool configuration::XmlProcessor::checkValidType(const Configurator::WrapperVariant& typeList) const noexcept
{
    return std::visit([this](const auto &arg) -> bool
    {
        using baseType = UtilityTypes::UnderlyingType<decltype(arg)>;
        return !std::is_same_v<Configurator::NullWrapper, baseType>;
    }, typeList);
}

template <typename ... Args>
constexpr bool configuration::XmlProcessor::isError(const 
                       std::variant<boost::hana::basic_type<Args>...>& variant, ErrorState::ProcessError checkError) const
{
   switch (checkError)
   {
      case ErrorState::ProcessError::TYPE_MISSING_LIST: return std::holds_alternative<boost::hana::basic_type<Configurator::NullWrapper>> (variant);
      case ErrorState::ProcessError::STRUCT_META_MISSING: return !extractor || !extractor->isVisitable(); 
      case ErrorState::ProcessError::ENUM_TYPE: return std::visit([](const auto& arg)-> bool{ return std::is_enum_v<UtilityTypes::UnderlyingType<
      decltype(arg)>>;}, variant);
      default: return false;
   }
}
template <typename ... Args>
constexpr bool configuration::XmlProcessor::isError(const std::variant<Args...>& variant, ErrorState::ProcessError checkError) const
{
   switch (checkError)
   {
      case ErrorState::ProcessError::TYPE_MISSING_LIST: return std::holds_alternative<Configurator::NullWrapper>(variant);
      case ErrorState::ProcessError::STRUCT_META_MISSING: return !extractor || !extractor->isVisitable();
      case ErrorState::ProcessError::ENUM_TYPE: return std::visit([](const auto& arg){ return std::is_enum_v<std::decay_t<decltype(arg)>>;}, variant);
      default: return false;
   }
}
template <typename T>
constexpr bool configuration::XmlProcessor::isError(const boost::hana::basic_type<T>& type, ErrorState::ProcessError checkError) noexcept
{
   switch (checkError)
   {
      case ErrorState::ProcessError::TYPE_MISSING_LIST: return std::is_same_v<T,Configurator::NullWrapper>;
      case ErrorState::ProcessError::STRUCT_META_MISSING: return !extractor || !extractor->isVisitable();
      case ErrorState::ProcessError::ENUM_TYPE: return std::is_enum_v<T>;
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

bool configuration::XmlProcessor::process(const char* const filename)
{
   bool result=parseConfig(filename);
   if (parChildLink.getChildFieldLinkSize())
   {
      setError(ErrorState::ProcessError::FIELD_LEFT_UNASSIGNED);
      result=false;
   }             
   return result;        
}

template <typename FieldType,typename ArgType,typename std::enable_if_t<configuration::is_static_castable<ArgType,FieldType>::value,void*> = nullptr>
bool configuration::XmlProcessor::processFieldInStruct(const char* fieldName, const ArgType& fieldVal) noexcept
{
         bool result=true;
         if (!extractor->isVisitable())
         {
            setError(ErrorState::ProcessError::STRUCT_META_MISSING);
            result=false;
         }
         else if (!extractor->isFieldInStruct(fieldName))
         {
            setError(ErrorState::ProcessError::FIELD_HASH_INVALID);
            result=false;
         }
         else
             extractor->setFieldForStruct(fieldName,static_cast<FieldType>(fieldVal));
         return result;
}

template <typename FieldType>
bool configuration::XmlProcessor::processFieldInStruct(const char* fieldName, const std::any& fieldVal) noexcept
{
         bool result=true;
               
         if (!extractor->isVisitable())
         {
            setError(ErrorState::ProcessError::STRUCT_META_MISSING);
            result=false;
         }
         else if (!extractor->isFieldInStruct(fieldName))
         {
            setError(ErrorState::ProcessError::FIELD_HASH_INVALID);
            result=false;
         }
         else
            try
            {
               if constexpr (is_enumeration<FieldType>::value)
                  extractor->setFieldForStruct(fieldName,std::any_cast<typename FieldType::type>(fieldVal));
               else
                  extractor->setFieldForStruct(fieldName,std::any_cast<FieldType>(fieldVal));
            }
            catch (std::bad_any_cast)
            {
               setError(ErrorState::ProcessError::FIELD_TYPE_MISMATCH_CONFIG);
               result=false;
             }
         return result;
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
         Configurator::WrapperVariant parentType=Configurator::typeMap.getType(getAttribute("type").data());
         std::visit([this](const auto& arg)
         {
            using baseType=UtilityTypes::UnderlyingType<decltype(arg)>;
            extractor=new (TypeAllocators::ExtractorAlloc<baseType>.allocate(1))
                      StructFieldExtractor<baseType>(&error,setErrorHelper);
         }, parentType);
         pushExtList(extractor); 
         if (isError(parentType,ErrorState::ProcessError::TYPE_MISSING_LIST))
         {
            setError(ErrorState::ProcessError::TYPE_MISSING_LIST);
            result=false;
         }
         else if (isError(parentType,ErrorState::ProcessError::ENUM_TYPE))
         {
            setError(ErrorState::ProcessError::ENUM_TYPE);
            result=false;
         }
         else if (isError(parentType,ErrorState::ProcessError::STRUCT_META_MISSING))
         {
            setError(ErrorState::ProcessError::STRUCT_META_MISSING);
            result=false;
         }
         else
         {
            inStruct=true;
            extractor->iterateFieldPositions();
            if (hasAttribute("id"))
               std::visit([this](const auto& arg)
               {
                  childTypes.emplace(getAttribute("id")+ ':'+ 
                  UtilityTypes::getTypeName<UtilityTypes::UnderlyingType<decltype(arg)>>(), extractor);
               },extractor->getType());
         }     
      }
      else if (hasAttribute("type") && !strcmp(element.Name(),"field"))
      {
         if (!extractor)
         {
            setError(ErrorState::ProcessError::PROCESSING_OUT_OF_ORDER);
            result=false;
         }
         else
         {
            inField=true;
            aa::string valStr=getAttribute("value");
            const char* fieldName=getAttribute("name").data();
            const char* typeStr=getAttribute("type").data();
            aa::string::size_type pos=valStr.find("&id:");
            if ( pos != aa::string::npos) // id reference-save for later
            {
                idTagFound = true;
                parChildLink.add(extractor, fieldName, typeStr, valStr.replace(pos,folly::constexpr_strlen("&id:"),""));
            }
            else
            {
               
               result=std::visit([this,fieldName,&valStr](const auto& arg)
               {
                  bool result=true;
                  // enumerations must be wrapped with enumeration struct
                  auto fieldVal=extractor->convertToFieldValue<UtilityTypes::UnderlyingType<
                                                          decltype(arg)>>(valStr.data());
                  if (fieldVal.has_value())
                     result=processFieldInStruct<UtilityTypes::UnderlyingType<decltype(arg)
                                                  >>(fieldName,fieldVal.value());
                  else
                  {
                     setError(ErrorState::ProcessError::TYPE_MISSING_LIST);
                     result=false;
                  }
                  return result;                                        
               }, Configurator::typeMap.getType(typeStr));
              
            }
         }
      }
          clearAttrMap();
   }
          return result;
}

bool configuration::XmlProcessor::saveStructAsFieldValue(const std::tuple<FieldExtractor*,aa::string,aa::string>& fieldInfo, const char* tagType, ChildTypeParentFieldMatching& childParFieldMatch)
{
    aa::string fieldName,fieldType;
    FieldExtractor* extractor;
    std::tie(extractor,fieldName,fieldType)=fieldInfo;
   return std::visit([this, extractor,tagType,&childParFieldMatch,&fieldName](const auto &arg)
   {
      bool result=false;
      using baseType=UtilityTypes::UnderlyingType < decltype(arg)>;

      auto value=childParFieldMatch.convertChildStructToVal<baseType>(extractor, tagType+1);
      if constexpr (std::is_pointer_v<decltype(value)>)
         result = processFieldInStruct<baseType>(fieldName.data(), value);
      else if (value.has_value())
         result = processFieldInStruct<baseType>(fieldName.data(), value.value());
      else
         setError(ErrorState::ProcessError::TYPE_MISSING_LIST);
      return result;
   }, Configurator::typeMap.getType(fieldType.data()));
}
bool configuration::XmlProcessor::processParentChildFieldInteraction(const aa::string& childTag,const char* parTagType, const std::tuple<FieldExtractor*,aa::string,aa::string>& fieldInfo,
                                                                      ChildTypeParentFieldMatching& childParFieldMatch)
{

//re-assign parent
    // if extractor is child backup and get parent
    FieldExtractor* const extractorBak=extractor;
    FieldExtractor* const parExtractor=std::get<0>(fieldInfo);
    if (parExtractor != extractorBak) // extractorBak is child, store
    {
        extractor=parExtractor;
        permReplaceContainer(extractorBak);
    }

    bool result=saveStructAsFieldValue(fieldInfo,parTagType,childParFieldMatch);

    if (result)
    {
// everything is okay-continue
        permReplaceContainer(parExtractor);
        addChildStructDeleteIndicator(childTag);
    }
    extractor=extractorBak;
    return result;
}
void configuration::XmlProcessor::deleteExtractorAlloc(FieldExtractor* extractor) const
{
    Configurator::WrapperVariant contType = extractor->getType();
    extractor->~FieldExtractor();
    std::visit([extractor](const auto& arg)
    {
            using baseType=UtilityTypes::UnderlyingType<decltype(arg)>;
            TypeAllocators::ExtractorAlloc<baseType>.deallocate(static_cast<
            StructFieldExtractor<baseType>*>(extractor),1);
    }, contType);
}

/*void configuration::XmlProcessor::cleanUpParentMatchIter(const KeyValType* childIter)
{
      auto parentIter=getParentMatch(childIter->first);
      if (parentIter != std::cend(childInfo))
      {
         using parTypeEntry=std::pair<decltype(childInfo)::key_type,decltype(childInfo)::mapped_type>;
         parTypeEntry* parEntries=TypeAllocators::TypeAlloc<parTypeEntry>.allocate(childInfo.size());
         //get entries key, value from map
         std::transform(std::cbegin(childInfo),std::cend(childInfo),parEntries,[](const auto& pair)
         {
            return pair;
         });
         // order parentType entries according to same as passed as child extractor
         const auto childExtDist=std::distance(parEntries,std::partition(parEntries, parEntries+childInfo.size(),[&parentIter](const parTypeEntry& pair)
         {
            return std::get<0>(pair.second) == std::get<0>(parentIter->second);
         }));
         TypeAllocators::TypeAlloc<parTypeEntry>.deallocate(parEntries,childInfo.size());
      
         if (childExtDist<=1)
            // delete parent if doesn't have any other refereces
            deleteExtractorAlloc(std::get<0>(parentIter->second));
      }
}*/
void configuration::XmlProcessor::cleanup(FieldExtractor* extractor)
{
    auto catchIter=std::find_if(std::cbegin(childTypes), std::cend(childTypes),[extractor](const auto& pair)
    {
       return pair.second == extractor;
    });
    if (catchIter == childTypes.cend())
       deleteExtractorAlloc(extractor);
}


inline void configuration::XmlProcessor::addChildStructDeleteIndicator(const UtilityTypes::KeyValType::first_type& tag) noexcept
{
    auto node = childTypes.extract(tag);
// add deletion indicator
    node.key() = "0:" + node.key();
    childTypes.insert(std::move(node));
}
inline void configuration::XmlProcessor::permStoreNewContainer(const FieldExtractor* extractor) noexcept
{
   std::visit([this,extractor](const auto& arg)
   {
      structs.emplace(UtilityTypes::getTypeName<UtilityTypes::UnderlyingType<decltype(arg)>>(),extractor->getContainer());
   },extractor->getType());
}
inline void configuration::XmlProcessor::permReplaceContainer(const FieldExtractor* extractor) noexcept
{
   std::visit([this, extractor](const auto &arg)
   {
      structs[UtilityTypes::getTypeName<UtilityTypes::UnderlyingType<decltype(arg)>>()] = extractor->getContainer();
   }, extractor->getType());
}

bool configuration::XmlProcessor::VisitExit (const tinyxml2::XMLElement & element)
{
   bool result=true;

   if (!inField && inStruct && extractor )
   {
      result=checkValidType(extractor->getType());
      if (!result)
         setError(ErrorState::ProcessError::PROCESSING_OUT_OF_ORDER);
      else if (idTagFound || (childTypes.size() && childTypes.size() <= structs.size()))
      {
          ChildTypeParentFieldMatching childParFieldMatch(childTypes,parChildLink);

            if (childParFieldMatch.getMatchCount())
            {
               idTagFound=false;
               //we have found children
               // get parent
               result=parChildLink.getTypeForField(this,&XmlProcessor::processParentChildFieldInteraction, childParFieldMatch);
           }
      }
      else
      {
         permStoreNewContainer(extractor);
         parChildLink.clearParTags();
         //cleanup(extractor);
      }
      /*if (extractor != extractorBak && extractorBak)
         cleanup(extractorBak);*/
         
         // not found          
      inStruct=false;
   }
   else if (inField) inField=false;
   
   return result;
}
configuration::XmlProcessor::~XmlProcessor()
{
    std::for_each_n(extractorList,extCount,[this](FieldExtractor *ext)
    {
        if (ext)
            deleteExtractorAlloc(ext);
    });
    extCount=0;
    extractor=nullptr;
}