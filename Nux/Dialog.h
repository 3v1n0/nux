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


#ifndef DIALOG_H
#define DIALOG_H

#include "FloatingWindow.h"

namespace nux
{

  class Button;

  class Dialog: public FloatingWindow
  {
  public:
    Dialog (const TCHAR *WindowName = TEXT ("") );
    virtual ~Dialog();

  protected:
    virtual void RecvOk() = 0;
    virtual void RecvCancel() = 0;
    virtual void Start (bool StartModal);

    Button     *m_OkButton;
    Button     *m_CancelButton;
    HLayout    *m_ButtonLayout;
  };


}

#endif // DIALOG_H
