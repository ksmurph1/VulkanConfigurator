#ifndef ERRORSTATE_HPP
#define ERRORSTATE_HPP
#include <cstdint>
#include <type_traits>
namespace configuration
{
class ErrorState
{
   friend class XmlProcessor;
   public:
   enum ProcessError : uint16_t
   {
      SUCCESS=0,
      TYPE_MISSING_LIST=1,
      PROCESSING_OUT_OF_ORDER=2,
      FIELD_HASH_INVALID=4,
      ENUM_TYPE=8,
      STRUCT_META_MISSING=16,
      VALUE_MISMATCH_CONFIG=32,
      FILE_LOAD_ERR=64,
      FIELD_TYPE_MISMATCH_CONFIG=128,
      FIELD_LEFT_UNASSIGNED=256,
      ENUM_OPER_INVAL=512,
      NOT_ENUM_IN_MAP=1024,
   };
   private:
       ProcessError errorState;
       constexpr inline void setError (const ProcessError flag) noexcept
       {
           errorState=static_cast<ProcessError>(static_cast<std::underlying_type_t<ProcessError>>(errorState) | flag);
       } 
       constexpr explicit ErrorState() noexcept :errorState(ProcessError::SUCCESS) 
       {}
   public:
   
   ErrorState(const ErrorState&)=delete;

   constexpr inline bool isError(const ProcessError error) const noexcept
   {
      return errorState & error;
   }
   constexpr inline bool isError() const noexcept
   {
      return errorState == ProcessError::SUCCESS;
   }
   friend constexpr void setErrorHelper(ErrorState*,const ErrorState::ProcessError) noexcept;
};
constexpr void setErrorHelper(ErrorState* const processor,const ErrorState::ProcessError error) noexcept
{
   processor->setError(error);
}
}
#endif