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


#ifndef FILE_SELECTOR_H
#define FILE_SELECTOR_H

namespace nux
{

  class Button;
  class EditTextBox;

  class FileSelector: public View
  {
  public:
    FileSelector (NUX_FILE_LINE_PROTO);
    ~FileSelector();

    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);

    virtual void Draw (GraphicsContext &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsContext &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsContext &GfxContext, bool force_draw);

    //void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    //void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    //void RecvMouseMove(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvOpenButtonClick (int x, int y, unsigned long button_flags, unsigned long key_flags);
    sigc::signal<void> sigClick;

  private:
    EditTextBox    *m_FileEditTextBox;
    CoreArea       *m_OpenButton;
    HLayout        *m_hlayout;
    NTexture2D      m_Texture;
  };

}

#endif // FILE_SELECTOR_H
