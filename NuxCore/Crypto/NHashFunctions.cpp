#include "NKernel.h"

NAMESPACE_BEGIN

unsigned int RSHash(const NString& str)
{
   unsigned int b    = 378551;
   unsigned int a    = 63689;
   unsigned int hash = 0;

   for(std::size_t i = 0; i < str.Length(); i++)
   {
      hash = hash * a + str[i];
      a    = a * b;
   }

   return hash;
}
/* End Of RS Hash Function */


unsigned int JSHash(const NString& str)
{
   unsigned int hash = 1315423911;

   for(std::size_t i = 0; i < str.Length(); i++)
   {
      hash ^= ((hash << 5) + str[i] + (hash >> 2));
   }

   return hash;
}
/* End Of JS Hash Function */


unsigned int PJWHash(const NString& str)
{
   unsigned int BitsInUnsignedInt = (unsigned int)(sizeof(unsigned int) * 8);
   unsigned int ThreeQuarters     = (unsigned int)((BitsInUnsignedInt  * 3) / 4);
   unsigned int OneEighth         = (unsigned int)(BitsInUnsignedInt / 8);
   unsigned int HighBits          = (unsigned int)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
   unsigned int hash              = 0;
   unsigned int test              = 0;

   for(std::size_t i = 0; i < str.Length(); i++)
   {
      hash = (hash << OneEighth) + str[i];

      if((test = hash & HighBits)  != 0)
      {
         hash = (( hash ^ (test >> ThreeQuarters)) & (~HighBits));
      }
   }

   return hash;
}
/* End Of  P. J. Weinberger Hash Function */


unsigned int ELFHash(const NString& str)
{
   unsigned int hash = 0;
   unsigned int x    = 0;

   for(std::size_t i = 0; i < str.Length(); i++)
   {
      hash = (hash << 4) + str[i];
      if((x = hash & 0xF0000000L) != 0)
      {
         hash ^= (x >> 24);
      }
      hash &= ~x;
   }

   return hash;
}
/* End Of ELF Hash Function */


unsigned int BKDRHash(const NString& str)
{
   unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
   unsigned int hash = 0;

   for(std::size_t i = 0; i < str.Length(); i++)
   {
      hash = (hash * seed) + str[i];
   }

   return hash;
}
/* End Of BKDR Hash Function */


unsigned int SDBMHash(const NString& str)
{
   unsigned int hash = 0;

   for(std::size_t i = 0; i < str.Length(); i++)
   {
      hash = str[i] + (hash << 6) + (hash << 16) - hash;
   }

   return hash;
}
/* End Of SDBM Hash Function */


unsigned int DJBHash(const NString& str)
{
   unsigned int hash = 5381;

   for(std::size_t i = 0; i < str.Length(); i++)
   {
      hash = ((hash << 5) + hash) + str[i];
   }

   return hash;
}
/* End Of DJB Hash Function */


unsigned int DEKHash(const NString& str)
{
   unsigned int hash = static_cast<unsigned int>(str.Length());

   for(std::size_t i = 0; i < str.Length(); i++)
   {
      hash = ((hash << 5) ^ (hash >> 27)) ^ str[i];
   }

   return hash;
}
/* End Of DEK Hash Function */


unsigned int BPHash(const NString& str)
{
   unsigned int hash = 0;
   for(std::size_t i = 0; i < str.Length(); i++)
   {
      hash = hash << 7 ^ str[i];
   }

   return hash;
}
/* End Of BP Hash Function */


unsigned int FNVHash(const NString& str)
{
   const unsigned int fnv_prime = 0x811C9DC5;
   unsigned int hash = 0;
   for(std::size_t i = 0; i < str.Length(); i++)
   {
      hash *= fnv_prime;
      hash ^= str[i];
   }

   return hash;
}
/* End Of FNV Hash Function */


unsigned int APHash(const NString& str)
{
   unsigned int hash = 0xAAAAAAAA;

   for(std::size_t i = 0; i < str.Length(); i++)
   {
      hash ^= ((i & 1) == 0) ? (  (hash <<  7) ^ str[i] * (hash >> 3)) :
                               (~((hash << 11) + str[i] ^ (hash >> 5)));
   }

   return hash;
}
/* End Of AP Hash Function */

NAMESPACE_END
