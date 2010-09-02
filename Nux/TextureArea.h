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


#ifndef TEXTUREAREA_H
#define TEXTUREAREA_H

#include "NuxGraphics/OpenGLEngine.h"

NAMESPACE_BEGIN_GUI

class TextureArea : public ActiveInterfaceObject
{
public:
    TextureArea();
    ~TextureArea();
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    void SetTexture(NTexture* texture);
    void SetPaintLayer(AbstractPaintLayer* layer);
    //void SetTexture(const TCHAR* TextureFilename);
    void RecvMouseDown(int x, int y, long button_flags, long key_flags);
    void RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    sigc::signal<void, int, int> sigMouseDown;
    sigc::signal<void, int, int> sigMouseDrag;

private:
    AbstractPaintLayer* m_PaintLayer;
};
NAMESPACE_END_GUI

#endif // TEXTUREAREA_H
