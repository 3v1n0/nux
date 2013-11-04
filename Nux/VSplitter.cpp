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
#include "VSplitter.h"

#include "NuxGraphics/GLTextureResourceManager.h"
#include "WindowCompositor.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(VSplitter);
  static const int VSPLITTERWIDTH = 5;
  static const int VSTICK_SIZE = 5;

  VSplitter::VSplitter(NUX_FILE_LINE_DECL)
    :   View(NUX_FILE_LINE_PARAM)
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
    SetGeometry(Geometry(m_current_x, m_current_y, m_current_width, m_current_height));
  }

  VSplitter::~VSplitter()
  {
    // Delete all the interface object: This is a problem... The widget should be destroy by there associated parameters
    //delete vlayout;
    std::vector< Area* >::iterator it0;
    for (it0 = m_InterfaceObject.begin(); it0 != m_InterfaceObject.end(); ++it0)
    {
      (*it0)->UnParentObject();
    }
    m_InterfaceObject.clear();

    std::vector< MySplitter* >::iterator it2;
    for (it2 = m_SplitterObject.begin(); it2 != m_SplitterObject.end(); ++it2)
    {
      (*it2)->UnParentObject();
    }
    m_SplitterObject.clear();

    m_SplitConfig.clear();
  }

  void VSplitter::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    Geometry base = GetGeometry();
    graphics_engine.PushClippingRectangle(base);
    std::vector<Area *>::iterator it;

    GetPainter().PaintBackground(graphics_engine, base);
    std::vector<MySplitter *>::iterator it_splitter;

    for (it_splitter = m_SplitterObject.begin(); it_splitter != m_SplitterObject.end(); ++it_splitter)
    {
      Geometry geo = (*it_splitter)->GetGeometry();
      Geometry grip_geo;
      int h = 20;

      if (geo.GetHeight() > h)
      {
        grip_geo.SetX(geo.x);
        grip_geo.SetY(geo.y + (geo.GetHeight() - h) / 2);
        grip_geo.SetWidth(geo.GetWidth());
        grip_geo.SetHeight(h);
      }
      else
      {
        grip_geo.SetX(geo.x);
        grip_geo.SetY(geo.y + (h - geo.GetHeight()) / 2);
        grip_geo.SetWidth(geo.GetWidth());
        grip_geo.SetHeight(h);
      }

      GetPainter().Draw2DLine(graphics_engine, grip_geo.x + 1, grip_geo.y, grip_geo.x + 1, grip_geo.y + grip_geo.GetHeight(), Color(0xFF111111));
      GetPainter().Draw2DLine(graphics_engine, grip_geo.x + 3, grip_geo.y, grip_geo.x + 3, grip_geo.y + grip_geo.GetHeight(), Color(0xFF111111));
    }

    graphics_engine.PopClippingRectangle();
  }

  void VSplitter::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    graphics_engine.PushClippingRectangle(GetGeometry());
    Geometry base = GetGeometry();
    bool need_redraw = IsRedrawNeeded();

    std::vector<MySplitter *>::iterator it_splitter;
    std::vector<Area *>::iterator it;

    for (it = m_InterfaceObject.begin(), it_splitter = m_SplitterObject.begin();
         it != m_InterfaceObject.end();
         ++it, ++it_splitter)
    {
      Geometry sgeo = (*it_splitter)->GetGeometry();
      graphics_engine.PushClippingRectangle(Rect(
                                          base.x, base.y, sgeo.x - base.x, base.GetHeight()));

      base.SetX(sgeo.x + sgeo.GetWidth());

      if (force_draw || need_redraw)
      {
        if ((*it)->Type().IsDerivedFromType(View::StaticObjectType))
        {
          View *ic = NUX_STATIC_CAST(View *, (*it));
          ic->ProcessDraw(graphics_engine, true);
        }
        else if ((*it)->Type().IsObjectType(HLayout::StaticObjectType))
        {
          HLayout *layout = NUX_STATIC_CAST(HLayout *, (*it));
          layout->ProcessDraw(graphics_engine, true);
        }
        else if ((*it)->Type().IsObjectType(VLayout::StaticObjectType))
        {
          VLayout *layout = NUX_STATIC_CAST(VLayout *, (*it));
          layout->ProcessDraw(graphics_engine, true);
        }
      }
      else
      {
        if ((*it)->Type().IsDerivedFromType(View::StaticObjectType))
        {
          View *ic = NUX_STATIC_CAST(View *, (*it));
          ic->ProcessDraw(graphics_engine, false);
        }
        else if ((*it)->Type().IsObjectType(HLayout::StaticObjectType))
        {
          HLayout *layout = NUX_STATIC_CAST(HLayout *, (*it));
          layout->ProcessDraw(graphics_engine, false);
        }
        else if ((*it)->Type().IsObjectType(VLayout::StaticObjectType))
        {
          VLayout *layout = NUX_STATIC_CAST(VLayout *, (*it));
          layout->ProcessDraw(graphics_engine, false);
        }
      }

      graphics_engine.PopClippingRectangle();
    }

    graphics_engine.PopClippingRectangle();
  }

  void VSplitter::OverlayDrawing(GraphicsEngine &graphics_engine)
  {
    unsigned int num_element = (unsigned int) m_SplitterObject.size();

    Geometry base = GetGeometry();

    if (m_focus_splitter_index >= 0)
    {
      Geometry geo = m_SplitterObject[m_focus_splitter_index]->GetGeometry();
      geo.OffsetPosition(mvt_dx, mvt_dy);

      if (m_focus_splitter_index == 0 && num_element > 1)
      {
        if (geo.x < base.x)
        {
          geo.SetX(base.x);
        }

        if (geo.x  + VSPLITTERWIDTH > m_SplitterObject[m_focus_splitter_index + 1]->GetGeometry().x)
        {
          geo.SetX(m_SplitterObject[m_focus_splitter_index+1]->GetGeometry().x - VSPLITTERWIDTH);
        }
      }

      if ((m_focus_splitter_index > 0) && m_focus_splitter_index < (int) num_element - 1)
      {
        if (geo.x < m_SplitterObject[m_focus_splitter_index - 1]->GetGeometry().x + VSPLITTERWIDTH)
        {
          geo.SetX(m_SplitterObject[m_focus_splitter_index - 1]->GetGeometry().x + VSPLITTERWIDTH);
        }

        if (geo.x + VSPLITTERWIDTH > m_SplitterObject[m_focus_splitter_index + 1]->GetGeometry().x)
        {
          geo.SetX(m_SplitterObject[m_focus_splitter_index + 1]->GetGeometry().x - VSPLITTERWIDTH);
        }
      }

      graphics_engine.GetRenderStates().SetBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      {
        GetPainter().Paint2DQuadColor(graphics_engine, geo, Color(0xBB868686));
      }
      graphics_engine.GetRenderStates().SetBlend(false);
    }
  }

  void VSplitter::AddWidget(Area *ic, float stretchfactor)
  {
    if (ic)
    {
      MySplitter* splitter = new MySplitter;
      splitter->SetParentObject(this);
      //splitter->SinkReference();

      unsigned int no = (unsigned int) m_InterfaceObject.size();
      splitter->mouse_down.connect(sigc::bind(sigc::mem_fun(this, &VSplitter::OnSplitterMouseDown), no));
      splitter->mouse_up.connect(sigc::bind(sigc::mem_fun(this, &VSplitter::OnSplitterMouseUp), no));
      splitter->mouse_drag.connect(sigc::bind(sigc::mem_fun(this, &VSplitter::OnSplitterMouseDrag), no));

      ic->SetParentObject(this);
      m_InterfaceObject.push_back(ic);
      m_SplitterObject.push_back(splitter);
      m_SplitConfig.push_back(stretchfactor);

      new_addition = true;
      ComputeContentSize();
    }
  }

  void VSplitter::clearContent()
  {
    m_InterfaceObject.clear();
  }

  long VSplitter::ComputeContentSize()
  {
    unsigned int num_element = (unsigned int) m_InterfaceObject.size();
    int x = GetBaseX();
    int y = GetBaseY();
    int w = GetBaseWidth();
    int h = GetBaseHeight();

    if ((w == 0) || (h == 0))
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
      for (unsigned int i = 0; i < num_element; i++)
      {
        Geometry splitter_geo = m_SplitterObject[i]->GetGeometry();
        splitter_geo.SetHeight(h);
        splitter_geo.SetY(y);

        m_SplitterObject[i]->SetGeometry(splitter_geo);
      }
    }

    if (m_current_width != w)
    {
      int size_to_distribute = w - num_element * VSPLITTERWIDTH;
      int previous_spliter_end = m_current_x;
      int new_spliter_end = x;

      for (unsigned int i = 0; i < num_element; i++)
      {
        Geometry splitter_geo = m_SplitterObject[i]->GetGeometry();
        // compute percentage of space occupied by the element i;
        // width of element i = m_SplitterObject[i]->GetX() - previous_splliter_end
        int splitter_start = m_SplitterObject[i]->GetBaseX();
        float percent = float(splitter_start - previous_spliter_end) / float(m_current_width - num_element * VSPLITTERWIDTH);

        if (percent > 1.0f)
          percent = 1.0f;

        splitter_geo.SetX(new_spliter_end + size_to_distribute * percent);
        previous_spliter_end = splitter_start + VSPLITTERWIDTH;
        new_spliter_end = new_spliter_end + size_to_distribute * percent + VSPLITTERWIDTH;
        m_SplitterObject[i]->SetGeometry(splitter_geo);
      }

      if (m_SplitterObject[0]->GetBaseX() < x)
      {
        m_SplitterObject[0]->SetBaseX(x);
      }

      m_SplitterObject[num_element-1]->SetBaseX(x + w - VSPLITTERWIDTH);
    }

    int accwidth = x;

    for (unsigned int i = 0; i < num_element; i++)
    {
      Geometry splitter_geo = m_SplitterObject[i]->GetGeometry();

      //m_InterfaceObject[i]->SetGeometry(Geometry(accwidth, y, splitter_geo.x - accwidth, h));

      if (m_InterfaceObject[i]->Type().IsDerivedFromType(View::StaticObjectType))
      {
        View *ic = NUX_STATIC_CAST(View *, m_InterfaceObject[i]);
        ic->SetGeometry(Geometry(accwidth, y, splitter_geo.x - accwidth, h));
        // if we are already computing the layout from the main window down, we need to call
        // ComputeElementLayout to force the computing of this element layout.
        GetWindowThread()->ComputeElementLayout(ic);
      }
      else if (m_InterfaceObject[i]->Type().IsDerivedFromType(Layout::StaticObjectType))
      {
        Layout *layout = NUX_STATIC_CAST(Layout *, m_InterfaceObject[i]);
        layout->SetGeometry(Geometry(accwidth, y, splitter_geo.x - accwidth, h));
        // if we are already computing the layout from the main window down, we need to call
        // ComputeElementLayout to force the computing of this element layout.
        GetWindowThread()->ComputeElementLayout(layout);
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
    int x = GetBaseX();
    int y = GetBaseY();
    int w = GetBaseWidth();
    int h = GetBaseHeight();
    unsigned int num_element = (unsigned int) m_InterfaceObject.size();

    if (num_element < 1)
    {
      return;
    }

    float max_stretch = 0.0f;
    float stretchfactor;

    for (unsigned int i = 0; i < (unsigned int) m_SplitConfig.size(); i++)
    {
      stretchfactor = m_SplitConfig[i];

      if (max_stretch < stretchfactor)
      {
        max_stretch = stretchfactor;
      }
    }

    float total = 0;

    for (unsigned int i = 0; i < (unsigned int) m_SplitConfig.size(); i++)
    {
      stretchfactor = m_SplitConfig[i];
      total += stretchfactor / max_stretch;
    }

    int availableheight = (w - num_element * VSPLITTERWIDTH);
    float max_size = float(availableheight) / total;

    for (unsigned int i = 0; i < (unsigned int) m_SplitConfig.size(); i++)
    {
      stretchfactor = m_SplitConfig[i];
      x += stretchfactor * max_size / max_stretch;
      Geometry geo(x, y, VSPLITTERWIDTH, h);
      m_SplitterObject[i]->SetGeometry(geo);
    }

    m_SplitterObject[num_element-1]->SetBaseX(x + w - VSPLITTERWIDTH);

    m_initial_config = true;
  }

  void VSplitter::OnSplitterMouseDown(int x, int y, unsigned long /* button_flags */, unsigned long /* key_flags */, int header_pos)
  {
    m_point = Point(x, y);

    m_focus_splitter_index = header_pos;
    GetWindowThread()->GetWindowCompositor().SetWidgetDrawingOverlay(this, GetWindowThread()->GetWindowCompositor().GetProcessingTopView());

    // Hint for the window to initiate a redraw
    GetWindowThread()->RequestRedraw();
  }

  void VSplitter::OnSplitterMouseUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */, int header_pos)
  {
    if (mvt_dx)
    {
      Geometry geo = m_SplitterObject[header_pos]->GetGeometry();
      geo.OffsetPosition(mvt_dx, 0);

      unsigned int num_element = (unsigned int) m_SplitterObject.size();

      if (header_pos < (int) num_element - 1)
      {
        // Make the splitter bar stick to the next one if the distance between them is less than VSTICK_SIZE
        if (m_SplitterObject[header_pos + 1]->GetGeometry().x - geo.x - VSPLITTERWIDTH < VSTICK_SIZE)
          geo.SetX( m_SplitterObject[header_pos + 1]->GetGeometry().x - VSPLITTERWIDTH );
      }

      m_SplitterObject[header_pos]->SetGeometry(geo);
      ResizeSplitter(header_pos);
    }

    m_focus_splitter_index = -1;
    mvt_dx = 0;
    mvt_dy = 0;

    // End overlay drawing;
    GetWindowThread()->GetWindowCompositor().SetWidgetDrawingOverlay(0, GetWindowThread()->GetWindowCompositor().GetProcessingTopView());
    // Hint for the window to initiate a redraw
    GetWindowThread()->RequestRedraw();
  }

  void VSplitter::OnSplitterMouseDrag(int x, int /* y */, int /* dx */, int /* dy */, unsigned long /* button_flags */, unsigned long /* key_flags */, int header_pos)
  {
    Geometry geo = m_SplitterObject[header_pos]->GetGeometry();
    int num_element = (int) m_SplitterObject.size();

    if (header_pos == num_element - 1)
    {
      // The last splitter cannot be moved
      return;
    }

    mvt_dx = x - m_point.x;
    mvt_dy = 0;

    if (m_ResizeOnSplitterRelease == false)
    {
      // Continuously redraw resize and redraw the 2 parts of the widget.
      // This is slow.
      geo.OffsetPosition(mvt_dx, mvt_dy);
      m_SplitterObject[header_pos]->SetGeometry(geo);
      ResizeSplitter(header_pos);

      mvt_dx = 0;
      mvt_dy = 0;
    }

    // Hint for the window to initiate a redraw
    GetWindowThread()->RequestRedraw();
  }

  void VSplitter::ResizeSplitter(int header_pos)
  {
    int num_element = (int) m_SplitterObject.size();

    if ((header_pos == 0) && (m_SplitterObject[header_pos]->GetBaseX() < GetBaseX()))
    {
      m_SplitterObject[header_pos]->SetBaseX(GetBaseX());
    }

    if ((header_pos == num_element - 1) && (m_SplitterObject[header_pos]->GetBaseX() > GetBaseX() + GetBaseWidth() - VSPLITTERWIDTH))
    {
      m_SplitterObject[header_pos]->SetBaseX(GetBaseX() + GetBaseWidth() - VSPLITTERWIDTH);
    }

    if (header_pos < (int) num_element - 1)
    {
      int posx0, posx1;
      posx0 = m_SplitterObject[header_pos]->GetBaseX();
      posx1 = m_SplitterObject[header_pos + 1]->GetBaseX();

      if (posx0 > posx1 - VSPLITTERWIDTH)
      {
        posx0 = posx1 - VSPLITTERWIDTH;
        m_SplitterObject[header_pos]->SetBaseX(posx0);
      }
    }

    if (0 < header_pos)
    {
      int posx0, posx1;
      posx0 = m_SplitterObject[header_pos]->GetBaseX();
      posx1 = m_SplitterObject[header_pos - 1]->GetBaseX();

      if (posx0 < posx1 + VSPLITTERWIDTH)
      {
        posx0 = posx1 + VSPLITTERWIDTH;
        m_SplitterObject[header_pos]->SetBaseX(posx0);
      }
    }

    m_initial_config = false;
    ComputeContentSize();

    QueueDraw();
  }

// VSplitter need to re implement DoneRedraw because it does not
// have a m_compositionlayout where its child are located;
  void VSplitter::DoneRedraw()
  {
    std::vector<Area *>::iterator it;

    for (it = m_InterfaceObject.begin(); it != m_InterfaceObject.end(); ++it)
    {
      //(*it)->DoneRedraw();
      if ((*it)->Type().IsDerivedFromType(View::StaticObjectType))
      {
        View *ic = NUX_STATIC_CAST(View *, (*it));
        ic->DoneRedraw();
      }
    }
  }

  Area* VSplitter::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (mouse_inside == false)
      return NULL;

    std::vector<MySplitter*>::iterator splitter_it;
    for (splitter_it = m_SplitterObject.begin(); splitter_it != m_SplitterObject.end(); ++splitter_it)
    {
      Area* found_area = (*splitter_it)->FindAreaUnderMouse(mouse_position, event_type);
      if (found_area)
        return found_area;
    }

    std::vector<Area *>::iterator it;
    for (it = m_InterfaceObject.begin(); it != m_InterfaceObject.end(); ++it)
    {
      Area* found_area = (*it)->FindAreaUnderMouse(mouse_position, event_type);

      if (found_area)
        return found_area;
    }

    if ((event_type == NUX_MOUSE_WHEEL) && (!AcceptMouseWheelEvent()))
      return NULL;
    return this;
  }

  bool VSplitter::AcceptKeyNavFocus()
  {
    return false;
  }

  Area* VSplitter::KeyNavIteration(KeyNavDirection direction)
  {
    if (m_InterfaceObject.empty())
      return NULL;

    if (next_object_to_key_focus_area_)
    {
      if ((direction == KEY_NAV_UP) || (direction == KEY_NAV_DOWN))
      {
        // Don't know what to do with this
        return NULL;
      }
      std::vector<Area*>::iterator it;
      std::vector<Area*>::iterator it_next;
      it = std::find(m_InterfaceObject.begin(), m_InterfaceObject.end(), next_object_to_key_focus_area_);

      if (it == m_InterfaceObject.end())
      {
        // Should never happen
        nuxAssert(0);
        return NULL;
      }

      it_next = it;
      ++it_next;

      if ((direction == KEY_NAV_LEFT) && (it == m_InterfaceObject.begin()))
      {
        // can't go further
        return NULL;
      }

      if ((direction == KEY_NAV_RIGHT) && (it_next == m_InterfaceObject.end()))
      {
        // can't go further
        return NULL;
      }

      if (direction == KEY_NAV_LEFT)
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

      if (direction == KEY_NAV_RIGHT)
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
      
      if (direction == KEY_NAV_LEFT)
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
