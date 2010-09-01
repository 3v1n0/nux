#include "NKernel.h"

NAMESPACE_BEGIN
// From: http://www.jb.man.ac.uk/~slowe/cpp/itoa.html
//    John Maloney has pointed out various problems with the previous implementation.
//    One of the major issues was the amount of heap allocation going on.
//    He suggested that a lot of this be removed to speed up the algorithm.
//    Below are two versions based on his excellent suggestions. The char* version 
//    is at least 10 times faster than the code above. The new std::string version
//    is 3 times faster than before. Although the char* version is faster, you should
//    check that you have allocated enough space to hold the output.

// C++ version std::string style "itoa":
tstring IntegerToChar(int value, int base)
{
    enum { kMaxDigits = 35 };
    tstring buf;
    buf.reserve( kMaxDigits ); // Pre-allocate enough space.
    // check that the base if valid
    if (base < 2 || base > 16)
        return buf;
    int quotient = value;
    // Translating number to string with base:
    do
    {
        buf += TEXT("0123456789abcdef")[ std::abs( quotient % base ) ];
        quotient /= base;
    } while ( quotient );
    // Append the negative sign for base 10
    if ( value < 0 && base == 10) buf += TEXT('-');
    std::reverse( buf.begin(), buf.end() );
    return buf;
}

// C++ version char* style "itoa":
TCHAR* IntegerToChar( int value, TCHAR* result, int base )
{
    // check that the base if valid
    if (base < 2 || base > 16)
    { 
        *result = 0; 
        return result;
    }
    TCHAR* out = result;
    int quotient = value;
    do
    {
        *out = TEXT("0123456789abcdef")[ std::abs( quotient % base ) ];
        ++out;
        quotient /= base;
    } while ( quotient );
    // Only apply negative sign for base 10
    if ( value < 0 && base == 10) *out++ = TEXT('-');

    std::reverse( result, out );
    *out = 0;
    return result;
}

// convert an hexadecimal string to t_u32
t_u32 HexCharToInteger(const TCHAR* s)
{
    int n = 0;         // position in string
    int m = 0;         // position in digit[] to shift
    int count;         // loop index
    unsigned long intValue = 0;  // integer value of hex string
    int digit[16];      // hold values to convert

    const TCHAR *hexStg = s;
    if((s[0] == TEXT('0')) && ((s[1] == TEXT('X')) || (s[1] == TEXT('x'))))
    {
        hexStg = s+2;
    }

    while (n < 16)
    {
        if (hexStg[n] == TEXT('\0'))
            break;
        if (hexStg[n] > 0x29 && hexStg[n] < 0x40 ) //if 0 to 9
            digit[n] = hexStg[n] & 0x0f;            //convert to int
        else if (hexStg[n] >= TEXT('a') && hexStg[n] <= TEXT('f')) //if a to f
            digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
        else if (hexStg[n] >= TEXT('A') && hexStg[n] <= TEXT('F')) //if A to F
            digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
        else break;
        n++;
    }
    count = n;
    m = n - 1;
    n = 0;
    while(n < count)
    {
        // digit[n] is value of hex digit at position n
        // (m << 2) is the number of positions to shift
        // OR the bits into return value
        intValue = intValue | (digit[n] << (m << 2));
        m--;   // adjust the position to set
        n++;   // next digit to process
    }
    return (intValue);
}

int CharToInteger(const TCHAR* digit)
{
    int i;
    if(CharToInteger(digit, i))
    {
        return i;
    }
    nuxAssertNoEntry();
    return t_s32_max;
}

BOOL CharToInteger(const TCHAR* digit, INT& ret)
{
    INT result = 0;
    INT sign = +1;

    //--- Convert each digit char and add into result.
    t_u32 i = 0;
    if(digit[i] == TEXT('-'))
    {
        sign = -1;
        i++;
    }
    else if(digit[i] == TEXT('+'))
    {
        sign = +1;
        i++;
    }

    while(digit[i] >= TEXT('0') && digit[i] <= TEXT('9'))
    {
        result = (result * 10) + (digit[i] - TEXT('0'));
        i++;
    }

    if( digit[i] == TEXT('.'))           // test for '.' in case the characters represent a double or a float value
    {
        i++;
        while(digit[i] >= TEXT('0') && digit[i] <= TEXT('9'))
        {
            i++;
        }
    }

    const char* hello = "HelloWorld";
    bool null_char = (digit[i] == 0);
    bool space_char = (digit[i] == TEXT(' '));
    bool tab_char = (digit[i] == TEXT('\t'));
    bool win_end_of_line = (digit[i] == TEXT('\r')) && (digit[i+1] == TEXT('\n'));
    bool line_feed = (digit[i] == ('\n'));
    bool carriage_return = (digit[i] == ('\r'));
    //--- Check that there were no non-digits at end.
    if((!null_char) && (!tab_char) && (!space_char) && (!win_end_of_line) && (!line_feed) && (!carriage_return))
    {
        nuxDebugMsg(TEXT("[CharToInteger]: Unexpected character at the end of number %d in string: %s"), sign*result, digit);
    }
    ret = sign * result;
    return TRUE;
}

double CharToDouble(const TCHAR* digit)
{
    double d;
    if(CharToDouble(digit, d))
        return d;
    nuxAssertNoEntry();
    return t_f64_max;
}

BOOL CharToDouble(const TCHAR* digit, double& ret)
{
    int sign = 1;

    t_u32 i = 0;
    double result = 0;
    
    // check sign
    if (digit[i] == TEXT('-'))
    {
        sign = -1;
        i++;
    }
    else if (digit[i] == TEXT('+'))
    {
        sign = +1;
        i++;
    }

    //--- get integer portion
    while (digit[i] >= TEXT('0') && digit[i] <= TEXT('9'))
    {
        result = (result * 10) + digit[i] - TEXT('0');
        i++;
    }

    //--- get decimal point and fraction, if any.
    if (digit[i] == TEXT('.'))
    {
        digit++;
        double scale = 0.1;
        while (digit[i] >= TEXT('0') && digit[i] <= TEXT('9'))
        {
            result += (digit[i] - TEXT('0')) * scale;
            scale *= 0.1;
            i++;
        }
    }

    //--- error if we're not at the end of the number
    if((digit[i] != 0)
        && (digit[i] != TEXT(' '))
        && (digit[i] != TEXT('\t'))
#ifdef _WIN32
        // end of line on Windows
        && (digit[i] != TEXT('\r')) 
        && (digit[i+1] != TEXT('\n')) 
#else
        // end of line on Unix.
        && (digit[i] != TEXT('\n')) 
#endif
        )
    {
        // not NULL terminated or unexpected character;
        //nuxAssert(0);
        return FALSE;
    }
    ret = result * sign;
    return TRUE;
}

float CharToFloat(const TCHAR* digit)
{
    float f;
    if(CharToFloat(digit, f))
        return f;
    nuxAssertNoEntry();
    return t_f32_max;
}

BOOL CharToFloat(const TCHAR* digit, float& ret)
{
    int sign = 1;

    t_u32 i = 0;
    float result = 0.0f;

    // check sign
    if (digit[i] == TEXT('-'))
    {
        sign = -1;
        i++;
    }
    else if (digit[i] == TEXT('+'))
    {
        sign = +1;
        i++;
    }

    //--- get integer portion
    while (digit[i] >= TEXT('0') && digit[i] <= TEXT('9'))
    {
        result = (result * 10) + digit[i] - TEXT('0');
        i++;
    }

    //--- get decimal point and fraction, if any.
    if (digit[i] == TEXT('.'))
    {
        digit++;
        float scale = 0.1f;
        while (digit[i] >= TEXT('0') && digit[i] <= TEXT('9'))
        {
            result += (digit[i] - TEXT('0')) * scale;
            scale *= 0.1f;
            i++;
        }
    }

    //--- error if we're not at the end of the number
    if((digit[i] != 0)
        && (digit[i] != TEXT(' '))
        && (digit[i] != TEXT('\t'))
#ifdef _WIN32
        // end of line on Windows
        && (digit[i] != TEXT('\r')) 
        && (digit[i+1] != TEXT('\n')) 
#else
        // end of line on Unix.
        && (digit[i] != TEXT('\n')) 
#endif
        )
    {
        // not NULL terminated or unexpected character;
        //nuxAssert(0);
        return FALSE;
    }
    ret = result * sign;
    return TRUE;
}

NAMESPACE_END
