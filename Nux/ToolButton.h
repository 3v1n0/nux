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


#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include "ActionItem.h"

NAMESPACE_BEGIN_GUI

class HLayout;
class ActionItem;
class NTexture2D;

class ToolButton : public ActiveInterfaceObject
{
public:
    ToolButton(const TCHAR* BitmapFilename = 0);

    ~ToolButton();
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);

    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    void SetAction(const ActionItem& action);
    void SetState(bool b);
    void SetBitmap(const NTexture2D& Bitmap);
    // emitters
    void EmitClick(int x, int y, unsigned long button_flags, unsigned long key_flags);
    
    void RecvMouseDoubleClick(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags);
    // signals
    sigc::signal<void> sigClick;

private:
    smptr(HLayout) hlayout;
    smptr(CoreArea) m_IconArea;
    ActionItem m_ActionItem;
    NTexture2D m_Bitmap;
    
};

NAMESPACE_END_GUI

#endif // TOOLBUTTON_H
