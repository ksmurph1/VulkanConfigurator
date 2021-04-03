//
// Created by kerry on 3/14/21.
//
#ifndef CHILDTYPEPARENTFIELDMATCHING_HPP
#define CHILDTYPEPARENTFIELDMATCHING_HPP
#include <type_traits>
#include "TypeAllocators.hpp"
#include <optional>
#include "enumeration_wrapper.hpp"
#include "UtilityTypes.hpp"
#include "string_hash.hpp"
namespace configuration
{
    class FieldExtractor;
    class ParentChildFieldLink;
    class ChildTypeParentFieldMatching
    {
        friend class ParentChildFieldLink;
        UtilityTypes::KeyValType *children = nullptr;
        uint8_t count = 0;

    public:
        ChildTypeParentFieldMatching(const aa::unordered_map_pool<UtilityTypes::KeyValType::first_type, UtilityTypes::KeyValType::second_type, configuration::hash<aa::string>> &
                                     ,const ParentChildFieldLink&);

        const UtilityTypes::KeyValType *findMatchChildLocForField(const char *) const noexcept;

        template <typename FieldType,std::enable_if_t<std::is_pointer_v<FieldType>,int> = 0>
        FieldType convertChildStructToVal(FieldExtractor* const, const char*) noexcept;
        template <typename FieldType,std::enable_if_t<!std::is_pointer_v<FieldType> && configuration::is_enumeration<FieldType>::value,int> = 0>
        std::optional<typename FieldType::type> convertChildStructToVal(FieldExtractor* const, const char *) noexcept;
        template <typename FieldType,std::enable_if_t<!std::is_pointer_v<FieldType> && !configuration::is_enumeration<FieldType>::value,int> = 0>
        std::optional<FieldType> convertChildStructToVal(FieldExtractor* const, const char*) noexcept;


        uint8_t getMatchCount() const noexcept;


        ~ChildTypeParentFieldMatching();

    };
}
#endif //CHILDTYPEPARENTFIELDMATCHING_HPP
