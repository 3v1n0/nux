#ifndef SYSTEMTYPES_H
#define SYSTEMTYPES_H

#include "NNamespace.h" 

NAMESPACE_BEGIN
// Note: do not use long: long is 64 bits in LP64 while it remains 32 bits on LLP64

typedef unsigned char           t_u8, t_byte, t_uchar;              // 0 to 255
typedef char                    t_char;                       // –128 to 127
// signed char is a distinct type. See ANSI C Draft Standard and the keyword "signed"
typedef signed char             t_s8, t_schar;                            // –128 to 127

typedef unsigned short          t_u16, t_uint16, t_ushort, t_word;  // 0 to 65,535
typedef short                   t_s16, t_int16, t_short;            // –32,768 to 32,767

typedef unsigned int            t_u32, t_uint32, t_uint, t_dword;   // 0 to 4,294,967,295
typedef int                     t_s32, t_int32, t_int;              // –2,147,483,648 to 2,147,483,647

typedef unsigned long           t_ulong;                            // 0 to 4,294,967,295
typedef long                    t_long;                             // –2,147,483,648 to 2,147,483,647

typedef unsigned long long      t_u64, t_uint64, t_qword, t_ulonglong;       // 0 to 18,446,744,073,709,551,615
typedef long long               t_s64, t_int64, t_sqword, t_slonglong;       // –9,223,372,036,854,775,808 to 9,223,372,036,854,775,807

typedef float                   t_float, t_f32;                     // 3.4E +/- 38 (7 digits)
typedef double                  t_double, t_f64;                    // 1.7E +/- 308 (15 digits)

typedef char                    *t_pchar;
typedef wchar_t                 t_wchar;
typedef unsigned short          t_unicodechar, *t_punicodechar;
typedef bool                    t_bool;

typedef void                    t_void, *t_pvoid;

typedef unsigned char           t_UTF8;
typedef unsigned int            t_UTF32;
#ifdef _WIN32
    typedef wchar_t             t_UTF16;
#elif defined(__linux)
    typedef wchar_t             t_UTF16;
#elif defined (__APPLE__)
    typedef wchar_t             t_UTF16;
#else
    #error t_UTF16 is undefined for this platform.
#endif

typedef size_t                  t_size;

// // HARDWARE
// #if defined _M_X64
//     typedef long long       t_integer;      // 64 bits
//     typedef long long       *t_pointer;     // 64 bits
//     const   long long       t_MaxInteger = 0xFFFFFFFFFFFFFFFF;
//     typedef long long       t_saddress;
//     typedef unsigned long long  t_uaddress;
// // #elif defined _WIN32
// //     typedef long           t_integer;   // 32 bits
// //     typedef long           *t_pointer;   // 32 bits
// //     const long t_MaxInteger = 0xFFFFFFFF;     
// //     typedef long            t_saddress;
// //     typedef unsigned long   t_uaddress;
// #elif defined(__linux) && (defined(__ia64__) || defined(__amd64__))
//     typedef long long       t_integer;      // 64 bits
//     typedef long long       *t_pointer;     // 64 bits
//     const   long long       t_MaxInteger = 0xFFFFFFFFFFFFFFFF;
// #elif defined __linux 
//     typedef long            t_integer;      // 32 bits
//     typedef long            *t_pointer;     // 32 bits
//     const   long            t_MaxInteger = 0xFFFFFFFF;
// #elif defined __APPLE__
//     typedef long long       t_integer;      // 64 bits
//     typedef long long       *t_pointer;     // 64 bits
//     const   long long       t_MaxInteger = 0xFFFFFFFFFFFFFFFF;
// #elif defined INL_PS3
//     typedef long            t_integer;      // 32 bits
//     typedef long            *t_pointer;     // 32 bits
//     const   long            t_MaxInteger = 0xFFFFFFFF;
// #endif




// Compile Time
#if defined(__LP64__) || defined(_LP64) || defined(_WIN64)
    typedef long long           t_saddress;
    typedef unsigned long long  t_uaddress;
    typedef t_u64               INL_POINTER;
    typedef long long           t_integer;
#else
    typedef int                 t_saddress;
    typedef unsigned int        t_uaddress;
    typedef t_uint32            INL_POINTER;
    typedef long                t_integer;
#endif

typedef unsigned long            t_addressu32;
typedef unsigned long long       t_addressu64;

#define INL_ADDRESS_TO_PTR(addr) (void*)(addr)
#define INL_PTR_TO_ADDRESS(ptr)  (t_uaddress)(ptr)

const t_s8  t_s8_min          =  -128;
const t_s8  t_s8_max          =  127;
const t_u8  t_u8_min          =  0;
const t_u8  t_u8_max          =  255;
const t_s16 t_s16_min         =  -32768;
const t_s16 t_s16_max         =  32767;
const t_u16 t_u16_min         =  0;
const t_u16 t_u16_max         =  65535;
const t_s32 t_s32_min         =  0x80000000;
const t_s32 t_s32_max         =  0x7FFFFFFF;
const t_u32 t_u32_min         =  0x00000000;
const t_u32 t_u32_max         =  0xFFFFFFFF;
const t_s64 t_s64_min         =  0x8000000000000000LL;
const t_s64 t_s64_max         =  0x7FFFFFFFFFFFFFFFLL;
const t_u64 t_u64_min         =  0x0000000000000000ULL;
const t_u64 t_u64_max         =  0xFFFFFFFFFFFFFFFFULL;

const t_f32 t_f32_min       =  1.175494351E-38F;
const t_f32 t_f32_max     	=  3.402823466E+38F;
const t_f64 t_f64_min       =  2.2250738585072014E-308;
const t_f64 t_f64_max     	=  1.7976931348623158E+308;

const t_f32 MinFloat        =  1.175494351E-38F;
const t_f32 MaxFloat     	=  3.402823466E+38F;
const t_f64 MinDouble       =  2.2250738585072014E-308;
const t_f64 MaxDouble     	=  1.7976931348623158E+308;

//             ILP32	LP64	LLP64	ILP64
// char	        8       8       8       8
// short	    16	    16      16      16
// int	        32	    32      32      64
// long	        32      64      32      64
// long long	64	    64      64      64
// pointer	    32	    64      64      64

// Window 64-bit supports LLP64
// Linux 64 bit  supports LP64 
// Mac OS 64 bit  supports LP64 

// The size of  a long integer in particular depends on the operating system and the targeted architecture as follows:
// 
// OS           arch           size
// Windows       IA-32        4 bytes
// Windows       Intel 64     4 bytes
// Windows       IA-64        4 bytes
// Linux         IA-32        4 bytes
// Linux         Intel 64     8 bytes
// Linux         IA-64        8 bytes
// Mac OS X      IA-32        4 bytes
// Mac OS X      Intel 64     8 bytes  

#ifdef _WIN32
    #define INL_PTR_TO_INT(ptr) (long)((long)(ptr))
#elif  _WIN64
    #define INL_PTR_TO_INT(ptr) (long long)((long long)(ptr))
#elif INL_PS3
    #define INL_PTR_TO_INT(ptr) (long)((long)(ptr))
#elif __linux
    #define INL_PTR_TO_INT(ptr) (long)((long)(ptr))
#elif defined(__linux) && (defined(__ia64__) || defined(__amd64__))
    #define INL_PTR_TO_INT(ptr) (long long)((long long)(ptr))
#elif __APPLE__
    #define INL_PTR_TO_INT(ptr) (long long)((long long)(ptr))
#endif

#define INL_BYTE_SIZE   1
#define INL_WORD_SIZE   2
#define INL_FLOAT_SIZE  4
#define INL_INT_SIZE    4
#define INL_DOUBLE_SIZE 8

// Polymorphic Types
#if defined(_WIN32) || defined(_WIN64)
    #define INL_PTRSIZE_INT     INT_PTR
    #define INL_PTRSIZE_UINT    UINT_PTR
    #define INL_PTRSIZE_DWORD   DWORD_PTR
    #define INL_PTRSIZE_LONG    LONG_PTR
    #define INL_PTRSIZE_ULONG   ULONG_PTR
    #define INL_SIZE_T          SIZE_T
    #define INL_SSIZE_T         SSIZE_T
#else
    #define INL_PTRSIZE_INT     int
    #define INL_PTRSIZE_UINT    unsigned int
    #define INL_PTRSIZE_DWORD   unsigned int
    #define INL_PTRSIZE_LONG    long
    #define INL_PTRSIZE_ULONG   unsigned long
    #define INL_SIZE_T          size_t
    #define INL_SSIZE_T         int
#endif


// DirectX D3DFormat
//    All formats are listed from left to right, most significant bit (MSB) to least significant bit (LSB). For example, 
//    D3DFORMAT_ARGB is ordered from the MSB channel A (alpha), to the LSB channel B (blue). When traversing surface data,
//    the data is stored in memory from LSB to MSB, which means that the channel order in memory is from LSB (blue) to MSB (alpha).
//
//    The default value for formats that contain undefined channels (G16R16, A8, and so on) is 1. The only exception is the
//    A8 format, which is initialized to 000 for the three color channels.
//
//    The order of the bits is from the most significant byte first, so D3DFMT_A8L8 indicates that the high byte of this 2-byte
//    format is alpha. D3DFMT_D16 indicates a 16-bit integer value and an application-lockable surface. 
//
//    Pixel formats have been chosen to enable the expression of hardware-vendor-defined extension formats, as well as to include
//    the well-established four-character code (FOURCC) method. The set of formats understood by the Microsoft Direct3D runtime 
//    is defined by D3DFORMAT.
//
//


//Format of RGBA colors is
//7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|    alpha        |      red    |     green      |     blue     |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//MSB 31                                                             0 LSB


#define RGBA_GET_ALPHA(rgba)      ((rgba) >> 24)
#define RGBA_GET_RED(rgba)        (((rgba) >> 16) & 0xff)
#define RGBA_GET_GREEN(rgba)      (((rgba) >> 8) & 0xff)
#define RGBA_GET_BLUE(rgba)       ((rgba) & 0xff)
#define RGBA_MAKE(r, g, b, a)     ((R8G8B8A8) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))

//
// RGB and RGBA may be used as initialisers for D3DCOLORs
// The t_float values must be in the range 0..1
//

#define INL_RGB(r, g, b)                                   \
                            ( (t_u32) (BYTE) (b) << 16) |   \
                            ( (t_u32) (BYTE) (g) << 8)  |   \
                            ( (t_u32) (BYTE) (r) )

#define INL_RGBA(r, g, b, a)                                    \
                            ( (t_u32) (BYTE) (a)) << 24) |   \
                            ( (t_u32) (BYTE) (b)) << 16) |   \
                            ( (t_u32) (BYTE) (g)  << 8)  |   \
                            ( (t_u32) (BYTE) (r) )


//Format of RGB colors is
//7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|   ignored     |      red      |     green     |     blue      |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//Format of BGR colors is
//7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|   ignored     |      blue     |     green     |      red      |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//Format of RGBA colors is
//7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|      red      |      green    |     blue      |     alpha     |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

//Format of BGRA colors is
//7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|      blue      |      green    |     red      |     alpha     |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


#define RGB_GET_RED(rgb)         (((rgb) >> 16) & 0xff)
#define RGB_GET_GREEN(rgb)       (((rgb) >> 8) & 0xff)
#define RGB_GET_BLUE(rgb)        ((rgb) & 0xff)
#define RGB_MAKE(r, g, b)        ((R8G8B8) (((r) << 16) | ((g) << 8) | (b)))
#define RGBA_SET_ALPHA(rgba, x)  (((x) << 24) | ((rgba) & 0x00ffffff))
#define RGBA_TO_RGB(rgba)        ((R8G8B8) (((rgba) >> 8)) & 0xffffff))
#define RGB_TO_RGBA(rgb)         ((R8G8B8A8) (((rgb) << 8) | 0x000000ff))


// #ifdef _WIN32
//     #define INL_HARDWARE_BREAK __asm {int 3}
//     #define INL_BREAK_ASM_INT3 __asm {int 3}
// #elif (defined INL_PS3)
//     #define INL_HARDWARE_BREAK
//     #define INL_BREAK_ASM_INT3
// #else
//     #define INL_HARDWARE_BREAK
//     #define INL_BREAK_ASM_INT3
// #endif

#define DEGTORAD(d) (d) * 3.1415927f / 180.0f
#define RADTODEG(d) (d) * 180.0f / 3.1415927f 


NAMESPACE_END
#endif // SYSTEMTYPES_H

/*

 C/C++ recognizes the types shown in the table below.

Type Name               Bytes               Other Names				Range of Values 
________________________________________________________________________________________________________________________________
int                     *                   signed,
                                            signed int                          System dependent 

unsigned int            *                   unsigned                            System dependent 

__int8                  1                   char,                               
                                            signed char                         -128 to 127 

__int16                 2                   short, 
                                            short int, 
                                            signed short int                    -32768 to 32767

__int32                 4                   signed, 
                                            signed int                          -2,147,483,648 to 2,147,483,647 

__int64                 8                   none                                -9,223,372,036,854,775,808 
                                                                                 to 9,223,372,036,854,775,807 

char                    1                   signed char                         -128 to 127

unsigned char           1                   none                                0 to 255 

short                   2                   short int,
                                            signed short int                    -32,768 to 32,767 

unsigned short          2                   unsigned short int                  0 to 65,535 

long                    4                   long int,
                                            signed long int                     -2,147,483,648 to 2,147,483,647 

unsigned long           4                   unsigned long int                   0 to 4,294,967,295 

enum                    *                   none                                Same as int 

float                   4                   none                                3.4E +/- 38 (7 digits) 

double                  8                   none                                1.7E +/- 308 (15 digits) 

long double             10                  none                                1.2E +/- 4932 (19 digits) 



*: OS dependant

*/


//http://www.mplayerhq.hu/DOCS/tech/colorspaces.txt
//
//In general
//==========
//
//There are planar and packed modes.
//- Planar mode means: You have 3 separate images, one for each component,
//each image 8 bits/pixel. To get the real colored pixel, you have to
//mix the components from all planes. The resolution of planes may differ!
//- Packed mode means: you have all components mixed/interleaved together,
//so you have small "packs" of components in a single, big image.
//
//There are RGB and YUV colorspaces.
//- RGB: Red, Green and Blue components. Used by analog VGA monitors.
//- YUV: Luminance (Y) and Chrominance (U,V) components. Used by some
//video systems, like PAL. Also most M(J)PEG/DCT based codecs use this.
//
//With YUV, they used to reduce the resolution of U,V planes:
//The most common YUV formats:
//FOURCC:    bpp: IEEE:      plane sizes: (w=width h=height of original image)
//               444P       24   YUV 4:4:4  Y: w * h  U,V: w * h
//               YUY2,UYVY  16   YUV 4:2:2  Y: w * h  U,V: (w/2) * h      [MJPEG]
//               YV12,I420  12   YUV 4:2:0  Y: w * h  U,V: (w/2) * (h/2)  [MPEG, H.263]
//               411P       12   YUV 4:1:1  Y: w * h  U,V: (w/4) * h      [DV-NTSC, CYUV]
//               YVU9,IF09   9   YUV 4:1:0  Y: w * h  U,V: (w/4) * (h/4)  [Sorenson, Indeo]
//
//               The YUV a:b:c naming style means: for <a> samples of Y there are <b> samples
//                   of UV in odd lines and <c> samples of UV in even lines.
//
//conversion: (some cut'n'paste from www and maillist)
//
//            RGB to YUV Conversion:
//               Y  =      (0.257 * R) + (0.504 * G) + (0.098 * B) + 16
//                   Cr = V =  (0.439 * R) - (0.368 * G) - (0.071 * B) + 128
//                   Cb = U = -(0.148 * R) - (0.291 * G) + (0.439 * B) + 128
//                   YUV to RGB Conversion:
//               B = 1.164(Y - 16)                  + 2.018(U - 128)
//                   G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
//                   R = 1.164(Y - 16) + 1.596(V - 128)
//
//                   In both these cases, you have to clamp the output values to keep them in
//                   the [0-255] range. Rumour has it that the valid range is actually a subset
//                   of [0-255] (I've seen an RGB range of [16-235] mentioned) but clamping the
//                   values into [0-255] seems to produce acceptable results to me.
//
//                   Julien (sorry, I can't recall his surname) suggests that there are
//                   problems with the above formula and proposes the following instead:
//               Y  = 0.299R + 0.587G + 0.114B
//                   Cb = U'= (B-Y)*0.565
//                   Cr = V'= (R-Y)*0.713
//                   with reciprocal versions:
//               R = Y + 1.403V'
//                   G = Y - 0.344U' - 0.714V'
//                   B = Y + 1.770U'
//Note: This formula doesn't contain the +128 offsets of U,V values!
//
//Conclusion:
//               Y = luminance, the weighted average of R G B components. (0=black 255=white)
//                   U = Cb = blue component (0=green 128=grey 255=blue)
//                   V = Cr = red component  (0=green 128=grey 255=red)
//
//
//                   Huh. The planar YUV modes.
//                   ==========================
//
//                   The most misunderstood thingie...
//
//                   In MPlayer, we usually have 3 pointers to the Y, U and V planes, so it
//                   doesn't matter what the order of the planes in the memory is:
//                   for mp_image_t and libvo's draw_slice():
//                       planes[0] = Y = luminance
//                       planes[1] = U = Cb = blue
//                       planes[2] = V = Cr = red
//Note: planes[1] is ALWAYS U, and planes[2] is V, the FOURCC
//      (YV12 vs. I420) doesn't matter here! So, every codec using 3 pointers
//      (not only the first one) normally supports YV12 and I420 (=IYUV), too!
//
//      But there are some codecs (VfW, dshow) and vo drivers (xv) ignoring the 2nd
//      and 3rd pointer that use only a single pointer to the planar YUV image. In
//      this case we must know the right order and alignment of planes in the memory!
//
//      from the webartz fourcc list:
//YV12:  12 bpp, full sized Y plane followed by 2x2 subsampled V and U planes
//I420:  12 bpp, full sized Y plane followed by 2x2 subsampled U and V planes
//IYUV:  the same as I420
//YVU9:   9 bpp, full sized Y plane followed by 4x4 subsampled V and U planes
//
//        Huh 2. RGB vs. BGR ?
//        ====================
//
//        The 2nd most misunderstood thingie...
//
//        You know, there are Intel and Motorola, and they use different byteorder.
//        There are also others, like MIPS or Alpha, but all follow either Intel
//        or Motorola byteorder.
//        Unfortunately, the packed colorspaces depend on CPU byteorder. So, RGB
//        on Intel and Motorola means different order of bytes.
//
//        In MPlayer, we have constants IMGFMT_RGBxx and IMGFMT_BGRxx.
//        Unfortunately, some codecs and vo drivers follow Intel, some follow Motorola
//        byteorder, so they are incompatible. We had to find a stable base, so long
//        time ago I've chosen OpenGL, as it's a wide-spreaded standard, and it well
//        defines what RGB is and what BGR is. So, MPlayer's RGB is compatible with
//        OpenGL's GL_RGB on all platforms, and the same goes for BGR - GL_BGR.
//        Unfortunately, most of the x86 codecs call our BGR RGB, so it sometimes
//        confuses developers.
//
//        memory order:           name
//        lowest address .. highest address
//        RGBA                    IMGFMT_RGBA
//        ARGB                    IMGFMT_ARGB
//        BGRA                    IMGFMT_BGRA
//        ABGR                    IMGFMT_ABGR
//        RGB                     IMGFMT_RGB24
//        BGR                     IMGFMT_BGR24
//
//        order in an int         name
//        most significant .. least significant bit
//        8A8R8G8B                IMGFMT_BGR32
//        8A8B8G8R                IMGFMT_RGB32
//        5R6G5B                  IMGFMT_BGR16
//        5B6G5R                  IMGFMT_RGB16
//        1A5R5G5B                IMGFMT_BGR15
//        1A5B5G5R                IMGFMT_RGB15
//
//        The following are palettized formats, the palette is in the second plane.
//        When they come out of the swscaler (this can be different when they
//        come from a codec!), their palette is organized in such a way
//        that you actually get:
//
//       3R3G2B                  IMGFMT_BGR8
//           2B3G3R                  IMGFMT_RGB8
//           1R2G1B                  IMGFMT_BGR4_CHAR
//           1B2G1R                  IMGFMT_RGB4_CHAR
//           1R2G1B1R2G1B            IMGFMT_BGR4
//           1B2G1R1B2G1R            IMGFMT_RGB4
//
//           Depending upon the CPU being little- or big-endian, different 'in memory' and
//           'in register' formats will be equal (LE -> BGRA == BGR32 / BE -> ARGB == BGR32)
//

