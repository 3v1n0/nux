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


/*
**********************************************************************
** md5.c                                                            **
** RSA Data Security, Inc. MD5 Message Digest Algorithm             **
** Created: 2/17/90 RLR                                             **
** Revised: 1/91 SRD,AJ,BSK,JT Reference C Version                  **
**********************************************************************
*/

/*
**********************************************************************
** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved. **
**                                                                  **
** License to copy and use this software is granted provided that   **
** it is identified as the "RSA Data Security, Inc. MD5 Message     **
** Digest Algorithm" in all material mentioning or referencing this **
** software or this function.                                       **
**                                                                  **
** License is also granted to make and use derivative works         **
** provided that such works are identified as "derived from the RSA **
** Data Security, Inc. MD5 Message Digest Algorithm" in all         **
** material mentioning or referencing the derived work.             **
**                                                                  **
** RSA Data Security, Inc. makes no representations concerning      **
** either the merchantability of this software or the suitability   **
** of this software for any particular purpose.  It is provided "as **
** is" without express or implied warranty of any kind.             **
**                                                                  **
** These notices must be retained in any copies of any part of this **
** documentation and/or software.                                   **
**********************************************************************
*/

/* -- include the following line if the md5.h header file is separate -- */
/* #include "md5.h" */

/*****************************************************************************************
CLASS:			CMD5Checksum
DESCRIPTION:	Implements the "RSA Data Security, Inc. MD5 Message-Digest Algorithm".
NOTES:			Calculates the RSA MD5 checksum for a file or congiguous array of data.	

Below are extracts from a memo on The MD5 Message-Digest Algorithm by R. Rivest of MIT 
Laboratory for Computer Science and RSA Data Security, Inc., April 1992. 

1. Executive Summary
This document describes the MD5 message-digest algorithm. The
algorithm takes as input a message of arbitrary length and produces
as output a 128-bit "fingerprint" or "message digest" of the input.
It is conjectured that it is computationally infeasible to produce
two messages having the same message digest, or to produce any
message having a given prespecified target message digest. The MD5
algorithm is intended for digital signature applications, where a
large file must be "compressed" in a secure manner before being
encrypted with a private (secret) key under a public-key cryptosystem
such as RSA.

The MD5 algorithm is designed to be quite fast on 32-bit machines. In
addition, the MD5 algorithm does not require any large substitution
tables; the algorithm can be coded quite compactly.
The MD5 algorithm is an extension of the MD4 message-digest algorithm
1,2]. MD5 is slightly slower than MD4, but is more "conservative" in
design. MD5 was designed because it was felt that MD4 was perhaps
being adopted for use more quickly than justified by the existing
critical review; because MD4 was designed to be exceptionally fast,
it is "at the edge" in terms of risking successful cryptanalytic
attack. MD5 backs off a bit, giving up a little in speed for a much
greater likelihood of ultimate security. It incorporates some
suggestions made by various reviewers, and contains additional
optimizations. The MD5 algorithm is being placed in the public domain
for review and possible adoption as a standard.


2. Terminology and Notation
In this document a "word" is a 32-bit quantity and a "byte" is an
eight-bit quantity. A sequence of bits can be interpreted in a
natural manner as a sequence of bytes, where each consecutive group
of eight bits is interpreted as a byte with the high-order (most
significant) bit of each byte listed first. Similarly, a sequence of
bytes can be interpreted as a sequence of 32-bit words, where each
consecutive group of four bytes is interpreted as a word with the
low-order (least significant) byte given first.
Let x_i denote "x sub i". If the subscript is an expression, we
surround it in braces, as in x_{i+1}. Similarly, we use ^ for
superscripts (exponentiation), so that x^i denotes x to the i-th   power.
Let the symbol "+" denote addition of words (i.e., modulo-2^32
addition). Let X <<< s denote the 32-bit value obtained by circularly
shifting (rotating) X left by s bit positions. Let not(X) denote the
bit-wise complement of X, and let X v Y denote the bit-wise OR of X
and Y. Let X xor Y denote the bit-wise XOR of X and Y, and let XY
denote the bit-wise AND of X and Y.


3. MD5 Algorithm Description
We begin by supposing that we have a b-bit message as input, and that
we wish to find its message digest. Here b is an arbitrary
nonnegative integer; b may be zero, it need not be a multiple of
eight, and it may be arbitrarily large. We imagine the bits of the
message written down as follows:          m_0 m_1 ... m_{b-1}
The following five steps are performed to compute the message digest
of the message.

3.1 Step 1. Append Padding Bits
The message is "padded" (extended) so that its length (in bits) is
congruent to 448, modulo 512. That is, the message is extended so
that it is just 64 bits shy of being a multiple of 512 bits long.
Padding is always performed, even if the length of the message is
already congruent to 448, modulo 512.
Padding is performed as follows: a single "1" bit is appended to the
message, and then "0" bits are appended so that the length in bits of
the padded message becomes congruent to 448, modulo 512. In all, at
least one bit and at most 512 bits are appended.

3.2 Step 2. Append Length
A 64-bit representation of b (the length of the message before the
padding bits were added) is appended to the result of the previous
step. In the unlikely event that b is greater than 2^64, then only
the low-order 64 bits of b are used. (These bits are appended as two
32-bit words and appended low-order word first in accordance with the
previous conventions.)
At this point the resulting message (after padding with bits and with
b) has a length that is an exact multiple of 512 bits. Equivalently,
this message has a length that is an exact multiple of 16 (32-bit)
words. Let M[0 ... N-1] denote the words of the resulting message,
where N is a multiple of 16.

3.3 Step 3. Initialize MD Buffer
A four-word buffer (A,B,C,D) is used to compute the message digest.
Here each of A, B, C, D is a 32-bit register. These registers are
initialized to the following values in hexadecimal, low-order bytes   first):
word A: 01 23 45 67          word B: 89 ab cd ef
word C: fe dc ba 98          word D: 76 54 32 10

3.4 Step 4. Process Message in 16-Word Blocks
We first define four auxiliary functions that each take as input
three 32-bit words and produce as output one 32-bit word.
F(X,Y,Z) = XY v not(X) Z          G(X,Y,Z) = XZ v Y not(Z)
H(X,Y,Z) = X xor Y xor Z          I(X,Y,Z) = Y xor (X v not(Z))
In each bit position F acts as a conditional: if X then Y else Z.
The function F could have been defined using + instead of v since XY
and not(X)Z will never have 1's in the same bit position.) It is
interesting to note that if the bits of X, Y, and Z are independent
and unbiased, the each bit of F(X,Y,Z) will be independent and   unbiased.
The functions G, H, and I are similar to the function F, in that they
act in "bitwise parallel" to produce their output from the bits of X,
Y, and Z, in such a manner that if the corresponding bits of X, Y,
and Z are independent and unbiased, then each bit of G(X,Y,Z),
H(X,Y,Z), and I(X,Y,Z) will be independent and unbiased. Note that
the function H is the bit-wise "xor" or "parity" function of its   inputs.
This step uses a 64-element table T[1 ... 64] constructed from the
sine function. Let T[i] denote the i-th element of the table, which
is equal to the integer part of 4294967296 times abs(sin(i)), where i
is in radians. The elements of the table are given in the appendix.
Do the following:   

//Process each 16-word block.
For i = 0 to N/16-1 do     // Copy block i into X.      
For j = 0 to 15 do
Set X[j] to M[i*16+j].     
end //of loop on j

// Save A as AA, B as BB, C as CC, and D as DD.
AA = A     BB = B
CC = C     DD = D     

// Round 1.
// Let [abcd k s i] denote the operation
// a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s).
// Do the following 16 operations.
[ABCD  0  7  1]  [DABC  1 12  2]  [CDAB  2 17  3]  [BCDA  3 22  4]
[ABCD  4  7  5]  [DABC  5 12  6]  [CDAB  6 17  7]  [BCDA  7 22  8]
[ABCD  8  7  9]  [DABC  9 12 10]  [CDAB 10 17 11]  [BCDA 11 22 12]
[ABCD 12  7 13]  [DABC 13 12 14]  [CDAB 14 17 15]  [BCDA 15 22 16]

// Round 2.      
// Let [abcd k s i] denote the operation 
// a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s).
// Do the following 16 operations.
[ABCD  1  5 17]  [DABC  6  9 18]  [CDAB 11 14 19]  [BCDA  0 20 20]
[ABCD  5  5 21]  [DABC 10  9 22]  [CDAB 15 14 23]  [BCDA  4 20 24]
[ABCD  9  5 25]  [DABC 14  9 26]  [CDAB  3 14 27]  [BCDA  8 20 28]
[ABCD 13  5 29]  [DABC  2  9 30]  [CDAB  7 14 31]  [BCDA 12 20 32]

// Round 3.      
// Let [abcd k s t] denote the operation
// a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s).
// Do the following 16 operations.
[ABCD  5  4 33]  [DABC  8 11 34]  [CDAB 11 16 35]  [BCDA 14 23 36]
[ABCD  1  4 37]  [DABC  4 11 38]  [CDAB  7 16 39]  [BCDA 10 23 40]
[ABCD 13  4 41]  [DABC  0 11 42]  [CDAB  3 16 43]  [BCDA  6 23 44]
[ABCD  9  4 45]  [DABC 12 11 46]  [CDAB 15 16 47]  [BCDA  2 23 48]

// Round 4. 
// Let [abcd k s t] denote the operation
// a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s).
// Do the following 16 operations.
[ABCD  0  6 49]  [DABC  7 10 50]  [CDAB 14 15 51]  [BCDA  5 21 52]
[ABCD 12  6 53]  [DABC  3 10 54]  [CDAB 10 15 55]  [BCDA  1 21 56]
[ABCD  8  6 57]  [DABC 15 10 58]  [CDAB  6 15 59]  [BCDA 13 21 60]
[ABCD  4  6 61]  [DABC 11 10 62]  [CDAB  2 15 63]  [BCDA  9 21 64]

// Then perform the following additions. (That is increment each
//   of the four registers by the value it had before this block
//   was started.) 
A = A + AA     B = B + BB     C = C + CC  D = D + DD   

end // of loop on i

3.5 Step 5. Output
The message digest produced as output is A, B, C, D. That is, we
begin with the low-order byte of A, and end with the high-order byte of D.
This completes the description of MD5.

Summary
The MD5 message-digest algorithm is simple to implement, and provides
a "fingerprint" or message digest of a message of arbitrary length.
It is conjectured that the difficulty of coming up with two messages
having the same message digest is on the order of 2^64 operations,
and that the difficulty of coming up with any message having a given
message digest is on the order of 2^128 operations. The MD5 algorithm
has been carefully scrutinized for weaknesses. It is, however, a
relatively new algorithm and further security analysis is of course
justified, as is the case with any new proposal of this sort.


5. Differences Between MD4 and MD5
The following are the differences between MD4 and MD5:
1.   A fourth round has been added.
2.   Each step now has a unique additive constant.
3.   The function g in round 2 was changed from (XY v XZ v YZ) to
(XZ v Y not(Z)) to make g less symmetric.
4.   Each step now adds in the result of the previous step.  This
promotes a faster "avalanche effect".
5.   The order in which input words are accessed in rounds 2 and
3 is changed, to make these patterns less like each other.
6.   The shift amounts in each round have been approximately
optimized, to yield a faster "avalanche effect." The shifts in
different rounds are distinct.

References
[1] Rivest, R., "The MD4 Message Digest Algorithm", RFC 1320, MIT and
RSA Data Security, Inc., April 1992.
[2] Rivest, R., "The MD4 message digest algorithm", in A.J.  Menezes
and S.A. Vanstone, editors, Advances in Cryptology - CRYPTO '90
Proceedings, pages 303-311, Springer-Verlag, 1991.
[3] CCITT Recommendation X.509 (1988), "The Directory -
Authentication Framework."APPENDIX A - Reference Implementation


The level of security discussed in this memo is considered to be
sufficient for implementing very high security hybrid digital-
signature schemes based on MD5 and a public-key cryptosystem.
Author's Address
Ronald L. Rivest   Massachusetts Institute of Technology
Laboratory for Computer Science   NE43-324   545 Technology Square
Cambridge, MA  02139-1986   Phone: (617) 253-5880
EMail: rivest@theory.lcs.mit.edu


*****************************************************************************************/

#include "NKernel.h"
#include "NMD5.h"
NAMESPACE_BEGIN

/* forward declaration */
static void Transform (UINT4 *buf, UINT4 *in);

static unsigned char PADDING[64] = {
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* F, G and H are basic MD5 functions: selection, majority, parity */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z))) 

/* ROTATE_LEFT rotates x left n bits */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4 */
/* Rotation is separate from addition to prevent recomputation */
#define FF(a, b, c, d, x, s, ac) \
{(a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) \
{(a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) \
{(a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}
#define II(a, b, c, d, x, s, ac) \
{(a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}

void MD5Init (MD5_CTX *mdContext)
{
    mdContext->i[0] = mdContext->i[1] = (UINT4)0;

    /* Load magic initialization constants.
    */
    mdContext->buf[0] = (UINT4)0x67452301;
    mdContext->buf[1] = (UINT4)0xefcdab89;
    mdContext->buf[2] = (UINT4)0x98badcfe;
    mdContext->buf[3] = (UINT4)0x10325476;
}

/* MD5 block update operation. Continues an MD5 message-digest
  operation, processing another message block, and updating the
  context.
 */
void MD5Update (MD5_CTX *mdContext, unsigned char *inBuf, unsigned int inLen)
{
    UINT4 in[16];
    int mdi;
    unsigned int i, ii;

    if( inLen <= 0 )
        return;

    /* compute number of bytes mod 64 */
    mdi = (int)((mdContext->i[0] >> 3) & 0x3F);

    /* update number of bits */
    if ((mdContext->i[0] + ((UINT4)inLen << 3)) < mdContext->i[0])
        mdContext->i[1]++;
    mdContext->i[0] += ((UINT4)inLen << 3);
    mdContext->i[1] += ((UINT4)inLen >> 29);

    while (inLen--) {
        /* add new character to buffer, increment mdi */
        mdContext->in[mdi++] = *inBuf++;

        /* transform if necessary */
        if (mdi == 0x40) {
            for (i = 0, ii = 0; i < 16; i++, ii += 4)
                in[i] = (((UINT4)mdContext->in[ii+3]) << 24) |
                (((UINT4)mdContext->in[ii+2]) << 16) |
                (((UINT4)mdContext->in[ii+1]) << 8) |
                ((UINT4)mdContext->in[ii]);
            Transform (mdContext->buf, in);
            mdi = 0;
        }
    }
}

void MD5Final (MD5_CTX *mdContext)
{
    UINT4 in[16];
    int mdi;
    unsigned int i, ii;
    unsigned int padLen;

    /* save number of bits */
    in[14] = mdContext->i[0];
    in[15] = mdContext->i[1];

    /* compute number of bytes mod 64 */
    mdi = (int)((mdContext->i[0] >> 3) & 0x3F);

    /* pad out to 56 mod 64 */
    padLen = (mdi < 56) ? (56 - mdi) : (120 - mdi);
    MD5Update (mdContext, PADDING, padLen);

    /* append length in bits and transform */
    for (i = 0, ii = 0; i < 14; i++, ii += 4)
        in[i] = (((UINT4)mdContext->in[ii+3]) << 24) |
        (((UINT4)mdContext->in[ii+2]) << 16) |
        (((UINT4)mdContext->in[ii+1]) << 8) |
        ((UINT4)mdContext->in[ii]);
    Transform (mdContext->buf, in);

    /* store buffer in digest */
    for (i = 0, ii = 0; i < 4; i++, ii += 4) {
        mdContext->digest[ii] = (unsigned char)(mdContext->buf[i] & 0xFF);
        mdContext->digest[ii+1] =
            (unsigned char)((mdContext->buf[i] >> 8) & 0xFF);
        mdContext->digest[ii+2] =
            (unsigned char)((mdContext->buf[i] >> 16) & 0xFF);
        mdContext->digest[ii+3] =
            (unsigned char)((mdContext->buf[i] >> 24) & 0xFF);
    }
}

/* Basic MD5 step. Transform buf based on in.
*/
static void Transform (UINT4 *buf, UINT4 *in)
{
    UINT4 a = buf[0], b = buf[1], c = buf[2], d = buf[3];

    /* Round 1 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
    FF ( a, b, c, d, in[ 0], S11, 3614090360UL); /* 1 */
    FF ( d, a, b, c, in[ 1], S12, 3905402710UL); /* 2 */
    FF ( c, d, a, b, in[ 2], S13,  606105819UL); /* 3 */
    FF ( b, c, d, a, in[ 3], S14, 3250441966UL); /* 4 */
    FF ( a, b, c, d, in[ 4], S11, 4118548399UL); /* 5 */
    FF ( d, a, b, c, in[ 5], S12, 1200080426UL); /* 6 */
    FF ( c, d, a, b, in[ 6], S13, 2821735955UL); /* 7 */
    FF ( b, c, d, a, in[ 7], S14, 4249261313UL); /* 8 */
    FF ( a, b, c, d, in[ 8], S11, 1770035416UL); /* 9 */
    FF ( d, a, b, c, in[ 9], S12, 2336552879UL); /* 10 */
    FF ( c, d, a, b, in[10], S13, 4294925233UL); /* 11 */
    FF ( b, c, d, a, in[11], S14, 2304563134UL); /* 12 */
    FF ( a, b, c, d, in[12], S11, 1804603682UL); /* 13 */
    FF ( d, a, b, c, in[13], S12, 4254626195UL); /* 14 */
    FF ( c, d, a, b, in[14], S13, 2792965006UL); /* 15 */
    FF ( b, c, d, a, in[15], S14, 1236535329UL); /* 16 */

    /* Round 2 */
#define S21 5
#define S22 9
#define S23 14
#define S24 20
    GG ( a, b, c, d, in[ 1], S21, 4129170786UL); /* 17 */
    GG ( d, a, b, c, in[ 6], S22, 3225465664UL); /* 18 */
    GG ( c, d, a, b, in[11], S23,  643717713UL); /* 19 */
    GG ( b, c, d, a, in[ 0], S24, 3921069994UL); /* 20 */
    GG ( a, b, c, d, in[ 5], S21, 3593408605UL); /* 21 */
    GG ( d, a, b, c, in[10], S22,   38016083UL); /* 22 */
    GG ( c, d, a, b, in[15], S23, 3634488961UL); /* 23 */
    GG ( b, c, d, a, in[ 4], S24, 3889429448UL); /* 24 */
    GG ( a, b, c, d, in[ 9], S21,  568446438UL); /* 25 */
    GG ( d, a, b, c, in[14], S22, 3275163606UL); /* 26 */
    GG ( c, d, a, b, in[ 3], S23, 4107603335UL); /* 27 */
    GG ( b, c, d, a, in[ 8], S24, 1163531501UL); /* 28 */
    GG ( a, b, c, d, in[13], S21, 2850285829UL); /* 29 */
    GG ( d, a, b, c, in[ 2], S22, 4243563512UL); /* 30 */
    GG ( c, d, a, b, in[ 7], S23, 1735328473UL); /* 31 */
    GG ( b, c, d, a, in[12], S24, 2368359562UL); /* 32 */

    /* Round 3 */
#define S31 4
#define S32 11
#define S33 16
#define S34 23
    HH ( a, b, c, d, in[ 5], S31, 4294588738UL); /* 33 */
    HH ( d, a, b, c, in[ 8], S32, 2272392833UL); /* 34 */
    HH ( c, d, a, b, in[11], S33, 1839030562UL); /* 35 */
    HH ( b, c, d, a, in[14], S34, 4259657740UL); /* 36 */
    HH ( a, b, c, d, in[ 1], S31, 2763975236UL); /* 37 */
    HH ( d, a, b, c, in[ 4], S32, 1272893353UL); /* 38 */
    HH ( c, d, a, b, in[ 7], S33, 4139469664UL); /* 39 */
    HH ( b, c, d, a, in[10], S34, 3200236656UL); /* 40 */
    HH ( a, b, c, d, in[13], S31,  681279174UL); /* 41 */
    HH ( d, a, b, c, in[ 0], S32, 3936430074UL); /* 42 */
    HH ( c, d, a, b, in[ 3], S33, 3572445317UL); /* 43 */
    HH ( b, c, d, a, in[ 6], S34,   76029189UL); /* 44 */
    HH ( a, b, c, d, in[ 9], S31, 3654602809UL); /* 45 */
    HH ( d, a, b, c, in[12], S32, 3873151461UL); /* 46 */
    HH ( c, d, a, b, in[15], S33,  530742520UL); /* 47 */
    HH ( b, c, d, a, in[ 2], S34, 3299628645UL); /* 48 */

    /* Round 4 */
#define S41 6
#define S42 10
#define S43 15
#define S44 21
    II ( a, b, c, d, in[ 0], S41, 4096336452UL); /* 49 */
    II ( d, a, b, c, in[ 7], S42, 1126891415UL); /* 50 */
    II ( c, d, a, b, in[14], S43, 2878612391UL); /* 51 */
    II ( b, c, d, a, in[ 5], S44, 4237533241UL); /* 52 */
    II ( a, b, c, d, in[12], S41, 1700485571UL); /* 53 */
    II ( d, a, b, c, in[ 3], S42, 2399980690UL); /* 54 */
    II ( c, d, a, b, in[10], S43, 4293915773UL); /* 55 */
    II ( b, c, d, a, in[ 1], S44, 2240044497UL); /* 56 */
    II ( a, b, c, d, in[ 8], S41, 1873313359UL); /* 57 */
    II ( d, a, b, c, in[15], S42, 4264355552UL); /* 58 */
    II ( c, d, a, b, in[ 6], S43, 2734768916UL); /* 59 */
    II ( b, c, d, a, in[13], S44, 1309151649UL); /* 60 */
    II ( a, b, c, d, in[ 4], S41, 4149444226UL); /* 61 */
    II ( d, a, b, c, in[11], S42, 3174756917UL); /* 62 */
    II ( c, d, a, b, in[ 2], S43,  718787259UL); /* 63 */
    II ( b, c, d, a, in[ 9], S44, 3951481745UL); /* 64 */

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}

NAMESPACE_END

/*
**********************************************************************
** End of md5.c                                                     **
******************************* (cut) ********************************
*/

// 
// 
// /*
// **********************************************************************
// ** md5driver.c -- sample routines to test                           **
// ** RSA Data Security, Inc. MD5 message digest algorithm.            **
// ** Created: 2/16/90 RLR                                             **
// ** Updated: 1/91 SRD                                                **
// **********************************************************************
// */
// 
// /*
// **********************************************************************
// ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved. **
// **                                                                  **
// ** RSA Data Security, Inc. makes no representations concerning      **
// ** either the merchantability of this software or the suitability   **
// ** of this software for any particular purpose.  It is provided "as **
// ** is" without express or implied warranty of any kind.             **
// **                                                                  **
// ** These notices must be retained in any copies of any part of this **
// ** documentation and/or software.                                   **
// **********************************************************************
// */
// 
// #include <stdio.h>
// #include <sys/types.h>
// #include <time.h>
// #include <string.h>
// /* -- include the following file if the file md5.h is separate -- */
// /* #include "md5.h" */
// 
// /* Prints message digest buffer in mdContext as 32 hexadecimal digits.
// Order is from low-order byte to high-order byte of digest.
// Each byte is printed with high-order hexadecimal digit first.
// */
// static void MDPrint (mdContext)
// MD5_CTX *mdContext;
// {
//     int i;
// 
//     for (i = 0; i < 16; i++)
//         printf ("%02x", mdContext->digest[i]);
// }
// 
// /* size of test block */
// #define TEST_BLOCK_SIZE 1000
// 
// /* number of blocks to process */
// #define TEST_BLOCKS 10000
// 
// /* number of test bytes = TEST_BLOCK_SIZE * TEST_BLOCKS */
// static long TEST_BYTES = (long)TEST_BLOCK_SIZE * (long)TEST_BLOCKS;
// 
// /* A time trial routine, to measure the speed of MD5.
// Measures wall time required to digest TEST_BLOCKS * TEST_BLOCK_SIZE
// characters.
// */
// static void MDTimeTrial ()
// {
//     MD5_CTX mdContext;
//     time_t endTime, startTime;
//     unsigned char data[TEST_BLOCK_SIZE];
//     unsigned int i;
// 
//     /* initialize test data */
//     for (i = 0; i < TEST_BLOCK_SIZE; i++)
//         data[i] = (unsigned char)(i & 0xFF);
// 
//     /* start timer */
//     printf ("MD5 time trial. Processing %ld characters...\n", TEST_BYTES);
//     time (&startTime);
// 
//     /* digest data in TEST_BLOCK_SIZE byte blocks */
//     MD5Init (&mdContext);
//     for (i = TEST_BLOCKS; i > 0; i--)
//         MD5Update (&mdContext, data, TEST_BLOCK_SIZE);
//     MD5Final (&mdContext);
// 
//     /* stop timer, get time difference */
//     time (&endTime);
//     MDPrint (&mdContext);
//     printf (" is digest of test input.\n");
//     printf
//         ("Seconds to process test input: %ld\n", (long)(endTime-startTime));
//     printf
//         ("Characters processed per second: %ld\n",
//         TEST_BYTES/(endTime-startTime));
// }
// 
// /* Computes the message digest for string inString.
// Prints out message digest, a space, the string (in quotes) and a
// carriage return.
// */
// static void MDString (inString)
// char *inString;
// {
//     MD5_CTX mdContext;
//     unsigned int len = strlen (inString);
// 
//     MD5Init (&mdContext);
//     MD5Update (&mdContext, inString, len);
//     MD5Final (&mdContext);
//     MDPrint (&mdContext);
//     printf (" \"%s\"\n\n", inString);
// }
// 
// /* Computes the message digest for a specified file.
// Prints out message digest, a space, the file name, and a carriage
// return.
// */
// static void MDFile (filename)
// char *filename;
// {
//     FILE *inFile = fopen (filename, "rb");
//     MD5_CTX mdContext;
//     int bytes;
//     unsigned char data[1024];
// 
//     if (inFile == NULL) {
//         printf ("%s can't be opened.\n", filename);
//         return;
//     }
// 
//     MD5Init (&mdContext);
//     while ((bytes = fread (data, 1, 1024, inFile)) != 0)
//         MD5Update (&mdContext, data, bytes);
//     MD5Final (&mdContext);
//     MDPrint (&mdContext);
//     printf (" %s\n", filename);
//     fclose (inFile);
// }
// 
// /* Writes the message digest of the data from stdin onto stdout,
// followed by a carriage return.
// */
// static void MDFilter ()
// {
//     MD5_CTX mdContext;
//     int bytes;
//     unsigned char data[16];
// 
//     MD5Init (&mdContext);
//     while ((bytes = fread (data, 1, 16, stdin)) != 0)
//         MD5Update (&mdContext, data, bytes);
//     MD5Final (&mdContext);
//     MDPrint (&mdContext);
//     printf ("\n");
// }
// 
// /* Runs a standard suite of test data.
// */
// static void MDTestSuite ()
// {
//     printf ("MD5 test suite results:\n\n");
//     MDString ("");
//     MDString ("a");
//     MDString ("abc");
//     MDString ("message digest");
//     MDString ("abcdefghijklmnopqrstuvwxyz");
//     MDString
//         ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
//     MDString
//         ("1234567890123456789012345678901234567890\
//          1234567890123456789012345678901234567890");
//     /* Contents of file foo are "abc" */
//     MDFile ("foo");
// }
// 
// void main (argc, argv)
// int argc;
// char *argv[];
// {
//     int i;
// 
//     /* For each command line argument in turn:
//     ** filename          -- prints message digest and name of file
//     ** -sstring          -- prints message digest and contents of string
//     ** -t                -- prints time trial statistics for 1M characters
//     ** -x                -- execute a standard suite of test data
//     ** (no args)         -- writes messages digest of stdin onto stdout
//     */
//     if (argc == 1)
//         MDFilter ();
//     else
//         for (i = 1; i < argc; i++)
//             if (argv[i][0] == '-' && argv[i][1] == 's')
//                 MDString (argv[i] + 2);
//             else if (strcmp (argv[i], "-t") == 0)
//                 MDTimeTrial ();
//             else if (strcmp (argv[i], "-x") == 0)
//                 MDTestSuite ();
//             else MDFile (argv[i]);
// }
// 
// /*
// **********************************************************************
// ** End of md5driver.c                                               **
// ******************************* (cut) ********************************
// */
