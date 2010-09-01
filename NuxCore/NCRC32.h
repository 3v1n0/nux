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


#ifndef NCRC32_H
#define NCRC32_H

NAMESPACE_BEGIN
// http://www.networkdls.com/Software.Asp?Review=22

// This is the official polynomial used by CRC-32 in PKZip, WinZip and Ethernet.

// There are multiple 16-bit CRC polynomials in common use, but this is *the* standard CRC-32 polynomial,
// first popularized by Ethernet. It is used by PKZip, WinZip and Ethernet.
// x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+x^0

// #define CRC32_POLYNOMIAL_LE 0xedb88320  // little Endian
#define CRC32_POLYNOMIAL 0x04c11db7     // big Endian

// How many bits at a time to use.  Requires a table of 4<<CRC_xx_BITS bytes. We use CRC_8_BITS
// CRC_xx_BITS must be a power of 2 between 1 and 8. We use CRC_8_BITS.
#define CRC32BUFSZ 1024 // 4 << 8 = 1024

class NCRC32
{
public:
    NCRC32();
    t_u32 FileCRC(const char *sFileName);
    t_u32 FullCRC(const char *sData, t_u32 ulLength);
    void PartialCRC(t_u32 *ulInCRC, const char *sData, t_u32 ulLength);

private:
    void Initialize(void);
    t_u32 Reflect(t_u32 ulReflect, char cChar);
    t_u32 CRCTable[256]; // CRC lookup table array.
};

NAMESPACE_END
#endif // NCRC32_H
