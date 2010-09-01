/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranties of 
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the applicable version of the GNU Lesser General Public 
 * License for more details.
 * 
 * You should have received a copy of both the GNU Lesser General Public 
 * License version 3 along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef MATHUTILITY_H
#define MATHUTILITY_H

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cmath>
#include <cstring>
#include <ctime>

#include "Constants.h"

NAMESPACE_BEGIN
    template< class T > inline T Max3( const T A, const T B, const T C )
    {
        return Max ( Max( A, B ), C );
    }

    template< class T > inline T Min3( const T A, const T B, const T C )
    {
        return Min ( Min( A, B ), C );
    }

    template< class T > inline T Square( const T A )
    {
        return A*A;
    }
    template< class T > inline T Clamp( const T X, const T Min, const T Max )
    {
        return X<Min ? Min : X<Max ? X : Max;
    }

    template< class T > inline T ClampUp( const T X, const T Min, const T Max )
    {
        return X<Min ? Min : X;
    }

    template< class T > inline T ClampDown( const T X, const T Max )
    {
        return X>Max ? Max : X;
    }

    template< class T > inline T Align( const T Ptr, int Alignment )
    {
        return (T)(((INL_POINTER)Ptr + Alignment - 1) & ~(Alignment-1));
    }

    //Bitwise rotation on the left. 
    template<typename T> inline const T Rol(const T& a,const unsigned int n=1) { return (a<<n)|(a>>((sizeof(T)<<3)-n)); }
    //Bitwise rotation on the right.
    template<typename T> inline const T Ror(const T& a,const unsigned int n=1) { return (a>>n)|(a<<((sizeof(T)<<3)-n)); }

    // Exchange the values of variables a and b
    template<typename T> inline void Swap(T& a,T& b) { const T t=a; a=b; b=t; }
    template<typename T> inline void Swap(T& a1,T& b1,T& a2,T& b2) { Swap(a1,b1); Swap(a2,b2); }
    template<typename T> inline void Swap(T& a1,T& b1,T& a2,T& b2,T& a3,T& b3) { Swap(a1,b1,a2,b2); Swap(a3,b3); }
    template<typename T> inline void Swap(T& a1,T& b1,T& a2,T& b2,T& a3,T& b3,T& a4,T& b4) { Swap(a1,b1,a2,b2,a3,b3); Swap(a4,b4); }
    template<typename T> inline void Swap(T& a1,T& b1,T& a2,T& b2,T& a3,T& b3,T& a4,T& b4,T& a5,T& b5)
    {
        Swap(a1,b1,a2,b2,a3,b3,a4,b4); Swap(a5,b5); 
    }
    template<typename T> inline void Swap(T& a1,T& b1,T& a2,T& b2,T& a3,T& b3,T& a4,T& b4,T& a5,T& b5,T& a6,T& b6)
    {
        Swap(a1,b1,a2,b2,a3,b3,a4,b4,a5,b5); Swap(a6,b6);
    }

    //Return the absolute value of a. 
    template<typename T> inline const T Abs(const T& a) { return a>=0?a:-a; }
    //Return the minimum between a and b. 
    template<typename T> inline const T& Min(const T& a,const T& b) { return a<=b?a:b; }
    //Return the minimum between a, b and c. 
    template<typename T> inline const T& Min(const T& a,const T& b,const T& c) { return min(min(a,b),c); }
    //Return the minimum between a, b, c and d.
    template<typename T> inline const T& Min(const T& a,const T& b,const T& c,const T& d) { return min(min(min(a,b),c),d); }
    //Return the maximum between a and b. 
    template<typename T> inline const T& Max(const T& a,const T& b) { return a>=b?a:b; }
    //Return the maximum between a, b and c. 
    template<typename T> inline const T& Max(const T& a,const T& b,const T& c) { return max(max(a,b),c); }
    //Return the maximum between a,b,c and d. 
    template<typename T> inline const T& Max(const T& a,const T& b,const T& c,const T& d) { return max(max(a,b,c),d); }
    //Return the sign of x. 
    template<typename T> inline T Sign(const T& x) { return (x<0)?-1:(x==0?0:1); }


    template<typename T> inline T Modulo(const T& x,const T& m) { return x-m*(T)std::floor((double)x/m); }

    inline int ModuloInt(const int x,const int m) { return x>=0?x%m:(x%m?m+x%m:0); }

    template<typename T> inline T MinMod(const T& a,const T& b) { return a*b<=0?0:(a>0?(a<b?a:b):(a<b?b:a)); }

    //Return a random variable between [0,1] (uniform distribution). 
    inline double Random() { return (double)std::rand()/RAND_MAX; }

    //Return a random variable between [-1,1] (uniform distribution). 
    inline double CRandom() { return 1-2*std::rand(); }

    //Return a random variable using a gaussian distribution and a variance of 1. 
    inline double RandomGaussian() { return std::sqrt(-2*std::log((double)(1e-10 + (1-2e-10)*std::rand())))*std::cos((double)(2*3.14f/*Const::pi*/*std::rand())); }

    inline unsigned int RandomUInt() { return std::rand(); }

    inline unsigned int RandomUInt(unsigned int max_random) { return std::rand() % max_random; }

    inline t_size DiffPointer(void* Ptr0, void* Ptr1) {if((t_size)Ptr0 >= (t_size)Ptr1) return (t_size)((t_size)Ptr0 - (t_size)Ptr1); return (t_size)((t_size)Ptr1 - (t_size)Ptr0);}
    // Dangerous to use!
    template<typename T> inline T SubstractPointer(void* Ptr, t_size Value) {return (T)(((t_size)Ptr) - Value);}
    template<typename T> inline T AddPointer(void* Ptr, t_size Value) {return (T)(((t_size)Ptr) + Value);}

    //! Round up to the nearest multiple of Alignment that is greater or equal to Value
    /*!
        @param Alignment Must be a power of 2
    */
    template<typename T> inline T RoundUp(T Value, int Alignment)
    {   
        return (Value + (Alignment-1)) & ~(Alignment-1);
    }

    //! Round down to the nearest multiple of Alignment that is smaller or equal to Value
    /*!
        @param Alignment Must be a power of 2
    */
    template<typename T> inline T RoundDown(T Value, int Alignment)
    {   
        return ((Value) & ~(Alignment-1));
    }

    //! Return true is Value is aligned on Alignment
    /*!
        @param Alignment Must be a power of 2
    */
    template<typename T> inline bool IsAligned(T Value, int Alignment)
    {   
        return (((Value) & ~(Alignment-1)) == 0);
    }

    /*!
        Revert Byte order
        0x0011 -> 0x1100
    */
    inline t_u16 ReverseByteOrdering(t_u16 value)
    {
        t_u16 temp;
        t_u8 * src = (t_u8 *) &value;
        t_u8 * dest = (t_u8 *) &temp;

        dest[0] = src[1];
        dest[1] = src[0];

        return temp;
    }

    /*!
        Revert Byte order
        0x00112233 -> 0x33221100
    */
    inline t_u32 ReverseByteOrdering(t_u32 value)
    {
        t_u32 temp;
        t_u8 * src = (t_u8 *) &value;
        t_u8 * dest = (t_u8 *) &temp;

        dest[0] = src[3];
        dest[1] = src[2];
        dest[2] = src[1];
        dest[3] = src[0];

        return temp;
    }

    /*!
        Revert Byte order
        0x0011223344556677 -> 0x7766554433221100
    */
    inline t_u64 ReverseByteOrdering(t_u64 value)
    {
        t_u64 temp;
        t_u8 * src = (t_u8 *) &value;
        t_u8 * dest = (t_u8 *) &temp;

        dest[0] = src[7];
        dest[1] = src[6];
        dest[2] = src[5];
        dest[3] = src[4];
        dest[4] = src[3];
        dest[5] = src[2];
        dest[6] = src[1];
        dest[7] = src[0];

        return temp;
    }

    // Bit Hack

    // Determining if an integer is a power of 2
    // http://graphics.stanford.edu/~seander/bithacks.html
    inline bool IsPowerOf2(unsigned int n)
    {
        // The algorithm does not 0 consider 0 a power of two. (this is right)
        return !(n & (n - 1)) && n;
    }

    // Compute the next highest power of 2 of 32-bit v
    // http://graphics.stanford.edu/~seander/bithacks.html
    inline unsigned int NextPowerOfTwo(unsigned int x)
    {
        x = x - 1;
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >> 16);
        return x + 1;
    }

    inline unsigned int GetLowerPowerOfTwoExponent(unsigned int x)
    {
        int count = 0;
        while(x > 1)
        {
            x >>= 1;
            count++;
        }
        return count;
    }

    inline unsigned int PowerOfTwo(int i)
    {
        int e = 0;
        unsigned int power = 1;
        while(e < i)
        {
            power = power << 1;
            e++;
        }
        return power;
    }

    // ClearLSBBit(0x01001100) = 0x01001000
    inline unsigned int Hak32_ClearLSBBit(unsigned int N)
    {
        return N &(N-1);
    }

    // http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan
    // Hak32_CountNumBits(0x01001100) = 3
    inline unsigned int Hak32_CountNumBits(unsigned int N)
    {
        unsigned int v = N; // count the number of bits set in v
        unsigned int c; // c accumulates the total bits set in v
        for(c = 0; v; c++)
        {
            v &= v - 1; // clear the least significant bit set
        }
        return c;
    }
    
    // http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan : Compute parity in parallel
    inline unsigned int Hak32_BitParity(unsigned int N)
    {
        unsigned int v = N;  // word value to compute the parity of
        v ^= v >> 16;
        v ^= v >> 8;
        v ^= v >> 4;
        v &= 0xf;
        return (0x6996 >> v) & 1;
    }

    #define HAK32_SWAP(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

    // Return true if the CPU is little endian
    inline bool Hak32_CPULittleEndian()
    {
        const int x=1;
        return ((unsigned char*)&x)[0]? true : false;
    }

    // http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious
    // Find the log base 10 of an N-bit integer in O(lg(N))
    inline unsigned int Hak32_Log2(unsigned int N)
    {
        unsigned int v = N; // find the log base 2 of 32-bit v
        int r;          // result goes here

        static const int MultiplyDeBruijnBitPosition[32] = 
        {
            0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
            31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
        };

        v |= v >> 1; // first round down to power of 2 
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v = (v >> 1) + 1;

        r = MultiplyDeBruijnBitPosition[static_cast<unsigned int>(v * 0x077CB531UL) >> 27];
        return r;
    }

    // http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious
    // Find the log base 2 of an N-bit integer in O(lg(N))
    inline unsigned int Hak32_Log10(unsigned int N)
    {
      unsigned int v = N; // non-zero 32-bit integer value to compute the log base 10 of 
      int r;          // result goes here
      int t;          // temporary

      static unsigned int const PowersOf10[] = 
      {1, 10, 100, 1000, 10000, 100000,
      1000000, 10000000, 100000000, 1000000000};

      t = (Hak32_Log2(v) + 1) * 1233 >> 12; // (use a lg2 method from above)
      r = t - (v < PowersOf10[t]);

      return r;
    }

    // http://graphics.stanford.edu/~seander/bithacks.html
    // Count the consecutive zero bits (trailing) on the right by binary search 
    inline unsigned int Hack32_TrailingZeroRight(unsigned int N)
    {
        unsigned int v = N;     // 32-bit word input to count zero bits on right
        unsigned int c;     // c will be the number of zero bits on the right,
        // so if v is 1101000 (base 2), then c will be 3
        // NOTE: if 0 == v, then c = 31.
        if (v & 0x1) 
        {
            // special case for odd v (assumed to happen half of the time)
            c = 0;
        }
        else
        {
            c = 1;
            if ((v & 0xffff) == 0) 
            {  
                v >>= 16;  
                c += 16;
            }
            if ((v & 0xff) == 0) 
            {  
                v >>= 8;  
                c += 8;
            }
            if ((v & 0xf) == 0) 
            {  
                v >>= 4;
                c += 4;
            }
            if ((v & 0x3) == 0) 
            {  
                v >>= 2;
                c += 2;
            }
            c -= v & 0x1;
        }	
        return c;
    }

NAMESPACE_END

#endif // MATHUTILITY_H
