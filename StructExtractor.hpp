#pragma once
#include "ArenaContainers.hpp"
#include <vector>
#include <string>
#include "magic_get/include/boost/pfr/precise/core.hpp"
template <typename T>
class Extractor
{
   static aa::recursive_allocator<aa::string> allocator(
       std::move(aa::Alloc<aa::string>(aa::ARENA)));
   std::vector<aa::string,decltype(allocator)> fieldNames;
   public:
    explicit Extractor() : fieldNames(allocator) {}
    Extractor& operator[](const char* fieldName)
    {
       
    }
    void AddFieldName(const char * name)
    {
       fieldNames.emplace_back(name);
    }
};