#include <any>
#include <unordered_map>
#include "VulkanConfigurator.hpp"
namespace configuration
{
class Initializer
{
   private:
       //static Configurator::MapBuilder::MapType mapExtractor;
       static aa::unordered_map<aa::string,std::any> container; 
       template<typename T>
      T* get() const noexcept
      {
         try
         {
            return std::any_cast<T*>(&container);
         }
         catch(...)
         {
            return nullptr;
         }
      }
   public:
      /*template <typename T>
      structure_any(T&& vulkanStruct) : container(vulkanStruct) {}
      structure_any(std::any&& container) noexcept : container(container) {}
      structure_any(const structure_any&)=delete;
      structure_any(structure_any&&)=default; 
      structure_any& operator=(const structure_any&)=delete;
      structure_any& operator=(structure_any&&)=default;*/
      template <std::size_t N>
      auto& operator[](const char (&structName)[N]) const noexcept
      {
          return *get<Configurator::HashTypeMap::BaseType<Configurator::getStringHash(structName)>>();
      }
     
      template<typename T>
      T move() const noexcept
      {
         try
         {
            return std::any_cast<T&&>(std::move(container));
         }
         catch(...)
         {
            return T{};
         }
      }
};
}