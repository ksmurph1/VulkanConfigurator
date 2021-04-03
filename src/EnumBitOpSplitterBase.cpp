#include "EnumBitOpSplitterBase.hpp"
#include "constexpr_strcpy.hpp"
#include "folly/src/portability/Constexpr.h"
#include "XmlProcessor.hpp"
#include "constexpr_trimspaces.hpp"
#include <algorithm>
#include "TypeAllocators.hpp"
configuration::EnumBitOpSplitterBase::EnumBitOpSplitterBase(
   const char* value): delimCount(findDelimCountEnums(value)+1),
   strLen(folly::constexpr_strlen(value)) 
       {
          coreVal=TypeAllocators::CharAlloc.allocate(strLen);
          strcpy(value,coreVal,strLen);
          coreVal=removeWhiteSpaces(coreVal);
       }

configuration::EnumBitOpSplitterBase::~EnumBitOpSplitterBase()
{
   TypeAllocators::CharAlloc.deallocate(coreVal,strLen);
   coreVal=nullptr;
}
std::size_t configuration::EnumBitOpSplitterBase::findDelimCountEnums(
    const char* enumStr)
{
   return std::count_if(BITWISEOPS,BITWISEOPS+sizeof(BITWISEOPS)-1,[enumStr](
      const char& character)
   {
      return std::strchr(enumStr,character) !=nullptr;
   });
}

void configuration::EnumBitOpSplitterBase::setDelimEnums(char** const delimEnums) const
{
   decltype(coreVal) copy=coreVal;
      std::generate_n(delimEnums,delimCount,[&copy]() -> char *
      {
         std::size_t invalCharPos=std::strcspn(copy,BITWISEOPS);
         
         const std::size_t strLen=folly::constexpr_strlen(copy);
         char* temp=nullptr;
         if (invalCharPos != strLen)
         {
            temp=TypeAllocators::CharAlloc.allocate(invalCharPos+1);
            strcpy(copy,temp,invalCharPos+1);
            copy=copy+invalCharPos+1;
         }
         else
         {
            temp=TypeAllocators::CharAlloc.allocate(strLen);
            strcpy(copy,temp,strLen);
            copy=copy+strLen;
         }
          
         return temp;
      });
}
