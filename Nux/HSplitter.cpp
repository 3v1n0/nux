/*
 * Copyright 2010 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "Nux.h"
#include "HSplitter.h"

#include "NuxGraphics/GLTextureResourceManager.h"

#include "WindowCompositor.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (HSplitter);
  static const t_s32 HSPLITTERHEIGHT = 5;
  static const t_s32 HSTICK_SIZE = 5;

  HSplitter::HSplitter (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
  {
    new_addition                = false;
    m_initial_config            = true;
    m_focus_splitter_index      = -1;
    m_ResizeOnSplitterRelease   = true;

    mvt_dx = 0;
    mvt_dy = 0;
    m_current_x = 0;
    m_current_y = 0;
    m_current_width = 200;
    m_current_height = 200;

    //SetMinimumSize(200,200);
    SetGeometry (Geometry (m_current_x, m_current_y, m_current_width, m_current_height) );
  }

  HSplitter::~HSplitter()
  {
    std::vector< Area* >::iterator it0;
    for (it0 = m_InterfaceObject.begin(); it0 != m_InterfaceObject.end(); it0++)
    {
      (*it0)->UnParentObject();
    }
    m_InterfaceObject.clear();

    std::vector< MySplitter* >::iterator it2;
    for (it2 = m_SplitterObject.begin(); it2 != m_SplitterObject.end(); it2++)
    {
      (*it2)->UnParentObject();
    }
    m_SplitterObject.clear();

    m_SplitConfig.clear();
  }

  void HSplitter::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.PushClippingRectangle (GetGeometry() );
    Geometry base = GetGeometry();
//    std::vector<View*>::iterator it;
//    for(it = m_InterfaceObject.begin(); it != m_InterfaceObject.end(); it++)
//    {
//        (*it)->ProcessDraw(force_draw);
//    }

    GetPainter().PaintBackground (GfxContext, base);
    std::vector<MySplitter *>::iterator it_splitter;

    for (it_splitter = m_SplitterObject.begin(); it_splitter != m_SplitterObject.end(); it_splitter++)
    {
      Geometry geo = (*it_splitter)->GetGeometry();
      Geometry grip_geo;
      t_s32 w = 20;

      if (geo.GetWidth() > w)
      {
        grip_geo.SetX (geo.x + (geo.GetWidth() - w) / 2);
        grip_geo.SetY (geo.y);
        grip_geo.SetWidth (w);
        grip_geo.SetHeight (geo.GetHeight() );
      }
      else
      {
        grip_geo.SetX (geo.x - (w - geo.GetWidth() ) / 2);
        grip_geo.SetY (geo.y);
        grip_geo.SetWidth (w);
        grip_geo.SetHeight (geo.GetHeight() );
      }

      GetPainter().Draw2DLine (GfxContext, grip_geo.x, grip_geo.y + 1, grip_geo.x + grip_geo.GetWidth(), grip_geo.y + 1, Color (0xFF111111) );
      GetPainter().Draw2DLine (GfxContext, grip_geo.x, grip_geo.y + 3, grip_geo.x + grip_geo.GetWidth(), grip_geo.y + 3, Color (0xFF111111) );
    }

    GfxContext.PopClippingRectangle();
  }

  void HSplitter::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();
    GfxContext.PushClippingRectangle (base);
    bool need_redraw = IsRedrawNeeded();

    std::vector<MySplitter *>::iterator it_splitter;
    std::vector<Area *>::iterator it;

    //for(it = m_InterfaceObject.begin(); it != m_InterfaceObject.end(); it++)
    for (it = m_InterfaceObject.begin(), it_splitter = m_SplitterObject.begin();
         it != m_InterfaceObject.end();
         it++, it_splitter++)
    {
      Geometry sgeo = (*it_splitter)->GetGeometry();
      GfxContext.PushClippingRectangle (Rect (
                                          base.x, base.y, base.GetWidth(), sgeo.y - base.y) );

      base.SetY (sgeo.y + sgeo.GetHeight() );

      if (force_draw || need_redraw)
      {
        if ( (*it)->Type().IsDerivedFromType (View::StaticObjectType) )
        {
          View *ic = static_cast<View *>(*it);
          ic->ProcessDraw (GfxContext, true);
        }
        else if ( (*it)->Type().IsObjectType (InputArea::StaticObjectType) )
        {
          InputArea *base_area = static_cast<InputArea *>(*it);
          base_area->OnDraw (GfxContext, true);
        }
        else if ( (*it)->Type().IsObjectType (HLayout::StaticObjectType) )
        {
          HLayout *layout = static_cast<HLayout *>(*it);
          layout->ProcessDraw (GfxContext, true);
        }
        else if ( (*it)->Type().IsObjectType (VLayout::StaticObjectType) )
        {
          VLayout *layout = static_cast<VLayout *>(*it);
          layout->ProcessDraw (GfxContext, true);
        }
      }
      else
      {
        if ( (*it)->Type().IsDerivedFromType (View::StaticObjectType) )
        {
          View *ic = static_cast<View *>(*it);
          ic->ProcessDraw (GfxContext, false);
        }
        else if ( (*it)->Type().IsObjectType (InputArea::StaticObjectType) )
        {
          InputArea *base_area = static_cast<InputArea *>(*it);
          base_area->OnDraw (GfxContext, false);
        }
        else if ( (*it)->Type().IsObjectType (HLayout::StaticObjectType) )
        {
          HLayout *layout = static_cast<HLayout *>(*it);
          layout->ProcessDraw (GfxContext, false);
        }
        else if ( (*it)->Type().IsObjectType (VLayout::StaticObjectType) )
        {
          VLayout *layout = static_cast<VLayout *>(*it);
          layout->ProcessDraw (GfxContext, false);
        }
      }

      GfxContext.PopClippingRectangle();
    }

    GfxContext.PopClippingRectangle();
  }

  void HSplitter::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void HSplitter::OverlayDrawing (GraphicsEngine &GfxContext)
  {
    t_u32 num_element = (t_u32) m_SplitterObject.size();

    Geometry base = GetGeometry();

    if (m_focus_splitter_index >= 0)
    {
      Geometry geo = m_SplitterObject[m_focus_splitter_index]->GetGeometry();
      geo.OffsetPosition (mvt_dx, mvt_dy);

      if (m_focus_splitter_index == 0 && num_element > 1)
      {
        if (geo.y < base.y)
        {
          geo.SetY (base.y);
        }

        if (geo.y  + HSPLITTERHEIGHT > m_SplitterObject[m_focus_splitter_index + 1]->GetGeometry().y)
        {
          geo.SetY (m_SplitterObject[m_focus_splitter_index+1]->GetGeometry().y - HSPLITTERHEIGHT);
        }
      }

      if ( (m_focus_splitter_index > 0) && (m_focus_splitter_index < (t_s32) num_element - 1) )
      {
        if (geo.y < m_SplitterObject[m_focus_splitter_index - 1]->GetGeometry().y + HSPLITTERHEIGHT)
        {
          geo.SetY (m_SplitterObject[m_focus_splitter_index - 1]->GetGeometry().y + HSPLITTERHEIGHT);
        }

        if (geo.y + HSPLITTERHEIGHT > m_SplitterObject[m_focus_splitter_index + 1]->GetGeometry().y)
        {
          geo.SetY (m_SplitterObject[m_focus_splitter_index + 1]->GetGeometry().y - HSPLITTERHEIGHT);
        }
      }

      GfxContext.GetRenderStates().SetBlend (true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      {
        GetPainter().Paint2DQuadColor (GfxContext, geo, Color (0xBB868686) );
      }
      GfxContext.GetRenderStates().SetBlend (false);
    }
  }

  void HSplitter::AddWidget (Area *ic, float stretchfactor)
  {
    if (ic)
    {
      MySplitter *splitter = new MySplitter;
      splitter->SetParentObject(this);
      //splitter->SinkReference();

      t_u32 no = (t_u32) m_InterfaceObject.size();
      splitter->mouse_down.connect (sigc::bind ( sigc::mem_fun (this, &HSplitter::OnSplitterMouseDown), no) );
      splitter->mouse_drag.connect (sigc::bind ( sigc::mem_fun (this, &HSplitter::OnSplitterMouseDrag), no) );
      splitter->mouse_up.connect (sigc::bind ( sigc::mem_fun (this, &HSplitter::OnSplitterMouseUp), no) );

      ic->SetParentObject (this);
      m_InterfaceObject.push_back (ic);
      m_SplitterObject.push_back (splitter);
      m_SplitConfig.push_back (stretchfactor);

      new_addition = true;
      ComputeChildLayout();
    }
  }

  void HSplitter::clearContent()
  {
    m_InterfaceObject.clear();
  }

  long HSplitter::ComputeChildLayout()
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

    if (m_current_width != w)
    {
      for (t_u32 i = 0; i < num_element; i++)
      {
        Geometry splitter_geo = m_SplitterObject[i]->GetGeometry();
        splitter_geo.SetWidth (w);
        splitter_geo.SetX (x);

        m_SplitterObject[i]->SetGeometry (splitter_geo);
      }
    }

    if (m_current_height != h)
    {
      t_s32 size_to_distribute = h - num_element * HSPLITTERHEIGHT;
      t_s32 previous_spliter_end = m_current_y;
      t_s32 new_spliter_end = y;

      for (t_u32 i = 0; i < num_element; i++)
      {
        Geometry splitter_geo = m_SplitterObject[i]->GetGeometry();
        // compute percentage of space occupied by the element i;
        // width of element i = m_SplitterObject[i]->GetY() - previous_splliter_end
        t_s32 splitter_start = m_SplitterObject[i]->GetBaseY();
        float percent = float (splitter_start - previous_spliter_end) / float (m_current_height - num_element * HSPLITTERHEIGHT);

        if (percent > 1.0f)
          percent = 1.0f;

        splitter_geo.SetY (new_spliter_end + size_to_distribute * percent);
        previous_spliter_end = splitter_start + HSPLITTERHEIGHT;
        new_spliter_end = new_spliter_end + size_to_distribute * percent + HSPLITTERHEIGHT;
        m_SplitterObject[i]->SetGeometry (splitter_geo);
      }

      if (m_SplitterObject[0]->GetBaseY() < y)
      {
        m_SplitterObject[0]->SetBaseY (y);
      }

      m_SplitterObject[num_element-1]->SetBaseY (y + h - HSPLITTERHEIGHT);
    }

    t_s32 accheight = y;

    for (t_u32 i = 0; i < num_element; i++)
    {
      Geometry splitter_geo = m_SplitterObject[i]->GetGeometry();

      //m_InterfaceObject[i]->SetGeometry(Geometry(x, accheight, w, splitter_geo.y - accheight));

      if (m_InterfaceObject[i]->Type().IsDerivedFromType (View::StaticObjectType) )
      {
        View *ic = static_cast<View *>(m_InterfaceObject[i]);
        ic->SetGeometry (Geometry (x, accheight, w, splitter_geo.y - accheight) );
        // if we are already computing the layout from the main window down, we need to call
        // ComputeElementLayout to force the computing of this element layout.
        GetWindowThread ()->ComputeElementLayout (ic);
      }
      else if (m_InterfaceObject[i]->Type().IsObjectType (InputArea::StaticObjectType) )
      {
        InputArea *base_area = static_cast<InputArea *>(m_InterfaceObject[i]);
        base_area->SetGeometry (Geometry (x, accheight, w, splitter_geo.y - accheight) );
      }
      else if (m_InterfaceObject[i]->Type().IsDerivedFromType (Layout::StaticObjectType) )
      {
        Layout *layout = static_cast<Layout *>(m_InterfaceObject[i]);
        layout->SetGeometry (Geometry (x, accheight, w, splitter_geo.y - accheight) );
        // if we are already computing the layout from the main window down, we need to call
        // ComputeElementLayout to force the computing of this element layout.
        GetWindowThread ()->ComputeElementLayout (layout);
      }

      accheight += splitter_geo.y - accheight + HSPLITTERHEIGHT;
    }

    m_current_height = h;
    m_current_width = w;
    m_current_x = x;
    m_current_y = y;

    return eCompliantHeight | eCompliantWidth;
  }

  void HSplitter::ResetSplitConfig()
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

    t_s32 availableheight = (h - num_element * HSPLITTERHEIGHT);
    float max_size = float (availableheight) / total;

    for (t_u32 i = 0; i < (t_u32) m_SplitConfig.size(); i++)
    {
      stretchfactor = m_SplitConfig[i];
      y += stretchfactor * max_size / max_stretch;
      Geometry geo (x, y, w, HSPLITTERHEIGHT);
      m_SplitterObject[i]->SetGeometry (geo);
    }

    m_SplitterObject[num_element-1]->SetBaseX (y + h - HSPLITTERHEIGHT);

    m_initial_config = true;
  }

  void HSplitter::OnSplitterMouseDown (t_s32 x, t_s32 y, unsigned long button_flags, unsigned long key_flags, t_s32 header_pos)
  {
    m_point = Point(x, y);

    m_focus_splitter_index = header_pos;
    GetWindowCompositor().SetWidgetDrawingOverlay (this, GetWindowCompositor ().GetProcessingTopView () );


    // Hint for the window to initiate a redraw
    GetWindowThread ()->RequestRedraw();
  }

  void HSplitter::OnSplitterMouseUp (t_s32 x, t_s32 y, unsigned long button_flags, unsigned long key_flags, t_s32 header_pos)
  {
    if (mvt_dy)
    {
      Geometry geo = m_SplitterObject[header_pos]->GetGeometry();
      geo.OffsetPosition (0, mvt_dy);

      t_u32 num_element = (t_u32) m_SplitterObject.size();

      if (header_pos < (t_s32) num_element - 1)
      {
        // Make the splitter bar stick to the next one if the distance between them is less than HSTICK_SIZE.
        if (m_SplitterObject[header_pos + 1]->GetGeometry().y - geo.y - HSPLITTERHEIGHT < HSTICK_SIZE)
          geo.SetY ( m_SplitterObject[header_pos + 1]->GetGeometry().y - HSPLITTERHEIGHT );
      }

      m_SplitterObject[header_pos]->SetGeometry (geo);
      ResizeSplitter (header_pos);
    }

    m_focus_splitter_index = -1;
    mvt_dx = 0;
    mvt_dy = 0;

    // End overlay drawing;
    GetWindowCompositor().SetWidgetDrawingOverlay (0, GetWindowCompositor().GetProcessingTopView() );
    // Hint for the window to initiate a redraw
    GetWindowThread ()->RequestRedraw();
  }

  void HSplitter::OnSplitterMouseDrag (t_s32 x, t_s32 y, t_s32 dx, t_s32 dy, unsigned long button_flags, unsigned long key_flags, t_s32 header_pos)
  {
    Geometry geo = m_SplitterObject[header_pos]->GetGeometry();
    t_s32 num_element = (t_s32) m_SplitterObject.size();

    if (header_pos == num_element - 1)
    {
      // The last splitter cannot be moved
      return;
    }

    mvt_dx = 0;
    mvt_dy = (y - m_point.y);

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
    GetWindowThread ()->RequestRedraw();
  }

  void HSplitter::ResizeSplitter (t_s32 header_pos)
  {
    Geometry geo = m_SplitterObject[header_pos]->GetGeometry();
    t_s32 num_element = (t_s32) m_SplitterObject.size();

    if ( (header_pos == 0) && (m_SplitterObject[header_pos]->GetBaseY() < GetBaseY() ) )
    {
      m_SplitterObject[header_pos]->SetBaseY (GetBaseY() );
    }

    if ( (header_pos == num_element - 1) && (m_SplitterObject[header_pos]->GetBaseY() > GetBaseY() + GetBaseHeight() - HSPLITTERHEIGHT) )
    {
      m_SplitterObject[header_pos]->SetBaseY (GetBaseY() + GetBaseHeight() - HSPLITTERHEIGHT);
    }

    if (header_pos < (t_s32) num_element - 1)
    {
      t_s32 posy0, posy1;
      posy0 = m_SplitterObject[header_pos]->GetBaseY();
      posy1 = m_SplitterObject[header_pos + 1]->GetBaseY();

      if (posy0 > posy1 - HSPLITTERHEIGHT)
      {
        posy0 = posy1 - HSPLITTERHEIGHT;
        m_SplitterObject[header_pos]->SetBaseY (posy0);
      }
    }

    if (0 < header_pos)
    {
      t_s32 posy0, posy1;
      posy0 = m_SplitterObject[header_pos]->GetBaseY();
      posy1 = m_SplitterObject[header_pos - 1]->GetBaseY();

      if (posy0 < posy1 + HSPLITTERHEIGHT)
      {
        posy0 = posy1 + HSPLITTERHEIGHT;
        m_SplitterObject[header_pos]->SetBaseY (posy0);
      }
    }

    ComputeChildLayout();
    QueueDraw();
  }

// HSplitter need to re implement DoneRedraw because it does not
// have a m_compositionlayout where its child are located;
  void HSplitter::DoneRedraw()
  {
    std::vector<Area *>::iterator it;

    for(it = m_InterfaceObject.begin(); it != m_InterfaceObject.end(); it++)
    {
      //(*it)->DoneRedraw();
      if((*it)->Type().IsDerivedFromType (View::StaticObjectType))
      {
        View *ic = static_cast<View *>(*it);
        ic->DoneRedraw();
      }

      else if((*it)->Type().IsObjectType (InputArea::StaticObjectType))
      {
        //InputArea* base_area = NUX_STATIC_CAST(InputArea*, (*it));
      }
    }
  }

  Area* HSplitter::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if(mouse_inside == false)
      return NULL;

    std::vector<MySplitter*>::iterator splitter_it;
    for (splitter_it = m_SplitterObject.begin(); splitter_it != m_SplitterObject.end(); splitter_it++)
    {
      Area* found_area = (*splitter_it)->FindAreaUnderMouse(mouse_position, event_type);
      if(found_area)
        return found_area;
    }

    std::vector<Area *>::iterator it;
    for(it = m_InterfaceObject.begin(); it != m_InterfaceObject.end(); it++)
    {
      Area* found_area = (*it)->FindAreaUnderMouse(mouse_position, event_type);

      if(found_area)
        return found_area;
    }

    if((event_type == NUX_MOUSE_WHEEL) && (!AcceptMouseWheelEvent()))
      return NULL;
    return this;
  }

  bool HSplitter::AcceptKeyNavFocus()
  {
    return false;
  }

  Area* HSplitter::KeyNavIteration(KeyNavDirection direction)
  {
    if (m_InterfaceObject.size() == 0)
      return NULL;

    if (next_object_to_key_focus_area_)
    {
      if ((direction == KEY_NAV_LEFT) || (direction == KEY_NAV_RIGHT))
      {
        // Don't know what to do with this
        return NULL;
      }
      std::vector<Area*>::iterator it;
      std::vector<Area*>::iterator it_next;
      it = std::find (m_InterfaceObject.begin(), m_InterfaceObject.end(), next_object_to_key_focus_area_);

      if (it == m_InterfaceObject.end())
      {
        // Should never happen
        nuxAssert (0);
        return NULL;
      }

      it_next = it;
      ++it_next;

      if ((direction == KEY_NAV_UP) && (it == m_InterfaceObject.begin()))
      {
        // can't go further
        return NULL;
      }

      if ((direction == KEY_NAV_DOWN) && (it_next == m_InterfaceObject.end()))
      {
        // can't go further
        return NULL;
      }

      if ((direction == KEY_NAV_UP))
      {
        --it;
        Area* key_nav_focus = (*it)->KeyNavIteration(direction);

        while (key_nav_focus == NULL)
        {
          if (it == m_InterfaceObject.begin())
            break;

          --it;
          key_nav_focus = (*it)->KeyNavIteration(direction);
        }

        return key_nav_focus;
      }

      if ((direction == KEY_NAV_DOWN))
      {
        ++it;
        Area* key_nav_focus = (*it)->KeyNavIteration(direction);

        while (key_nav_focus == NULL)
        {
          ++it;
          if (it == m_InterfaceObject.end())
            break;

          key_nav_focus = (*it)->KeyNavIteration(direction);
        }

        return key_nav_focus;
      }
    }
    else
    {
      Area* key_nav_focus = NULL;
      if (direction == KEY_NAV_UP)
      {
        std::vector<Area*>::reverse_iterator it = m_InterfaceObject.rbegin();
        key_nav_focus = (*it)->KeyNavIteration(direction);

        while (key_nav_focus == NULL)
        {
          ++it;
          if (it == m_InterfaceObject.rend())
            break;

          key_nav_focus = (*it)->KeyNavIteration(direction);
        }
      }
      else
      {
        std::vector<Area*>::iterator it = m_InterfaceObject.begin();
        key_nav_focus = (*it)->KeyNavIteration(direction);

        while (key_nav_focus == NULL)
        {
          ++it;
          if (it == m_InterfaceObject.end())
            break;

          key_nav_focus = (*it)->KeyNavIteration(direction);
        }
      }
      return key_nav_focus;
    }

    return NULL;
  }
}
