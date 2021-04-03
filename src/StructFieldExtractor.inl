#ifndef StructFieldExtractor_inl
#define StructFieldExtractor_inl
VISITABLE_STRUCT(configuration::Configurator::NullWrapper, dummy);


template <typename T>
template <typename S>
void configuration::StructFieldExtractor<T,typename std::enable_if_t<std::is_nothrow_default_constructible_v<T> && 
     std::is_trivially_copy_constructible_v<T> && std::is_class_v<T> && 
     !configuration::is_enumeration<T>::value>>::setFieldForStruct(const char* fieldName,const S& value)
{
   std::size_t structHash=getStringHash(fieldName, scaleFactor);
   setFieldValue(structHash, value);
}
/*template <typename T>
void  configuration::StructFieldExtractor<T,typename std::enable_if_t<std::is_nothrow_default_constructible_v<T> && 
     std::is_trivially_copy_constructible_v<T> && std::is_class_v<T> && 
     !configuration::is_enumeration<T>::value>>::iterateFieldPositions(const std::any & type)
      {
         auto* result=std::any_cast<boost::hana::basic_type<enumeration<T>>>(&type);
         if (result)
            iterateFieldPositions(*result);
         else
            iterateFieldPositions(std::any_cast<boost::hana::basic_type<T>>(type));     
      }*/
      
template <typename T>
void configuration::StructFieldExtractor<T,typename std::enable_if_t<std::is_nothrow_default_constructible_v<T> && 
     std::is_trivially_copy_constructible_v<T> && std::is_class_v<T> && 
     !configuration::is_enumeration<T>::value>>::iterateFieldPositions()
{
    visit_struct::for_each(T{}, [this] (const char* fieldName, const int idx, auto*) constexpr noexcept -> void
    {
       auto hash=getStringHash(fieldName, scaleFactor);
       fieldPositions[hash]=(uint8_t)idx;
    });
}
template <typename T>
template <typename S>
void configuration::StructFieldExtractor<T,typename std::enable_if_t<std::is_nothrow_default_constructible_v<T> && 
     std::is_trivially_copy_constructible_v<T> && std::is_class_v<T> && 
     !configuration::is_enumeration<T>::value>>::setFieldValue(const std::size_t hash, const S* value)
{
   visit_struct::for_each(container, 
      [this,hash,&value] (const char*,const int idx, auto* field) -> void
      {
         if (idx ==  fieldPositions[hash])
               if constexpr ( std::is_pointer_v<std::decay_t<decltype(*field)>> && 
                              is_static_castable<decltype(value),std::decay_t<decltype(*field)>>::value)
                  // copy array
      //            for (uint8_t i=0; i < sizeof(*field)/sizeof(std::remove_reference_t<decltype(*field)>); ++i)
                     //*(*field+i)=*(value+i);
                     *field=value;
      });
}

template <typename T>
template <typename S,std::size_t N>
void configuration::StructFieldExtractor<T,typename std::enable_if_t<std::is_nothrow_default_constructible_v<T> && 
     std::is_trivially_copy_constructible_v<T> && std::is_class_v<T> && 
     !configuration::is_enumeration<T>::value>>::setFieldValue(const std::size_t hash, S (&value) [N])
{
   visit_struct::for_each(container, 
      [this,hash,&value] (const char*,const int idx, auto* field) -> void
      {
         if (idx ==  fieldPositions[hash])
         {
            if constexpr (std::is_array_v<std::remove_reference_t<decltype(*field)>>)
            {
               using arrayToPointer=std::add_pointer_t<std::remove_all_extents_t<std::remove_reference_t<decltype(*field)>>>;
               arrayToPointer dest=*field;
               // copy array
               for (uint8_t i=0; i < sizeof(*field)/sizeof(std::remove_all_extents_t<std::remove_reference_t<decltype(*field)>>); ++i)
                  *(dest+i)=*(std::addressof(value)+i);
            }
         }
      });
}

template <typename T>
template <typename S>
void configuration::StructFieldExtractor<T,typename std::enable_if_t<std::is_nothrow_default_constructible_v<T> && 
     std::is_trivially_copy_constructible_v<T> && std::is_class_v<T> && 
     !configuration::is_enumeration<T>::value>>::setFieldValue(const std::size_t hash, const S& value)
{
   // get the underlying type
   //using baseType=Configurator::UnderlyingType<T>;
      visit_struct::for_each(container, 
      [this,hash,&value] (const char*,const int idx, auto* field) -> void
      {
         if (idx ==  fieldPositions[hash])
         {
            if constexpr (!std::is_same_v<std::nullptr_t, S> && !std::is_array_v<std::remove_reference_t<decltype(*field)>>
                         && is_static_castable<S,std::decay_t<decltype(*field)>>::value)
            {
                  std::printf("field:%p\n",field);
                  std::printf("typeid:%s\n",typeid(std::remove_reference_t<decltype(*field)>).name());
                  std::printf("S:%s\n",typeid(S).name());
                  *field=static_cast<std::decay_t<decltype(*field)>>(value);
            }
            else
               if constexpr (std::is_pointer_v<decltype(*field)>)
                  *field=nullptr;
         }
      });
}

#endif



