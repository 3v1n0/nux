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
#include "VToolBar.h"
#include "VLayout.h"
#include "ToolButton.h"
#include "ActionItem.h"
#include "AbstractSeparator.h"
#include "HSeparator.h"

namespace nux
{

  VToolBar::VToolBar (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_vlayout (0)
  {
    InitializeLayout();
    InitializeWidgets();
  }

  VToolBar::~VToolBar()
  {
    DestroyLayout();
  }

  void VToolBar::InitializeWidgets()
  {
    m_vlayout->SetVerticalInternalMargin (2);
    m_vlayout->SetVerticalExternalMargin (0);
    m_vlayout->SetHorizontalExternalMargin (0);
    m_vlayout->SetContentDistribution (eStackTop);
    SetMinimumSize (32, 32);
    SetMaximumWidth (32);
    SetGeometry (Geometry (0, 0, 20, 200) );
    SetCompositionLayout (m_vlayout);
  }

  void VToolBar::InitializeLayout()
  {
    m_vlayout = new VLayout (NUX_TRACKER_LOCATION);
  }

  void VToolBar::DestroyLayout()
  {
  }

  long VToolBar::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    long ProcEvInfo = 0;

    if (ievent.e_event == NUX_MOUSE_PRESSED)
    {
      if (!GetGeometry().IsPointInside (ievent.e_x, ievent.e_y) )
      {
        ProcEvInfo = eDoNotProcess;
        return TraverseInfo;
      }
    }

    ret = m_vlayout->ProcessEvent (ievent, TraverseInfo, ProcessEventInfo);

    // PostProcessEvent2 must always have its last parameter set to 0
    // because the m_BackgroundArea is the real physical limit of the window.
    // So the previous test about IsPointInside do not prevail over m_BackgroundArea
    // testing the event by itself.
    ret = PostProcessEvent2 (ievent, ret, 0);
    return ret;
  }

  void VToolBar::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();
    GfxContext.PushClippingRectangle (base);
    GfxContext.QRP_Color (base.x, base.y, base.GetWidth(), base.GetHeight(), COLOR_BACKGROUND_PRIMARY);

    GfxContext.PopClippingRectangle();
  }

  void VToolBar::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();
    GfxContext.PushClippingRectangle (base);
    GetPainter().PushColorLayer (GfxContext, base, COLOR_BACKGROUND_PRIMARY);
    m_vlayout->ProcessDraw (GfxContext, force_draw);
    GetPainter().PopBackground();
    GfxContext.PopClippingRectangle();
  }

  void VToolBar::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void VToolBar::AddToolButton (ToolButton *toolbutton)
  {
    nuxAssert (toolbutton);

    if (toolbutton == 0)
      return;

    m_vlayout->AddView (toolbutton, 0, eCenter, eFix);
    // 0: the WidgetLayout geometry will be set to SetGeometry(0,0,1,1);
    // and the children will take their natural size by expending WidgetLayout.
    // If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.
    ComputeChildLayout();
  }

  void VToolBar::AddAction (ActionItem *action)
  {
//    nuxAssert(action);
//    if(action == 0)
//        return;

    ToolButton *tool_button (new ToolButton);
    tool_button->SetAction (action);

    m_vlayout->AddView (tool_button, 0, eCenter, eFix);
    // 0: the WidgetLayout geometry will be set to SetGeometry(0,0,1,1);
    // and the children will take their natural size by expending WidgetLayout.
    // If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.
    ComputeChildLayout();
  }

  void VToolBar::AddSpace (int size)
  {
    SpaceLayout *layout (new SpaceLayout() );
    layout->SetMaximumHeight (size);
    layout->SetMinimumHeight (size);
    m_vlayout->AddLayout (layout);
    ComputeChildLayout();
  }

  void VToolBar::AddSeparator()
  {
    HSeparator *separator (new HSeparator() );
    m_vlayout->AddView (separator, 0);
    ComputeChildLayout();
  }

  void VToolBar::ClearWidget()
  {
    m_CompositionLayout->Clear();
  }

  void VToolBar::RecvMouseDownOnIcon (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void VToolBar::RecvMouseUpOnIcon (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void VToolBar::RecvMouseMoveOnIcon (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void VToolBar::RecvMouseDragOnIcon (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void VToolBar::RecvMouseEnterIcon (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void VToolBar::RecvMouseLeaveIcon (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }
}
