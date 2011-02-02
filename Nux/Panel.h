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


#ifndef PANEL_H
#define PANEL_H

#include "ScrollView.h"

namespace nux
{

  class Layout;

  class Panel: public ScrollView
  {
  public:
    Panel (NUX_FILE_LINE_PROTO);
    ~Panel();

    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);

    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);

    //void AddWidget(View* ic);
    void AddWidget (View *ic, int stretchfactor = 1);
    void AddWidget (std::list<View *> *ViewList);
    virtual bool SetLayout (Layout *layout);
    void clearContent();

    // Get a change to do any work on an element.
    // Here we need to position the header by hand because it is not under the control of vlayout.
    virtual void PreLayoutManagement();
    // Get a change to do any work on an element.
    // Here we need to position the header by hand because it is not under the control of vlayout.
    virtual long PostLayoutManagement (long LayoutResult);
    // Get a change to do any work on an element.
    // Here we need to position the header by hand because it is not under the control of vlayout.
    virtual void PositionChildLayout (float offsetX, float offsetY);

  protected:
    virtual void ScrollLeft (float stepx, int mousedx);
    virtual void ScrollRight (float stepx, int mousedx);
    virtual void ScrollUp (float stepy, int mousedy);
    virtual void ScrollDown (float stepy, int mousedy);

  private:
    Layout *m_layout;
  };

}
#endif // PANEL_H
