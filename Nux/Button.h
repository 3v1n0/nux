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


#ifndef BUTTON_H
#define BUTTON_H
#include "AbstractButton.h"

namespace nux
{

  class HLayout;
  class VLayout;
  class TextureArea;

  enum Position {
    NUX_POSITION_LEFT,
    NUX_POSITION_RIGHT,
    NUX_POSITION_TOP,
    NUX_POSITION_BOTTOM
  };

  class Button: public AbstractButton
  {
    NUX_DECLARE_OBJECT_TYPE (Button, View);
  public:
    Button (TextureArea *image, NUX_FILE_LINE_PROTO);
    Button (const std::string label, NUX_FILE_LINE_PROTO);
    Button (const std::string label, TextureArea *image, NUX_FILE_LINE_PROTO);
    Button (NUX_FILE_LINE_PROTO);
    ~Button();

    // This api sucks, it deviates from the property system api, but properties
    // do not support nux objects
    void SetImage (TextureArea *image);
    TextureArea *GetImage ();

    Property<std::string>   label;
    Property<Position>      image_position;

  protected:
    void Init ();

    void OnStateChanged (int value);
    void OnLabelChanged (std::string value);
    void OnImageChanged (TextureArea *value);
    void OnImagePositionChanged (int value);
    void RebuildLayout ();

    TextureArea *image;

    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    //virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);

  };
}

#endif // BUTTON_H
