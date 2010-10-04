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


#ifndef BOX_H
#define BOX_H

#include "Point.h"
#include "Size.h"

namespace nux
{
    class Box
    {
    public:
        Box();
        Box(t_int32 left, t_int32 top, t_int32 right, t_int32 bottom);
        ~Box();
        Box(const Box& r);

        Box& operator = (const Box& r);
        t_bool operator == (const Box& p) const;
        t_bool operator != (const Box& p) const;
        void Set(t_int32 left, t_int32 top, t_int32 right, t_int32 bottom);

    public:
        t_s32 _left; 
        t_s32 _top;
        t_s32 _bottom;
        t_s32 _right;

        t_int32 x, y;
        t_int32 width, height;
    };

}

#endif // BOX_H

