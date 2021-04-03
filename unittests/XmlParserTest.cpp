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
         ASSERT_STREQ(map[configuration::getStringHash("type",scaleFactor)].data(),"VkImageViewCreateFlags");
         ASSERT_STREQ(map[configuration::getStringHash("value",scaleFactor)].data(),"0");
         ASSERT_STREQ(map[configuration::getStringHash("name",scaleFactor)].data(),"flags");
   }
   else
      FAIL();    
}

TEST_F(XmlProcFixture, ErrorCondition)
{
   bool result=parseConfig(filename2);
   ASSERT_TRUE(!result && isError(configuration::ErrorState::ProcessError::FILE_LOAD_ERR));
}