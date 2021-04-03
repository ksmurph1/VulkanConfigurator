//
// Created by kerry on 3/14/21.
//
#include "ParentChildFieldLink.hpp"
#include "VulkanConfigurator.hpp"
#include "UtilityTypes.hpp"
#include "ChildTypeParentFieldMatching.hpp"
#include <variant>
void configuration::ParentChildFieldLink::parseChildRef(const aa::string& val,const char* parentTag) noexcept
{
    std::for_each(std::begin(val),std::end(val),[this, parentTag](const char& c)
    {
        if (std::isdigit(c))
            parentIds[childCount]+=c;
        else if (c == IDREFSEP)
        {
            parentIds[childCount]+=parentTag;
            childCount++;
        }
    });
    parentIds[childCount]+=parentTag;
    if (!parentIds[childCount].empty())
        childCount++; // childCount is num elements in parentIds
}
void configuration::ParentChildFieldLink::add(FieldExtractor* extractor, const char* fieldName, const char* typeStr, const aa::string& valStr) noexcept
{
   // get string version of underlying type with pointer,reference, qualifiers removed
   aa::string typeName=std::visit([](const auto& arg)
   {
      return UtilityTypes::getTypeName<std::decay_t<std::remove_pointer_t<UtilityTypes::UnderlyingType<decltype(arg)>>>>();
   }, Configurator::typeMap.getType(typeStr));

  // set parent tag
  aa::string parTag=":" + typeName;
  // save info for later lookup
  parseChildRef(valStr,parTag.data());
  childInfo.try_emplace(parTag,std::make_tuple(extractor,fieldName,typeStr));//:common field type->parent extractor,field name,real field type
}


bool configuration::ParentChildFieldLink::getTypeForField(XmlProcessor* processor, bool (XmlProcessor::*callback)(const aa::string&,const char*,
                                                          const std::tuple<FieldExtractor*, aa::string,aa::string>&, ChildTypeParentFieldMatching&), const ChildTypeParentFieldMatching& childParFieldMatch)
{
    bool result=false;
    const UtilityTypes::KeyValType *childLoc = nullptr;
    for (const auto&[tag, value]: childInfo)
    {
        childLoc=childParFieldMatch.findMatchChildLocForField(tag.data());
        if (childLoc - childParFieldMatch.children < childParFieldMatch.count)
            result=(processor->*callback)(childLoc->first,tag.data(),value,const_cast<ChildTypeParentFieldMatching&>(childParFieldMatch));
    }
    if (childLoc - childParFieldMatch.children < childParFieldMatch.count)
        erase(childLoc->first);
    return result;
}