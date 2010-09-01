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



// From http://www.packetizer.com/security/sha1/
/*
*  sha1.h
*
*	Copyright (C) 1998
*	Paul E. Jones <paulej@arid.us>
*	All Rights Reserved
*
*****************************************************************************
*	$Id: sha1.h,v 1.2 2004/03/27 18:00:33 paulej Exp $
*****************************************************************************
*
*  Description:
*      This class implements the Secure Hashing Standard as defined
*      in FIPS PUB 180-1 published April 17, 1995.
*
*      Many of the variable names in the SHA1Context, especially the
*      single character names, were used because those were the names
*      used in the publication.
*
*      Please read the file sha1.c for more information.
*
*/


#ifndef NSHA1_H
#define NSHA1_H

NAMESPACE_BEGIN
/* 
*  This structure will hold context information for the hashing
*  operation
*/
typedef struct SHA1Context
{
    unsigned Message_Digest[5]; /* Message Digest (output)          */

    unsigned Length_Low;        /* Message length in bits           */
    unsigned Length_High;       /* Message length in bits           */

    unsigned char Message_Block[64]; /* 512-bit message blocks      */
    int Message_Block_Index;    /* Index into message block array   */

    int Computed;               /* Is the digest computed?          */
    int Corrupted;              /* Is the message digest corruped?  */
} SHA1Context;

/*
*  Function Prototypes
*/
void SHA1Reset(SHA1Context *);
int SHA1Result(SHA1Context *);
void SHA1Input(SHA1Context *,
               const unsigned char *,
               unsigned);

NAMESPACE_END

#endif // NSHA1_H

