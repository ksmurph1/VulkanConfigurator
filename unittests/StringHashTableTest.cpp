#include "StringHashTableTest.hpp"
#include "../src/get_string_hash.hpp"

TEST_F(HashTableFixture, CheckHashTableSetup)
{ 
   using tableType=configuration::StrHashTable<std::numeric_limits<uint8_t>::max(), int>;
   const char* key[]={"one"};
   int values[]={1};
   tableType hashTable=tableType::setup(key,values);
   auto result=hashTable.get("zero");
   ASSERT_FALSE(result);
   result=hashTable.get("one");
   ASSERT_EQ(result.value(),1);
}

/*
TEST_F(HashTableFixture, CheckHashTableGetOnConflict)
{ 
   using tableType=configuration::StrHashTable<4, uint8_t>;
   const char* key[]={"two","three","four","five"};
   int values[]={2,3,4,5};
   tableType hashTable=tableType::setup(key,values);
   auto result=hashTable.get("two");
   ASSERT_EQ(result.value(),2);
}*/