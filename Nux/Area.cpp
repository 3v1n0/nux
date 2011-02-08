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
    ,   m_IsSizeDirty (true)
    ,   m_ParentObject (0)
    ,   m_Application (0)
    ,   m_Geometry (0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT)
    ,   m_minSize (AREA_MIN_WIDTH, AREA_MIN_HEIGHT)
    ,   m_maxSize (AREA_MAX_WIDTH, AREA_MAX_HEIGHT)
    ,   m_stretchfactor (1)
  {
  }


  Area::~Area()
  {
  }

  const NString &Area::GetBaseString() const
  {
    return m_BaseString;
  }

  void Area::SetBaseString (const TCHAR *Caption)
  {
    m_BaseString = Caption;
  }

  void Area::CheckMinSize()
  {
    int w = m_minSize.GetWidth();
    w = Max<int>(AREA_MIN_WIDTH, w);
    int h = m_minSize.GetHeight();
    h = Max<int>(AREA_MIN_HEIGHT, h);

    m_minSize.SetWidth(w);
    m_minSize.SetHeight(h);

    if (m_minSize.GetWidth() > m_maxSize.GetWidth())
    {
      //temp = m_maxSize.GetWidth();
      m_maxSize.SetWidth (m_minSize.GetWidth());
      //m_minSize.SetWidth(temp);
    }

    if (m_minSize.GetHeight() > m_maxSize.GetHeight())
    {
      //temp = m_maxSize.GetBaseHeight();
      m_maxSize.SetHeight (m_minSize.GetHeight());
      //m_minSize.SetHeight(temp);
    }

    if (m_Geometry.GetWidth() < m_minSize.GetWidth() )
    {
      m_Geometry.SetWidth (m_minSize.GetWidth() );
    }

    if (m_Geometry.GetHeight() < m_minSize.GetHeight() )
    {
      m_Geometry.SetHeight (m_minSize.GetHeight() );
    }
  }

  void Area::CheckMaxSize()
  {
    int w = m_maxSize.GetWidth();
    w = Min<int>(AREA_MAX_WIDTH, w);
    int h = m_maxSize.GetHeight();
    h = Min<int>(AREA_MAX_HEIGHT, h);

    m_maxSize.SetWidth(w);
    m_maxSize.SetHeight(h);

    if (m_minSize.GetWidth() > m_maxSize.GetWidth() )
    {
      //temp = m_maxSize.GetWidth();
      m_minSize.SetWidth (m_maxSize.GetWidth() );
      //m_minSize.SetWidth(temp);
    }

    if (m_minSize.GetHeight() > m_maxSize.GetHeight() )
    {
      //temp = m_maxSize.GetBaseHeight();
      m_minSize.SetHeight (m_maxSize.GetHeight() );
      //m_minSize.SetHeight(temp);
    }

    if (m_Geometry.GetWidth() > m_maxSize.GetWidth() )
    {
      m_Geometry.SetWidth (m_maxSize.GetWidth() );
    }

    if (m_Geometry.GetHeight() > m_maxSize.GetHeight() )
    {
      m_Geometry.SetHeight (m_maxSize.GetHeight() );
    }
  }

  void Area::SetMinimumSize (int w, int h)
  {
    nuxAssert (w >= 0);
    nuxAssert (h >= 0);
    m_minSize.SetWidth (w);
    m_minSize.SetHeight (h);

    CheckMinSize();

    InitiateResizeLayout();
  }

  void Area::SetMaximumSize (int w, int h)
  {
    nuxAssert (w >= 0);
    nuxAssert (h >= 0);
    m_maxSize.SetWidth (w);
    m_maxSize.SetHeight (h);

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
    m_Geometry.SetWidth (m_minSize.GetWidth() );

    InitiateResizeLayout();
  }

  void Area::ApplyMinHeight()
  {
    m_Geometry.SetHeight (m_minSize.GetHeight() );

    InitiateResizeLayout();
  }

  void Area::ApplyMaxWidth()
  {
    m_Geometry.SetWidth (m_maxSize.GetWidth() );

    InitiateResizeLayout();
  }

  void Area::ApplyMaxHeight()
  {
    m_Geometry.SetHeight (m_maxSize.GetHeight() );

    InitiateResizeLayout();
  }

  Size Area::GetMinimumSize() const
  {
    return m_minSize;
  }

  Size Area::GetMaximumSize() const
  {
    return m_maxSize;
  }

  void Area::SetMinimumWidth (int w)
  {
    nuxAssert (w >= 0);
    m_minSize.SetWidth (w);
    CheckMinSize();
    InitiateResizeLayout();
  }

  void Area::SetMaximumWidth (int w)
  {
    nuxAssert (w >= 0);
    m_maxSize.SetWidth (w);
    CheckMaxSize();
    InitiateResizeLayout();
  }

  void Area::SetMinimumHeight (int h)
  {
    nuxAssert (h >= 0);
    m_minSize.SetHeight (h);
    CheckMinSize();
    InitiateResizeLayout();
  }

  void Area::SetMaximumHeight (int h)
  {
    nuxAssert (h >= 0);
    m_maxSize.SetHeight (h);
    CheckMaxSize();
    InitiateResizeLayout();
  }

  int Area::GetMinimumWidth() const
  {
    return m_minSize.GetWidth();
  }

  int Area::GetMaximumWidth() const
  {
    return m_maxSize.GetWidth();
  }

  int Area::GetMinimumHeight() const
  {
    return m_minSize.GetHeight();
  }

  int Area::GetMaximumHeight() const
  {
    return m_maxSize.GetHeight();
  }

  unsigned int Area::GetStretchFactor()
  {
    return m_stretchfactor;
  }

  void Area::SetStretchFactor (unsigned int sf)
  {
    // re implemented by Layout
    m_stretchfactor = sf;
  }

  void Area::SetParentObject (Area *bo)
  {
    if (bo == 0)
    {
      nuxAssertMsg (0, TEXT ("[Area::SetParentObject] Invalid parent obejct.") );
      return;
    }

    if (m_ParentObject)
    {
      nuxAssertMsg (0, TEXT ("[Area::SetParentObject] Object already has a parent. You must UnParent the object before you can parenting again.") );
      return;
    }

    m_ParentObject = bo;
    Reference();
  }

  void Area::UnParentObject()
  {
    if (m_ParentObject)
    {
      m_ParentObject = 0;
      UnReference();
    }
  }

  Area *Area::GetParentObject()
  {
    return m_ParentObject;
  }

  void Area::SetGeometry (int x, int y, int w, int h)
  {
    h = nux::Clamp<int> (h, m_minSize.GetHeight (), m_maxSize.GetHeight ());
    w = nux::Clamp<int> (w, m_minSize.GetWidth (), m_maxSize.GetWidth ());
    
    if (m_Geometry.x == x && m_Geometry.y == y && m_Geometry.width == w && m_Geometry.height == h)
      return;
    
    GeometryChangePending ();
    m_Geometry = nux::Geometry (x, y, w, h);
    InitiateResizeLayout();
    GeometryChanged ();
  }

  void Area::SetGeometry (const Geometry &geo)
  {
    SetGeometry (geo.x, geo.y, geo.width, geo.height);
  }
  
  void Area::SetBaseX    (int x)
  {
    SetGeometry (x, m_Geometry.y, m_Geometry.width, m_Geometry.height);
  }
  
  void Area::SetBaseY    (int y)
  {
    SetGeometry (m_Geometry.x, y, m_Geometry.width, m_Geometry.height);
  }
  
  void Area::SetBaseXY    (int x, int y)
  {
    SetGeometry (x, y, m_Geometry.width, m_Geometry.height);
  }

  void Area::SetBaseSize (int w, int h)
  {
    SetGeometry (m_Geometry.x, m_Geometry.y, w, h);
  }

  void Area::SetBaseWidth (int w)
  {
    SetGeometry (m_Geometry.x, m_Geometry.y, w, m_Geometry.height);
  }

  void Area::SetBaseHeight (int h)
  {
    SetGeometry (m_Geometry.x, m_Geometry.y, m_Geometry.width, h);
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
      else if (ic->m_ParentObject)
        ic->m_ParentObject->InitiateResizeLayout (this);
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

      if (layout->m_ParentObject)
      {
        if (layout->m_ParentObject->Type().IsDerivedFromType (View::StaticObjectType) )
        {
          View *ic = (View *) (layout->m_ParentObject);

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
            layout->m_ParentObject->InitiateResizeLayout (this);
          }
        }
        else
        {
          layout->m_ParentObject->InitiateResizeLayout (this);
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
      // The object that is being resized is a CoreArea object.
      if (this->m_ParentObject)
      {
        // The object that is being resized is a View object and it has a parent.
        if (this->OwnsTheReference() == false && this->GetParentObject())
        {
          // Only reference parented areas.
          this->Reference();
        }

        // The parent object of an object of type CoreArea is a Layout object type.
        this->m_ParentObject->InitiateResizeLayout (this);
      }
    }
  }

  void Area::RequestBottomUpLayoutComputation (Area *bo_initiator)
  {
    if (m_ParentObject && m_ParentObject->IsLayout() )
    {
      m_ParentObject->RequestBottomUpLayoutComputation (bo_initiator);
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
}
