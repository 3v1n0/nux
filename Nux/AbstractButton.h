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


#ifndef ABSTRACTBUTTON_H
#define ABSTRACTBUTTON_H

namespace nux { //NUX_NAMESPACE_BEGIN

class AbstractButton : public ActiveInterfaceObject
{
public:
    AbstractButton(const TCHAR* Caption = TEXT(""), NUX_FILE_LINE_PROTO);
    ~AbstractButton();

    virtual bool IsCheckable() const = 0;

    virtual void SetCaption(const TCHAR* Caption) = 0;
    virtual const NString& GetCaption() const = 0;
    
    virtual void SetState(bool b) = 0;
    virtual void SetState(bool State, bool EmitSignal) = 0;
    virtual bool GetState() const = 0;

protected:
    bool m_State;

};

} //NUX_NAMESPACE_END

#endif // ABSTRACTBUTTON_H
