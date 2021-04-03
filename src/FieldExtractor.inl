#ifndef FieldExtractor_inl
#define FieldExtractor_inl
#include "EnumBitOpSplitter.hpp"
#include "TypeAllocators.hpp"
#include "EnumBitOpSplitter.inl"
template <typename TYPE, typename T, std::enable_if_t<std::is_pointer_v<T> && std::is_pointer_v<TYPE> &&
             !std::is_convertible_v<T,TYPE> || std::is_void_v<std::remove_pointer_t<TYPE>> || std::is_void_v<std::remove_pointer_t<T>>,int> = 0>
constexpr TYPE configuration::FieldExtractor::getItemArr(T&, const uint8_t)
{
   return nullptr;
}

template <typename TYPE, typename T, std::enable_if_t<std::is_pointer_v<T> && std::is_pointer_v<TYPE> &&
             std::is_convertible_v<T,TYPE> && !std::is_void_v<std::remove_pointer_t<TYPE>> && !std::is_void_v<std::remove_pointer_t<T>>,int> = 0>
TYPE configuration::FieldExtractor::getItemArr(T& fieldVal, const uint8_t N)
{
    using ptrLessType= std::remove_pointer_t<TYPE>;
    auto ptr=TypeAllocators::TypeAlloc<std::remove_const_t<ptrLessType>>.allocate(N);
    std::copy(fieldVal, fieldVal+N,ptr);
    return static_cast<TYPE>(ptr);
}

template <typename R, typename T, std::enable_if_t<std::is_pointer_v<R>,int> = 0>
R configuration::FieldExtractor::convertToFieldValue(T* fieldVals, const uint8_t N)
{
            return getItemArr<R>(fieldVals,N);
}

template <typename R,typename T, std::enable_if_t<std::is_pointer_v<R> && 
         std::is_pointer_v<std::remove_pointer_t<R>>,int> = 0>
R configuration::FieldExtractor::convertToFieldValue(T** fieldVals, const uint8_t N)
{
      return getItemArr<R>(fieldVals, N);
}
template <typename R, typename T, std::enable_if_t<configuration::is_enumeration<R>::value && !std::is_same_v<char *,T>,int> = 0>
constexpr std::optional<typename R::type> configuration::FieldExtractor::convertToFieldValue(const T& fieldVal)
{
      if constexpr (std::is_convertible_v<T, typename R::type>)
         return typename R::type(fieldVal);
      else
         return std::nullopt;
}
template <typename R, typename T, std::enable_if_t<!configuration::is_enumeration<R>::value && !std::is_same_v<char *,T>,int> = 0>
std::optional<R> configuration::FieldExtractor::convertToFieldValue(const T& fieldVal)
{
      std::optional<R> result;
      if constexpr (std::is_pointer_v<R> && std::is_convertible_v<T,std::remove_pointer_t<R>> && 
         std::is_convertible_v<T*,R>)
      {
            result=new (TypeAllocators::TypeAlloc<T>.allocate(1)) T(fieldVal);
      }
      else if constexpr (std::is_pointer_v<R> && std::is_pointer_v<std::remove_pointer_t<R>> && 
         std::is_convertible_v<T,std::remove_pointer_t<std::remove_pointer_t<R>>>)
      {
            result=new (TypeAllocators::TypeAlloc<T*>.allocate(1)) T*(fieldVal);
      }
      else if constexpr (std::is_convertible_v<T,R> )
      {
            result=fieldVal;
      }
      return result;
}
template <typename R,std::enable_if_t<configuration::is_enumeration<R>::value,int> = 0>
constexpr typename R::type configuration::FieldExtractor::getEnumFlag(const char* enumStr) const noexcept
{
     typename R::type result;
     auto enumResult=magic_enum::enum_cast<typename R::type>(std::string_view(enumStr));
     if (enumResult.has_value())
        result=typename R::type(enumResult.value());
     else
        // cases where magic enum doesn't work
        result=getFromMap<R>(enumStr);
    return result;
}


template <typename R,std::enable_if_t<configuration::is_enumeration<R>::value,int> = 0>
constexpr std::optional<typename R::type> configuration::FieldExtractor::convertToFieldValue(const char* value)
{
   std::optional<typename R::type> result;

   EnumBitOpSplitter<typename R::type> enumSplitter(value,errState,errHandler);
   return enumSplitter.calculate(this,&FieldExtractor::getEnumFlag<R>);
}
template <typename R,std::enable_if_t<configuration::is_enumeration<R>::value,int> = 0>
constexpr typename R::type configuration::FieldExtractor::getFromMap(const char* value) const noexcept
{
   auto opt=Configurator::enumMap.get(value);
   typename R::type result;
   if (opt.has_value())
      result=typename R::type(opt.value());
   else
   {
      result=typename R::type();
      errHandler(errState,ErrorState::ProcessError::NOT_ENUM_IN_MAP);
   }
   return result;
}

template <typename R,std::enable_if_t<!configuration::is_enumeration<R>::value,int> = 0>
constexpr R configuration::FieldExtractor::getFromMap(const char* value) const noexcept
{
   auto opt=Configurator::enumMap.get(value);
   R result;
   if (opt.has_value())
      result=R(opt.value());
   else
   {
      result=R();
      errHandler(errState,ErrorState::ProcessError::NOT_ENUM_IN_MAP);
   }
   return result;
} 
template <typename R,std::enable_if_t<!configuration::is_enumeration<R>::value,int> = 0>
std::optional<R> configuration::FieldExtractor::convertToFieldValue(const char* value)
{ 
   std::optional<R> result;
      if constexpr (std::is_same_v<R,bool>)
      {
         bool boolRes=false;
         if(tinyxml2::XMLUtil::ToBool(value,&boolRes))
         {
            result=R(boolRes);
         }
      }
      else if constexpr (std::is_same_v<R,const char *>)
      {
         char * ptr=new (TypeAllocators::CharAlloc.allocate(folly::constexpr_strlen(value))) char;
         strcpy(value,ptr,folly::constexpr_strlen(value));
         result=static_cast<const char *>(ptr);
      }
      else if constexpr (std::is_pointer_v<R>)
      {
         if (!folly::constexpr_strcmp("nullptr",value))
            result=std::nullptr_t{};
      }
      else if constexpr (std::is_convertible_v<R,uint64_t>)
      {
         int64_t valueRes=0;
         if (tinyxml2::XMLUtil::ToInt64(value,&valueRes))
            result=static_cast<R>(valueRes);
         else
         {
            EnumBitOpSplitter<R> enumSplitter(value,errState,errHandler);
            result=enumSplitter.calculate(this,&FieldExtractor::getFromMap<R>);
         }
      }
      return result;        
}
constexpr bool configuration::FieldExtractor::isFieldInStruct(const char * name) noexcept
{
   bool result=true;
// set values based on fields of type
   std::size_t structHash=getStringHash(name, scaleFactor);
   if (fieldPositions[structHash] == -1)
   {
      result=false;
   }
   return result;
}


#endif
