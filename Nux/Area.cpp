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


#include "Nux.h"
#include "Area.h"
#include "NuxGraphics/GraphicsEngine.h"
#include "Layout.h"
#include "VSplitter.h"
#include "HSplitter.h"
#include "BaseWindow.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (Area);

  Area::Area (NUX_FILE_LINE_DECL)
    :   InitiallyUnownedObject (NUX_FILE_LINE_PARAM)
    ,   _parent_area (0)
    ,   _layout_properties (NULL)
    ,   m_visible (true)
    ,   m_sensitive (true)
    ,   _geometry (0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT)
    ,   _min_size (AREA_MIN_WIDTH, AREA_MIN_HEIGHT)
    ,   _max_size (AREA_MAX_WIDTH, AREA_MAX_HEIGHT)
    ,   _stretch_factor (1)
  {
  }


  Area::~Area()
  {
    if (_layout_properties)
      delete _layout_properties;
  }

  const NString &Area::GetBaseString() const
  {
    return _base_string;
  }

  void Area::SetBaseString (const TCHAR *Caption)
  {
    _base_string = Caption;
  }

  void Area::CheckMinSize()
  {
    int w = _min_size.GetWidth();
    w = Max<int>(AREA_MIN_WIDTH, w);
    int h = _min_size.GetHeight();
    h = Max<int>(AREA_MIN_HEIGHT, h);

    _min_size.SetWidth(w);
    _min_size.SetHeight(h);

    if (_min_size.GetWidth() > _max_size.GetWidth())
    {
      //temp = _max_size.GetWidth();
      _max_size.SetWidth (_min_size.GetWidth());
      //_min_size.SetWidth(temp);
    }

    if (_min_size.GetHeight() > _max_size.GetHeight())
    {
      //temp = _max_size.GetBaseHeight();
      _max_size.SetHeight (_min_size.GetHeight());
      //_min_size.SetHeight(temp);
    }

    if (_geometry.GetWidth() < _min_size.GetWidth() )
    {
      _geometry.SetWidth (_min_size.GetWidth() );
    }

    if (_geometry.GetHeight() < _min_size.GetHeight() )
    {
      _geometry.SetHeight (_min_size.GetHeight() );
    }
  }

  void Area::CheckMaxSize()
  {
    int w = _max_size.GetWidth();
    w = Min<int>(AREA_MAX_WIDTH, w);
    int h = _max_size.GetHeight();
    h = Min<int>(AREA_MAX_HEIGHT, h);

    _max_size.SetWidth(w);
    _max_size.SetHeight(h);

    if (_min_size.GetWidth() > _max_size.GetWidth() )
    {
      //temp = _max_size.GetWidth();
      _min_size.SetWidth (_max_size.GetWidth() );
      //_min_size.SetWidth(temp);
    }

    if (_min_size.GetHeight() > _max_size.GetHeight() )
    {
      //temp = _max_size.GetBaseHeight();
      _min_size.SetHeight (_max_size.GetHeight() );
      //_min_size.SetHeight(temp);
    }

    if (_geometry.GetWidth() > _max_size.GetWidth() )
    {
      _geometry.SetWidth (_max_size.GetWidth() );
    }

    if (_geometry.GetHeight() > _max_size.GetHeight() )
    {
      _geometry.SetHeight (_max_size.GetHeight() );
    }
  }

  void Area::SetMinimumSize (int w, int h)
  {
    nuxAssert (w >= 0);
    nuxAssert (h >= 0);
    _min_size.SetWidth (w);
    _min_size.SetHeight (h);

    CheckMinSize();

    InitiateResizeLayout();
  }

  void Area::SetMaximumSize (int w, int h)
  {
    nuxAssert (w >= 0);
    nuxAssert (h >= 0);
    _max_size.SetWidth (w);
    _max_size.SetHeight (h);

    CheckMaxSize();

    InitiateResizeLayout();
  }

  void Area::SetMinMaxSize (int w, int h)
  {
    nuxAssert (w >= 0);
    nuxAssert (h >= 0);
    SetMinimumSize (w, h);
    SetMaximumSize (w, h);

    //InitiateResizeLayout();
  }

  void Area::ApplyMinWidth()
  {
    _geometry.SetWidth (_min_size.GetWidth() );

    InitiateResizeLayout();
  }

  void Area::ApplyMinHeight()
  {
    _geometry.SetHeight (_min_size.GetHeight() );

    InitiateResizeLayout();
  }

  void Area::ApplyMaxWidth()
  {
    _geometry.SetWidth (_max_size.GetWidth() );

    InitiateResizeLayout();
  }

  void Area::ApplyMaxHeight()
  {
    _geometry.SetHeight (_max_size.GetHeight() );

    InitiateResizeLayout();
  }

  Size Area::GetMinimumSize() const
  {
    return _min_size;
  }

  Size Area::GetMaximumSize() const
  {
    return _max_size;
  }

  void Area::SetMinimumWidth (int w)
  {
    nuxAssert (w >= 0);
    _min_size.SetWidth (w);
    CheckMinSize();
    InitiateResizeLayout();
  }

  void Area::SetMaximumWidth (int w)
  {
    nuxAssert (w >= 0);
    _max_size.SetWidth (w);
    CheckMaxSize();
    InitiateResizeLayout();
  }

  void Area::SetMinimumHeight (int h)
  {
    nuxAssert (h >= 0);
    _min_size.SetHeight (h);
    CheckMinSize();
    InitiateResizeLayout();
  }

  void Area::SetMaximumHeight (int h)
  {
    nuxAssert (h >= 0);
    _max_size.SetHeight (h);
    CheckMaxSize();
    InitiateResizeLayout();
  }

  int Area::GetMinimumWidth() const
  {
    return _min_size.GetWidth();
  }

  int Area::GetMaximumWidth() const
  {
    return _max_size.GetWidth();
  }

  int Area::GetMinimumHeight() const
  {
    return _min_size.GetHeight();
  }

  int Area::GetMaximumHeight() const
  {
    return _max_size.GetHeight();
  }

  unsigned int Area::GetStretchFactor()
  {
    return _stretch_factor;
  }

  void Area::SetStretchFactor (unsigned int sf)
  {
    // re implemented by Layout
    _stretch_factor = sf;
  }

  void Area::SetParentObject (Area *bo)
  {
    if (bo == 0)
    {
      nuxAssertMsg (0, TEXT ("[Area::SetParentObject] Invalid parent obejct.") );
      return;
    }

    if (_parent_area)
    {
      nuxAssertMsg (0, TEXT ("[Area::SetParentObject] Object already has a parent. You must UnParent the object before you can parenting again.") );
      return;
    }

    _parent_area = bo;
    Reference();
  }

  void Area::UnParentObject()
  {
    if (_parent_area)
    {
      _parent_area = 0;
      UnReference();
    }
  }

  Area *Area::GetParentObject()
  {
    return _parent_area;
  }

  int Area::GetBaseX     () const
  {
    return _geometry.x;
  }

  int Area::GetBaseY     () const
  {
    return _geometry.y;
  }

  int Area::GetBaseWidth    () const
  {
    return _geometry.GetWidth();
  }

  int Area::GetBaseHeight   () const
  {
    return _geometry.GetHeight();
  }

  void Area::SetGeometry (int x, int y, int w, int h)
  {
    h = nux::Clamp<int> (h, _min_size.GetHeight (), _max_size.GetHeight ());
    w = nux::Clamp<int> (w, _min_size.GetWidth (), _max_size.GetWidth ());
    
    if (_geometry.x == x && _geometry.y == y && _geometry.width == w && _geometry.height == h)
      return;
    
    GeometryChangePending ();
    _geometry = nux::Geometry (x, y, w, h);
    InitiateResizeLayout();
    GeometryChanged ();
  }

  void Area::SetGeometry (const Geometry &geo)
  {
    SetGeometry (geo.x, geo.y, geo.width, geo.height);
  }
  
  Geometry Area::GetGeometry() const
  {
    return _geometry;
  }

  void Area::SetBaseX    (int x)
  {
    SetGeometry (x, _geometry.y, _geometry.width, _geometry.height);
  }
  
  void Area::SetBaseY    (int y)
  {
    SetGeometry (_geometry.x, y, _geometry.width, _geometry.height);
  }
  
  void Area::SetBaseXY    (int x, int y)
  {
    SetGeometry (x, y, _geometry.width, _geometry.height);
  }

  void Area::SetBaseSize (int w, int h)
  {
    SetGeometry (_geometry.x, _geometry.y, w, h);
  }

  void Area::SetBaseWidth (int w)
  {
    SetGeometry (_geometry.x, _geometry.y, w, _geometry.height);
  }

  void Area::SetBaseHeight (int h)
  {
    SetGeometry (_geometry.x, _geometry.y, _geometry.width, h);
  }

  void Area::IncreaseSize (int x, int y)
  {
    _geometry.OffsetPosition (x, y);
    OnResize.emit (_geometry.x, _geometry.y, _geometry.GetWidth(), _geometry.GetHeight() );
  }

  long Area::ComputeChildLayout()
  {
    return 0;
  }

  void Area::PositionChildLayout (float offsetX, float offsetY)
  {

  }

  long Area::ComputeLayout2()
  {
    return (eCompliantWidth | eCompliantHeight);
  }

  void Area::ComputePosition2 (float offsetX, float offsetY)
  {

  }

  void Area::InitiateResizeLayout (Area *child)
  {
    if (GetWindowThread ()->IsComputingLayout() )
    {
      // there is no need to do the following while we are already computing the layout.
      // If we do, we will end up in an infinite loop.
      return;
    }

    if (this->Type().IsDerivedFromType (View::StaticObjectType) )
    {
      // The object that is being resized is a View object and it has a parent.
      if (this->OwnsTheReference() == false && this->GetParentObject())
      {
        // Only reference parented areas.
        this->Reference();
      }

      View *ic = NUX_STATIC_CAST (View *, this);

      if (ic->CanBreakLayout() )
      {

        if ( (child != 0) &&
             (ic->Type().IsObjectType (VSplitter::StaticObjectType) || ic->Type().IsObjectType (HSplitter::StaticObjectType) ) )
        {
          // If this element is a Splitter, then we submit its child to the refresh list. We don't want to submit the
          // splitter because this will cause a redraw of all parts of the splitter (costly and unnecessary).
          GetWindowThread ()->QueueObjectLayout (child);
        }
        else
        {
          GetWindowThread ()->QueueObjectLayout (ic);
        }
      }
      else if (ic->_parent_area)
        ic->_parent_area->InitiateResizeLayout (this);
      else
      {
        GetWindowThread ()->QueueObjectLayout (ic);
      }
    }
    else if (this->Type().IsDerivedFromType (Layout::StaticObjectType) )
    {
      // The object that is being resized is a View object and it has a parent.
      if (this->OwnsTheReference() == false && this->GetParentObject())
      {
        // Only reference parented areas.
        this->Reference();
      }

      Layout *layout = NUX_STATIC_CAST (Layout *, this);

      if (layout->_parent_area)
      {
        if (layout->_parent_area->Type().IsDerivedFromType (View::StaticObjectType) )
        {
          View *ic = (View *) (layout->_parent_area);

          if (ic->CanBreakLayout() )
          {
            if ( (child != 0) &&
                 (ic->Type().IsObjectType (VSplitter::StaticObjectType) || ic->Type().IsObjectType (HSplitter::StaticObjectType) ) )
            {
              // If the parent of this element is a splitter, then we submit its child to the refresh list. We don't want to submit the
              // splitter because this will cause a redraw of all parts of the splitter (costly and unnecessary).
              GetWindowThread ()->QueueObjectLayout (this);
            }
            else
            {
              GetWindowThread ()->QueueObjectLayout (ic);
            }
          }
          else
          {
            // The parent object of an object of type View is a Layout object type.
            layout->_parent_area->InitiateResizeLayout (this);
          }
        }
        else
        {
          layout->_parent_area->InitiateResizeLayout (this);
        }
      }
      else
      {
        // This is possibly the Main layout or the layout of a floating object (popup for example) unless the layout is not part of the object tree.
        GetWindowThread ()->QueueObjectLayout (layout);
      }
    }
    else
    {
      // The object that is being resized is a InputArea object.
      if (this->_parent_area)
      {
        // The object that is being resized is a View object and it has a parent.
        if (this->OwnsTheReference() == false && this->GetParentObject())
        {
          // Only reference parented areas.
          this->Reference();
        }

        // The parent object of an object of type InputArea is a Layout object type.
        this->_parent_area->InitiateResizeLayout (this);
      }
    }
  }

  void Area::RequestBottomUpLayoutComputation (Area *bo_initiator)
  {
    if (_parent_area && _parent_area->IsLayout() )
    {
      _parent_area->RequestBottomUpLayoutComputation (bo_initiator);
    }
  }
  
  Area * Area::GetToplevel ()
  {
    if (Type ().IsDerivedFromType (BaseWindow::StaticObjectType) || this == GetWindowThread ()->GetMainLayout ())
      return this;

    Area * parent = GetParentObject ();
    if (!parent) //we didn't find a way to salvation!
      return 0;
    return parent->GetToplevel ();
  }

  void Area::SetLayoutProperties (LayoutProperties *properties)
  {
    if (_layout_properties)
      delete _layout_properties;

    _layout_properties = properties;
  }

  Area::LayoutProperties * Area::GetLayoutProperties ()
  {
    return _layout_properties;
  }

  void Area::SetVisible (bool visible)
  {
    if (m_visible == visible)
      return;

    m_visible = visible;

    OnVisibleChanged.emit (this, m_visible);
  }

  bool Area::IsVisible ()
  {
    return m_visible;
  }

  void Area::SetSensitive (bool sensitive)
  {
    if (m_sensitive == sensitive)
      return;

    m_sensitive = sensitive;

    OnSensitiveChanged.emit (this, m_sensitive);
  }

  bool Area::IsSensitive ()
  {
    return m_sensitive;
  }

  MinorDimensionPosition Area::GetPositioning()
  {
    return _positioning;
  }

  void Area::SetPositioning (MinorDimensionPosition p)
  {
    _positioning = p;
  }

  MinorDimensionSize Area::GetExtend()
  {
    return _extend;
  }
  
  void Area::SetExtend (MinorDimensionSize ext)
  {
    _extend = ext;
  }

  float Area::GetPercentage()
  {
    return _percentage;
  }
  
  void Area::SetPercentage (float p)
  {
    _percentage = p;
  }

  bool Area::IsLayoutDone()
  {
    return _layout_done;
  }
  
  void Area::SetLayoutDone (bool b)
  {
    _layout_done = b;
  }

  bool Area::IsArea() const
  {
    return this->Type().IsDerivedFromType(Area::StaticObjectType);;
  }

  bool Area::IsInputArea() const
  {
    return this->Type().IsDerivedFromType(InputArea::StaticObjectType);;
  }

  bool Area::IsView() const
  {
    return this->Type().IsDerivedFromType(View::StaticObjectType);;
  }

  bool Area::IsLayout() const
  {
    return this->Type().IsDerivedFromType(Layout::StaticObjectType);
  }

  bool Area::IsSpaceLayout() const
  {
    return this->Type().IsDerivedFromType(SpaceLayout::StaticObjectType);;
  }

}
