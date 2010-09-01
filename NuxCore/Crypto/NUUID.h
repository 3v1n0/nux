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


#ifndef NUUID_H
#define NUUID_H

NAMESPACE_BEGIN

#undef uuid_t

typedef t_ulong     unsigned32;
typedef t_u16    unsigned16;
typedef t_u8        unsigned8;
typedef t_u8        byte;

typedef struct {
    unsigned32  time_low;
    unsigned16  time_mid;
    unsigned16  time_hi_and_version;
    unsigned8   clock_seq_hi_and_reserved;
    unsigned8   clock_seq_low;
    byte        node[6];
} uuid_t;

INL_DECLSPEC_DLL int GenerateUUID1(uuid_t *uuid);
INL_DECLSPEC_DLL void GenerateUUID3(uuid_t *uuid, uuid_t nsid, void *name, int namelen);
INL_DECLSPEC_DLL void GenerateUUID5(uuid_t *uuid, uuid_t nsid, void *name, int namelen);

NAMESPACE_END

#endif // NUUID_H
