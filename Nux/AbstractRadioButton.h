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


#ifndef ABSTRACTRADIOBUTTON_H
#define ABSTRACTRADIOBUTTON_H

namespace nux
{
  //! The base class of Button, CheckBox, RadioButton and PushButton.
  class AbstractRadioButton : public View
  {
    NUX_DECLARE_OBJECT_TYPE (AbstractRadioButton, View);
  public:
    AbstractRadioButton (const TCHAR *Caption = TEXT (""), NUX_FILE_LINE_PROTO);
    ~AbstractRadioButton();

    virtual void SetCaption (const TCHAR *Caption) = 0;
    virtual const NString &GetCaption() const = 0;

    virtual void SetState (bool b) = 0;
    virtual void SetState (bool State, bool EmitSignal) = 0;
    virtual bool GetState() const = 0;

  protected:
    bool _state;
  };
}

#endif // ABSTRACTRADIOBUTTON_H
