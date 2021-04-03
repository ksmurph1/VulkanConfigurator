//
// Created by kerry on 3/14/21.
//
#ifndef PARCHILDFIELDLINK_HPP
#define PARCHILDFIELDLINK_HPP
#include "ArenaContainers.hpp"
#include <tuple>
#include "char_array_limits.hpp"
#include "string_hash.hpp"
namespace configuration
{
    class FieldExtractor;
    class XmlProcessor;
    class ChildTypeParentFieldMatching;
    class ParentChildFieldLink
    {
        uint8_t childCount=0;
        aa::string parentIds[MAX_CHAR_SIZE]={""};
        // key=field type name as stored in typelist, val=pointer to parent type, field name, real field type
        aa::unordered_map_pool<aa::string, std::tuple<FieldExtractor*,aa::string,aa::string>,configuration::hash<aa::string>> childInfo;
        inline const static char IDREFSEP=',';

        void parseChildRef(const aa::string&, const char* ) noexcept;
        auto getParentMatch(const aa::string& structId) const noexcept
        {
            return std::find_if(std::cbegin(childInfo),std::cend(childInfo),[&structId](const auto& pair)
            {
                return structId.find(pair.first) != aa::string::npos;
            });
        }
        public:
        ParentChildFieldLink() noexcept : childInfo()
        {}

        void add(FieldExtractor*, const char*, const char*, const aa::string&) noexcept;

        std::size_t getChildFieldLinkSize() const noexcept
        {
            return childInfo.size();
        }
        void clearParTags() noexcept
        {
            childCount=0;
        }
        void erase(const aa::string& matchTag)
        {
            auto parentIter = getParentMatch(matchTag);
            if (parentIter != std::cend(childInfo))
                childInfo.erase(parentIter);
        }

        bool getTypeForField(XmlProcessor*, bool (XmlProcessor::*)(const aa::string&,const char*,const std::tuple<FieldExtractor*,aa::string,aa::string>&,
                                                                         ChildTypeParentFieldMatching&),const ChildTypeParentFieldMatching& childParFieldMatch);
        std::tuple<const aa::string*,uint8_t> getMatchIds() const noexcept
        {
            return std::make_tuple(parentIds, childCount);
        }

        ~ParentChildFieldLink()
        {
        }
    };
}
#endif //PARCHILDFIELDLINK_HPP
