#include "StringHashTest.hpp"
#include "../src/get_string_hash.hpp"

aatest::StringAlloc StringHashFixture::allocator;
aatest::string StringHashFixture::largeString(std::numeric_limits<uint8_t>::max(),(char)CHAR_MAX,allocator);
TEST_F(StringHashFixture, HashInRange)
{
   aatest::string midString(std::numeric_limits<uint8_t>::max()/2,(char)(CHAR_MAX),allocator);
   ASSERT_GT(-1,configuration::getStringHash("",scaleFactor));
   ASSERT_LT(configuration::getStringHash(largeString,scaleFactor),limit);
   ASSERT_LT(configuration::getStringHash(midString,scaleFactor),limit);
}

TEST_F(StringHashFixture, HashNoConflicts)
{
    constexpr uint8_t length=std::numeric_limits<uint8_t>::max();

    decltype(configuration::getStringHash(0,0.0)) hashes[length]={0};
    std::generate_n(hashes,length,[this] ()-> decltype(configuration::getStringHash(0,0.0))
    {                                                                                                                                                                               
        return configuration::getStringHash(aatest::string(getRandomChar(),getRandomChar(),aatest::StringAlloc()),
        scaleFactor);
    });
    decltype(configuration::getStringHash(0,0.0)) * hashesEnd=hashes+length;
   
    std::sort(hashes,hashesEnd);
    decltype(configuration::getStringHash(0,0.0)) duplicates[length];
    constexpr decltype(configuration::getStringHash(0,0.0)) sentinal=(decltype(configuration::getStringHash(0,0.0)))-1;
    std::inclusive_scan(hashes,hashesEnd,duplicates,[sentinal](const auto& a, const auto& b)
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
    // allow at least one conflict in hash to pass in this diverse pool
    ASSERT_LT(std::count_if(duplicates, duplicates+length,[](const auto& val) {return val != sentinal;}),2);
}