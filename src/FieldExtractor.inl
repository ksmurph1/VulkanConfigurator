#pragma once
#include "get_string_hash.hpp"
#include "enumeration_wrapper.hpp"
#include "constexpr_strcpy.hpp"
#include "folly/src/portability/Constexpr.h"
VISITABLE_STRUCT(configuration::Configurator::NullWrapper, dummy);

template <typename T>
void configuration::FieldExtractor::setFieldForStruct(const std::any& value)
{
   std::size_t structHash=getStringHash(getTypeName(T{}),Configurator::scaleFactor);
   setFieldValue<T>(structHash, value);
}

template <typename T>
constexpr void configuration::FieldExtractor::iterateFieldPositions(const boost::hana::basic_type<T>&)
{
   if constexpr (std::is_class_v<T>) // T has to be struct or class
   {
    visit_struct::for_each(T{}, [this] (const char* fieldName, const int idx, auto*) constexpr noexcept -> void
    {
       fieldPositions[getStringHash(fieldName,scaleFactor)]=(int8_t)idx;
    });
   }
}

template <typename T>
constexpr bool configuration::FieldExtractor::isVisitable() const noexcept
{
   return visit_struct::traits::is_visitable<T>::value;
}

template <typename T>
constexpr bool configuration::FieldExtractor::isFieldInStruct(const boost::hana::basic_type<T>& type) noexcept
{
   bool result=true;
// set values based on fields of type
   std::size_t structHash=getStringHash(getTypeName(type),Configurator::scaleFactor);
   if (fieldPositions[structHash] == -1)
   {
      result=false;
   }
   return result;
}

std::any configuration::FieldExtractor::convertToFieldValue(const char* value, const char* typeName)
{
   return std::visit([this,&value](const auto& arg) -> std::any
   {
      using type=configuration::Configurator::UnderlyingType<decltype(arg)>;
      std::any result;

      if constexpr (is_enumeration<type>::value)
      {
         auto enumResult=magic_enum::enum_cast<typename type::type>(std::string_view(value));
         if (enumResult.has_value())
         {
            result=typename type::type(enumResult.value());
         } 
      }
      else if constexpr (std::is_same_v<type,bool>)
      {
         bool boolRes=false;
         if(tinyxml2::XMLUtil::ToBool(value,&boolRes))
         {
            result=type(boolRes);
         }
      }
      else if (std::is_pointer_v<type> && !folly::constexpr_strcmp("nullptr",value))
      {
         result=type{},result=nullptr;
      }
      else if (std::is_same_v<type,const char *>)
      {
         char * ptr=new (charAlloc.allocate(folly::constexpr_strlen(value))) char;
         strcpy(value,ptr,folly::constexpr_strlen(value));
         result=static_cast<const char *>(ptr);
      }
      else if constexpr (std::is_convertible_v<type,uint64_t>)
      {
         int64_t valueRes=0;
         if (tinyxml2::XMLUtil::ToInt64(value,&valueRes))
         {
            result=static_cast<type>(valueRes);
         }
      }
      else
      {
         // value is another type name
         configuration::Configurator::WrapperVariant wrapperTypes=
                                          Configurator::hanaTypeForStrHash.getType(value);
         if (!std::holds_alternative<configuration::Configurator::WrapperType<
         configuration::Configurator::NullWrapper>>(wrapperTypes))
         {
            result=std::visit([](const auto& arg) -> std::any
            {
               if constexpr (std::is_convertible_v<type,Configurator::UnderlyingType<decltype(arg)>>)
               {
                  return type(configuration::Configurator::UnderlyingType<decltype(arg)>{});
               }
               else
                  return std::any();  //an empty any
            },wrapperTypes);
         }
      }
      return result;
   },  configuration::Configurator::hanaTypeForStrHash.getType(typeName));
    /*setFieldValue(structType,Configurator::getStringHash(
                  getTypeName<std::decay_t<decltype(type)>>(),scaleFactor),*/
}

template <typename T, typename R>
aa::Function<void (const int8_t)> configuration::FieldExtractor::assignValueToField(
   T& field,const R& value,std::size_t idx) noexcept
{
   aa::Alloc<void (const int8_t)> allocator;
   return aa::Function<decltype(allocator)::value_type>(
      [field,idx,value](int8_t storedIdx) 
   {
      if (idx == storedIdx) field=static_cast<T>(value);
   },allocator);
}


template <typename T>
void configuration::FieldExtractor::setFieldValue(const std::size_t hash,const std::any& value)
{
   // get the underlying type
   using baseType=Configurator::UnderlyingType<T>;
   if constexpr (std::is_class_v<baseType> && !is_enumeration<baseType>::value)  // T has to be struct or class
   {
      visit_struct::for_each(std::any_cast<baseType&>(container), 
      [this,hash,value] (const char*,const int idx, auto* field) -> void
      {
         if (idx ==  fieldPositions[hash])
         {
            if constexpr (std::is_array_v<std::remove_reference_t<decltype(*field)>>)
            {
               using arrayToPointer=std::add_pointer_t<std::remove_all_extents_t<std::remove_reference_t<decltype(*field)>>>;
               arrayToPointer dest=*field;
               auto source=*std::any_cast<std::remove_reference_t<decltype(*field)>>(&value);
               // copy array
               for (uint8_t i=0; i < sizeof(*field)/sizeof(std::remove_all_extents_t<std::remove_reference_t<decltype(*field)>>); ++i)
               {
                  *(dest+i)=*(source+i);
               }
            }
            else
               *field=*std::any_cast<std::remove_reference_t<decltype(*field)>>(&value);
         }
      });
   }
}