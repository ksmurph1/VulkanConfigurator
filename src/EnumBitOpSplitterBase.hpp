#ifndef ENUMBITOPSPLITBASE_HPP
#define ENUMBITOPSPLITBASE_HPP
#include <cstddef>
#include <cstdint>
#include <limits>
namespace configuration
{
    class EnumBitOpSplitterBase
    {
       protected:
       constexpr inline static const char BITWISEOR='|';
       constexpr inline static const char BITWISEAND='&';
       constexpr inline static const char SPACECHAR=' ';
       constexpr inline static const char BITWISEOPS[]={BITWISEOR,BITWISEAND,'\0'};
       constexpr inline static uint8_t MAXFIELDS=std::numeric_limits<uint8_t>::max();
       enum BITWISE_OP : int8_t {BITWISE_OR=-3,BITWISE_AND=-5};
       char* coreVal=nullptr;
       const uint8_t delimCount;
       const std::size_t strLen=0;

       void setDelimEnums(char** const) const;
       explicit EnumBitOpSplitterBase(const char*);
       ~EnumBitOpSplitterBase();
       public:
          static std::size_t findDelimCountEnums(const char*);
    };
}
#endif
