#pragma once
#include <cstddef>
#include "compile_time_string_hash.hpp"

namespace configuration
{
   template <std::size_t N>
   constexpr static std::size_t getStringHash(const char * value, float scaleFactor)
   {
      return stringview_hash(stringhash_uint32(value))*scaleFactor;
   }
   template <std::size_t N>
   constexpr static std::size_t getStringHash(char (&value) [N], float scaleFactor)
   {
      return stringview_hash(stringhash_uint32(value))*scaleFactor;
   }
   
   constexpr static std::size_t getStringHash(const char * value,std::size_t len, float scaleFactor)
   {
      return stringview_hash(stringhash_uint32(value,len))*scaleFactor;
   }

   constexpr static std::size_t getStringHash(const std::string_view& value, float scaleFactor)
   {
      return stringview_hash(stringhash_uint32(value))*scaleFactor;
   }

}