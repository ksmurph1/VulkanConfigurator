#ifndef pair_hash_hpp
#define pair_hash_hpp
#include "ArenaContainers.hpp"
#include "compile_time_string_hash.hpp"
namespace std
{

	template<> struct hash<std::pair<unsigned char,aa::string>>
    {
        std::size_t operator()(std::pair<unsigned char,aa::string> const& p) const noexcept
        {
            return stringview_hash(stringhash_uint32(p.second.data()))+
            p.first;
        }
    };
    
}
#endif