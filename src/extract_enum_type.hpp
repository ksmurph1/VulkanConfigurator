#ifndef extract_enum_type_hpp
#define extract_enum_type_hpp
#include <string_view>
namespace configuration
{
   constexpr std::string_view extractEnum(std::string_view src)
   {
      std::size_t leftAnglePos=src.find_first_of("<");
      std::size_t rightAnglePos=src.find_first_of(">");

      return src.substr(leftAnglePos+1,rightAnglePos-leftAnglePos-1);
   }
}
#endif