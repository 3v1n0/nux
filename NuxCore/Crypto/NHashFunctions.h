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


#ifndef NHASHFUNCTIONS_H
#define NHASHFUNCTIONS_H

NAMESPACE_BEGIN

unsigned int RSHash  (const NString& str);
unsigned int JSHash  (const NString& str);
unsigned int PJWHash (const NString& str);
unsigned int ELFHash (const NString& str);
unsigned int BKDRHash(const NString& str);
unsigned int SDBMHash(const NString& str);
unsigned int DJBHash (const NString& str);
unsigned int DEKHash (const NString& str);
unsigned int BPHash  (const NString& str);
unsigned int FNVHash (const NString& str);
unsigned int APHash  (const NString& str);

NAMESPACE_END

#endif // NHASHFUNCTIONS_H

