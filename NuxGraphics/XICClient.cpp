/*
* Copyright 2012 Inalogic® Inc.
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

XICClient::XICClient()
  : xic_(NULL),
    xim_style_(0),
    focused_(false)
{
}

XICClient::~XICClient()
{
}

void XICClient::ResetXIC(XIM xim, Window window)
{
  if (!xim_style_)
    SetupXIMStyle(xim);
  SetupXIC(xim, window);
}

void XICClient::SetupXIC(XIM xim, Window window)
{
  xic_ = XCreateIC(xim, XNInputStyle, xim_style_, XNClientWindow, window, XNFocusWindow, window, NULL);
}

void XICClient::SetupXIMStyle(XIM xim)
{
  int i;
  XIMStyles *xim_styles = NULL;
  XIMStyle root_style = (XIMPreeditNothing|XIMStatusNothing);

  XGetIMValues(xim, XNQueryInputStyle, &xim_styles, NULL);

  for (i = 0; i < xim_styles->count_styles; ++i)
    if (xim_styles->supported_styles[i] == root_style)
      break;

  if (i >= xim_styles->count_styles)
    xim_style_ = 0;
  xim_style_ = root_style;
}

bool XICClient::HasXIC() const
{
  if (xic_ == NULL)
    return false;
  return true;
}

XIC XICClient::GetXIC() const
{
  return xic_;
}

void XICClient::Reinitialize()
{
  xic_ = NULL;
  xim_style_ = 0;
  focused_ = false;
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

void XICClient::DestroyXIC()
{
  if (xic_)
  {
    XDestroyIC(xic_);
    xic_ = NULL;
  }
}
