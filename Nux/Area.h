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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include "NuxCore/InitiallyUnownedObject.h"

#include "Utils.h"
#include "WidgetMetrics.h"
#include "WidgetSmartPointer.h"



namespace nux
{
  class WindowThread;
  class GraphicsEngine;

// In a Horizontal/Vertical Layout, the following enum have the respective meanings:
// eFull: the object has the full height/width of the parent layout(minus margins)
// ePercentage: the object has a height/width that is a percentage of the parent layout(minus margins)
// eFix: the object has a fixed height/width

// Another way to see it
// eFix = The minor dimension of the object will remain what it is. But the positioning in the
//        minor dimension inside the layout can be controlled with MinorDimensionPosition.
// eFull = The minor dimension of the object will take the entire size that is offered by the parent layout.
//         MinorDimensionPosition has no effect if MinorDimensionSize = eFull
// eContent = The minor dimension of the object will be set to 1 by its parent and later on, the minor dimemsion will be
//         resized larger by the children of the element if necessary.
//


  typedef enum
  {
    MINOR_SIZE_FULL,
    MINOR_SIZE_PERCENTAGE,
    MINOR_SIZE_FIX,
    MINOR_SIZE_MATCHCONTENT,
    eFull = MINOR_SIZE_FULL,                  //!< Deprecated.
    ePercentage = MINOR_SIZE_PERCENTAGE,      //!< Deprecated.
    eFix  = MINOR_SIZE_FIX,                   //!< Deprecated.
    eMatchContent = MINOR_SIZE_MATCHCONTENT,  //!< Deprecated.
  } MinorDimensionSize;

//! Policy for and element position in the minor dimension of a layout.
  typedef enum
  {
    MINOR_POSITION_TOP,           //!< Place the element on the top side of the layout (Hlayout)
    MINOR_POSITION_BOTTOM,        //!< Place the element on the bottom side of the layout (Hlayout)
    MINOR_POSITION_LEFT,          //!< Place the element on the left side of the layout (Vlayout)
    MINOR_POSITION_RIGHT,         //!< Place the element on the right side of the layout (Hlayout)
    MINOR_POSITION_CENTER,        //!< Place the element at the center of the layout (Hlayout and VLayout)
    eAbove = MINOR_POSITION_TOP,      //!< Deprecated.
    eBelow = MINOR_POSITION_BOTTOM,   //!< Deprecated.
    eLeft = MINOR_POSITION_LEFT,      //!< Deprecated.
    eRight = MINOR_POSITION_RIGHT,    //!< Deprecated.
    eCenter = MINOR_POSITION_CENTER,  //!< Deprecated.
  } MinorDimensionPosition;

  //! Control how a layout distribute its children position.
  /*!
      Applies to layouts that have more space than their children can use.
      These options control how a layouts places its children inside of itself.
  */
  typedef enum
  {
    MAJOR_POSITION_TOP,      //!< Stack elements at the top (for VLayout only).
    MAJOR_POSITION_BOTTOM,   //!< Stack elements at the bottom (for VLayout only).
    MAJOR_POSITION_LEFT,     //!< Stack elements at the left (for HLayout only).
    MAJOR_POSITION_RIGHT,    //!< Stack elements at the right (for HLayout only).
    MAJOR_POSITION_CENTER,   //!< Stack elements in the center of the layout (for HLayout and VLayout).
    MAJOR_POSITION_EXPAND,   //!< Spread elements evenly inside the layout (for HLayout and VLayout).

    eStackTop = MAJOR_POSITION_TOP,         //!< Deprecated.
    eStackBottom = MAJOR_POSITION_BOTTOM,   //!< Deprecated.
    eStackLeft = MAJOR_POSITION_LEFT,       //!< Deprecated.
    eStackRight = MAJOR_POSITION_RIGHT,     //!< Deprecated.
    eStackCenter = MAJOR_POSITION_CENTER,   //!< Deprecated.
    eStackExpand = MAJOR_POSITION_EXPAND,   //!< Deprecated.
  }  LayoutContentDistribution;

  //! For internal use only.
  /*!
      For internal use only.
  */
  typedef enum
  {
    SIZE_EQUAL_WIDTH     = (1L),
    SIZE_EQUAL_HEIGHT    = (1L) << 1,
    SIZE_SMALLER_WIDTH   = (1L) << 2,
    SIZE_SMALLER_HEIGHT  = (1L) << 3,
    SIZE_LARGER_WIDTH    = (1L) << 4,
    SIZE_LARGER_HEIGHT   = (1L) << 5,
    SIZE_FORCE_COMPLY    = (1L) << 6,

    eCompliantWidth     = SIZE_EQUAL_WIDTH,     //!< Deprecated.
    eCompliantHeight    = SIZE_EQUAL_HEIGHT,    //!< Deprecated.
    eSmallerWidth       = SIZE_SMALLER_WIDTH,   //!< Deprecated.
    eSmallerHeight      = SIZE_SMALLER_HEIGHT,  //!< Deprecated.
    eLargerWidth        = SIZE_LARGER_WIDTH,    //!< Deprecated.
    eLargerHeight       = SIZE_LARGER_HEIGHT,   //!< Deprecated.
    eForceComply        = SIZE_FORCE_COMPLY,    //!< Deprecated.
  } SizeCompliance;

  class Layout;
  class View;
  class Area;

  class Area: public InitiallyUnownedObject
  {
  public:
    NUX_DECLARE_OBJECT_TYPE (Area, InitiallyUnownedObject);
    //static NObjectType StaticObjectType;
    //virtual NObjectType* Type() { return &StaticObjectType; }

    class LayoutProperties
    {
      public:
      virtual ~LayoutProperties ()
      {

      }
    };

  public:
    Area (NUX_FILE_LINE_DECL);
    virtual ~Area();

    int GetBaseX() const;
    int GetBaseY() const;
    int GetBaseWidth() const;
    int GetBaseHeight() const;
    void SetBaseX(int x);
    void SetBaseY(int y);
    void SetBaseXY(int x, int y);
    void SetBaseWidth(int w);
    void SetBaseHeight(int h);

    //! Set the size of the object.
    /*
        Set the size of the object.
        The size is adjusted to respect the min and max size policy
        \sa SetWidth(), SetHeight(), SetMinimumSize(), SetMaximumSize().
    */
    void SetBaseSize (int w, int h);

    void SetMinimumSize(int w, int h);
    void SetMaximumSize(int w, int h);
    void SetMinMaxSize(int w, int h);

    void SetMinimumWidth(int w);
    void SetMaximumWidth(int w);
    void SetMinimumHeight(int h);
    void SetMaximumHeight(int h);

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
    Geometry GetGeometry() const;

    //! Set the geometry of the object.
    /*!
        Set the width, height, and x, y position of the object on the screen.
        \param x Horizontal position.
        \param y Vertical position.
        \param w Width.
        \param h Height.

        \sa SetBaseWidth(), SetBaseHeight(), SetBaseX(), SetBaseY().
    */
    void SetGeometry (int x, int y, int w, int h);

    //! Set the geometry of the object.
    /*!
        This is an overloaded member function, provided for convenience.
        It behaves essentially like SetGeometry(int x, int y, int w, int h).
        \param geo Geometry object.
        \sa SetWidth(), SetHeight(), SetX(), SetY().
    */
    void SetGeometry (const Geometry &geo);

    void IncreaseSize (int x, int y);

    void SetBaseString (const TCHAR *Caption);
    const NString &GetBaseString() const;
    
    Area * GetToplevel ();

    virtual long ComputeChildLayout();
    virtual void PositionChildLayout (float offsetX, float offsetY);
    virtual long ComputeLayout2();
    virtual void ComputePosition2 (float offsetX, float offsetY);
    
    virtual bool IsArea() const;
    virtual bool IsInputArea() const;
    virtual bool IsView() const;
    virtual bool IsLayout() const;
    virtual bool IsSpaceLayout() const;

    //! Set the layout properties for this area
    /*!
    Allows the Layout managing this area to store the properties specifc to this area. Layouts
    should create a sub-class of LayoutProperties. The LayoutProperties of an area will
    be deleted upon destruction.
    \param properties the LayoutProperties sub-class  associated with this area. Can be NULL to
     unset.
    */
    void SetLayoutProperties (LayoutProperties *properties);

    //! Get the layout properties for this area
    /*!
    Retrieves the LayoutProperties sub-class with this area. See SetLayoutProperties
    \return LayoutProperties sub-class associated with this area.
    */
    LayoutProperties * GetLayoutProperties ();

    Area * GetParentObject();

    //! Set visibility of the area
    /*!
        If visible, an area will be drawn. Default: true.
        @param visible if the area is visible to the user
    */
    void SetVisible  (bool visible);

    //! Get the visibility of the area
    /*!
        Gets whether the area is visible to the user and will be visible to the user. Default is true.
        @return whether the area is visible
    */
    bool IsVisible ();

    //! Set sensitivity of the area
    /*!
        If sensitive, an area will receive input events. Default is true.
        @param  if the area should receive input events
    */
    void SetSensitive  (bool sensitive);

    //! Get whether the area is sensitive
    /*!
        Gets whether the area is sensitive to input events
        @return whether the area is visible
    */
    bool IsSensitive ();

    virtual unsigned int GetStretchFactor();
    virtual void SetStretchFactor (unsigned int sf);

    virtual MinorDimensionPosition GetPositioning();
    virtual void SetPositioning (MinorDimensionPosition p);

    virtual MinorDimensionSize GetExtend();
    virtual void SetExtend (MinorDimensionSize ext);

    virtual float GetPercentage();
    virtual void SetPercentage (float f);
    virtual bool IsLayoutDone();
    virtual void SetLayoutDone (bool b);

    sigc::signal<void, int, int, int, int> OnResize; //!< Signal emitted when an area is resized.
    sigc::signal<void, Area *, bool> OnVisibleChanged;
    sigc::signal<void, Area *, bool> OnSensitiveChanged;

  protected:
    
    //unsigned int m_StretchFactor;

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
    
    virtual void GeometryChangePending () {}
    virtual void GeometryChanged () {}

    //! Request a Layout recompute after a change of size
    /*
        When an object size changes, it is necessary for its parent structure to initiate a layout
        re computation in order preserve the layout structure defined by the user through the API.
    */
    virtual void RequestBottomUpLayoutComputation (Area *bo_initiator);

  private:
    void InitiateResizeLayout (Area *child = 0);
    void CheckMinSize();
    void CheckMaxSize();


    Geometry                _geometry;        //!< The area geometry.

    //! Define a parent child structure
    /*
        An object of the class Area may have another of the class Layout as Parent.
        An object of the class View may have an object of the class Layout as parent.
        An object of the class Layout may have a parent of the class Layout or View as parent.
        A Area cannot have children (that may change later).
    */
    Area                    *_parent_area;

    LayoutProperties        *_layout_properties;
    bool                    m_visible;
    bool                    m_sensitive;

    NString                 _base_string;     //!< A text string property for this area.

    Size                    _min_size;        //!< A text string property for this area.
    Size                    _max_size;        //!< A text string property for this area.

    unsigned int            _stretch_factor;  //!< Factor for element expansion.
    MinorDimensionPosition  _positioning;     //!< Area position hint
    MinorDimensionSize      _extend;          //!< Area dimension hint
    float                   _percentage;      //!< Area size percentage value.
    bool                    _layout_done;     //!< Area layout status flag.

    friend class Layout;
    friend class View;
    friend class WindowThread;
    friend class HSplitter;
    friend class VSplitter;
    friend class WindowCompositor;
  };

}
#endif // BASEOBJECT_H

