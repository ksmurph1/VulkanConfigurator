#include "TypeHashTest.hpp"
#include "../src/ctti_type_index.hpp"
#include "../src/get_string_hash.hpp"
#include "../src/constexpr_strcpy.hpp"
#include <numeric>

TEST_F(TypeHashFixture, CheckTypeHashConflicts)
{
    tuple_foreach(indexing_tuple(std::make_index_sequence<std::variant_size_v<WrapperVariant>>{}),
    [this](auto index)
    {
       using underlyingType=typename std::variant_alternative_t<index,WrapperVariant>::type;
      
       const char* name=boost::typeindex::ctti_type_index::type_id_with_cvr<underlyingType>().pretty_name();
       char dst[strlen(name)+1];

       ASSERT_EQ(typeMap.getType(configuration::strcpyEnum(name,dst,strlen(name)+1)).index(), index);
    });
}


/*TEST_F(TypeHashFixture, CheckTypeHashConflicts)
{
    constexpr hashType length=std::extent_v<HashTypeMap::TypeMap>;
    aatest::Alloc<hashType> hashAlloc(aatest::ARENATEST);
    std::vector<hashType,aatest::Alloc<hashType>> hashes(length,hashAlloc);
    tuple_foreach(indexing_tuple(std::make_index_sequence<std::variant_size_v<WrapperVariant>>{}),
    [this,&hashes](auto index)
    {
       using underlyingType=typename std::variant_alternative_t<index,WrapperVariant>::type;
      

       boost::typeindex::ctti_type_index typeInfo=boost::typeindex::ctti_type_index::type_id_with_cvr<underlyingType>();
       hashes.push_back(configuration::getStringHash(typeInfo.pretty_name(),scaleFactor));
    });

    std::sort(hashes.begin(),hashes.end());
    hashType duplicates[length];
    constexpr hashType sentinal=(hashType)-1;
    std::inclusive_scan(hashes.begin(),hashes.end(),duplicates,[sentinal](const auto& a, const auto& b)
    {
        if (a == b)
        {
            return b;
        }
        else
        {
           return sentinal;
        }
        
    });
    auto count=std::count_if(duplicates, duplicates+length,[sentinal](const auto& val) {return val != sentinal;});
    // must not be any conflicts
    ASSERT_EQ(count,0);
}*/