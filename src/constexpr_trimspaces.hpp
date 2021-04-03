#ifndef CONSTEXPR_TRIMWHITESPACE
#define CONSTEXPR_TRIMWHITESPACE
#define TAB '\t'
#define SPACE ' '
namespace configuration
{
   template <std::size_t N>
   constexpr std::array<char, N> removeWhiteSpaces(const char (&s)[N])
   {
      std::array<char, N> res{};
      std::size_t i = 0;

      for (auto c : s)
         if (c != SPACE || c != TAB) 
            res[i++] = c;
      return res;
   }

   constexpr char * removeWhiteSpaces(const char * val)
   {
       char * temp=const_cast<char *>(val);
       std::size_t i=0,j=0;
       while (*(val+i)) 
       {
          if (*(val+i) != SPACE || *(val+i) != TAB)  //don't copy spaces
          {
             *(temp+j)=*(val+i);
             j++;i++;
          }
          else
             i++;
       }
       return temp;
   }
  
}
#endif
