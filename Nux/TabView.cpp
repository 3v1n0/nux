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
#include "TabView.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"

namespace nux
{

  int TabView::TAB_BUTTON_WIDTH   = 12;
  int TabView::TAB_BUTTON_HEIGHT  = 20;
  int TabView::TAB_HEIGHT         = 24;
  int TabView::TAB_X_BORDER       = 4;
  int TabView::TAB_Y_BORDER       = 4;

  Color TabView::TAB_HEADER_BACKGROUND_COLOR    = Color(0xFF000000);
  Color TabView::TAB_BACKGROUND_COLOR           = Color(0xFF191919);
  Color TabView::TAB_HEADER_COLOR               = Color(0xFF333333);
  Color TabView::TAB_HEADER_FOCUS_COLOR         = Color(0xFF5D5D5D);

  TabView::TabElement::TabElement(std::string TabName, Layout *TabLayout)
  {
    _index = 0;
    _tab_name = TabName;
    _tab_content_layout = TabLayout;
    _tab_area = new BasicView(NUX_TRACKER_LOCATION);

    _tab_content_layout->Reference();
    _tab_area->Reference();
  }

  TabView::TabElement::~TabElement()
  {
    _index = 0;
    _tab_content_layout->UnReference();
    _tab_area->UnReference();
  }

  void TabView::TabElement::SetIndex(int index)
  {
    _index = index;
  }

  int TabView::TabElement::GetIndex() const
  {
    return _index;
  }

  void TabView::TabElement::SetGeometry(const Geometry &geo)
  {
    _tab_area->SetGeometry(geo);
  }

  Geometry const& TabView::TabElement::GetGeometry() const
  {
    return _tab_area->GetGeometry();
  }

  const std::string &TabView::TabElement::GetName() const
  {
    return _tab_name;
  }

  TabView::TabView(NUX_FILE_LINE_DECL)
    :   View(NUX_FILE_LINE_PARAM)
  {
    _scroll_right           = NULL;
    _scroll_left           = NULL;
    _visible_tab_content_layout      = NULL;
    _tabview_heads_layout         = NULL;
    _tabview_scroll_button_layout  = NULL;

    m_FocusTabIndex     = 0;
    m_TabPositionOffset = 0;
    m_DrawBackgroundOnPreviousGeometry = false;

    _tabview_heads_layout         = new HLayout(NUX_TRACKER_LOCATION);
    //_tabview_heads_layout->SetParentObject(this);
    _tabview_scroll_button_layout  = new HLayout(NUX_TRACKER_LOCATION);
    _tabview_scroll_button_layout->SetParentObject(this);
    _scroll_right           = new BasicView(NUX_TRACKER_LOCATION);
    _scroll_left           = new BasicView(NUX_TRACKER_LOCATION);

    _scroll_right->SetMinimumSize(TAB_BUTTON_WIDTH, TAB_BUTTON_HEIGHT);
    _scroll_left->SetMinimumSize(TAB_BUTTON_WIDTH, TAB_BUTTON_HEIGHT);
    _scroll_right->SetMaximumSize(TAB_BUTTON_WIDTH, TAB_BUTTON_HEIGHT);
    _scroll_left->SetMaximumSize(TAB_BUTTON_WIDTH, TAB_BUTTON_HEIGHT);

    _scroll_right->mouse_enter.connect(sigc::mem_fun(this, &TabView::RecvMouseEnter));
    _scroll_right->mouse_leave.connect(sigc::mem_fun(this, &TabView::RecvMouseLeave));
    _scroll_left->mouse_enter.connect(sigc::mem_fun(this, &TabView::RecvMouseEnter));
    _scroll_left->mouse_leave.connect(sigc::mem_fun(this, &TabView::RecvMouseLeave));

    _scroll_right->mouse_down.connect(sigc::mem_fun(this, &TabView::RecvTabRightMouseDown));
    _scroll_left->mouse_down.connect(sigc::mem_fun(this, &TabView::RecvTabLeftMouseDown));
    _scroll_right->mouse_up.connect(sigc::mem_fun(this, &TabView::RecvTabButtonMouseUp));
    _scroll_left->mouse_up.connect(sigc::mem_fun(this, &TabView::RecvTabButtonMouseUp));

    _tabview_scroll_button_layout->AddView(_scroll_left, 1, eCenter);
    _tabview_scroll_button_layout->AddView(_scroll_right, 1, eCenter);


    tabright_callback = new TimerFunctor;
    tabright_callback->tick.connect(sigc::mem_fun(this, &TabView::RecvTabRightTimerExpired));
    tableft_callback = new TimerFunctor;
    tableft_callback->tick.connect(sigc::mem_fun(this, &TabView::RecvTabLeftTimerExpired));
  }

  TabView::~TabView()
  {
    _tabview_heads_layout->UnReference();
    _tabview_scroll_button_layout->UnReference();
    RemoveCompositionLayout();
    delete(tabright_callback);
    delete(tableft_callback);
    
    std::vector<TabElement *>::iterator it;

    for (it = _tab_array.begin(); it != _tab_array.end(); it++)
    {
      (*it)->_tab_area->UnParentObject();
      (*it)->_tab_content_layout->UnParentObject();
      delete(*it);
    }

    _tab_array.clear();
  }

  Area* TabView::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (mouse_inside == false)
      return NULL;

    NUX_RETURN_VALUE_IF_TRUE(_scroll_right->TestMousePointerInclusion(mouse_position, event_type), _scroll_right);
    NUX_RETURN_VALUE_IF_TRUE(_scroll_left->TestMousePointerInclusion(mouse_position, event_type), _scroll_left);

    unsigned int vector_size = (unsigned int) _tab_array.size();
    for (unsigned int i = 0; i < vector_size; i++)
    {
      NUX_RETURN_VALUE_IF_TRUE(_tab_array[i]->_tab_area->TestMousePointerInclusion(mouse_position, event_type), _tab_array[i]->_tab_area);
    }

    if (_visible_tab_content_layout)
    {
      nuxAssert(_visible_tab_content_layout->IsLayout());
      Area* found_area = _visible_tab_content_layout->FindAreaUnderMouse(mouse_position, event_type);
      if (found_area)
        return found_area;
    }

    return this;
  }


  void TabView::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    if (m_DrawBackgroundOnPreviousGeometry)
    {
      GetPainter().PaintBackground(graphics_engine, m_PreviousGeometry);
      m_DrawBackgroundOnPreviousGeometry = false;
    }

    graphics_engine.PushClippingRectangle(GetGeometry());
    Geometry base = GetGeometry();

    GetPainter().PushDrawShapeLayer(graphics_engine, Geometry(base.x, base.y, base.GetWidth(), TAB_HEIGHT), eSHAPE_CORNER_ROUND4, TAB_HEADER_BACKGROUND_COLOR, eCornerTopLeft | eCornerTopRight);

    if (_visible_tab_content_layout)
      _visible_tab_content_layout->QueueDraw();

    unsigned int vector_size = (unsigned int) _tab_array.size();

    Geometry geo = GetGeometry();
    Geometry clip_geo;
    clip_geo.SetX(geo.x);
    clip_geo.SetY(geo.y);
    clip_geo.SetWidth(geo.GetWidth() - 2 * TAB_BUTTON_WIDTH);
    clip_geo.SetHeight(_tabview_heads_layout->GetBaseHeight());

    graphics_engine.PushClippingRectangle(clip_geo);

    for (unsigned int i = 0; i < vector_size; i++)
    {
      Geometry tab_geo = _tab_array[i]->_tab_area->GetGeometry();
      const char *tab_text = _tab_array[i]->GetName().c_str();

      if (_tab_array[i]->_index == m_FocusTabIndex)
      {
        tab_geo.OffsetSize(-2, 0);
        GetPainter().PaintShapeCorner(graphics_engine, tab_geo, TAB_HEADER_FOCUS_COLOR, eSHAPE_CORNER_ROUND4, eCornerTopLeft | eCornerTopRight, false);
        GetPainter().PaintTextLineStatic(graphics_engine, GetSysBoldFont(), tab_geo, tab_text, Color(0xFFFFFFFF), true, eAlignTextCenter);
      }
      else
      {
        tab_geo.OffsetSize(-2, 0);
        GetPainter().PaintShapeCorner(graphics_engine, tab_geo, TAB_HEADER_COLOR, eSHAPE_CORNER_ROUND4, eCornerTopLeft | eCornerTopRight, false);
        GetPainter().PaintTextLineStatic(graphics_engine, GetSysBoldFont(), tab_geo, tab_text, Color(0xFF000000), true, eAlignTextCenter);
      }
    }

    graphics_engine.PopClippingRectangle();

    GetPainter().PaintShapeCorner(graphics_engine, Geometry(base.x, base.y + TAB_HEIGHT, base.GetWidth(), base.GetHeight() - TAB_HEIGHT),
                               TAB_BACKGROUND_COLOR, eSHAPE_CORNER_ROUND4, eCornerBottomLeft | eCornerBottomRight, false);

    GetPainter().Paint2DQuadColor(graphics_engine, _scroll_right->GetGeometry(), TAB_HEADER_BACKGROUND_COLOR);
    GeometryPositioning gp(eHACenter, eVACenter);
    Geometry GeoPo = ComputeGeometryPositioning(_scroll_right->GetGeometry(), GetTheme().GetImageGeometry(eTAB_RIGHT), gp);

    if (_scroll_right->IsMouseInside())
      GetPainter().PaintShape(graphics_engine, GeoPo, Color(0xFFFFFFFF), eTAB_RIGHT);
    else
      GetPainter().PaintShape(graphics_engine, GeoPo, TAB_HEADER_FOCUS_COLOR, eTAB_RIGHT);

    GetPainter().Paint2DQuadColor(graphics_engine, _scroll_left->GetGeometry(), TAB_HEADER_BACKGROUND_COLOR);
    gp.SetAlignment(eHACenter, eVACenter);
    GeoPo = ComputeGeometryPositioning(_scroll_left->GetGeometry(), GetTheme().GetImageGeometry(eTAB_LEFT), gp);

    if (_scroll_left->IsMouseInside())
      GetPainter().PaintShape(graphics_engine, GeoPo, Color(0xFFFFFFFF), eTAB_LEFT);
    else
      GetPainter().PaintShape(graphics_engine, GeoPo, TAB_HEADER_FOCUS_COLOR, eTAB_LEFT);

    GetPainter().PopBackground();
    graphics_engine.PopClippingRectangle();
  }

  void TabView::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    Geometry base = GetGeometry();

    graphics_engine.PushClippingRectangle(GetGeometry());
    GetPainter().PushShapeLayer(graphics_engine, Geometry(base.x, base.y + TAB_HEIGHT, base.GetWidth(), base.GetHeight() - TAB_HEIGHT),
                             eSHAPE_CORNER_ROUND4, TAB_BACKGROUND_COLOR, eCornerBottomLeft | eCornerBottomRight);

    if (_visible_tab_content_layout)
    {
      graphics_engine.PushClippingRectangle(_visible_tab_content_layout->GetGeometry());
      _visible_tab_content_layout->ProcessDraw(graphics_engine, force_draw);
      graphics_engine.PopClippingRectangle();
    }

    GetPainter().PopBackground();
    graphics_engine.PopClippingRectangle();
  }

  void TabView::PreLayoutManagement()
  {
    // Give the managed layout appropriate size and position..
    if (view_layout_)
    {
      Geometry layout_geo = GetGeometry();
      layout_geo.OffsetPosition(TAB_X_BORDER, TAB_HEIGHT);
      layout_geo.OffsetSize(-2 * TAB_X_BORDER, - (TAB_HEIGHT) - TAB_Y_BORDER);
      view_layout_->SetGeometry(layout_geo);
    }
  }

  long TabView::PostLayoutManagement(long LayoutResult)
  {
    // Set the geometry of the control to be the same as the managed layout.
    // Only the size is changed. The position of the composition layout hasn't
    // been changed by ComputeContentSize.
    if (view_layout_)
    {
      Geometry base = view_layout_->GetGeometry();
      base.OffsetPosition(-TAB_X_BORDER, -TAB_HEIGHT);
      base.OffsetSize(2 * TAB_X_BORDER, TAB_HEIGHT + TAB_Y_BORDER);
      Area::SetGeometry(base);
    }

    Geometry base = GetGeometry();

    int tab_x = view_layout_->GetGeometry().x + view_layout_->GetGeometry().GetWidth() - 2 * TAB_BUTTON_WIDTH;
    int tab_y = base.y;

    _scroll_left->SetBaseXY(tab_x, tab_y);
    _scroll_left->SetBaseSize(TAB_BUTTON_WIDTH, TAB_BUTTON_HEIGHT);
    _scroll_right->SetBaseXY(tab_x + TAB_BUTTON_WIDTH, tab_y);
    _scroll_right->SetBaseSize(TAB_BUTTON_WIDTH, TAB_BUTTON_HEIGHT);

    _tabview_heads_layout->SetBaseY(tab_y);
    _tabview_heads_layout->SetBaseHeight(TAB_HEIGHT);
    TranslateTabLayout(0);

    _tabview_scroll_button_layout->SetBaseXY(tab_x, tab_y);
    _tabview_scroll_button_layout->SetBaseSize(2 * TAB_BUTTON_WIDTH, TAB_HEIGHT);
    GetWindowThread()->ComputeElementLayout(_tabview_scroll_button_layout);

    if (_visible_tab_content_layout)
    {
//        _visible_tab_content_layout->SetGeometry(view_layout_->GetGeometry());
//        GetWindowThread()->ComputeElementLayout(_visible_tab_content_layout);
      _visible_tab_content_layout->QueueDraw();
    }

    return LayoutResult;
  }

  void TabView::ComputeContentPosition(float offsetX, float offsetY)
  {
    if (view_layout_)
    {
      view_layout_->SetBaseX(GetBaseX() + TAB_X_BORDER);
      view_layout_->SetBaseY(GetBaseY() + TAB_HEIGHT);
      view_layout_->ComputeContentPosition(offsetX, offsetY);
    }
  }


  void TabView::AddTab(const char *tab_name, Layout *tab_layout)
  {
    if (tab_layout == 0)
      return;

    TabElement *Tab = new TabElement(tab_name, tab_layout);

    Tab->SetIndex(_tab_array.size());

    if (Tab->GetIndex() == 0)
    {
      m_FocusTabIndex = 0;
      _visible_tab_content_layout = Tab->_tab_content_layout;
      //_visible_tab_content_layout->SetGeometry(view_layout_->GetGeometry());
      SetCompositionLayout(_visible_tab_content_layout);
      GetWindowThread()->ComputeElementLayout(this);
    }

    Tab->_tab_area->SetMinimumSize(6 + GetSysBoldFont()->GetStringWidth(tab_name), PRACTICAL_WIDGET_HEIGHT);
    Tab->_tab_area->SetMaximumSize(6 + GetSysBoldFont()->GetStringWidth(tab_name), PRACTICAL_WIDGET_HEIGHT);

    Tab->_tab_area->mouse_down.connect(sigc::bind(sigc::mem_fun(this, &TabView::RecvTabMouseDown), Tab));
    Tab->_tab_area->mouse_up.connect(sigc::bind(sigc::mem_fun(this, &TabView::RecvTabMouseUp), Tab));

    _tabview_heads_layout->AddView(Tab->_tab_area, 1, eCenter);
    GetWindowThread()->ComputeElementLayout(_tabview_heads_layout);

    _tab_array.push_back(Tab);
  }

  void TabView::SetActiveTad(int index)
  {
    if (index >= (int) _tab_array.size())
    {
      m_FocusTabIndex = (int) _tab_array.size() - 1;
    }
    else
    {
      m_FocusTabIndex = index;
    }

    _visible_tab_content_layout = _tab_array[m_FocusTabIndex]->_tab_content_layout;

    if (_visible_tab_content_layout)
      SetCompositionLayout(_visible_tab_content_layout);

    GetWindowThread()->ComputeElementLayout(this);

    sigTabChanged(this);
    sigTabIndexChanged(m_FocusTabIndex);

    QueueDraw();
  }

  void TabView::TranslateLeft(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    TranslateTabLayout(-5);
  }

  void TabView::TranslateRight(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    TranslateTabLayout(5);
  }

  void TabView::TranslateTabLayout(int offset)
  {
//    if ((m_TabPositionOffset == 0) && (offset > 0))
//        return;

    m_TabPositionOffset += offset;
    int lx = view_layout_->GetBaseX() + m_TabPositionOffset;

    if (lx > view_layout_->GetBaseX())
    {
      // end of scroll left;
      m_TabPositionOffset = 0;
      lx = view_layout_->GetBaseX() + m_TabPositionOffset;
    }

    if (lx + _tabview_heads_layout->GetBaseWidth() < view_layout_->GetBaseX() +
        view_layout_->GetBaseWidth() - 2 * TAB_BUTTON_WIDTH)
    {

      lx = (view_layout_->GetBaseX() +
            view_layout_->GetBaseWidth() - 2 * TAB_BUTTON_WIDTH) - _tabview_heads_layout->GetBaseWidth();


      if (lx > view_layout_->GetBaseX())
      {
        m_TabPositionOffset = 0;
        lx = view_layout_->GetBaseX() + m_TabPositionOffset;
      }
      else
        m_TabPositionOffset = - (view_layout_->GetBaseX() - lx);
    }

    _tabview_heads_layout->SetBaseX(lx);
    GetWindowThread()->ComputeElementLayout(_tabview_heads_layout);
    QueueDraw();

  }

  void TabView::RecvTabMouseDown(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */, TabElement *tab)
  {
    m_FocusTabIndex = tab->_index;
    _visible_tab_content_layout = tab->_tab_content_layout;

    if (_visible_tab_content_layout)
    {
      SetCompositionLayout(_visible_tab_content_layout);
    }

    //_visible_tab_content_layout->SetGeometry(view_layout_->GetGeometry());

    m_PreviousGeometry = GetGeometry();

    int PrevWidth = GetBaseWidth();
    int PrevHeight = GetBaseHeight();

    GetWindowThread()->ComputeElementLayout(this, true);


    int NewWidth = GetBaseWidth();
    int NewHeight = GetBaseHeight();

    if ((NewWidth != PrevWidth) ||
         (NewHeight != PrevHeight))
    {
      // We need to draw the background on the previous size of the Table if its
      // size is set to match the content(IsSizeMatchContent) and an item is close.
      //m_PreviousGeometry;
      m_DrawBackgroundOnPreviousGeometry = true;
    }

    m_DrawBackgroundOnPreviousGeometry = true;

    sigTabChanged(this);
    sigTabIndexChanged(m_FocusTabIndex);
    QueueDraw();
  }

  void TabView::RecvTabMouseUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */, TabElement * /* tab */)
  {

  }

  void TabView::RecvMouseEnter(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }

  void TabView::RecvMouseLeave(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }

  void TabView::RecvTabRightMouseDown(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    RecvTabRightTimerExpired(tabright_callback);
  }

  void TabView::RecvTabLeftMouseDown(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    RecvTabLeftTimerExpired(tableft_callback);
  }

  void TabView::RecvTabButtonMouseUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (m_TabRightTimerHandler.IsValid())
      GetTimer().RemoveTimerHandler(m_TabRightTimerHandler);

    if (m_TabLeftTimerHandler.IsValid())
      GetTimer().RemoveTimerHandler(m_TabLeftTimerHandler);

    m_TabRightTimerHandler = 0;
    m_TabLeftTimerHandler = 0;
  }

  void TabView::RecvTabRightTimerExpired(void * /* v */)
  {
    TranslateTabLayout(-10);
    m_TabRightTimerHandler = GetTimer().AddOneShotTimer(10, tabright_callback, this);
  }

  void TabView::RecvTabLeftTimerExpired(void * /* v */)
  {
    TranslateTabLayout(10);
    m_TabLeftTimerHandler = GetTimer().AddOneShotTimer(10, tableft_callback, this);
  }

  bool TabView::AcceptKeyNavFocus()
  {
    return false;
  }
}
