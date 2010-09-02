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


#include "NKernel.h"

NAMESPACE_BEGIN

// template<typename T>
// void NSerializer::SerializeBuffer(T* buffer, t_u64 Length, t_u64 ElementSize)
// {
//     for(t_u64 i = 0; i < Length; i++)
//     {
//         t_u8* bytebuffer = (t_u8*)(&buffer[i]);
//         Serialize(bytebuffer, ElementSize);
//     }
// }

// NSerializer& operator << (NSerializer& Sr, NString& s)
// {
//     Sr.SerializeBuffer<TCHAR>(INL_CONST_CAST(TCHAR*, s.GetTCharPtr()), s.Size()+1, sizeof(TCHAR));
//     return Sr;
// }
// 
// NSerializer& operator << (NSerializer& Sr, std::string& s)
// {
//     Sr.SerializeBuffer<TCHAR>(INL_CONST_CAST(char*, s.c_str()), s.size()+1, sizeof(char));
//     return Sr;
// }

void NSerializer::Serialize(t_char&   data) {SerializeFinal(&data, sizeof(t_char)   );}
void NSerializer::Serialize(t_wchar&  data) {SerializeFinal(&data, sizeof(t_wchar)   );}
void NSerializer::Serialize(t_bool&   data) {SerializeFinal(&data, sizeof(t_bool)    );} 
void NSerializer::Serialize(t_s8&     data) {SerializeFinal(&data, sizeof(t_s8)      );}
void NSerializer::Serialize(t_u8&     data) {SerializeFinal(&data, sizeof(t_u8)      );}
void NSerializer::Serialize(t_u16&    data) {SerializeFinal(&data, sizeof(t_u16)     );}
void NSerializer::Serialize(t_s16&    data) {SerializeFinal(&data, sizeof(t_s16)     );}
void NSerializer::Serialize(t_uint32& data) {SerializeFinal(&data, sizeof(t_uint32)  );}
void NSerializer::Serialize(t_int32&  data) {SerializeFinal(&data, sizeof(t_int32)   );}
void NSerializer::Serialize(t_long&   data) {SerializeFinal(&data, sizeof(t_long)    );}
void NSerializer::Serialize(t_ulong&  data) {SerializeFinal(&data, sizeof(t_ulong)   );}
void NSerializer::Serialize(t_float&  data) {SerializeFinal(&data, sizeof(t_float)   );}
void NSerializer::Serialize(t_double& data) {SerializeFinal(&data, sizeof(t_double)  );}
void NSerializer::Serialize(t_u64&    data) {SerializeFinal(&data, sizeof(t_u64)     );}
void NSerializer::Serialize(t_s64&    data) {SerializeFinal(&data, sizeof(t_s64)     );}

void NSerializer::Serialize(t_char*   buffer, t_u32 len, t_u32 stride) {SerializeFinal(buffer, len*stride);}
void NSerializer::Serialize(t_wchar*  buffer, t_u32 len, t_u32 stride) {SerializeFinal(buffer, len*stride);}
void NSerializer::Serialize(t_bool*   buffer, t_u32 len, t_u32 stride) {SerializeFinal(buffer, len*stride);}
void NSerializer::Serialize(t_s8*     buffer, t_u32 len, t_u32 stride) {SerializeFinal(buffer, len*stride);}
void NSerializer::Serialize(t_u8*     buffer, t_u32 len, t_u32 stride) {SerializeFinal(buffer, len*stride);}
void NSerializer::Serialize(t_u16*    buffer, t_u32 len, t_u32 stride) {SerializeFinal(buffer, len*stride);}
void NSerializer::Serialize(t_s16*    buffer, t_u32 len, t_u32 stride) {SerializeFinal(buffer, len*stride);}
void NSerializer::Serialize(t_uint32* buffer, t_u32 len, t_u32 stride) {SerializeFinal(buffer, len*stride);}
void NSerializer::Serialize(t_int32*  buffer, t_u32 len, t_u32 stride) {SerializeFinal(buffer, len*stride);}
void NSerializer::Serialize(t_long*   buffer, t_u32 len, t_u32 stride) {SerializeFinal(buffer, len*stride);}
void NSerializer::Serialize(t_ulong*  buffer, t_u32 len, t_u32 stride) {SerializeFinal(buffer, len*stride);}
void NSerializer::Serialize(t_float*  buffer, t_u32 len, t_u32 stride) {SerializeFinal(buffer, len*stride);}
void NSerializer::Serialize(t_double* buffer, t_u32 len, t_u32 stride) {SerializeFinal(buffer, len*stride);}
void NSerializer::Serialize(t_u64*    buffer, t_u32 len, t_u32 stride) {SerializeFinal(buffer, len*stride);}
void NSerializer::Serialize(t_s64*    buffer, t_u32 len, t_u32 stride) {SerializeFinal(buffer, len*stride);}

NAMESPACE_END