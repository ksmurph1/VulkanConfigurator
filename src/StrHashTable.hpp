#pragma once
#include <array>
#include <optional>
#include "get_string_hash.hpp"
#include "char_array_limits.hpp"
namespace configuration
{
    class Configurator;
template <std::size_t size, typename T,typename=typename std::enable_if_t<
std::is_copy_constructible_v<T> && std::is_default_constructible_v<T> && 
std::is_move_constructible_v<T>>>
class StrHashTable
{
    constexpr inline static uint8_t CHAINSIZE=5;
    
    constexpr inline static float scaleFactor=(float)size/
                                 std::numeric_limits<stringhash_uint32::value_type>::max();
    struct Bucket
    {
        friend class StrHashTable<size,T>;
        private:
        char key[MAX_CHAR_SIZE];
        T value;
        /*constexpr void copy(const char*&& array) noexcept
        {
            // copy string into array
            std::decay_t<decltype(MAX_CHAR_SIZE)> i=0;
            for (; *(array +i) != '\0' && i < MAX_CHAR_SIZE; i++)
            {
               this->key[i]=*(array+i);
            }
            this->key[i]='\0';
        }*/
        constexpr void copy(const char*array) noexcept
        {
            // copy string into array
            std::decay_t<decltype(MAX_CHAR_SIZE)> i=0;
            for (; *(array +i) != '\0' && i < MAX_CHAR_SIZE; i++)
            {
               this->key[i]=*(array+i);
            }
            this->key[i]='\0';
        }
        public:
        constexpr Bucket(const char* key,T&& value) noexcept : key{'\0'},value(std::forward<T>(value))
        {
            // copy string into array
            copy(key);
        }
        // copy value
        constexpr Bucket(const char* key,const T& value) noexcept : key{'\0'},value(value)
        {
            // copy string into array
            copy(key);
        }
        constexpr Bucket() noexcept : key{'\0'},value{}  {}
        std::decay_t<decltype(CHAINSIZE)> count=0;
        Bucket(const Bucket&)=delete;
       
         template <typename=typename std::enable_if_t<std::is_nothrow_move_assignable_v<T>>>
        Bucket& operator=(const Bucket& rhs)
        {
           return operator=(std::move(rhs));
        }

         template <typename=typename std::enable_if_t<std::is_nothrow_move_assignable_v<T>>>
        Bucket(Bucket&& bucket)
        {
           value=std::move(bucket.value);
           copy(bucket.key); // won't do any good to move this array of chars
        }
        template <typename=typename std::enable_if_t<std::is_nothrow_move_assignable_v<T>>>
        constexpr Bucket& operator=(Bucket&& rhs) noexcept
        {
            copy(rhs.key);
            value=std::move(rhs.value);// won't do any good to move this array of chars
            return *this;
        }
    };

    std::array<Bucket,CHAINSIZE> buckets[size];

     constexpr StrHashTable() :buckets{} {}
   public:
    constexpr StrHashTable(const StrHashTable& src) noexcept : buckets() 
    {
       for (decltype(size) i=0; i < size; i++)
       {
          buckets[i]=src.buckets[i];
       }
    } 
    template <std::size_t N,typename=typename std::enable_if_t<N<=size>>
    constexpr static StrHashTable setup(const char*(&strs)[N], const T (&values)[N])
    {
    StrHashTable base;
    bool marked[size]={false};

    for (decltype(size) i=0; i < N; i++)
    {
        decltype(size) hash=getStringHash(strs[i],scaleFactor);
        std::decay_t<std::remove_extent_t<decltype(buckets)>>& chain= base.buckets[hash];
        if (marked[hash])
        {  
            chain[chain[0].count+1]=Bucket(strs[i],std::move(values[i]));
            chain[0].count+=1;
        }
        else
        {
            marked[hash]=true;
            chain[0]=Bucket(strs[i],std::move(values[i]));
        }
    }
    return base;
    }

    constexpr std::optional<T> get(const char* key) const noexcept
    {
        decltype(getStringHash(0,0.0)) hash=getStringHash(key,scaleFactor);
        if (hash < size)
        {
            // find bucket based on hash
            const std::array<Bucket,CHAINSIZE>& chain=buckets[hash];
            for (std::decay_t<decltype(CHAINSIZE)> i=0; i < chain.size(); i++)
            {
                // compare constexpr written out as strcmp is not yet constexpr
                std::decay_t<decltype(CHAINSIZE)> j;
                for(j=0;*(key+j) == chain[i].key[j] && *(key+j) != '\0'; ++j);
                
                if (*(key + j) == '\0')
                {
                   // found value in chain so return
                   return chain[i].value;
                }
                // if reach end of chain then we have not found it
                else if (chain[0].count == i)
                {
                    break;
                }
            }
        }
        return std::nullopt;  // error condition
    }

    constexpr static decltype(size) getSize() noexcept
    {
        return size;
    }
};
}