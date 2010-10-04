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


#ifndef UTILS_H
#define UTILS_H

// check if a value lies within a closed interval
#ifndef IN_BOUNDS
#define IN_BOUNDS( x, lo, hi ) ( (x) >= (lo) && (x) <= (hi) )
#endif

//check if a 2D point lies within a 2D box 
#ifndef PT_IN_BOX
#define PT_IN_BOX( x, y, lo_x, hi_x, lo_y, hi_y ) ( IN_BOUNDS(x,lo_x,hi_x) && IN_BOUNDS(y,lo_y,hi_y) )
#endif

namespace nux { //NUX_NAMESPACE_BEGIN

//enum ObjectType
//{
//    eArea = (1L),
//    eBaseArea   = (1L) << 1,
//    eComposite  = (1L) << 2,
//    eLayout     = (1L) << 3,
//    eHLayout    = (1L) << 4,
//    eVLayout    = (1L) << 5,
//    eSpace     = (1L) << 6,
//};

enum EventTraverseInfo
{
    eSolveNone          = (0L),
    eMouseEventSolved   = (1L),
    eKeyEventSolved     = (1L) << 1,
    eDoNotProcess       = (1L) << 2,
};

enum PositionPolicy
{
    ePositionLeft, 
    ePositionRight,
    ePositionCenter,
    ePositionFull, 
    ePositionProportional,
};

enum SizePolicy
{
    eSizeFixed,
    eSizeResizeable,
};

typedef enum /*RectangleShapeCorners*/
{
    eCornerNone         = 0,
    eCornerTopLeft      = (1L),
    eCornerTopRight     = (1L<<1),
    eCornerBottomLeft   = (1L<<2),
    eCornerBottomRight  = (1L<<3),
    eAllCorners         = eCornerTopLeft|eCornerTopRight|eCornerBottomLeft|eCornerBottomRight,
} ShapeCornerType;

enum TextureAlignment
{
    eTACenter,
    eTALeft,
    eTARight,
    eTATop,
    eTABottom
};

enum VerticalAlignment
{
    eVACenter,
    eVATop,
    eVABottom
};

enum HorizontalAlignment
{
    eHACenter,
    eHALeft,
    eHARight,
};

struct MouseEvent
{
    int PosX;
    int PosY;
    bool MouseIn;
    bool MouseMove;
    bool MouseDown;
    bool MouseUp;
    bool mouseClicked;

    int DeltaX;
    int DeltaY;

    bool m_HWMouseMove;
    bool m_HWMouseDown;
    bool m_HWMouseUp;
    bool m_HWMouseLeave;
    bool m_HWMouseEnter;
};

typedef Rect Geometry;
//struct Geometry: public Rect
//{
//    Geometry()
//        :Rect(0, 0, 1, 1)
//    {
////        X = 0;
////        Y = 0;
////        Width = 1;
////        Height = 1;
//    }
//    //~Geometry(){ }
//    Geometry(int x, int y, int w, int h)
//        :Rect(x, y, w, h)
//    {
////        X = x;
////        Y = y;
////        Width = w;
////        Height = h;
//    }

//private:
//    int X;
//    int Y;
//    int Width;
//    int Height;
//};

typedef struct
{
    int x;
    int y;
    int width;
    int height;

    int x_clipregion;
    int y_clipregion;
    int width_clipregion;
    int height_clipregion;

} DrawAreaContext;


bool IsMouseOwned();

} //NUX_NAMESPACE_END

#endif // UTILS_H
