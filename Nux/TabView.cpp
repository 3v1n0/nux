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

  Color TabView::TAB_HEADER_BACKGROUND_COLOR    = Color (0xFF000000);
  Color TabView::TAB_BACKGROUND_COLOR           = Color (0xFF191919);
  Color TabView::TAB_HEADER_COLOR               = Color (0xFF333333);
  Color TabView::TAB_HEADER_FOCUS_COLOR         = Color (0xFF5D5D5D);

  TabView::TabElement::TabElement (NString TabName, Layout *TabLayout)
  {
    _index = 0;
    _tab_name = TabName;
    _tab_layout = TabLayout;
    _tab_area = new InputArea (NUX_TRACKER_LOCATION);

    _tab_layout->Reference();
    _tab_area->Reference();
  }

  TabView::TabElement::~TabElement()
  {
    _index = 0;
    _tab_layout->UnReference();
    _tab_area->UnReference();
  }

  void TabView::TabElement::SetIndex (int index)
  {
    _index = index;
  }

  int TabView::TabElement::GetIndex() const
  {
    return _index;
  }

  void TabView::TabElement::SetGeometry (const Geometry &geo)
  {
    _tab_area->SetGeometry (geo);
  }

  Geometry TabView::TabElement::GetGeometry() const
  {
    return _tab_area->GetGeometry();
  }

  const NString &TabView::TabElement::GetName() const
  {
    return _tab_name;
  }

  TabView::TabView (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
  {
    m_IncrTab           = NULL;
    m_DecrTab           = NULL;
    m_ClientLayout      = NULL;
    m_TabLayout         = NULL;
    m_TabControlLayout  = NULL;

    m_FocusTabIndex     = 0;
    m_TabPositionOffset = 0;
    m_DrawBackgroundOnPreviousGeometry = false;

    m_TabLayout         = new HLayout (NUX_TRACKER_LOCATION);
    m_TabControlLayout  = new HLayout (NUX_TRACKER_LOCATION);
    m_IncrTab           = new InputArea (NUX_TRACKER_LOCATION);
    m_DecrTab           = new InputArea (NUX_TRACKER_LOCATION);

    m_IncrTab->SetMinimumSize (TAB_BUTTON_WIDTH, TAB_BUTTON_HEIGHT);
    m_DecrTab->SetMinimumSize (TAB_BUTTON_WIDTH, TAB_BUTTON_HEIGHT);
    m_IncrTab->SetMaximumSize (TAB_BUTTON_WIDTH, TAB_BUTTON_HEIGHT);
    m_DecrTab->SetMaximumSize (TAB_BUTTON_WIDTH, TAB_BUTTON_HEIGHT);

    m_IncrTab->OnMouseEnter.connect (sigc::mem_fun (this, &TabView::RecvMouseEnter) );
    m_IncrTab->OnMouseLeave.connect (sigc::mem_fun (this, &TabView::RecvMouseLeave) );
    m_DecrTab->OnMouseEnter.connect (sigc::mem_fun (this, &TabView::RecvMouseEnter) );
    m_DecrTab->OnMouseLeave.connect (sigc::mem_fun (this, &TabView::RecvMouseLeave) );

    m_IncrTab->OnMouseDown.connect (sigc::mem_fun (this, &TabView::RecvTabRightMouseDown) );
    m_DecrTab->OnMouseDown.connect (sigc::mem_fun (this, &TabView::RecvTabLeftMouseDown) );
    m_IncrTab->OnMouseUp.connect (sigc::mem_fun (this, &TabView::RecvTabButtonMouseUp) );
    m_DecrTab->OnMouseUp.connect (sigc::mem_fun (this, &TabView::RecvTabButtonMouseUp) );

    m_TabControlLayout->AddView (m_DecrTab, 1, eCenter);
    m_TabControlLayout->AddView (m_IncrTab, 1, eCenter);


    tabright_callback = new TimerFunctor;
    tabright_callback->OnTimerExpired.connect (sigc::mem_fun (this, &TabView::RecvTabRightTimerExpired) );
    tableft_callback = new TimerFunctor;
    tableft_callback->OnTimerExpired.connect (sigc::mem_fun (this, &TabView::RecvTabLeftTimerExpired) );
  }

  TabView::~TabView()
  {
    m_TabLayout->Dispose();
    m_TabControlLayout->Dispose();
    RemoveCompositionLayout();
    delete (tabright_callback);
    delete (tableft_callback);
    
    std::vector<TabElement *>::iterator it;

    for (it = m_TabVector.begin(); it != m_TabVector.end(); it++)
    {
      delete (*it);
    }

    m_TabVector.clear();
  }

  long TabView::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;

    ret = m_IncrTab->OnEvent (ievent, ret, ProcessEventInfo);
    ret = m_DecrTab->OnEvent (ievent, ret, ProcessEventInfo);

//    if(m_IncrTab->HasMouseFocus())
//        TranslateTabLayout(-10);
//    if(m_DecrTab->HasMouseFocus())
//        TranslateTabLayout(10);

    t_u32 vector_size = (t_u32) m_TabVector.size();

    for (t_u32 i = 0; i < vector_size; i++)
    {
      ret = m_TabVector[i]->_tab_area->OnEvent (ievent, ret, ProcessEventInfo);
    }

    if (m_ClientLayout)
      ret = m_ClientLayout->ProcessEvent (ievent, ret, ProcessEventInfo);

    ret = PostProcessEvent2 (ievent, ret, 0);
    return ret;
  }


  void TabView::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    if (m_DrawBackgroundOnPreviousGeometry)
    {
      GetPainter().PaintBackground (GfxContext, m_PreviousGeometry);
      m_DrawBackgroundOnPreviousGeometry = false;
    }

    GfxContext.PushClippingRectangle (GetGeometry() );
    Geometry base = GetGeometry();

    GetPainter().PushDrawShapeLayer (GfxContext, Geometry (base.x, base.y, base.GetWidth(), TAB_HEIGHT), eSHAPE_CORNER_ROUND4, TAB_HEADER_BACKGROUND_COLOR, eCornerTopLeft | eCornerTopRight);

    if (m_ClientLayout)
      m_ClientLayout->NeedRedraw();

    t_u32 vector_size = (t_u32) m_TabVector.size();

    Geometry geo = GetGeometry();
    Geometry clip_geo;
    clip_geo.SetX (geo.x);
    clip_geo.SetY (geo.y);
    clip_geo.SetWidth (geo.GetWidth() - 2 * TAB_BUTTON_WIDTH);
    clip_geo.SetHeight (m_TabLayout->GetBaseHeight() );

    GfxContext.PushClippingRectangle (clip_geo);

    for (t_u32 i = 0; i < vector_size; i++)
    {
      Geometry tab_geo = m_TabVector[i]->_tab_area->GetGeometry();
      const char *tab_text = m_TabVector[i]->GetName().GetTCharPtr();

      if (m_TabVector[i]->_index == m_FocusTabIndex)
      {
        tab_geo.OffsetSize (-2, 0);
        GetPainter().PaintShapeCorner (GfxContext, tab_geo, TAB_HEADER_FOCUS_COLOR, eSHAPE_CORNER_ROUND4, eCornerTopLeft | eCornerTopRight, false);
        GetPainter().PaintTextLineStatic (GfxContext, GetSysBoldFont(), tab_geo, tab_text, Color (0xFFFFFFFF), true, eAlignTextCenter);
      }
      else
      {
        tab_geo.OffsetSize (-2, 0);
        GetPainter().PaintShapeCorner (GfxContext, tab_geo, TAB_HEADER_COLOR, eSHAPE_CORNER_ROUND4, eCornerTopLeft | eCornerTopRight, false);
        GetPainter().PaintTextLineStatic (GfxContext, GetSysBoldFont(), tab_geo, tab_text, Color (0xFF000000), true, eAlignTextCenter);
      }
    }

    GfxContext.PopClippingRectangle();

    GetPainter().PaintShapeCorner (GfxContext, Geometry (base.x, base.y + TAB_HEIGHT, base.GetWidth(), base.GetHeight() - TAB_HEIGHT),
                               TAB_BACKGROUND_COLOR, eSHAPE_CORNER_ROUND4, eCornerBottomLeft | eCornerBottomRight, false);

    GetPainter().Paint2DQuadColor (GfxContext, m_IncrTab->GetGeometry(), TAB_HEADER_BACKGROUND_COLOR);
    GeometryPositioning gp (eHACenter, eVACenter);
    Geometry GeoPo = ComputeGeometryPositioning (m_IncrTab->GetGeometry(), GetTheme().GetImageGeometry (eTAB_RIGHT), gp);

    if (m_IncrTab->IsMouseInside() )
      GetPainter().PaintShape (GfxContext, GeoPo, Color (0xFFFFFFFF), eTAB_RIGHT);
    else
      GetPainter().PaintShape (GfxContext, GeoPo, TAB_HEADER_FOCUS_COLOR, eTAB_RIGHT);

    GetPainter().Paint2DQuadColor (GfxContext, m_DecrTab->GetGeometry(), TAB_HEADER_BACKGROUND_COLOR);
    gp.SetAlignment (eHACenter, eVACenter);
    GeoPo = ComputeGeometryPositioning (m_DecrTab->GetGeometry(), GetTheme().GetImageGeometry (eTAB_LEFT), gp);

    if (m_DecrTab->IsMouseInside() )
      GetPainter().PaintShape (GfxContext, GeoPo, Color (0xFFFFFFFF), eTAB_LEFT);
    else
      GetPainter().PaintShape (GfxContext, GeoPo, TAB_HEADER_FOCUS_COLOR, eTAB_LEFT);

    GetPainter().PopBackground();
    GfxContext.PopClippingRectangle();
  }

  void TabView::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    GfxContext.PushClippingRectangle (GetGeometry() );
    GetPainter().PushShapeLayer (GfxContext, Geometry (base.x, base.y + TAB_HEIGHT, base.GetWidth(), base.GetHeight() - TAB_HEIGHT),
                             eSHAPE_CORNER_ROUND4, TAB_BACKGROUND_COLOR, eCornerBottomLeft | eCornerBottomRight);

    if (m_ClientLayout)
    {
      GfxContext.PushClippingRectangle (m_ClientLayout->GetGeometry() );
      m_ClientLayout->ProcessDraw (GfxContext, force_draw);
      GfxContext.PopClippingRectangle();
    }

    GetPainter().PopBackground();
    GfxContext.PopClippingRectangle();
  }

  void TabView::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void TabView::PreLayoutManagement()
  {
    // Give the managed layout appropriate size and position..
    if (m_CompositionLayout)
    {
      Geometry layout_geo = GetGeometry();
      layout_geo.OffsetPosition (TAB_X_BORDER, TAB_HEIGHT);
      layout_geo.OffsetSize (-2 * TAB_X_BORDER, - (TAB_HEIGHT) - TAB_Y_BORDER);
      m_CompositionLayout->SetGeometry (layout_geo);
    }
  }

  long TabView::PostLayoutManagement (long LayoutResult)
  {
    // Set the geometry of the control to be the same as the managed layout.
    // Only the size is changed. The position of the composition layout hasn't
    // been changed by ComputeLayout2.
    if (m_CompositionLayout)
    {
      Geometry base = m_CompositionLayout->GetGeometry();
      base.OffsetPosition (-TAB_X_BORDER, -TAB_HEIGHT);
      base.OffsetSize (2 * TAB_X_BORDER, TAB_HEIGHT + TAB_Y_BORDER);
      Area::SetGeometry (base);
    }

    Geometry base = GetGeometry();

    int tab_x = m_CompositionLayout->GetGeometry().x + m_CompositionLayout->GetGeometry().GetWidth() - 2 * TAB_BUTTON_WIDTH;
    int tab_y = base.y;

    m_DecrTab->SetBaseXY (tab_x, tab_y);
    m_DecrTab->SetBaseSize (TAB_BUTTON_WIDTH, TAB_BUTTON_HEIGHT);
    m_IncrTab->SetBaseXY (tab_x + TAB_BUTTON_WIDTH, tab_y);
    m_IncrTab->SetBaseSize (TAB_BUTTON_WIDTH, TAB_BUTTON_HEIGHT);

    m_TabLayout->SetBaseY (tab_y);
    m_TabLayout->SetBaseHeight (TAB_HEIGHT);
    TranslateTabLayout (0);

    m_TabControlLayout->SetBaseXY (tab_x, tab_y);
    m_TabControlLayout->SetBaseSize (2 * TAB_BUTTON_WIDTH, TAB_HEIGHT);
    GetWindowThread ()->ComputeElementLayout (m_TabControlLayout);

    if (m_ClientLayout)
    {
//        m_ClientLayout->SetGeometry(m_CompositionLayout->GetGeometry());
//        GetWindowThread ()->ComputeElementLayout(m_ClientLayout);
      m_ClientLayout->NeedRedraw();
    }

    return LayoutResult;
  }

  void TabView::PositionChildLayout (float offsetX, float offsetY)
  {
    if (m_CompositionLayout)
    {
      m_CompositionLayout->SetBaseX (GetBaseX() + TAB_X_BORDER);
      m_CompositionLayout->SetBaseY (GetBaseY() + TAB_HEIGHT);
      m_CompositionLayout->ComputePosition2 (offsetX, offsetY);
    }
  }


  void TabView::AddTab (const char *tab_name, Layout *tab_layout)
  {
    if (tab_layout == 0)
      return;

    TabElement *Tab = new TabElement (tab_name, tab_layout);
    Tab->SetIndex (m_TabVector.size() );

    if (Tab->GetIndex() == 0)
    {
      m_FocusTabIndex = 0;
      m_ClientLayout = Tab->_tab_layout;
      //m_ClientLayout->SetGeometry(m_CompositionLayout->GetGeometry());
      SetCompositionLayout (m_ClientLayout);
      GetWindowThread ()->ComputeElementLayout (this);
    }

    Tab->_tab_area->SetMinimumSize (6 + GetSysBoldFont()->GetStringWidth (tab_name), PRACTICAL_WIDGET_HEIGHT);
    Tab->_tab_area->SetMaximumSize (6 + GetSysBoldFont()->GetStringWidth (tab_name), PRACTICAL_WIDGET_HEIGHT);

    Tab->_tab_area->OnMouseDown.connect (sigc::bind ( sigc::mem_fun (this, &TabView::RecvTabMouseDown), Tab) );
    Tab->_tab_area->OnMouseUp.connect (sigc::bind ( sigc::mem_fun (this, &TabView::RecvTabMouseUp), Tab) );

    m_TabLayout->AddView (Tab->_tab_area, 1, eCenter);
    GetWindowThread ()->ComputeElementLayout (m_TabLayout);

    m_TabVector.push_back (Tab);
  }

  void TabView::SetActiveTad (int index)
  {
    if (index >= (t_s32) m_TabVector.size() )
    {
      m_FocusTabIndex = (t_s32) m_TabVector.size() - 1;
    }
    else
    {
      m_FocusTabIndex = index;
    }

    m_ClientLayout = m_TabVector[m_FocusTabIndex]->_tab_layout;

    if (m_ClientLayout)
      SetCompositionLayout (m_ClientLayout);

    GetWindowThread ()->ComputeElementLayout (this);

    sigTabChanged (this);
    sigTabIndexChanged (m_FocusTabIndex);

    NeedRedraw();
  }

  void TabView::TranslateLeft (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    TranslateTabLayout (-5);
  }

  void TabView::TranslateRight (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    TranslateTabLayout (5);
  }

  void TabView::TranslateTabLayout (int offset)
  {
//    if((m_TabPositionOffset == 0) && (offset > 0))
//        return;

    m_TabPositionOffset += offset;
    int lx = m_CompositionLayout->GetBaseX() + m_TabPositionOffset;

    if (lx > m_CompositionLayout->GetBaseX() )
    {
      // end of scroll left;
      m_TabPositionOffset = 0;
      lx = m_CompositionLayout->GetBaseX() + m_TabPositionOffset;
    }

    if (lx + m_TabLayout->GetBaseWidth() < m_CompositionLayout->GetBaseX() +
        m_CompositionLayout->GetBaseWidth() - 2 * TAB_BUTTON_WIDTH)
    {

      lx = (m_CompositionLayout->GetBaseX() +
            m_CompositionLayout->GetBaseWidth() - 2 * TAB_BUTTON_WIDTH) - m_TabLayout->GetBaseWidth();


      if (lx > m_CompositionLayout->GetBaseX() )
      {
        m_TabPositionOffset = 0;
        lx = m_CompositionLayout->GetBaseX() + m_TabPositionOffset;
      }
      else
        m_TabPositionOffset = - (m_CompositionLayout->GetBaseX() - lx);
    }

    m_TabLayout->SetBaseX (lx);
    GetWindowThread ()->ComputeElementLayout (m_TabLayout);
    NeedRedraw();

  }

  void TabView::RecvTabMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags, TabElement *tab)
  {
    m_FocusTabIndex = tab->_index;
    m_ClientLayout = tab->_tab_layout;

    if (m_ClientLayout)
    {
      SetCompositionLayout (m_ClientLayout);
    }

    //m_ClientLayout->SetGeometry(m_CompositionLayout->GetGeometry());

    m_PreviousGeometry = GetGeometry();

    int PrevWidth = GetBaseWidth();
    int PrevHeight = GetBaseHeight();

    GetWindowThread ()->ComputeElementLayout (this, true);


    int NewWidth = GetBaseWidth();
    int NewHeight = GetBaseHeight();

    if ( (NewWidth != PrevWidth) ||
         (NewHeight != PrevHeight) )
    {
      // We need to draw the background on the previous size of the Table if its
      // size is set to match the content(IsSizeMatchContent) and an item is close.
      //m_PreviousGeometry;
      m_DrawBackgroundOnPreviousGeometry = true;
    }

    m_DrawBackgroundOnPreviousGeometry = true;

    sigTabChanged (this);
    sigTabIndexChanged (m_FocusTabIndex);
    NeedRedraw();
  }

  void TabView::RecvTabMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags, TabElement *tab)
  {

  }

  void TabView::RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void TabView::RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void TabView::RecvTabRightMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    RecvTabRightTimerExpired (tabright_callback);
  }

  void TabView::RecvTabLeftMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    RecvTabLeftTimerExpired (tableft_callback);
  }

  void TabView::RecvTabButtonMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_TabRightTimerHandler.IsValid() )
      GetTimer().RemoveTimerHandler (m_TabRightTimerHandler);

    if (m_TabLeftTimerHandler.IsValid() )
      GetTimer().RemoveTimerHandler (m_TabLeftTimerHandler);

    m_TabRightTimerHandler = 0;
    m_TabLeftTimerHandler = 0;
  }

  void TabView::RecvTabRightTimerExpired (void *v)
  {
    TranslateTabLayout (-10);
    m_TabRightTimerHandler = GetTimer().AddTimerHandler (10, tabright_callback, this);
  }

  void TabView::RecvTabLeftTimerExpired (void *v)
  {
    TranslateTabLayout (10);
    m_TabLeftTimerHandler = GetTimer().AddTimerHandler (10, tableft_callback, this);
  }

}
