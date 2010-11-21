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


#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <sigc++/trackable.h>
#include <sigc++/signal.h>
#include <sigc++/functors/ptr_fun.h>
#include <sigc++/functors/mem_fun.h>

#include "NuxCore/InitiallyUnownedObject.h"

#include "Utils.h"
#include "WidgetMetrics.h"
#include "WidgetSmartPointer.h"

#define inlptr(T) T*
#define smptr(T) nux::IntrusiveSP<T>
#define weaksmptr(T) nux::IntrusiveWeakSP<T>
#define smptrnull(T) nux::IntrusiveSP<T> (0)

namespace nux
{

  class WindowThread;
  class GraphicsEngine;


// #if 0
//     #define usingwsptr
//     #define smartptr(T, a)      WSPtr<T>(a)
//     #define smartptrobj(a)      smptr(Area)(a)
//     #define smartptrtype(T)     WSPtr<T>
//     #define smartptrwrap(T)     WrapWSPtr<T>
//     #define smartptrweak(T)     WeakWSPtr<T>
// #else
//     #define smartptr(T, a) T* a
//     #define smartptrobj(a) smptr(Area) a
//     #define smartptrtype(T) (T*)
//     #define smartptrwrap(T) (T*)
//     #define smartptrweak(T) T*
// #endif

////////////////////////////////////////////////////////////////////////////
// Base Area
////////////////////////////////////////////////////////////////////////////

// In a Horizontal/Vertical Layout, the following enum have the respective meanings:
// eFull: the object has the full height/width of the parent layout(minus margins)
// ePercentage: the object has a height/width that is a percentage of the parent layout(minus margins)
// eFix: the object has a fixed height/width

// Another way to see it
// eFix = The minor dimension of the object will remain what it is. But the positioning in the
//        minor dimension inside the layout can be controlled with eMinorPosition.
// eFull = The minor dimension of the object will take the entire size that is offered by the parent layout.
//         eMinorPosition has no effect if eMinorSize = eFull
// eContent = The minor dimension of the object will be set to 1 by its parent and later on, the minor dimemsion will be
//         resized larger by the children of the element if necessary.
//


  enum eMinorSize
  {
    eFull,
    ePercentage,
    eFix,
    eMatchContent,
  };

//! Policy for and element position in the minor dimension of a layout.
  enum eMinorPosition
  {
    eAbove,         //!< Place the element on the top side of the layout (Hlayout)
    eBelow,         //!< Place the element on the bottom side of the layout (Hlayout)
    eLeft,          //!< Place the element on the left side of the layout (Vlayout)
    eRight,         //!< Place the element on the right side of the layout (Hlayout)
    eCenter,        //!< Place the element at the center of the layout (Hlayout and VLayout)
  };

//! Stacking order inside a layout.
  typedef enum
  {
    eStackTop,      //!< Stack elements at the top (VLayout)
    eStackBottom,   //!< Stack elements at the bottom (VLayout)
    eStackLeft,     //!< Stack elements at the left (HLayout)
    eStackRight,    //!< Stack elements at the right (HLayout)
    eStackCenter,   //!< Stack elements in the center of the layout (HLayout and VLayout)
    eStackExpand,   //!< Spread elements evenly inside the layout (HLayout and VLayout)
  }  LayoutContentDistribution;

  enum eSizeCompliance
  {
    eCompliantWidth     = (1L),
    eCompliantHeight    = (1L) << 1,
    eSmallerWidth       = (1L) << 2,
    eSmallerHeight      = (1L) << 3,
    eLargerWidth        = (1L) << 4,
    eLargerHeight       = (1L) << 5,
    eForceComply        = (1L) << 6,
  };

  class Layout;
  class View;
  class Area;


  class Area: public InitiallyUnownedObject, public sigc::trackable
  {
  public:
    NUX_DECLARE_OBJECT_TYPE (Area, InitiallyUnownedObject);
    //static NObjectType StaticObjectType;
    //virtual NObjectType* Type() { return &StaticObjectType; }

  public:
    Area (NUX_FILE_LINE_DECL);
    virtual ~Area();

    virtual int GetBaseX     () const
    {
      return m_Geometry.x;
    }
    virtual int GetBaseY     () const
    {
      return m_Geometry.y;
    }
    virtual int GetBaseWidth    () const
    {
      return m_Geometry.GetWidth();
    }
    virtual int GetBaseHeight   () const
    {
      return m_Geometry.GetHeight();
    }

    virtual void SetBaseX    (int x)
    {
      m_Geometry.SetX (x);
    }
    virtual void SetBaseY    (int y)
    {
      m_Geometry.SetY (y);
    }
    virtual void SetBaseXY    (int x, int y)
    {
      m_Geometry.SetX (x);
      m_Geometry.SetY (y);
    }
    virtual void SetBaseWidth (int w);

    virtual void SetBaseHeight (int h);

    //! Set the size of the object.
    /*
        Set the size of the object.
        The size is adjusted to respect the min and max size policy
        \sa SetWidth(), SetHeight(), SetMinimumSize(), SetMaximumSize().
    */
    virtual void SetBaseSize (int w, int h);

    virtual void SetMinimumSize (int w, int h);
    virtual void SetMaximumSize (int w, int h);
    virtual void SetMinMaxSize (int w, int h);

    virtual void SetMinimumWidth (int w);
    virtual void SetMaximumWidth (int w);
    virtual void SetMinimumHeight (int h);
    virtual void SetMaximumHeight (int h);

    int GetMinimumWidth() const;
    int GetMaximumWidth() const;
    int GetMinimumHeight() const;
    int GetMaximumHeight() const;

    void ApplyMinWidth();
    void ApplyMinHeight();
    void ApplyMaxWidth();
    void ApplyMaxHeight();

    Size GetMinimumSize() const;
    Size GetMaximumSize() const;

    //! Get the geometry of the object.
    /*!
        @return The Geometry of the object.
        @sa GetBaseWidth(), GetBaseHeight(), GetBaseX(), GetBaseY().
    */
    virtual Geometry GetGeometry() const
    {
      return m_Geometry;
    }

    //! Set the geometry of the object.
    /*!
        Set the width, height, and x, y position of the object on the screen.
        \param x Horizontal position.
        \param y Vertical position.
        \param w Width.
        \param h Height.

        \sa SetBaseWidth(), SetBaseHeight(), SetBaseX(), SetBaseY().
    */
    virtual void SetGeometry (int x, int y, int w, int h);

    //! Set the geometry of the object.
    /*!
        This is an overloaded member function, provided for convenience.
        It behaves essentially like SetGeometry(int x, int y, int w, int h).
        \param geo Geometry object.
        \sa SetWidth(), SetHeight(), SetX(), SetY().
    */
    virtual void SetGeometry (const Geometry &geo);

    void IncreaseSize (int x, int y)
    {
      //m_Geometry.Width     += x;
      //m_Geometry.Height    += y;
      m_Geometry.OffsetPosition (x, y);
      OnResize.emit (m_Geometry.x, m_Geometry.y, m_Geometry.GetWidth(), m_Geometry.GetHeight() );
    }

    void SetBaseString (const TCHAR *Caption);
    const NString &GetBaseString() const;

    virtual long ComputeChildLayout()
    {
      return 0;
    }
    virtual void PositionChildLayout (float offsetX, float offsetY) {}
    virtual void DrawLayout() {};

    virtual long ComputeLayout2()
    {
      return (eCompliantWidth | eCompliantHeight);
    };
    virtual void ComputePosition2 (float offsetX, float offsetY) {};
    virtual bool IsLayout() const
    {
      return false;
    }
    virtual bool IsSpaceLayout() const
    {
      return false;
    }
    virtual bool IsArea() const
    {
      return false;
    }
    virtual bool IsView() const
    {
      return false;
    }

  protected:

    /*
        This function is reimplemented in Layout as it need to perform some special operations.
        It does nothing for Area and View classes.
    */
    //virtual void RemoveChildObject(smptr(Area));

    /*
        SetParentObject/UnParentObject are protected API. it is not meant to be used directly by users.
        Users add widgets to layouts and layout have to be attached to a composition for objects to be rendered.
        Setting a parent to and child widget does not mean that when the parent is rendered, the child is also rendered.
        For instance, setting a button the be the child of a checkbox means absolutely nothing is terms of rendering.
        A widget with a parent cannot be added to a added to a layout for rendering. The widget has to be unparented first.
        A layout with a parent cannot be added to a widget or another layout for rendering. The layout has to be unparented first.
        In essence only View and Layouts should be calling SetParentObject/UnParentObject.
    */
    virtual void SetParentObject (Area *);
    virtual void UnParentObject();

    Area *GetParentObject();

    //! Request a Layout recompute after a change of size
    /*
        When an object size changes, it is necessary for its parent structure to initiate a layout
        re computation in order preserve the layout structure defined by the user through the API.
    */
    virtual void RequestBottomUpLayoutComputation (Area *bo_initiator);

  private:
    //! Flags that set an object as dirty with regard to is size.
    /*
        Every time an object is resized (through SetGeometry, SetHeight or SetWidth), it become dirty.
        This flag is set to true in InitiateResizeLayout when the object request a parent layout to recompute itself.
        When a layout is computed, it resets the flag to false.
    */
    bool m_IsSizeDirty;

    //! Define a parent child structure
    /*
        An object of the class Area may have another of the class Layout as Parent.
        An object of the class View may have an object of the class Layout as parent.
        An object of the class Layout may have a parent of the class Layout or View as parent.
        A Area cannot have children (that may change later).
    */
    Area *m_ParentObject;

    void _SetBaseWidth (int w);
    void _SetBaseHeight (int h);

  private:
    WindowThread *m_Application;

  private:

    void InitiateResizeLayout (Area *child = 0);
    void CheckMinSize();
    void CheckMaxSize();

  protected:
    Geometry m_Geometry;
    NString m_BaseString;

    Size m_minSize;
    Size m_maxSize;
    //unsigned int m_StretchFactor;

  public:

    virtual unsigned int GetStretchFactor();
    virtual void SetStretchFactor (unsigned int sf);

    virtual eMinorPosition getPositioning()
    {
      return positioning;
    }
    virtual void setPositioning (eMinorPosition p)
    {
      positioning = p;
    }

    virtual eMinorSize GetExtend()
    {
      return extend;
    }
    virtual void SetExtend (eMinorSize ext)
    {
      extend = ext;
    }

    virtual float GetPercentage()
    {
      return percentage;
    }
    virtual void SetPercentage (float f)
    {
      percentage = f;
    }

    virtual bool isOutofBound()
    {
      return outofbound;
    }
    virtual void setOutofBound (bool b)
    {
      outofbound = b;
    }


    unsigned int    m_stretchfactor;
    eMinorPosition    positioning;
    eMinorSize         extend;
    float           percentage;
    bool            outofbound;
  public:
    sigc::signal<void, int, int, int, int> OnResize;

    friend class Layout;
    friend class View;
    friend class WindowThread;
    friend class HSplitter;
    friend class VSplitter;
    friend class WindowCompositor;
  };

}
#endif // BASEOBJECT_H

