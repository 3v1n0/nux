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


#ifndef NUNIQUEINDEX_H
#define NUNIQUEINDEX_H

NAMESPACE_BEGIN

class NUniqueIndex
{
    INL_DECLARE_GLOBAL_OBJECT(NUniqueIndex, NGlobalSingletonInitializer);
public:
    t_u64 GetUniqueIndex();
private:
    NThreadSafeCounter m_UniqueIndex;
    static NCriticalSection m_CriticalSection;
};

NAMESPACE_END

#endif // NUNIQUEINDEX_H
