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

#include <stdlib.h>
#include <string.h>

#include "XIMClient.h"
#include "NuxCore/NuxCore.h"

#include <X11/extensions/shape.h>
#include <X11/Xlibint.h>

XIMClient::XIMClient(Display* display, Window window)
  : display_(display),
    window_(window),
    xim_(NULL),
    xic_(NULL),
    default_style_((XIMPreeditNothing|XIMStatusNothing)),
    focused_(false)
{
  SetupCallback();
}

XIMClient::~XIMClient()
{
  if (xic_)
    XDestroyIC(xic_);

  if (xim_)
    XCloseIM(xim_);
}

void XIMClient::SetupCallback()
{
  const char *xmodifier;

  /* don't do anything if we are using ibus */
  xmodifier = getenv("XMODIFIERS");
  if (xmodifier && strstr(xmodifier,"ibus") != NULL)
  {
    nuxDebugMsg("[GraphicsDisplay::InitXIM] ibus natively supported");
    return;
  }

  if (setlocale(LC_ALL, "") == NULL)
  {
    nuxDebugMsg("[GraphicsDisplay::InitXIM] cannot setlocale");
  }

  if (XSupportsLocale())
  {
    if (XSetLocaleModifiers("") == NULL)
    {
      nuxDebugMsg("[GraphicsDisplay::InitXIM] XSetLocaleModifiers failed.");
    }

    XRegisterIMInstantiateCallback(display_, NULL, NULL, NULL,
                                   XIMClient::SetupXIMClientCallback,
                                   (XPointer)this);
  }
}

void XIMClient::SetupXIMClientCallback(Display *dpy, XPointer client_data, XPointer call_data)
{
  XIMClient* self = (XIMClient*)client_data;
  self->SetupXIMClient();
}

// FIXME We need to have a list of the xics...and only have 1 XIM
// will need to make an XIMClientController class....otherwise a hang will happen
void XIMClient::EndXIMClientCallback(Display *dpy, XPointer client_data, XPointer call_data)
{
  XIMClient* self = (XIMClient*)client_data;
  self->xim_ = NULL;
  XDestroyIC(self->xic_);
  self->xic_ = NULL;
}

void XIMClient::SetupXIMClient()
{
  SetupXIM();
  if (xim_ && CheckRootStyleSupport())
    SetupXIC();
}

void XIMClient::SetupXIM()
{
  xim_ = XOpenIM(display_, NULL, NULL, NULL);
  if (xim_)
  {
    XIMCallback destroy_callback;
    destroy_callback.client_data = (XPointer)this;
    destroy_callback.callback = (XIMProc)XIMClient::EndXIMClientCallback;
    XSetIMValues (xim_, XNDestroyCallback, &destroy_callback, NULL);

    XUnregisterIMInstantiateCallback (display_, NULL, NULL, NULL,
                                      XIMClient::SetupXIMClientCallback,
                                      (XPointer)this);
  }
  else
  {
    nuxDebugMsg("[GraphicsDisplay::XIMStartCallback] Failed to open IM.");
  }
}

bool XIMClient::CheckRootStyleSupport() const
{
  int i;
  XIMStyles *xim_styles = NULL;

  XGetIMValues(xim_, XNQueryInputStyle, &xim_styles, NULL);

  for (i = 0; i < xim_styles->count_styles; ++i)
    if (xim_styles->supported_styles[i] == default_style_)
      break;

  if (i >= xim_styles->count_styles)
    return false;
  return true;
}

void XIMClient::SetupXIC()
{
  xic_ = XCreateIC(xim_, XNInputStyle, default_style_, XNClientWindow, window_, NULL);
  if (!xic_)
    nuxDebugMsg("[GraphicsDisplay::XIMStartCallback] failed to register xic");
}

XIC& XIMClient::GetXIC()
{
  return xic_;
}

void XIMClient::ResetXIC()
{
  if (!xim_)
    SetupXIMClient();

  if (xim_ && CheckRootStyleSupport())
  {
    if (xic_)
      XDestroyIC(xic_);
    SetupXIC();
  }
}

void XIMClient::FocusInXIC()
{
  if (xic_ && !focused_)
  {
    XSetICFocus(xic_);
    focused_ = true;
  }
}

void XIMClient::FocusOutXIC()
{
  if (xic_ && focused_)
  {
    XUnsetICFocus(xic_);
    focused_ = false;
  }
}

bool XIMClient::IsFocused() const
{
  return focused_;
}
