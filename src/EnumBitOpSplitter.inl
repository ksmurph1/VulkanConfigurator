#ifndef ENUMBITOPSPLITTER_INL
#define ENUMBITOPSPLITTER_INL

template <typename R>
R configuration::EnumBitOpSplitter<R,typename std::enable_if_t<
!configuration::is_enumeration<R>::value && (std::is_arithmetic_v<R> || std::is_enum_v<R>)>>::calculate(const FieldExtractor* extractor,
R (FieldExtractor::*op)(const char*) const noexcept) const
{
   char** const delimEnums=TypeAllocators::TypeAlloc<char*>.allocate(delimCount);
   setDelimEnums(delimEnums);
  
   R result=calcResultEnum(delimEnums,extractor,op);
   
    std::for_each_n(delimEnums, delimCount,[](char * delimEnum)
    {
       TypeAllocators::CharAlloc.deallocate(delimEnum, folly::constexpr_strlen(delimEnum));
    });
     TypeAllocators::TypeAlloc<char*>.deallocate(delimEnums, delimCount);
    return result;
}




template <typename R>
constexpr R configuration::EnumBitOpSplitter<R,typename std::enable_if_t<
!configuration::is_enumeration<R>::value && (std::is_arithmetic_v<R> || std::is_enum_v<R>)>>::getApplyEnum(const int64_t *stack, const uint8_t stackLen) const noexcept
{
    const uint8_t limit=2;
     std::decay_t<decltype(*stack)> accum;
     if (stackLen < limit-1)
        // error if nothing left in stack
        errHandler(errState,ErrorState::ProcessError::ENUM_OPER_INVAL);
     else
     {
        accum=*stack;
        if (stackLen >= limit)
        {        
           BITWISE_OP oper=static_cast<BITWISE_OP>(*(stack+limit-1));
           decltype(accum) result=static_cast<decltype(accum)>(
                                     getApplyEnum(stack+limit,stackLen-limit)); 
           switch (oper)
           {
              case BITWISE_OP::BITWISE_OR: accum|=result;
              break;
              case BITWISE_OP::BITWISE_AND: accum&=result;
              break;
           }
        }
     }
     return static_cast<R>(accum);
}
template <typename R>
R configuration::EnumBitOpSplitter<R,typename std::enable_if_t<
!configuration::is_enumeration<R>::value && (std::is_arithmetic_v<R> || std::is_enum_v<R>)>>::calcResultEnum(char** const delimEnums, 
const FieldExtractor* extractor,R (FieldExtractor::*operation)(const char*) const noexcept) const
{
      uint8_t index=0;
      int64_t stack[MAXFIELDS];
      std::for_each_n(delimEnums, delimCount,[extractor,operation,&stack,&index](char* delimEnum)
      {
         const std::size_t strLen=folly::constexpr_strlen(delimEnum);
         if (delimEnum[strLen-1] == BITWISEOR)
         {
            stack[index++]=static_cast<std::decay_t<decltype(*stack)>>((extractor->*operation)(std::strtok(delimEnum,BITWISEOPS)));
            stack[index++]=static_cast<std::decay_t<decltype(*stack)>>(BITWISE_OP::BITWISE_OR);
         }
         else if (delimEnum[strLen-1] == BITWISEAND)
         {
            stack[index++]=static_cast<std::decay_t<decltype(*stack)>>((extractor->*operation)(std::strtok(delimEnum,BITWISEOPS)));
            stack[index++]=static_cast<std::decay_t<decltype(*stack)>>(BITWISE_OP::BITWISE_AND);
         }
         else
            stack[index++]=static_cast<std::decay_t<decltype(*stack)>>((extractor->*operation)(delimEnum));
       });
       return getApplyEnum(stack, index);
}
#endif
