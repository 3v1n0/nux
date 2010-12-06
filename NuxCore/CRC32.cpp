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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "NuxCore.h"
#include "CRC32.h"


namespace nux
{
// The constants here are for the CRC-32 generator
// polynomial, as defined in the Microsoft
// Systems Journal, March 1995, pp. 107-108

  const unsigned int CRCTable [256] =
  {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
    0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
    0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
    0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
    0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
    0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,

    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
    0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
    0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
    0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
    0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
    0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
    0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,

    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
    0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
    0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
    0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
    0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
    0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,

    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
    0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
    0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
    0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
    0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
    0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
  };

  CRC32::CRC32()
  {
    Initialize();
  }

  void CRC32::Initialize (void)
  {
    Memset (&CRCTable, 0, sizeof (CRCTable) );

    // 256 values representing ASCII character codes.
    for (int iCodes = 0; iCodes <= 0xFF; iCodes++)
    {
      CRCTable[iCodes] = Reflect (iCodes, 8) << 24;

      for (int iPos = 0; iPos < 8; iPos++)
      {
        CRCTable[iCodes] = (CRCTable[iCodes] << 1) ^ (CRCTable[iCodes] & (1 << 31) ? CRC32_POLYNOMIAL : 0);
      }

      CRCTable[iCodes] = Reflect (CRCTable[iCodes], 32);
    }
  }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Reflection is a requirement for the official CRC-32 standard.
//	You can create CRCs without it, but they won't conform to the standard.
  t_u32 CRC32::Reflect (t_u32 ulReflect, char cChar)
  {
    t_u32 ulValue = 0;

    // Swap bit 0 for bit 7 bit 1 For bit 6, etc....
    for (int iPos = 1; iPos < (cChar + 1); iPos++)
    {
      if (ulReflect & 1)
        ulValue |= 1 << (cChar - iPos);

      ulReflect >>= 1;
    }

    return ulValue;
  }

  t_u32 CRC32::FileCRC (const char *sFileName)
  {
    t_u32 ulCRC = 0xffffffff;

    FILE *fSource = NULL;
    char sBuf[CRC32BUFSZ];
    t_u32 iBytesRead = 0;

#ifdef WIN32_SECURE

    if (FOPEN_S (&fSource, sFileName, "rb") != 0)
#else
    if (FOPEN_S (fSource, sFileName, "rb") != 0)
#endif
    {
      return 0xffffffff;
    }

    do
    {
      iBytesRead = (t_u32) fread (sBuf, sizeof (char), CRC32BUFSZ, fSource);
      PartialCRC (&ulCRC, sBuf, iBytesRead);
    }
    while (iBytesRead == CRC32BUFSZ);

    fclose (fSource);

    return (ulCRC ^ 0xffffffff);
  }

// This function uses the CRCTable lookup table to generate a CRC for sData
  t_u32 CRC32::FullCRC (const char *sData, t_u32 ulLength)
  {
    t_u32 ulCRC = 0xffffffff;
    PartialCRC (&ulCRC, sData, ulLength);
    return ulCRC ^ 0xffffffff;
  }

// Perform the algorithm on each character
// in the string, using the lookup table values.
  void CRC32::PartialCRC (t_u32 *ulInCRC, const char *sData, t_u32 ulLength)
  {
    while (ulLength--)
    {
      *ulInCRC = (*ulInCRC >> 8) ^ CRCTable[ (*ulInCRC & 0xFF) ^ *sData++];
    }
  }



  /*
  * A brief CRC tutorial.
  *
  * A CRC is a long-division remainder.  You add the CRC to the message,
  * and the whole thing (message+CRC) is a multiple of the given
  * CRC polynomial.  To check the CRC, you can either check that the
  * CRC matches the recomputed value, *or* you can check that the
  * remainder computed on the message+CRC is 0.  This latter approach
  * is used by a lot of hardware implementations, and is why so many
  * protocols put the end-of-frame flag after the CRC.
  *
  * It's actually the same long division you learned in school, except that
  * - We're working in binary, so the digits are only 0 and 1, and
  * - When dividing polynomials, there are no carries.  Rather than add and
  *   subtract, we just xor.  Thus, we tend to get a bit sloppy about
  *   the difference between adding and subtracting.
  *
  * A 32-bit CRC polynomial is actually 33 bits long.  But since it's
  * 33 bits long, bit 32 is always going to be set, so usually the CRC
  * is written in hex with the most significant bit omitted.  (If you're
  * familiar with the IEEE 754 floating-point format, it's the same idea.)
  *
  * Note that a CRC is computed over a string of *bits*, so you have
  * to decide on the endianness of the bits within each byte.  To get
  * the best error-detecting properties, this should correspond to the
  * order they're actually sent.  For example, standard RS-232 serial is
  * little-endian; the most significant bit (sometimes used for parity)
  * is sent last.  And when appending a CRC word to a message, you should
  * do it in the right order, matching the endianness.
  *
  * Just like with ordinary division, the remainder is always smaller than
  * the divisor (the CRC polynomial) you're dividing by.  Each step of the
  * division, you take one more digit (bit) of the dividend and append it
  * to the current remainder.  Then you figure out the appropriate multiple
  * of the divisor to subtract to being the remainder back into range.
  * In binary, it's easy - it has to be either 0 or 1, and to make the
  * XOR cancel, it's just a copy of bit 32 of the remainder.
  *
  * When computing a CRC, we don't care about the quotient, so we can
  * throw the quotient bit away, but subtract the appropriate multiple of
  * the polynomial from the remainder and we're back to where we started,
  * ready to process the next bit.
  *
  * A big-endian CRC written this way would be coded like:
  * for (i = 0; i < input_bits; i++) {
  * 	multiple = remainder & 0x80000000 ? CRCPOLY : 0;
  * 	remainder = (remainder << 1 | next_input_bit()) ^ multiple;
  * }
  * Notice how, to get at bit 32 of the shifted remainder, we look
  * at bit 31 of the remainder *before* shifting it.
  *
  * But also notice how the next_input_bit() bits we're shifting into
  * the remainder don't actually affect any decision-making until
  * 32 bits later.  Thus, the first 32 cycles of this are pretty boring.
  * Also, to add the CRC to a message, we need a 32-bit-long hole for it at
  * the end, so we have to add 32 extra cycles shifting in zeros at the
  * end of every message,
  *
  * So the standard trick is to rearrage merging in the next_input_bit()
  * until the moment it's needed.  Then the first 32 cycles can be precomputed,
  * and merging in the final 32 zero bits to make room for the CRC can be
  * skipped entirely.
  * This changes the code to:
  * for (i = 0; i < input_bits; i++) {
  *      remainder ^= next_input_bit() << 31;
  * 	multiple = (remainder & 0x80000000) ? CRCPOLY : 0;
  * 	remainder = (remainder << 1) ^ multiple;
  * }
  * With this optimization, the little-endian code is simpler:
  * for (i = 0; i < input_bits; i++) {
  *      remainder ^= next_input_bit();
  * 	multiple = (remainder & 1) ? CRCPOLY : 0;
  * 	remainder = (remainder >> 1) ^ multiple;
  * }
  *
  * Note that the other details of endianness have been hidden in CRCPOLY
  * (which must be bit-reversed) and next_input_bit().
  *
  * However, as long as next_input_bit is returning the bits in a sensible
  * order, we can actually do the merging 8 or more bits at a time rather
  * than one bit at a time:
  * for (i = 0; i < input_bytes; i++) {
  * 	remainder ^= next_input_byte() << 24;
  * 	for (j = 0; j < 8; j++) {
  * 		multiple = (remainder & 0x80000000) ? CRCPOLY : 0;
  * 		remainder = (remainder << 1) ^ multiple;
  * 	}
  * }
  * Or in little-endian:
  * for (i = 0; i < input_bytes; i++) {
  * 	remainder ^= next_input_byte();
  * 	for (j = 0; j < 8; j++) {
  * 		multiple = (remainder & 1) ? CRCPOLY : 0;
  * 		remainder = (remainder << 1) ^ multiple;
  * 	}
  * }
  * If the input is a multiple of 32 bits, you can even XOR in a 32-bit
  * word at a time and increase the inner loop count to 32.
  *
  * You can also mix and match the two loop styles, for example doing the
  * bulk of a message byte-at-a-time and adding bit-at-a-time processing
  * for any fractional bytes at the end.
  *
  * The only remaining optimization is to the byte-at-a-time table method.
  * Here, rather than just shifting one bit of the remainder to decide
  * in the correct multiple to subtract, we can shift a byte at a time.
  * This produces a 40-bit (rather than a 33-bit) intermediate remainder,
  * but again the multiple of the polynomial to subtract depends only on
  * the high bits, the high 8 bits in this case.
  *
  * The multile we need in that case is the low 32 bits of a 40-bit
  * value whose high 8 bits are given, and which is a multiple of the
  * generator polynomial.  This is simply the CRC-32 of the given
  * one-byte message.
  *
  * Two more details: normally, appending zero bits to a message which
  * is already a multiple of a polynomial produces a larger multiple of that
  * polynomial.  To enable a CRC to detect this condition, it's common to
  * invert the CRC before appending it.  This makes the remainder of the
  * message+crc come out not as zero, but some fixed non-zero value.
  *
  * The same problem applies to zero bits prepended to the message, and
  * a similar solution is used.  Instead of starting with a remainder of
  * 0, an initial remainder of all ones is used.  As long as you start
  * the same way on decoding, it doesn't make a difference.
  */

}
