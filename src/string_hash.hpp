#ifndef string_hash_hpp
#define string_hash_hpp
#include "ArenaContainers.hpp"
#include "compile_time_string_hash.hpp"
namespace configuration
{
    template <typename T>
    struct hash;

	template<> struct hash<aa::string>
    {
        std::size_t operator()(aa::string const& s) const noexcept
        {
            return stringview_hash(stringhash_uint32(s.data()));
        }
    };
}
#endif