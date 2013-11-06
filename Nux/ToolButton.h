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


#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include "ActionItem.h"

namespace nux
{

  class HLayout;
  class ActionItem;

  class ToolButton : public View
  {
  public:
    ToolButton(const char *BitmapFilename = 0, NUX_FILE_LINE_PROTO);

    ~ToolButton();

    void SetAction(ActionItem *action);
    void SetState(bool b);
    void SetBitmap(const BaseTexture* Texture);
    // emitters
    void EmitClick(int x, int y, unsigned long button_flags, unsigned long key_flags);

    void RecvMouseDoubleClick (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags);
    // signals
    sigc::signal<void> sigClick;

  protected:
    virtual void Draw(GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsEngine &GfxContext, bool force_draw);

  private:
    ActionItem *m_ActionItem;
    BaseTexture   *m_Texture;

  };

}

#endif // TOOLBUTTON_H
