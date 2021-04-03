#pragma once

#include "VulkanConfigurator.hpp"
#include "enumeration_wrapper.hpp"
#include "ErrorState.hpp" 
namespace configuration
{
   
   class FieldExtractor
   {
     /* template<bool...> struct bool_pack;
      template<bool... bs>
      using all_false=std::is_same<bool_pack<bs...,false>,bool_pack<false,bs...>>;

      template <typename R, typename... Ts>
      using are_not_same=all_false<std::is_same_v<std::decay_t<Ts>,R>...>;*/
 
      protected:
              // this class should be a singleton,static variables allowed
      constexpr inline static std::size_t maxFields=std::numeric_limits<uint8_t>::max();
      constexpr inline static float scaleFactor=(float)maxFields/std::numeric_limits<stringhash_uint32::value_type>::max();

     /* constexpr inline static float scaleFactor=1.0f/  // scale result down so fits in array
                                                // like container-make sure we have twice space
               (std::numeric_limits<stringhash_uint32::value_type>::max()/(maxFields/2.0f));*/

      uint8_t fieldPositions[maxFields];

      ErrorState* errState=nullptr;
      void (* errHandler)(ErrorState*,const ErrorState::ProcessError) noexcept=nullptr;
          template <typename TYPE, typename T, std::enable_if_t<std::is_pointer_v<T> && std::is_pointer_v<TYPE> &&
             !std::is_convertible_v<T,TYPE> || std::is_void_v<std::remove_pointer_t<TYPE>> || std::is_void_v<std::remove_pointer_t<T>>,int> = 0>
          constexpr TYPE getItemArr(T&, const uint8_t);
          template <typename TYPE, typename T, std::enable_if_t<std::is_pointer_v<T> && std::is_pointer_v<TYPE> &&
             std::is_convertible_v<T,TYPE> && !std::is_void_v<std::remove_pointer_t<TYPE>> && !std::is_void_v<std::remove_pointer_t<T>>,int> = 0>
          TYPE getItemArr(T&, const uint8_t);
          template <typename R,std::enable_if_t<configuration::is_enumeration<R>::value,int> = 0>
          constexpr typename R::type getFromMap(const char*) const noexcept;
          template <typename R,std::enable_if_t<!configuration::is_enumeration<R>::value,int> = 0>
          constexpr R getFromMap(const char*) const noexcept;
          template <typename R,std::enable_if_t<configuration::is_enumeration<R>::value,int> = 0>
          constexpr typename R::type getEnumFlag(const char*) const noexcept;
          
          explicit FieldExtractor(ErrorState* const processor, void (* const errHandler)
                         (ErrorState*,const ErrorState::ProcessError) noexcept) noexcept: errState(processor),errHandler(errHandler)
          {
              std::memset(fieldPositions, -1, sizeof(fieldPositions));
          }
      public:
       
          template <typename R,std::enable_if_t<is_enumeration<R>::value,int> = 0>
          constexpr std::optional<typename R::type> convertToFieldValue(const char*);

          template <typename R,std::enable_if_t<!is_enumeration<R>::value,int> = 0>
          std::optional<R> convertToFieldValue(const char*);

          // substitute for other combinations-not defined
         /* template <typename R, typename T, std::enable_if_t<std::is_pointer_v<R> && 
          !std::is_convertible_v<T,std::remove_pointer_t<R>> && !std::is_convertible_v<T*,R>,int> = 0>
          R convertToFieldValue(T*, const uint8_t);*/

          template <typename R, typename T, std::enable_if_t<std::is_pointer_v<R>,int> = 0>
          R convertToFieldValue(T*, const uint8_t);

          template <typename R,typename T, std::enable_if_t<std::is_pointer_v<R> && 
                                          std::is_pointer_v<std::remove_pointer_t<R>>,int> = 0>
          R convertToFieldValue(T**, const uint8_t);

          template <typename R, typename T, std::enable_if_t<!is_enumeration<R>::value && !std::is_same_v<char *,T>,int> = 0>
          std::optional<R> convertToFieldValue(const T&);
          template <typename R, typename T, std::enable_if_t<is_enumeration<R>::value && !std::is_same_v<char *,T>,int> = 0>
          constexpr std::optional<typename R::type> convertToFieldValue(const T&);
          
          constexpr bool isFieldInStruct(const char*) noexcept;

          virtual ~FieldExtractor() {}
          //virtual std::any convertToFieldValue(const std::any&, const char*, const char*, const uint8_t) = 0;

          virtual void setFieldForStruct(const char*, const Configurator::UnderlyingTypes& )=0;
         
          virtual std::any getContainer() const noexcept
          {
             return std::nullptr_t{};
          }
          
          virtual Configurator::WrapperVariant getType() const noexcept=0;
        
          virtual bool isVisitable() const noexcept=0;
          virtual void iterateFieldPositions()=0;
   };
}