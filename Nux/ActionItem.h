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


#ifndef ACTIONITEM_H
#define ACTIONITEM_H

#include "NuxGraphics/OpenGLEngine.h"

NAMESPACE_BEGIN_GUI

class MenuBar;
class MenuPage;
class NTexture2D;

class ActionItem
{
    DECLARE_ROOT_OBJECT_TYPE(ActionItem);
public:
    ActionItem(const TCHAR* label = 0, int UserValue = 0);
    ~ActionItem();

    void Activate(bool b);
    void Trigger() const;

    void DrawAsMenuItem(GraphicsContext& GfxContext, CoreArea& area, bool is_highlighted, bool draw_icone);
    void DrawAsToolButton(GraphicsContext& GfxContext, CoreArea& area);
    void Enable(bool b);
    bool isEnabled() const;

    void SetLabel(const TCHAR* label);
    const TCHAR* GetLabel() const;
    int GetUserValue() const {return m_UserValue;}

//    void SetMenu(MenuPage* menu);
//    MenuPage* GetMenu() const;

    void SetFont(const FontTexture& font);
    void SetIcon(const NTexture2D& icon);
    NTexture2D& GetIcon();
    
    sigc::signal<void> sigAction;

private:
    //! A value defined by the user at construction time.
    int m_UserValue;
    NTexture2D m_Icon;
    NString m_Label;
    bool m_IsActivated;
    //CoreArea m_Area;
    MenuPage* m_Menu;
    bool m_Enable;

    friend class MenuBar;
    friend class MenuPage;
};



NAMESPACE_END_GUI

#endif // ACTIONITEM_H
