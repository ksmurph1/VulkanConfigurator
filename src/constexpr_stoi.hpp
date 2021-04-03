#ifndef CONSTEXPR_STOI_HPP
#define CONSTEXPR_STOI_HPP
namespace configuration
{
   constexpr int stoi_impl(const char* str, int value = 0)
{
   int result=0;
   if (*str)
   {
      if (*str <= '9' && *str >= '0')
         result=stoi_impl(str + 1, (*str - '0') + value * 10);
   }
   else
      result=value;
   return result; 
}

constexpr int stoi(const char* str)
{
    return stoi_impl(str);
}
}
#endif