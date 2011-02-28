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


#ifndef ABSTRACTCOMBOBOX_H
#define ABSTRACTCOMBOBOX_H

namespace nux
{
  class StaticText;
  class HLayout;

  class AbstractComboBox: public View
  {
  public:
    AbstractComboBox (NUX_FILE_LINE_PROTO);
    virtual ~AbstractComboBox();

    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo) = 0;
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);

    void RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags);

  protected:
    HLayout    *m_hlayout;
    InputArea   *_combo_box_area;
    InputArea   *_combo_box_opening_area;
    StaticText  *_pango_static_text;
    bool        m_MenuIsActive;
    bool        m_IsOpeningMenu;

    static Color m_sCOMBO_COLOR;
    static Color m_sCOMBO_BUTTON_COLOR;
    static Color m_sCOMBO_MOUSEOVER_COLOR;
  };


}

#endif // ABSTRACTCOMBOBOX_H
