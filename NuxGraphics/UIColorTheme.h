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


#ifndef UICOLORTHEME_H
#define UICOLORTHEME_H

#include "NuxCore/Color.h"

namespace nux { //NUX_NAMESPACE_BEGIN

class UIColorThemeImpl
{

public:
    void Initialize();

    Color mUI_BackgroundColor0;
    Color mUI_BackgroundColor1;

    Color mUI_ForegroundColor0;
    Color mUI_ForegroundColor1;
    
    Color mUI_TextColor;
    Color mUI_DisabledText_Color;
    Color mUI_EditableText_Color;

    Color mUI_EditableText_BackgroundColor;
    Color mUI_DisableEditableText_BackgroundColor;


    static UIColorThemeImpl& Instance();
private:
    static UIColorThemeImpl* m_pInstance;

    UIColorThemeImpl();
    UIColorThemeImpl(const UIColorThemeImpl&);
    // Does not make sense for a singleton. This is a self assignment.
    UIColorThemeImpl& operator=(const UIColorThemeImpl&);
    // Declare operator adress-of as private 
    UIColorThemeImpl* operator &();
    ~UIColorThemeImpl(){};
};

#define gUIColorTheme UIColorThemeImpl::Instance()

} //NUX_NAMESPACE_END

#endif // UICOLORTHEME_H
