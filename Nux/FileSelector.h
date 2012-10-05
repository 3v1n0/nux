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


#ifndef FILE_SELECTOR_H
#define FILE_SELECTOR_H

namespace nux
{

  class Button;
  class EditTextBox;

  class FileSelector: public View
  {
  public:
    FileSelector(NUX_FILE_LINE_PROTO);
    ~FileSelector();

    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);

    //void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    //void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    //void RecvMouseMove(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvOpenButtonClick(int x, int y, unsigned long button_flags, unsigned long key_flags);
    sigc::signal<void> sigClick;

  private:
    EditTextBox    *m_FileEditTextBox;
    InputArea       *m_OpenButton;
    HLayout        *m_hlayout;
    BaseTexture       *m_Texture;
  };

}

#endif // FILE_SELECTOR_H
