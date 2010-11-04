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

#include "NuxGraphics/GraphicsEngine.h"

namespace nux
{

  class MenuBar;
  class MenuPage;

  class ActionItem: public Object
  {
  public:
    NUX_DECLARE_OBJECT_TYPE (ActionItem, Object);

    ActionItem (const TCHAR *label = 0, int UserValue = 0, NUX_FILE_LINE_PROTO);
    ~ActionItem();

    void Activate (bool b);
    void Trigger() const;

    void DrawAsMenuItem (GraphicsEngine &GfxContext, CoreArea &area, bool is_highlighted, bool draw_icone);
    void DrawAsToolButton (GraphicsEngine &GfxContext, CoreArea &area);
    void Enable (bool b);
    bool isEnabled() const;

    void SetLabel (const TCHAR *label);
    const TCHAR *GetLabel() const;
    int GetUserValue() const
    {
      return m_UserValue;
    }

//    void SetMenu(MenuPage* menu);
//    MenuPage* GetMenu() const;

    void SetFont (IntrusiveSP<FontTexture> font);
    void SetIcon (const BaseTexture* icon);
    const BaseTexture* GetIcon();

    sigc::signal<void> sigAction;

  private:
    //! A value defined by the user at construction time.
    int m_UserValue;
    BaseTexture* m_Icon;
    NString m_Label;
    bool m_IsActivated;
    MenuPage *m_Menu;
    bool m_Enable;

    friend class MenuBar;
    friend class MenuPage;
  };



}

#endif // ACTIONITEM_H
