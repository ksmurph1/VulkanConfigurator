//
// Created by kerry on 3/14/21.
//
#ifndef TYPEALLOCATORS_HPP
#define TYPEALLOCATORS_HPP
#include "ArenaContainers.hpp"
#include "StructFieldExtractor.hpp"
namespace configuration
{
    struct TypeAllocators
    {
        template<typename T>
        inline static aa::Alloc<StructFieldExtractor < T>> ExtractorAlloc{};

        inline static aa::StringAlloc StringAlloc{};
        inline static aa::Alloc<char> CharAlloc{};

        template<typename T>
        inline static aa::Alloc<T> TypeAlloc{};
    };
}
#endif //TYPEALLOCATORS_HPP
