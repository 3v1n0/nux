/*
* Copyright 2010-2012 Inalogic® Inc.
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
* Authored by: Brandon Schaefer <brandon.schaefer@canonical.com>
*
*/


#include "XICClient.h"
#include "NuxCore/NuxCore.h"

XICClient::XICClient(Window window)
  : window_(window),
    xic_(NULL),
    focused_(false)
{
}

XICClient::~XICClient()
{
  if (xic_)
    XDestroyIC(xic_);
}

void XICClient::SetupXIC(XIM xim, XIMStyle style)
{
  xic_ = XCreateIC(xim, XNInputStyle, style, XNClientWindow, window_, NULL);
  if (!xic_)
    nuxDebugMsg("[GraphicsDisplay::XIMStartCallback] failed to register xic");
}

XIC& XICClient::GetXIC()
{
  return xic_;
}

void XICClient::ResetXIC(XIM xim, XIMStyle style)
{
  if (xim)
  {
    if (xic_)
      XDestroyIC(xic_);
    SetupXIC(xim, style);
  }
}

void XICClient::FocusInXIC()
{
  if (xic_ && !focused_)
  {
    XSetICFocus(xic_);
    focused_ = true;
  }
}

void XICClient::FocusOutXIC()
{
  if (xic_ && focused_)
  {
    XUnsetICFocus(xic_);
    focused_ = false;
  }
}

bool XICClient::IsFocused() const
{
  return focused_;
}

void XICClient::Destroy()
{
  if (xic_)
  {
    XDestroyIC(xic_);
    xic_ = NULL;
  }
}
