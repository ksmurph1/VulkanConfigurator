#pragma once
#include <cstddef>
#include "extract_enum_type.hpp"
namespace configuration
{

constexpr char * strcpy(const char* src, char* dst, std::size_t N)
{
   std::size_t i=0;
   while(src[i] != '\0' && i < N)
   {
       dst[i]=src[i];
       i++;
   }
   // get rid of right angle bracket
   if (dst[i-1] == '>')
      dst[i-1]='\0';
   dst[i]='\0';
   return dst;
}

constexpr char * strcpy(const char* src, char** dst, std::size_t N)
{
   std::size_t i=0;
   while(src[i] != '\0' && i < N)
   {
       (*dst)[i]=src[i];
       i++;
   }
   (*dst)[i]='\0';
   return *dst;
}

constexpr char * strcpyEnum(const char* src, char* dst, std::size_t N)
{
   if (std::string_view(src).find_first_of("enumeration<") != std::string_view::npos)
   {
      return strcpy(extractEnum(src).data(),dst,N);
   }
   else
   {
      return strcpy(src,dst,N);
   }
     
}
}