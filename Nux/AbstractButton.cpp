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


#include "Nux.h"

#include "AbstractButton.h"
#include "HLayout.h"

namespace nux { //NUX_NAMESPACE_BEGIN

AbstractButton::AbstractButton(const TCHAR* Caption, NUX_FILE_LINE_DECL)
:   View(NUX_FILE_LINE_PARAM)
,   m_State(false)
{

}

AbstractButton::~AbstractButton()
{
    
}


} //NUX_NAMESPACE_END
