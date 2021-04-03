//
// Created by kerry on 3/16/21.
//

#ifndef UTILITYTYPES_HPP
#define UTILITYTYPES_HPP
#include "ArenaContainers.hpp"
#include <boost/hana/type.hpp>
namespace configuration
{
    class FieldExtractor;
    struct UtilityTypes
    {
        using KeyValType = std::pair<aa::string, FieldExtractor *>;
        template <typename T>
        using  UnderlyingType=typename std::decay_t<T>::type;
        template <typename T>
        using WrapperType=boost::hana::basic_type<T>;

        template <typename T>
        static inline aa::string getTypeName()
        {
            return aa::string(boost::typeindex::ctti_type_index::type_id_with_cvr<T>().pretty_name());
        }
    };
}
#endif //UTILITYTYPES_HPP
