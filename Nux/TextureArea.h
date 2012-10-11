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

#ifndef TEXTUREAREA_H
#define TEXTUREAREA_H

#include "NuxGraphics/GraphicsEngine.h"

namespace nux
{
  //! Represent a rectangular area painted with a color or a texture layer.
  /*!
      By default TextureArea contains a ColorLayer with the color set to Black.
  */
  class TextureArea: public View
  {
    NUX_DECLARE_OBJECT_TYPE(TextureArea, View);
  public:
    TextureArea(NUX_FILE_LINE_PROTO);
    virtual ~TextureArea();

    /*!
        Use the provided BaseTexture to create a TextureLayer.
        The current layer is destroyed. If the \a texture argument is invalid the function returns without changing this object.

        @param texture Pointer to a BaseTexture class.
    */
    void SetTexture(BaseTexture *texture);

    /*!
        Use the provided \a color argument to create a ColorLayer.
        The current layer is destroyed.

        @param texture Pointer to a BaseTexture class.
    */
    void SetColor(const Color &color);

    /*!
        Set the paint layer of this area. The layer argument to this function is cloned.
        \sa AbstractPaintLayer, ColorLayer, ShapeLayer, SliceScaledTextureLayer, TextureLayer;

        @param layer A pointer to a concrete class that inherit from AbstractPaintLayer.
    */
    void SetPaintLayer(AbstractPaintLayer *layer);

    void LoadImageFile(const std::string &filename);

    /*!
        Get a copy of the paint layer of this area. The layer must be destroyed with delete when it is no longer needed.
        \sa AbstractPaintLayer, ColorLayer, ShapeLayer, SliceScaledTextureLayer, TextureLayer;
        
        @return A copy of the layer inside this object.
    */
    AbstractPaintLayer* GetPaintLayer() const;

    //! Convenience function to set a 2D rotation when rendering the area.
    /*!
        The rotation is only used for rendering. It should not be used for something else.
    */
    void Set2DRotation(float angle);

    Matrix4 Get2DRotation() const;

    sigc::signal<void, int, int> sigMouseDown;  //!< Signal emmitted when a mouse button is pressed over this area.
    sigc::signal<void, int, int> sigMouseDrag;  //!< Signal emmitted when the mouse is dragged over this area.

  protected:
    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);

    void RecvMouseDown(int x, int y, long button_flags, long key_flags);
    void RecvMouseUp(int x, int y, long button_flags, long key_flags);
    void RecvMouseEnter(int x, int y, long button_flags, long key_flags);
    void RecvMouseLeave(int x, int y, long button_flags, long key_flags);
    void RecvMouseClick(int x, int y, long button_flags, long key_flags);
    void RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

  private:
    AbstractPaintLayer *paint_layer_;

    Matrix4 rotation_2d_;  //!< 2D rotation matrix for this area. Used for rendering only.
  };

  typedef TextureArea Image;
}

#endif // TEXTUREAREA_H
