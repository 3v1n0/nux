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


#ifndef TEXTUREAREA_H
#define TEXTUREAREA_H

#include "NuxGraphics/GraphicsEngine.h"

namespace nux
{

  class TextureArea : public View
  {
    NUX_DECLARE_OBJECT_TYPE (TextureArea, View);
  public:
    TextureArea (NUX_FILE_LINE_PROTO);
    virtual ~TextureArea();

    /*!
        Render this area with a Texture. The reference count of the device texture which is cached by \a texture is increased by 1.
        It \a layer was allocated on the heap, it must be deleted later.
        @param layer A pointer to a BaseTexture class.
    */
    void SetTexture (BaseTexture *texture);

    /*!
        Set the paint layer of this area. The \a layer argument to this function is cloned by this object.
        It \a layer was allocated on the heap, it must be deleted later.
        @param layer A pointer to a concrete class that inherit from AbstractPaintLayer.
    */
    void SetPaintLayer (AbstractPaintLayer *layer);

    //! Convenience function to set a 2D rotation when rendering the area.
    /*!
        The rotation is only used for rendering. It should not be used for something else.
    */
    void Set2DRotation (float angle);

    Matrix4 Get2DRotation () const;

    sigc::signal<void, int, int> sigMouseDown;  //!< Signal emmitted when a mouse button is pressed over this area.
    sigc::signal<void, int, int> sigMouseDrag;  //!< Signal emmitted when the mouse is dragged over this area.

  protected:
    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);
    void RecvMouseDown (int x, int y, long button_flags, long key_flags);
    void RecvMouseUp (int x, int y, long button_flags, long key_flags);
    void RecvMouseEnter (int x, int y, long button_flags, long key_flags);
    void RecvMouseLeave (int x, int y, long button_flags, long key_flags);
    void RecvMouseClick (int x, int y, long button_flags, long key_flags);
    void RecvMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

  private:
    AbstractPaintLayer *m_PaintLayer;

    Matrix4 _2d_rotate;  //!< 2D rotation matrix for this area. Used for rendering only.
  };
}

#endif // TEXTUREAREA_H
