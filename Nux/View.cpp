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
#include "WindowCompositor.h"
#include "Layout.h"
#include "View.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (View);

  View::View (NUX_FILE_LINE_DECL)
    :   InputArea (NUX_FILE_LINE_PARAM)
  {
    m_CompositionLayout = 0;
    m_NeedRedraw        = false;
    m_UseStyleDrawing   = true;
    m_TextColor         = Color (1.0f, 1.0f, 1.0f, 1.0f);
    m_IsEnabled         = true;
    m_font              = GetSysFont();

    // Set widget default size;
    SetMinimumSize (DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
  }

  View::~View()
  {
    // It is possible that the object is in the refresh list. Remove it here before it is deleted.
    GetGraphicsThread()->RemoveObjectFromRefreshList (this);

    if (m_CompositionLayout)
      m_CompositionLayout->UnParentObject();
  }

  long View::BaseProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    if ((GetWindowCompositor ().GetExclusiveInputArea () == this) && (!(ProcessEventInfo & EVENT_CYCLE_EXCLUSIVE)))
    {
      // Skip the area that has the exclusivity on events
      return TraverseInfo;
    }

    return ProcessEvent (ievent, TraverseInfo, ProcessEventInfo);
  }

  long View::ComputeChildLayout()
  {
    if (m_CompositionLayout)
    {
      m_CompositionLayout->SetDirty (true);
//        if(m_CompositionLayout->GetStretchFactor() != 0)
//        {
//            PreLayoutManagement();
//            long ret = m_CompositionLayout->ComputeLayout2();
//            return PostLayoutManagement(ret);
//        }
//        else
      {
        PreLayoutManagement();

        int PreWidth = /*m_CompositionLayout->*/GetBaseWidth();
        int PreHeight = /*m_CompositionLayout->*/GetBaseHeight();

        long ret = m_CompositionLayout->ComputeLayout2();

        PostLayoutManagement (ret);
        //return eCompliantWidth | eCompliantHeight;

        int PostWidth = /*m_CompositionLayout->*/GetBaseWidth();
        int PostHeight = /*m_CompositionLayout->*/GetBaseHeight();

        long size_compliance = 0;

        // The layout has been resized to tightly pack its content
        if (PostWidth > PreWidth)
        {
          size_compliance |= eLargerWidth; // need scrollbar
        }
        else if (PostWidth < PreWidth)
        {
          size_compliance |= eSmallerWidth;
        }
        else
        {
          size_compliance |= eCompliantWidth;
        }

        // The layout has been resized to tightly pack its content
        if (PostHeight > PreHeight)
        {
          size_compliance |= eLargerHeight; // need scrollbar
        }
        else if (PostHeight < PreHeight)
        {
          size_compliance |= eSmallerHeight;
        }
        else
        {
          size_compliance |= eCompliantHeight;
        }

        //Area::SetGeometry(m_CompositionLayout->GetGeometry());
        return size_compliance;
      }
    }
    else
    {
      PreLayoutManagement();
      int ret = PostLayoutManagement (eCompliantHeight | eCompliantWidth);
      return ret;
    }

    return 0;
  }

  void View::PositionChildLayout (float offsetX, float offsetY)
  {
    if (m_CompositionLayout)
    {
      // This section from //1 to //2 is not needed. here we should not do any size management. Only position..
      //1
      if (m_CompositionLayout->GetStretchFactor() != 0)
      {
        m_CompositionLayout->SetGeometry (GetGeometry() );
      }
      else //2
      {
        m_CompositionLayout->SetBaseX (GetBaseX() );
        m_CompositionLayout->SetBaseY (GetBaseY() );
      }

      m_CompositionLayout->ComputePosition2 (offsetX, offsetY);

    }
  }

  void View::PreLayoutManagement()
  {
    // Give the managed layout the same size and position as the Control.
    if (m_CompositionLayout)
      m_CompositionLayout->SetGeometry (GetGeometry() );
  }

  long View::PostLayoutManagement (long LayoutResult)
  {
    // Set the geometry of the control to be the same as the managed layout.
    // Only the size is changed. The position of the composition layout hasn't
    // been changed by ComputeLayout2.
    if (m_CompositionLayout)
    {
      // If The layout is empty, do not change the size of the parent element.
      if (!m_CompositionLayout->IsEmpty() )
        Area::SetGeometry (m_CompositionLayout->GetGeometry() );
    }

    return LayoutResult;
  }

  void View::PreResizeGeometry()
  {

  }

  void View::PostResizeGeometry()
  {
  }

  long View::PostProcessEvent2 (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    return OnEvent (ievent, TraverseInfo, ProcessEventInfo);
  }

  void View::ProcessDraw (GraphicsEngine &GfxContext, bool force_draw)
  {
    m_IsFullRedraw = false;

    if (force_draw)
    {
      m_NeedRedraw = true;
      m_IsFullRedraw = true;
      Draw (GfxContext, force_draw);
      DrawContent (GfxContext, force_draw);
      PostDraw (GfxContext, force_draw);
    }
    else
    {
      if (m_NeedRedraw)
      {
        m_IsFullRedraw = true;
        Draw (GfxContext, false);
        DrawContent (GfxContext, false);
        PostDraw (GfxContext, false);
      }
      else
      {
        DrawContent (GfxContext, false);
        PostDraw (GfxContext, false);
      }
    }

    m_NeedRedraw = false;
    m_IsFullRedraw = false;
  }

  void View::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void View::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void View::NeedRedraw()
  {
    //GetWindowCompositor()..AddToDrawList(this);
    WindowThread* application = GetGraphicsThread();
    if(application)
    {
        application->AddToDrawList(this);
        application->RequestRedraw();
        //GetWindowCompositor().AddToDrawList(this);
    }
    m_NeedRedraw = true;
  }

  void View::NeedSoftRedraw()
  {
    //GetWindowCompositor()..AddToDrawList(this);
    WindowThread* application = GetGraphicsThread();
    if(application)
    {
        application->AddToDrawList(this);
        application->RequestRedraw();
    }
    //m_NeedRedraw = false;
  }

  bool View::IsRedrawNeeded()
  {
    return m_NeedRedraw;
  }

  void View::DoneRedraw()
  {
    m_NeedRedraw = false;

    if (m_CompositionLayout)
    {
      m_CompositionLayout->DoneRedraw();
    }
  }

  void View::DrawLayout()
  {
    if (m_CompositionLayout)
    {
      m_CompositionLayout->Draw();
    }
    else
    {
      int x, y, width, height;
      x = GetBaseX();
      y = GetBaseY();
      width = GetBaseWidth();
      height = GetBaseHeight();

      glColor3f (0.0f, 1.0f, 0.0f);
      glDisable (GL_TEXTURE_2D);
      glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
      glBegin (GL_QUADS);
      glVertex3i (x,  y, 1);
      glVertex3i (x + width,  y, 1);
      glVertex3i (x + width,  y + height, 1);
      glVertex3i (x,  y + height, 1);
      glEnd();
      glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
      glEnable (GL_TEXTURE_2D);
    }
  }

  Layout *View::GetCompositionLayout() const
  {
    return m_CompositionLayout;
  }

  void View::SetCompositionLayout (Layout *layout)
  {
    if (layout == 0)
    {
      nuxAssertMsg (0, TEXT ("[View::SetCompositionLayout] Invalid parent obejct.") );
      return;
    }

    Area *parent = layout->GetParentObject();

    if (parent == this)
    {
      nuxAssert (m_CompositionLayout == layout);
      return;
    }
    else if (parent != 0)
    {
      nuxAssertMsg (0, TEXT ("[View::SetCompositionLayout] Object already has a parent. You must UnParent the object before you can parenting again.") );
      return;
    }

    if (m_CompositionLayout)
      m_CompositionLayout->UnParentObject();

    layout->SetParentObject (this);
    m_CompositionLayout = layout;
  }

  void View::RemoveCompositionLayout()
  {
    if (m_CompositionLayout)
      m_CompositionLayout->UnParentObject();

    m_CompositionLayout = 0;
  }

  bool View::SearchInAllSubNodes (Area *bo)
  {
    if (m_CompositionLayout)
      return m_CompositionLayout->SearchInAllSubNodes (bo);

    return false;
  }

  bool View::SearchInFirstSubNodes (Area *bo)
  {
    if (m_CompositionLayout)
      return m_CompositionLayout->SearchInFirstSubNodes (bo);

    return false;
  }

  void View::SetGeometry (const Geometry &geo)
  {
    Area::SetGeometry (geo);
    ComputeChildLayout();
    PostResizeGeometry();
  }

  void View::SetFont (ObjectPtr<FontTexture> Font)
  {
    m_font = Font;
  }

  ObjectPtr<FontTexture> View::GetFont()
  {
    return m_font;
  }

  void View::SetTextColor (const Color &color)
  {
    m_TextColor = color;
  }

  Color View::GetTextColor()
  {
    return m_TextColor;
  }

  void View::DisableWidget()
  {
    m_IsEnabled = false;
  }

  void View::EnableWidget()
  {
    m_IsEnabled = true;
  }

  bool View::IsWidgetEnabled()
  {
    return m_IsEnabled;
  }


}
