#pragma once

#include "VulkanConfigurator.hpp"
#include "folly/src/Function.h"
#include "enumeration_wrapper.hpp"  
  namespace configuration
  {
   class FieldExtractor final
   {
      friend class XmlProcessor;
         // this class should be a singleton,static variables allowed
      constexpr inline static std::size_t maxFields=std::numeric_limits<uint8_t>::max();

      constexpr inline static float scaleFactor=1.0f/  // scale result down so fits in array
                                                // like container-make sure we have twice space
               (std::numeric_limits<stringhash_uint32::value_type>::max()/(maxFields/2.0f));
      std::any container;


      int8_t fieldPositions[maxFields]={-1};

      aa::Alloc<char> charAlloc;

      template <typename T>
      void setFieldForStruct(const std::any&);

      /*template <typename T>
      void setFieldValue(const T&, const std::size_t, const std::any&);*/

      template <typename T>
      void setFieldValue(const std::size_t, const std::any&);

   
      std::any convertToFieldValue(const char*, const char*);
      
      template <typename T,typename R>
      aa::Function<void (const int8_t)> assignValueToField(T&,const R&,std::size_t) noexcept;

  /* template <typename T>
   constexpr bool isVisitable(const boost::hana::basic_type<T>&) const noexcept;*/
      template <typename T>
      inline constexpr const char * getTypeName(const boost::hana::basic_type<T>&)
      {
      return boost::typeindex::ctti_type_index::type_id_with_cvr<T>().pretty_name();
      }

      template <typename T>
      constexpr bool isFieldInStruct(const boost::hana::basic_type<T>&) noexcept;

      template <typename T>
      constexpr bool isVisitable() const noexcept;
      inline std::any getContainer() const noexcept
      {
         return container;
      }
      template <typename T>
      constexpr void iterateFieldPositions(const boost::hana::basic_type<enumeration<T>>&)
      {}

      template <typename T>
      constexpr void iterateFieldPositions(const boost::hana::basic_type<T>&);
   public:
      template <typename T,typename=typename std::enable_if_t<std::conjunction_v<
      std::is_nothrow_default_constructible<T>,std::is_trivially_copy_constructible<T>>>>
      explicit FieldExtractor(const boost::hana::basic_type<T>& ) : container(T{}),charAlloc()
      {
         
      }
      
   };
  }