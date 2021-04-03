#ifndef ENUMBITOPSPLITTER_HPP
#define ENUMBITOPSPLITTER_HPP
#include "enumeration_wrapper.hpp"
#include "EnumBitOpSplitterBase.hpp"
#include "ErrorState.hpp"
namespace configuration
{
   template <typename R, typename Enable=void>
   class EnumBitOpSplitter;

class FieldExtractor;
template <typename R>    
class EnumBitOpSplitter<R,typename std::enable_if_t<!configuration::is_enumeration<R>::value && 
                                                  (std::is_arithmetic_v<R> || std::is_enum_v<R>)>> : public EnumBitOpSplitterBase
{
    constexpr R getApplyEnum(const int64_t *, const uint8_t) const noexcept;
 
    R calcResultEnum(char** const, const FieldExtractor*,R (FieldExtractor::*)(const char*) const noexcept) const;
    ErrorState* errState=nullptr;
    void (* const errHandler)(ErrorState*,const ErrorState::ProcessError) noexcept=nullptr;
    public:
    explicit EnumBitOpSplitter(const char* value,ErrorState* errStat,void (* const errHandler)
    (ErrorState*,const ErrorState::ProcessError) noexcept) : 
                                    EnumBitOpSplitterBase(value),errState(errStat),errHandler(errHandler)
    {
    }
    R calculate(const FieldExtractor*, R (FieldExtractor::*)(const char*) const noexcept) const;
};
}
#endif
