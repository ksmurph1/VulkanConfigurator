#ifndef ARENACONTAINERS_HPP
#define ARENACONTAINERS_HPP
#include <unordered_map>
#include <scoped_allocator>
#define USE_JEMALLOC
#include "folly/src/memory/Arena.h"
//#include "cxx_function/cxx_function.hpp"
#include "cxx_function/function2.hpp"
#include <boost/pool/pool_alloc.hpp>

namespace aa
{
    constexpr std::size_t ARENA_SIZE= 2097152; //2^17 * 2^4=2^21  want at least 2MB and multiple of 16(alignof(std::max_align_t))
    extern folly::SysArena ARENA;
    template <typename T>
    class CxxAllocatorAdaptorArena 
    {
       private:
          using Self = CxxAllocatorAdaptorArena<T>;
          
          template <typename U>
          friend class CxxAllocatorAdaptorArena;

          folly::CxxAllocatorAdaptor<T,folly::SysArena> allocator;
          using Inner = decltype(allocator);
      public:
         using value_type = T;
         typedef std::size_t    size_type;        // An unsigned integral type that can represent the size of the largest object to be allocated. 
         using propagate_on_container_copy_assignment = std::true_type;
         using propagate_on_container_move_assignment = std::true_type;
         using propagate_on_container_swap = std::true_type;

        explicit CxxAllocatorAdaptorArena() : allocator(aa::ARENA)
         {} 
        template <typename U>
        explicit CxxAllocatorAdaptorArena(CxxAllocatorAdaptorArena<U> const& other)
            : allocator(other.allocator){}
        T* allocate(size_type n)
        {
              return allocator.allocate(n);
        }
        void deallocate(T* p, size_type n) 
        {
              allocator.deallocate(p, n);
        }
      
        const folly::SysArena& inner_allocator()
        {
        return allocator.inner_allocator();
        }
        friend bool operator==(Self const& a, Self const& b) noexcept 
        {
            return a.allocator == b.allocator;
        }
        friend bool operator!=(Self const& a, Self const& b) noexcept
        {
            return a.allocator != b.allocator;
        }
    };
    class ArenaPoolAlloc
    {
       static CxxAllocatorAdaptorArena<char> arenaAlloc;
       public:
       typedef std::size_t    size_type;
       private:
       static inline std::array<std::pair<char*,size_type>,
                     std::numeric_limits<uint8_t>::max()> ptrStore;
       static inline size_type index=0;
       static inline size_type nextAvailPos=index+1; 
       public:
       // types
       typedef std::ptrdiff_t difference_type;  // A signed integral type that can represent the difference of any two pointers. 

       // public static functions for pool allocators
       static char * malloc(const size_type n)
       {
          ptrStore[index]=std::make_pair(arenaAlloc.allocate(n),n);
          const size_type curPos=index;
          if (index == nextAvailPos)
             index=nextAvailPos++;
          else
             index=nextAvailPos;
          return ptrStore[curPos].first;
       }
       static void free(char *const ptr)
       {
          auto loc=std::find_if(std::begin(ptrStore), std::end(ptrStore),[ptr](auto item)
                   {return ptr== item.first;});
          index=loc-std::begin(ptrStore);
          arenaAlloc.deallocate(loc->first,loc->second);
       }
    };
    template <typename T>
    using Alloc=CxxAllocatorAdaptorArena<T>;

   

    template <typename T, typename U>
    using recursive_allocator=std::scoped_allocator_adaptor<Alloc<T>,Alloc<U>>;

    template <typename T>
    using recursive_allocator_single=std::scoped_allocator_adaptor<Alloc<T>>;

    template <class CharT, class Traits = std::char_traits<CharT>>
    using basic_string = std::basic_string< CharT, Traits,Alloc<CharT>>;

    typedef basic_string<char> string;
    using StringAlloc=aa::Alloc<string::value_type>;
    
    // unordered_map has trouble with stateful allocators so need to use boost::pool
    // with unordered_map since boost::pool is stateless
    template <class Key,
              class T,
              class Hash = std::hash<Key>,
              class Pred = std::equal_to<Key>>
    using unordered_map_pool = std::unordered_map<Key, T, Hash, Pred,
    boost::pool_allocator<std::pair<const Key,T>,ArenaPoolAlloc>>;

//using ipc_row = std::vector<int, alloc<int>>;
//std::scoped_allocator_adaptor<alloc<ipc_row>>
   

    template <typename FunctionType>
   //using Function=cxx_function::unique_function_container<Alloc<FunctionType>,FunctionType>;
    using Function=cxx_function::unique_function<FunctionType>;
}
#endif