//
// Created by kerry on 3/14/21.
//
#include "ChildTypeParentFieldMatching.hpp"
#include "ParentChildFieldLink.hpp"
#include <variant>
configuration::ChildTypeParentFieldMatching::ChildTypeParentFieldMatching(const aa::unordered_map_pool<UtilityTypes::KeyValType::first_type,
                                                                          UtilityTypes::KeyValType::second_type,configuration::hash<aa::string>>& childTypes,
                                                                          const ParentChildFieldLink& parChildLink)
{
    children = TypeAllocators::TypeAlloc<UtilityTypes::KeyValType>.allocate(childTypes.size());

    const aa::string* matchIds=nullptr;
    uint8_t matchLen;
    std::tie(matchIds, matchLen)=parChildLink.getMatchIds();

// check if all children are accounted for
    std::copy_if(std::cbegin(childTypes), std::cend(childTypes), children, [this, matchLen, matchIds](const UtilityTypes::KeyValType &keyVal)
    {
       bool result = false;
// loop for each child id
       std::for_each_n(matchIds, matchLen,[this, &result, &keyVal](const aa::string &tag)
       {
          if (keyVal.first.compare(tag) == 0)
          {
             this->count++;
             result = true;
          }
        });
        return result;
    });
}

const configuration::UtilityTypes::KeyValType *configuration::ChildTypeParentFieldMatching::findMatchChildLocForField(const char *parFieldTag) const noexcept
{
    // find matching child for parent id
    return std::find_if(children, children + count, [parFieldTag](const UtilityTypes::KeyValType &keyVal)
    {
        return keyVal.first.find(parFieldTag) != aa::string::npos;
    });
}


configuration::ChildTypeParentFieldMatching::~ChildTypeParentFieldMatching()
{
    if (children)
    {
        TypeAllocators::TypeAlloc<UtilityTypes::KeyValType>.deallocate(children, count);
        children=nullptr; count=0;
    }
}