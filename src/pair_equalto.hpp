#ifndef pair_equalto_hpp
#define pair_equalto_hpp
#include "ArenaContainers.hpp"
namespace std
{

	template<> struct equal_to<std::pair<unsigned char,aa::string>>
    {
       constexpr bool operator()(const std::pair<unsigned char,aa::string> &lhs, 
       const std::pair<unsigned char,aa::string> &rhs) const 
       {
           return lhs == rhs;
       }
    };
    
}
#endif