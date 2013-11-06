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

  NUX_IMPLEMENT_OBJECT_TYPE(HSplitter);
  static const int HSPLITTERHEIGHT = 5;
  static const int HSTICK_SIZE = 5;

  HSplitter::HSplitter(NUX_FILE_LINE_DECL)
    :   View(NUX_FILE_LINE_PARAM)
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
    SetGeometry(Geometry(m_current_x, m_current_y, m_current_width, m_current_height));
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

  void HSplitter::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    graphics_engine.PushClippingRectangle(GetGeometry());
    Geometry base = GetGeometry();
//    std::vector<View*>::iterator it;
//    for (it = m_InterfaceObject.begin(); it != m_InterfaceObject.end(); it++)
//    {
//        (*it)->ProcessDraw(force_draw);
//    }

    GetPainter().PaintBackground(graphics_engine, base);
    std::vector<MySplitter *>::iterator it_splitter;

    for (it_splitter = m_SplitterObject.begin(); it_splitter != m_SplitterObject.end(); it_splitter++)
    {
      Geometry geo = (*it_splitter)->GetGeometry();
      Geometry grip_geo;
      int w = 20;

      if (geo.GetWidth() > w)
      {
        grip_geo.SetX(geo.x + (geo.GetWidth() - w) / 2);
        grip_geo.SetY(geo.y);
        grip_geo.SetWidth(w);
        grip_geo.SetHeight(geo.GetHeight());
      }
      else
      {
        grip_geo.SetX(geo.x - (w - geo.GetWidth()) / 2);
        grip_geo.SetY(geo.y);
        grip_geo.SetWidth(w);
        grip_geo.SetHeight(geo.GetHeight());
      }

      GetPainter().Draw2DLine(graphics_engine, grip_geo.x, grip_geo.y + 1, grip_geo.x + grip_geo.GetWidth(), grip_geo.y + 1, Color(0xFF111111));
      GetPainter().Draw2DLine(graphics_engine, grip_geo.x, grip_geo.y + 3, grip_geo.x + grip_geo.GetWidth(), grip_geo.y + 3, Color(0xFF111111));
    }

    graphics_engine.PopClippingRectangle();
  }

  void HSplitter::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    Geometry base = GetGeometry();
    graphics_engine.PushClippingRectangle(base);
    bool need_redraw = IsRedrawNeeded();

    std::vector<MySplitter *>::iterator it_splitter;
    std::vector<Area *>::iterator it;

    //for(it = m_InterfaceObject.begin(); it != m_InterfaceObject.end(); it++)
    for (it = m_InterfaceObject.begin(), it_splitter = m_SplitterObject.begin();
         it != m_InterfaceObject.end();
         it++, it_splitter++)
    {
      Geometry sgeo = (*it_splitter)->GetGeometry();
      graphics_engine.PushClippingRectangle(Rect(
                                          base.x, base.y, base.GetWidth(), sgeo.y - base.y));

      base.SetY(sgeo.y + sgeo.GetHeight());

      if (force_draw || need_redraw)
      {
        if ((*it)->Type().IsDerivedFromType(View::StaticObjectType))
        {
          View *ic = static_cast<View *>(*it);
          ic->ProcessDraw(graphics_engine, true);
        }
        else if ((*it)->Type().IsObjectType(HLayout::StaticObjectType))
        {
          HLayout *layout = static_cast<HLayout *>(*it);
          layout->ProcessDraw(graphics_engine, true);
        }
        else if ((*it)->Type().IsObjectType(VLayout::StaticObjectType))
        {
          VLayout *layout = static_cast<VLayout *>(*it);
          layout->ProcessDraw(graphics_engine, true);
        }
      }
      else
      {
        if ((*it)->Type().IsDerivedFromType(View::StaticObjectType))
        {
          View *ic = static_cast<View *>(*it);
          ic->ProcessDraw(graphics_engine, false);
        }
        else if ((*it)->Type().IsObjectType(HLayout::StaticObjectType))
        {
          HLayout *layout = static_cast<HLayout *>(*it);
          layout->ProcessDraw(graphics_engine, false);
        }
        else if ((*it)->Type().IsObjectType(VLayout::StaticObjectType))
        {
          VLayout *layout = static_cast<VLayout *>(*it);
          layout->ProcessDraw(graphics_engine, false);
        }
      }

      graphics_engine.PopClippingRectangle();
    }

    graphics_engine.PopClippingRectangle();
  }

  void HSplitter::OverlayDrawing(GraphicsEngine &graphics_engine)
  {
    unsigned int num_element = (unsigned int) m_SplitterObject.size();

    Geometry base = GetGeometry();

    if (m_focus_splitter_index >= 0)
    {
      Geometry geo = m_SplitterObject[m_focus_splitter_index]->GetGeometry();
      geo.OffsetPosition(mvt_dx, mvt_dy);

      if (m_focus_splitter_index == 0 && num_element > 1)
      {
        if (geo.y < base.y)
        {
          geo.SetY(base.y);
        }

        if (geo.y  + HSPLITTERHEIGHT > m_SplitterObject[m_focus_splitter_index + 1]->GetGeometry().y)
        {
          geo.SetY(m_SplitterObject[m_focus_splitter_index+1]->GetGeometry().y - HSPLITTERHEIGHT);
        }
      }

      if ((m_focus_splitter_index > 0) && (m_focus_splitter_index < (int) num_element - 1))
      {
        if (geo.y < m_SplitterObject[m_focus_splitter_index - 1]->GetGeometry().y + HSPLITTERHEIGHT)
        {
          geo.SetY(m_SplitterObject[m_focus_splitter_index - 1]->GetGeometry().y + HSPLITTERHEIGHT);
        }

        if (geo.y + HSPLITTERHEIGHT > m_SplitterObject[m_focus_splitter_index + 1]->GetGeometry().y)
        {
          geo.SetY(m_SplitterObject[m_focus_splitter_index + 1]->GetGeometry().y - HSPLITTERHEIGHT);
        }
      }

      graphics_engine.GetRenderStates().SetBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      {
        GetPainter().Paint2DQuadColor(graphics_engine, geo, Color(0xBB868686));
      }
      graphics_engine.GetRenderStates().SetBlend(false);
    }
  }

  void HSplitter::AddWidget(Area *ic, float stretchfactor)
  {
    if (ic)
    {
      MySplitter *splitter = new MySplitter;
      splitter->SetParentObject(this);
      //splitter->SinkReference();

      unsigned int no = (unsigned int) m_InterfaceObject.size();
      splitter->mouse_down.connect(sigc::bind(sigc::mem_fun(this, &HSplitter::OnSplitterMouseDown), no));
      splitter->mouse_drag.connect(sigc::bind(sigc::mem_fun(this, &HSplitter::OnSplitterMouseDrag), no));
      splitter->mouse_up.connect(sigc::bind(sigc::mem_fun(this, &HSplitter::OnSplitterMouseUp), no));

      ic->SetParentObject(this);
      m_InterfaceObject.push_back(ic);
      m_SplitterObject.push_back(splitter);
      m_SplitConfig.push_back(stretchfactor);

      new_addition = true;
      ComputeContentSize();
    }
  }

  void HSplitter::clearContent()
  {
    m_InterfaceObject.clear();
  }

  long HSplitter::ComputeContentSize()
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

    if (m_current_width != w)
    {
      for (unsigned int i = 0; i < num_element; i++)
      {
        Geometry splitter_geo = m_SplitterObject[i]->GetGeometry();
        splitter_geo.SetWidth(w);
        splitter_geo.SetX(x);

        m_SplitterObject[i]->SetGeometry(splitter_geo);
      }
    }

    if (m_current_height != h)
    {
      int size_to_distribute = h - num_element * HSPLITTERHEIGHT;
      int previous_spliter_end = m_current_y;
      int new_spliter_end = y;

      for (unsigned int i = 0; i < num_element; i++)
      {
        Geometry splitter_geo = m_SplitterObject[i]->GetGeometry();
        // compute percentage of space occupied by the element i;
        // width of element i = m_SplitterObject[i]->GetY() - previous_splliter_end
        int splitter_start = m_SplitterObject[i]->GetBaseY();
        float percent = float(splitter_start - previous_spliter_end) / float(m_current_height - num_element * HSPLITTERHEIGHT);

        if (percent > 1.0f)
          percent = 1.0f;

        splitter_geo.SetY(new_spliter_end + size_to_distribute * percent);
        previous_spliter_end = splitter_start + HSPLITTERHEIGHT;
        new_spliter_end = new_spliter_end + size_to_distribute * percent + HSPLITTERHEIGHT;
        m_SplitterObject[i]->SetGeometry(splitter_geo);
      }

      if (m_SplitterObject[0]->GetBaseY() < y)
      {
        m_SplitterObject[0]->SetBaseY(y);
      }

      m_SplitterObject[num_element-1]->SetBaseY(y + h - HSPLITTERHEIGHT);
    }

    int accheight = y;

    for (unsigned int i = 0; i < num_element; i++)
    {
      Geometry splitter_geo = m_SplitterObject[i]->GetGeometry();

      //m_InterfaceObject[i]->SetGeometry(Geometry(x, accheight, w, splitter_geo.y - accheight));

      if (m_InterfaceObject[i]->Type().IsDerivedFromType(View::StaticObjectType))
      {
        View *ic = static_cast<View *>(m_InterfaceObject[i]);
        ic->SetGeometry(Geometry(x, accheight, w, splitter_geo.y - accheight));
        // if we are already computing the layout from the main window down, we need to call
        // ComputeElementLayout to force the computing of this element layout.
        GetWindowThread()->ComputeElementLayout(ic);
      }
      else if (m_InterfaceObject[i]->Type().IsDerivedFromType(Layout::StaticObjectType))
      {
        Layout *layout = static_cast<Layout *>(m_InterfaceObject[i]);
        layout->SetGeometry(Geometry(x, accheight, w, splitter_geo.y - accheight));
        // if we are already computing the layout from the main window down, we need to call
        // ComputeElementLayout to force the computing of this element layout.
        GetWindowThread()->ComputeElementLayout(layout);
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

    int availableheight = (h - num_element * HSPLITTERHEIGHT);
    float max_size = float(availableheight) / total;

    for (unsigned int i = 0; i < (unsigned int) m_SplitConfig.size(); i++)
    {
      stretchfactor = m_SplitConfig[i];
      y += stretchfactor * max_size / max_stretch;
      Geometry geo(x, y, w, HSPLITTERHEIGHT);
      m_SplitterObject[i]->SetGeometry(geo);
    }

    m_SplitterObject[num_element-1]->SetBaseX(y + h - HSPLITTERHEIGHT);

    m_initial_config = true;
  }

  void HSplitter::OnSplitterMouseDown(int x, int y, unsigned long /* button_flags */, unsigned long /* key_flags */, int header_pos)
  {
    m_point = Point(x, y);

    m_focus_splitter_index = header_pos;
    GetWindowThread()->GetWindowCompositor().SetWidgetDrawingOverlay(this, GetWindowThread()->GetWindowCompositor().GetProcessingTopView());


    // Hint for the window to initiate a redraw
    GetWindowThread()->RequestRedraw();
  }

  void HSplitter::OnSplitterMouseUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */, int header_pos)
  {
    if (mvt_dy)
    {
      Geometry geo = m_SplitterObject[header_pos]->GetGeometry();
      geo.OffsetPosition(0, mvt_dy);

      unsigned int num_element = (unsigned int) m_SplitterObject.size();

      if (header_pos < (int) num_element - 1)
      {
        // Make the splitter bar stick to the next one if the distance between them is less than HSTICK_SIZE.
        if (m_SplitterObject[header_pos + 1]->GetGeometry().y - geo.y - HSPLITTERHEIGHT < HSTICK_SIZE)
          geo.SetY( m_SplitterObject[header_pos + 1]->GetGeometry().y - HSPLITTERHEIGHT );
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

  void HSplitter::OnSplitterMouseDrag(int /* x */, int y, int /* dx */, int /* dy */, unsigned long /* button_flags */, unsigned long /* key_flags */, int header_pos)
  {
    Geometry geo = m_SplitterObject[header_pos]->GetGeometry();
    int num_element = (int) m_SplitterObject.size();

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
      geo.OffsetPosition(mvt_dx, mvt_dy);
      m_SplitterObject[header_pos]->SetGeometry(geo);
      ResizeSplitter(header_pos);

      mvt_dx = 0;
      mvt_dy = 0;
    }

    // Hint for the window to initiate a redraw
    GetWindowThread()->RequestRedraw();
  }

  void HSplitter::ResizeSplitter(int header_pos)
  {
    int num_element = (int) m_SplitterObject.size();

    if ((header_pos == 0) && (m_SplitterObject[header_pos]->GetBaseY() < GetBaseY()))
    {
      m_SplitterObject[header_pos]->SetBaseY(GetBaseY());
    }

    if ((header_pos == num_element - 1) && (m_SplitterObject[header_pos]->GetBaseY() > GetBaseY() + GetBaseHeight() - HSPLITTERHEIGHT))
    {
      m_SplitterObject[header_pos]->SetBaseY(GetBaseY() + GetBaseHeight() - HSPLITTERHEIGHT);
    }

    if (header_pos < (int) num_element - 1)
    {
      int posy0, posy1;
      posy0 = m_SplitterObject[header_pos]->GetBaseY();
      posy1 = m_SplitterObject[header_pos + 1]->GetBaseY();

      if (posy0 > posy1 - HSPLITTERHEIGHT)
      {
        posy0 = posy1 - HSPLITTERHEIGHT;
        m_SplitterObject[header_pos]->SetBaseY(posy0);
      }
    }

    if (0 < header_pos)
    {
      int posy0, posy1;
      posy0 = m_SplitterObject[header_pos]->GetBaseY();
      posy1 = m_SplitterObject[header_pos - 1]->GetBaseY();

      if (posy0 < posy1 + HSPLITTERHEIGHT)
      {
        posy0 = posy1 + HSPLITTERHEIGHT;
        m_SplitterObject[header_pos]->SetBaseY(posy0);
      }
    }

    ComputeContentSize();
    QueueDraw();
  }

// HSplitter need to re implement DoneRedraw because it does not
// have a m_compositionlayout where its child are located;
  void HSplitter::DoneRedraw()
  {
    std::vector<Area *>::iterator it;

    for (it = m_InterfaceObject.begin(); it != m_InterfaceObject.end(); it++)
    {
      //(*it)->DoneRedraw();
      if ((*it)->Type().IsDerivedFromType(View::StaticObjectType))
      {
        View *ic = static_cast<View *>(*it);
        ic->DoneRedraw();
      }
    }
  }

  Area* HSplitter::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (mouse_inside == false)
      return NULL;

    std::vector<MySplitter*>::iterator splitter_it;
    for (splitter_it = m_SplitterObject.begin(); splitter_it != m_SplitterObject.end(); splitter_it++)
    {
      Area* found_area = (*splitter_it)->FindAreaUnderMouse(mouse_position, event_type);
      if (found_area)
        return found_area;
    }

    std::vector<Area *>::iterator it;
    for (it = m_InterfaceObject.begin(); it != m_InterfaceObject.end(); it++)
    {
      Area* found_area = (*it)->FindAreaUnderMouse(mouse_position, event_type);

      if (found_area)
        return found_area;
    }

    if ((event_type == NUX_MOUSE_WHEEL) && (!AcceptMouseWheelEvent()))
      return NULL;
    return this;
  }

  bool HSplitter::AcceptKeyNavFocus()
  {
    return false;
  }

  Area* HSplitter::KeyNavIteration(KeyNavDirection direction)
  {
    if (m_InterfaceObject.empty())
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
      it = std::find(m_InterfaceObject.begin(), m_InterfaceObject.end(), next_object_to_key_focus_area_);

      if (it == m_InterfaceObject.end())
      {
        // Should never happen
        nuxAssert(0);
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

      if (direction == KEY_NAV_UP)
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

      if (direction == KEY_NAV_DOWN)
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
