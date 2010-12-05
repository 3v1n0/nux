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


#include "Nux.h"
#include "VSplitter.h"

#include "NuxGraphics/GLTextureResourceManager.h"
#include "WindowCompositor.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (VSplitter);
  static const t_s32 VSPLITTERWIDTH = 5;
  static const t_s32 VSTICK_SIZE = 5;

  VSplitter::VSplitter (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
  {
    new_addition = false;
    m_initial_config = true;
    m_focus_splitter_index = -1;
    m_ResizeOnSplitterRelease = true;

    mvt_dx = 0;
    mvt_dy = 0;
    m_current_x = 0;
    m_current_y = 0;
    m_current_width = 100;
    m_current_height = 100;

    //SetMinimumSize(m_current_width, m_current_height);
    SetGeometry (Geometry (m_current_x, m_current_y, m_current_width, m_current_height) );
  }

  VSplitter::~VSplitter()
  {
    // Delete all the interface object: This is a problem... The widget should be destroy by there associated parameters
    //delete vlayout;
    std::vector< Area* >::iterator it0;
    for (it0 = m_InterfaceObject.begin(); it0 != m_InterfaceObject.end(); it0++)
    {
      (*it0)->UnParentObject();
    }
    m_InterfaceObject.clear();

    std::vector< MySplitter* >::iterator it2;
    for (it2 = m_SplitterObject.begin(); it2 != m_SplitterObject.end(); it2++)
    {
      (*it2)->UnReference();
    }
    m_SplitterObject.clear();

    m_SplitConfig.clear();
  }

  long VSplitter::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    long ProcEvInfo = ProcessEventInfo;

    if (ievent.e_event == NUX_MOUSE_PRESSED)
    {
      if (!m_Geometry.IsPointInside (ievent.e_x, ievent.e_y) )
      {
        ProcEvInfo |= eDoNotProcess;
        //return TraverseInfo;
      }
    }

//    Geometry base = GetGeometry();
//    // The child layout get the Mouse down button only if the MouseDown happened inside the client view Area
//    Geometry viewGeometry = Geometry(base.x, base.y, base.GetWidth(), base.GetBaseHeight());
//    bool traverse = true;
//    if(ievent.e_event == I_ButtonPress)
//    {
//        if(!viewGeometry.IsPointInside(ievent.x, ievent.y))
//        {
//            ProcEvInfo = eDoNotProcess;
//            traverse = false;
//        }
//    }

    //if(traverse)
    {
      std::vector<MySplitter *>::iterator it_splitter;

      for (it_splitter = m_SplitterObject.begin(); it_splitter != m_SplitterObject.end(); it_splitter++)
      {
        ret = (*it_splitter)->OnEvent (ievent, ret, ProcEvInfo);
      }

      std::vector<Area *>::iterator it;

      for (it = m_InterfaceObject.begin(),
           it_splitter = m_SplitterObject.begin();
           it != m_InterfaceObject.end();
           it++, it_splitter++)
      {
        Geometry clip_geo;
        clip_geo.SetX ( ( (*it)->GetGeometry().x) );
        clip_geo.SetY ( ( (*it)->GetGeometry().y) );
        clip_geo.SetWidth ( (*it_splitter)->GetGeometry().x - (*it)->GetGeometry().x);
        clip_geo.SetHeight ( (*it)->GetGeometry().GetHeight() );

        long DoNotProcess = ProcEvInfo;

        if (clip_geo.IsPointInside (ievent.e_x, ievent.e_y) == false)
        {
          DoNotProcess |= eDoNotProcess;
        }

        if ( (*it)->Type().IsDerivedFromType (View::StaticObjectType) )
        {
          View *ic = NUX_STATIC_CAST (View *, (*it) );
          ret = ic->BaseProcessEvent (ievent, ret, ProcEvInfo | DoNotProcess);
        }
        else if ( (*it)->Type().IsObjectType (InputArea::StaticObjectType) )
        {
          InputArea *base_area = NUX_STATIC_CAST (InputArea *, (*it) );
          ret = base_area->OnEvent (ievent, ret, ProcEvInfo | DoNotProcess);
        }
        else if ( (*it)->Type().IsDerivedFromType (Layout::StaticObjectType) )
        {
          Layout *layout = NUX_STATIC_CAST (Layout *, (*it) );
          ret = layout->ProcessEvent (ievent, ret, ProcEvInfo | DoNotProcess);
        }
      }
    }

    ret = PostProcessEvent2 (ievent, ret, ProcEvInfo);

    return ret;
  }

  void VSplitter::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();
    GfxContext.PushClippingRectangle (base);
    std::vector<Area *>::iterator it;

    GetPainter().PaintBackground (GfxContext, base);
    std::vector<MySplitter *>::iterator it_splitter;

    for (it_splitter = m_SplitterObject.begin(); it_splitter != m_SplitterObject.end(); it_splitter++)
    {
      Geometry geo = (*it_splitter)->GetGeometry();
      Geometry grip_geo;
      t_s32 h = 20;

      if (geo.GetHeight() > h)
      {
        grip_geo.SetX (geo.x);
        grip_geo.SetY (geo.y + (geo.GetHeight() - h) / 2);
        grip_geo.SetWidth (geo.GetWidth() );
        grip_geo.SetHeight (h);
      }
      else
      {
        grip_geo.SetX (geo.x);
        grip_geo.SetY (geo.y + (h - geo.GetHeight() ) / 2);
        grip_geo.SetWidth (geo.GetWidth() );
        grip_geo.SetHeight (h);
      }

      GetPainter().Draw2DLine (GfxContext, grip_geo.x + 1, grip_geo.y, grip_geo.x + 1, grip_geo.y + grip_geo.GetHeight(), Color (0xFF111111) );
      GetPainter().Draw2DLine (GfxContext, grip_geo.x + 3, grip_geo.y, grip_geo.x + 3, grip_geo.y + grip_geo.GetHeight(), Color (0xFF111111) );
    }

    GfxContext.PopClippingRectangle();
  }

  void VSplitter::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.PushClippingRectangle (GetGeometry() );
    Geometry base = GetGeometry();
    bool need_redraw = IsRedrawNeeded();

    std::vector<MySplitter *>::iterator it_splitter;
    std::vector<Area *>::iterator it;

    for (it = m_InterfaceObject.begin(), it_splitter = m_SplitterObject.begin();
         it != m_InterfaceObject.end();
         it++, it_splitter++)
    {
      Geometry sgeo = (*it_splitter)->GetGeometry();
      GfxContext.PushClippingRectangle (Rect (
                                          base.x, base.y, sgeo.x - base.x, base.GetHeight() ) );

      base.SetX (sgeo.x + sgeo.GetWidth() );

      if (force_draw || need_redraw)
      {
        if ( (*it)->Type().IsDerivedFromType (View::StaticObjectType) )
        {
          View *ic = NUX_STATIC_CAST (View *, (*it) );
          ic->ProcessDraw (GfxContext, true);
        }
        else if ( (*it)->Type().IsObjectType (InputArea::StaticObjectType) )
        {
          InputArea *base_area = NUX_STATIC_CAST (InputArea *, (*it) );
          base_area->OnDraw (GfxContext, true);
        }
        else if ( (*it)->Type().IsObjectType (HLayout::StaticObjectType) )
        {
          HLayout *layout = NUX_STATIC_CAST (HLayout *, (*it) );
          layout->ProcessDraw (GfxContext, true);
        }
        else if ( (*it)->Type().IsObjectType (VLayout::StaticObjectType) )
        {
          VLayout *layout = NUX_STATIC_CAST (VLayout *, (*it) );
          layout->ProcessDraw (GfxContext, true);
        }
      }
      else
      {
        if ( (*it)->Type().IsDerivedFromType (View::StaticObjectType) )
        {
          View *ic = NUX_STATIC_CAST (View *, (*it) );
          ic->ProcessDraw (GfxContext, false);
        }
        else if ( (*it)->Type().IsObjectType (InputArea::StaticObjectType) )
        {
          InputArea *base_area = NUX_STATIC_CAST (InputArea *, (*it) );
          base_area->OnDraw (GfxContext, false);
        }
        else if ( (*it)->Type().IsObjectType (HLayout::StaticObjectType) )
        {
          HLayout *layout = NUX_STATIC_CAST (HLayout *, (*it) );
          layout->ProcessDraw (GfxContext, false);
        }
        else if ( (*it)->Type().IsObjectType (VLayout::StaticObjectType) )
        {
          VLayout *layout = NUX_STATIC_CAST (VLayout *, (*it) );
          layout->ProcessDraw (GfxContext, false);
        }
      }

      GfxContext.PopClippingRectangle();
    }

    GfxContext.PopClippingRectangle();
  }

  void VSplitter::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void VSplitter::OverlayDrawing (GraphicsEngine &GfxContext)
  {
    t_u32 num_element = (t_u32) m_SplitterObject.size();

    Geometry base = GetGeometry();

    if (m_focus_splitter_index >= 0)
    {
      Geometry geo = m_SplitterObject[m_focus_splitter_index]->GetGeometry();
      geo.OffsetPosition (mvt_dx, mvt_dy);

      if (m_focus_splitter_index == 0 && num_element > 1)
      {
        if (geo.x < base.x)
        {
          geo.SetX (base.x);
        }

        if (geo.x  + VSPLITTERWIDTH > m_SplitterObject[m_focus_splitter_index + 1]->GetGeometry().x)
        {
          geo.SetX (m_SplitterObject[m_focus_splitter_index+1]->GetGeometry().x - VSPLITTERWIDTH);
        }
      }

      if ( (m_focus_splitter_index > 0) && m_focus_splitter_index < (t_s32) num_element - 1)
      {
        if (geo.x < m_SplitterObject[m_focus_splitter_index - 1]->GetGeometry().x + VSPLITTERWIDTH)
        {
          geo.SetX (m_SplitterObject[m_focus_splitter_index - 1]->GetGeometry().x + VSPLITTERWIDTH);
        }

        if (geo.x + VSPLITTERWIDTH > m_SplitterObject[m_focus_splitter_index + 1]->GetGeometry().x)
        {
          geo.SetX (m_SplitterObject[m_focus_splitter_index + 1]->GetGeometry().x - VSPLITTERWIDTH);
        }
      }

      GfxContext.GetRenderStates().SetBlend (true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      {
        GetPainter().Paint2DQuadColor (GfxContext, geo, Color (0xBB868686) );
      }
      GfxContext.GetRenderStates().SetBlend (false);
    }
  }

  void VSplitter::AddWidget (Area *ic, float stretchfactor)
  {
    if (ic)
    {
      MySplitter *splitter = new MySplitter;
      splitter->SinkReference();
      t_u32 no = (t_u32) m_InterfaceObject.size();
      splitter->OnMouseDown.connect (sigc::bind ( sigc::mem_fun (this, &VSplitter::OnSplitterMouseDown), no) );
      splitter->OnMouseUp.connect (sigc::bind ( sigc::mem_fun (this, &VSplitter::OnSplitterMouseUp), no) );
      splitter->OnMouseDrag.connect (sigc::bind ( sigc::mem_fun (this, &VSplitter::OnSplitterMouseDrag), no) );

      ic->SetParentObject (this);
      m_InterfaceObject.push_back (ic);
      m_SplitterObject.push_back (splitter);
      m_SplitConfig.push_back (stretchfactor);

      new_addition = true;
      ComputeChildLayout();
    }
  }

  void VSplitter::clearContent()
  {
    m_InterfaceObject.clear();
  }

  long VSplitter::ComputeChildLayout()
  {
    t_u32 num_element = (t_u32) m_InterfaceObject.size();
    t_s32 x = GetBaseX();
    t_s32 y = GetBaseY();
    t_s32 w = GetBaseWidth();
    t_s32 h = GetBaseHeight();

    if ( (w == 0) || (h == 0) )
    {
      return eCompliantHeight | eCompliantWidth;
    }

    if (num_element < 1)
    {
      m_current_height = h;
      m_current_width = w;
      m_current_x = x;
      m_current_y = y;

      new_addition = false;
      return eCompliantHeight | eCompliantWidth;
    }

    std::vector<Area *>::iterator it;
    std::vector<MySplitter *>::iterator it_splitter;

    if (new_addition)
    {
      ResetSplitConfig();
      new_addition = false;
    }

    if (m_current_height != h)
    {
      for (t_u32 i = 0; i < num_element; i++)
      {
        Geometry splitter_geo = m_SplitterObject[i]->GetGeometry();
        splitter_geo.SetHeight (h);
        splitter_geo.SetY (y);

        m_SplitterObject[i]->SetGeometry (splitter_geo);
      }
    }

    if (m_current_width != w)
    {
      t_s32 size_to_distribute = w - num_element * VSPLITTERWIDTH;
      t_s32 previous_spliter_end = m_current_x;
      t_s32 new_spliter_end = x;

      for (t_u32 i = 0; i < num_element; i++)
      {
        Geometry splitter_geo = m_SplitterObject[i]->GetGeometry();
        // compute percentage of space occupied by the element i;
        // width of element i = m_SplitterObject[i]->GetX() - previous_splliter_end
        t_s32 splitter_start = m_SplitterObject[i]->GetBaseX();
        float percent = float (splitter_start - previous_spliter_end) / float (m_current_width - num_element * VSPLITTERWIDTH);

        if (percent > 1.0f)
          percent = 1.0f;

        splitter_geo.SetX (new_spliter_end + size_to_distribute * percent);
        previous_spliter_end = splitter_start + VSPLITTERWIDTH;
        new_spliter_end = new_spliter_end + size_to_distribute * percent + VSPLITTERWIDTH;
        m_SplitterObject[i]->SetGeometry (splitter_geo);
      }

      if (m_SplitterObject[0]->GetBaseX() < x)
      {
        m_SplitterObject[0]->SetBaseX (x);
      }

      m_SplitterObject[num_element-1]->SetBaseX (x + w - VSPLITTERWIDTH);
    }

    t_s32 accwidth = x;

    for (t_u32 i = 0; i < num_element; i++)
    {
      Geometry splitter_geo = m_SplitterObject[i]->GetGeometry();

      //m_InterfaceObject[i]->SetGeometry(Geometry(accwidth, y, splitter_geo.x - accwidth, h));

      if (m_InterfaceObject[i]->Type().IsDerivedFromType (View::StaticObjectType) )
      {
        View *ic = NUX_STATIC_CAST (View *, m_InterfaceObject[i]);
        ic->SetGeometry (Geometry (accwidth, y, splitter_geo.x - accwidth, h) );
        // if we are already computing the layout from the main window down, we need to call
        // ComputeElementLayout to force the computing of this element layout.
        GetGraphicsThread()->ComputeElementLayout (ic);
      }
      else if (m_InterfaceObject[i]->Type().IsObjectType (InputArea::StaticObjectType) )
      {
        InputArea *base_area = NUX_STATIC_CAST (InputArea *, m_InterfaceObject[i]);
        base_area->SetGeometry (Geometry (accwidth, y, splitter_geo.x - accwidth, h) );
      }
      else if (m_InterfaceObject[i]->Type().IsDerivedFromType (Layout::StaticObjectType) )
      {
        Layout *layout = NUX_STATIC_CAST (Layout *, m_InterfaceObject[i]);
        layout->SetGeometry (Geometry (accwidth, y, splitter_geo.x - accwidth, h) );
        // if we are already computing the layout from the main window down, we need to call
        // ComputeElementLayout to force the computing of this element layout.
        GetGraphicsThread()->ComputeElementLayout (layout);
      }

      accwidth += splitter_geo.x - accwidth + VSPLITTERWIDTH;
    }

    m_current_height = h;
    m_current_width = w;
    m_current_x = x;
    m_current_y = y;

    return eCompliantHeight | eCompliantWidth;
  }

  void VSplitter::ResetSplitConfig()
  {
    t_s32 x = GetBaseX();
    t_s32 y = GetBaseY();
    t_s32 w = GetBaseWidth();
    t_s32 h = GetBaseHeight();
    t_u32 num_element = (t_u32) m_InterfaceObject.size();

    if (num_element < 1)
    {
      return;
    }

    float max_stretch = 0.0f;
    float stretchfactor;

    for (t_u32 i = 0; i < (t_u32) m_SplitConfig.size(); i++)
    {
      stretchfactor = m_SplitConfig[i];

      if (max_stretch < stretchfactor)
      {
        max_stretch = stretchfactor;
      }
    }

    float total = 0;

    for (t_u32 i = 0; i < (t_u32) m_SplitConfig.size(); i++)
    {
      stretchfactor = m_SplitConfig[i];
      total += stretchfactor / max_stretch;
    }

    t_s32 availableheight = (w - num_element * VSPLITTERWIDTH);
    float max_size = float (availableheight) / total;

    for (t_u32 i = 0; i < (t_u32) m_SplitConfig.size(); i++)
    {
      stretchfactor = m_SplitConfig[i];
      x += stretchfactor * max_size / max_stretch;
      Geometry geo (x, y, VSPLITTERWIDTH, h);
      m_SplitterObject[i]->SetGeometry (geo);
    }

    m_SplitterObject[num_element-1]->SetBaseX (x + w - VSPLITTERWIDTH);

    m_initial_config = true;
  }

  void VSplitter::OnSplitterMouseDown (t_s32 x, t_s32 y, unsigned long button_flags, unsigned long key_flags, t_s32 header_pos)
  {
    m_point.Set (x, y);

    m_focus_splitter_index = header_pos;
    GetWindowCompositor().SetWidgetDrawingOverlay (this, GetWindowCompositor().GetCurrentWindow() );

    // Hint for the window to initiate a redraw
    GetGraphicsThread()->RequestRedraw();
  }

  void VSplitter::OnSplitterMouseUp (t_s32 x, t_s32 y, unsigned long button_flags, unsigned long key_flags, t_s32 header_pos)
  {
    if (mvt_dx)
    {
      Geometry geo = m_SplitterObject[header_pos]->GetGeometry();
      geo.OffsetPosition (mvt_dx, 0);

      t_u32 num_element = (t_u32) m_SplitterObject.size();

      if (header_pos < (t_s32) num_element - 1)
      {
        // Make the splitter bar stick to the next one if the distance between them is less than VSTICK_SIZE
        if (m_SplitterObject[header_pos + 1]->GetGeometry().x - geo.x - VSPLITTERWIDTH < VSTICK_SIZE)
          geo.SetX ( m_SplitterObject[header_pos + 1]->GetGeometry().x - VSPLITTERWIDTH );
      }

      m_SplitterObject[header_pos]->SetGeometry (geo);
      ResizeSplitter (header_pos);
    }

    m_focus_splitter_index = -1;
    mvt_dx = 0;
    mvt_dy = 0;

    // End overlay drawing;
    GetWindowCompositor().SetWidgetDrawingOverlay (0, GetWindowCompositor().GetCurrentWindow() );
    // Hint for the window to initiate a redraw
    GetGraphicsThread()->RequestRedraw();
  }

  void VSplitter::OnSplitterMouseDrag (t_s32 x, t_s32 y, t_s32 dx, t_s32 dy, unsigned long button_flags, unsigned long key_flags, t_s32 header_pos)
  {
    bool recompute = false;
    Geometry geo = m_SplitterObject[header_pos]->GetGeometry();
    t_s32 num_element = (t_s32) m_SplitterObject.size();

    if (header_pos == num_element - 1)
    {
      // The last splitter cannot be moved
      return;
    }

    recompute = true;
    mvt_dx = x - m_point.x;
    mvt_dy = 0;

    if (m_ResizeOnSplitterRelease == false)
    {
      // Continuously redraw resize and redraw the 2 parts of the widget.
      // This is slow.
      geo.OffsetPosition (mvt_dx, mvt_dy);
      m_SplitterObject[header_pos]->SetGeometry (geo);
      ResizeSplitter (header_pos);

      mvt_dx = 0;
      mvt_dy = 0;
    }

    // Hint for the window to initiate a redraw
    GetGraphicsThread()->RequestRedraw();
  }

  void VSplitter::ResizeSplitter (t_s32 header_pos)
  {
    Geometry geo = m_SplitterObject[header_pos]->GetGeometry();
    t_s32 num_element = (t_s32) m_SplitterObject.size();

    if ( (header_pos == 0) && (m_SplitterObject[header_pos]->GetBaseX() < GetBaseX() ) )
    {
      m_SplitterObject[header_pos]->SetBaseX (GetBaseX() );
    }

    if ( (header_pos == num_element - 1) && (m_SplitterObject[header_pos]->GetBaseX() > GetBaseX() + GetBaseWidth() - VSPLITTERWIDTH) )
    {
      m_SplitterObject[header_pos]->SetBaseX (GetBaseX() + GetBaseWidth() - VSPLITTERWIDTH);
    }

    if (header_pos < (t_s32) num_element - 1)
    {
      t_s32 posx0, posx1;
      posx0 = m_SplitterObject[header_pos]->GetBaseX();
      posx1 = m_SplitterObject[header_pos + 1]->GetBaseX();

      if (posx0 > posx1 - VSPLITTERWIDTH)
      {
        posx0 = posx1 - VSPLITTERWIDTH;
        m_SplitterObject[header_pos]->SetBaseX (posx0);
      }
    }

    if (0 < header_pos)
    {
      t_s32 posx0, posx1;
      posx0 = m_SplitterObject[header_pos]->GetBaseX();
      posx1 = m_SplitterObject[header_pos - 1]->GetBaseX();

      if (posx0 < posx1 + VSPLITTERWIDTH)
      {
        posx0 = posx1 + VSPLITTERWIDTH;
        m_SplitterObject[header_pos]->SetBaseX (posx0);
      }
    }

    m_initial_config = false;
    ComputeChildLayout();

    NeedRedraw();
  }

// VSplitter need to re implement DoneRedraw because it does not
// have a m_compositionlayout where its child are located;
  void VSplitter::DoneRedraw()
  {
    std::vector<Area *>::iterator it;

    for (it = m_InterfaceObject.begin(); it != m_InterfaceObject.end(); it++)
    {
      //(*it)->DoneRedraw();
      if ( (*it)->Type().IsDerivedFromType (View::StaticObjectType) )
      {
        View *ic = NUX_STATIC_CAST (View *, (*it) );
        ic->DoneRedraw();
      }
      else if ( (*it)->Type().IsObjectType (InputArea::StaticObjectType) )
      {
        //InputArea* base_area = NUX_STATIC_CAST(InputArea*, (*it));
      }
    }
  }

}
