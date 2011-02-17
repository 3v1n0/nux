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


#ifndef HTOOLBAR_H
#define HTOOLBAR_H
namespace nux
{
  class ActionItem;
  class ToolButton;
  class EditTextBox;
  class StaticTextBox;
  class HLayout;

  class HToolBar: public View
  {
  public:
    HToolBar (NUX_FILE_LINE_PROTO);
    ~HToolBar();

    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);

    void AddToolButton (ToolButton *);
    void AddAction (ActionItem *action);
    void AddSpace (int size);
    void AddSeparator();
    void AddEditTextLine (EditTextBox *edittextline);
    void AddStaticTextLine (StaticTextBox *statictextline);

    void ClearWidget();
    /////////////////
    //  EMITERS    //
    /////////////////

    void RecvMouseDownOnIcon (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseUpOnIcon (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseMoveOnIcon (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseEnterIcon (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeaveIcon (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDragOnIcon (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

  private:
    HLayout *m_hlayout;
  };

}
#endif // HTOOLBAR_H
