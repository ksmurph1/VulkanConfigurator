#ifndef STRUCTFIELDEXTRACTOR_HPP
#define STRUCTFIELDEXTRACTOR_HPP

#include "visit_struct.hpp"
#include "VulkanConfigurator.hpp"
#include "folly/src/Function.h"
#include "FieldExtractor.hpp"
#include "enumeration_wrapper.hpp"
#include "UtilityTypes.hpp"

  namespace configuration
  {
     template <typename T, typename Enable=void>
     class StructFieldExtractor final : public FieldExtractor
     {
         Configurator::WrapperVariant getType() const noexcept
         {
             return Configurator::WrapperVariant{};
         }
         void setFieldForStruct(const char*, const Configurator::UnderlyingTypes& ) override
         {}

          bool isVisitable() const noexcept override
          {
             return false;
          }
          void iterateFieldPositions() override
          {
          }
          public: 
          StructFieldExtractor(ErrorState* const,
           void (* const) (ErrorState*,const ErrorState::ProcessError)) noexcept : FieldExtractor(nullptr,nullptr)
          {}
     };
     
     template<typename T> 
   class StructFieldExtractor<T,typename std::enable_if_t<std::is_nothrow_default_constructible_v<T> && 
     std::is_trivially_copy_constructible_v<T> && std::is_class_v<T> && 
     !configuration::is_enumeration<T>::value>> : public FieldExtractor 
   {
      friend class XmlProcessor;

      T container;
      protected:
      // std::any convertToFieldValue(const std::any&,const char*, const char *, const uint8_t) override;
      inline void setFieldForStruct(const char* fieldName, const Configurator::UnderlyingTypes& value) override
      {
         std::visit([this,fieldName](const auto& arg)
         {
            setFieldForStruct(fieldName,arg);
         },value);
      }
      template <typename S>
      void setFieldForStruct(const char*, const S&);

      /*template <typename T>
      void setFieldValue(const T&, const std::size_t, const std::any&);*/
      template <typename S>
      void setFieldValue(const std::size_t, const S*);

      template <typename S, std::size_t N>
      void setFieldValue(const std::size_t, S (&)[N]);

      template <typename S>
      void setFieldValue(const std::size_t, const S&);
      
     /* template <typename T,typename R>
      aa::Function<void (const int8_t)> assignValueToField(T&,const R&,std::size_t) noexcept;*/

  /* template <typename T>
   constexpr bool isVisitable(const boost::hana::basic_type<T>&) const noexcept;*/

      /*aa::string getTypeName()
      {
         return std::visit([](const auto& arg)
         {   {
             return Configurator::WrapperVariant{};
           }::UnderlyingType<decltype(arg)>>>().pretty_name());
         }, containerType);
      }*/

      inline bool isVisitable() const noexcept override
      {
         return visit_struct::traits::is_visitable<T>::value;
      }

      inline std::any getContainer() const noexcept override
      {
         return container;
      }
/*      inline Configurator::WrapperVariant getType() noexcept
      {
         return containerType;
      }*/
     
      void iterateFieldPositions() override;
     
     /* void iterateFieldPositions(const boost::hana::basic_type<enumeration<T>>&)
      {

      }*/
      inline Configurator::WrapperVariant getType() const noexcept
      {
         return UtilityTypes::WrapperType<T>{};
      }

     
   public:
      
      explicit StructFieldExtractor(ErrorState* const processor, void (* const errHandler)
                         (ErrorState*,const ErrorState::ProcessError) noexcept,const T& arg) :  FieldExtractor(processor,errHandler),
                         container(arg)
      {
        
      }
      explicit StructFieldExtractor(ErrorState* const processor, void (* const errHandler)
                         (ErrorState*,const ErrorState::ProcessError) noexcept) :  FieldExtractor(processor,errHandler), container()
      {
        
      }
      ~StructFieldExtractor()=default;
   };
     template <typename T>
     class StructFieldExtractor<boost::hana::basic_type<T>> final : public StructFieldExtractor<T>
     {
     };
  }
#endif