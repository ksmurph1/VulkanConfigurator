#include "XmlParserTest.hpp"
#include "../src/get_string_hash.hpp"

bool XmlProcFixture::VisitEnter (const tinyxml2::XMLElement & element,const tinyxml2::XMLAttribute * attribute)
{
   if (attribute != nullptr)
   {
      populateAttrMap(attribute);
   }
   return true;
}

TEST_F(XmlProcFixture, AttribMapPopulated)
{
   bool result=parseConfig(filename1);
   if (result)
   {
         auto [map, size]=getAttribMap();
       
         ASSERT_FALSE(size == 0);
         ASSERT_EQ(map[configuration::getStringHash("type",scaleFactor)].compare("VkImageViewCreateFlags"),0);
         ASSERT_EQ(map[configuration::getStringHash("value",scaleFactor)].compare("0"),0);
         ASSERT_EQ(map[configuration::getStringHash("name",scaleFactor)].compare("flags"),0);
   }
   else
      FAIL();    
}

TEST_F(XmlProcFixture, ErrorCondition)
{
   bool result=parseConfig(filename2);
   ASSERT_TRUE(!result);
   ASSERT_EQ(getErrorState(), ProcessError::FILE_LOAD_ERR);
}