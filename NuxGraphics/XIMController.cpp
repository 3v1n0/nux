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

#include "XIMController.h"
#include "NuxCore/NuxCore.h"
#include "GraphicsDisplayX11.h"


XIMController::XIMController(Display* display)
  : display_(display),
    xim_(NULL),
    xim_style_(0),
    current_xic_(xic_clients_.end())
{
  InitXIMCallback();
}

XIMController::~XIMController()
{
  if (xim_)
    XCloseIM(xim_);
}

void XIMController::AddXICClient(Window window)
{
  xic_clients_.insert(std::make_pair(window, XICClient(window)));
}

void XIMController::SetCurrentXICClient(Window window)
{
  current_xic_ = xic_clients_.find(window);
  (current_xic_->second).ResetXIC(xim_, xim_style_);
}

bool XIMController::HasXICClientForWindow(Window window)
{
  return (xic_clients_.find(window) != xic_clients_.end());
}

bool XIMController::IsCurrentXIMValid()
{
  if (xim_ && current_xic_ != xic_clients_.end() && (current_xic_->second).GetXIC() != NULL)
    return true;
  return false;
}

XIC& XIMController::GetXIC() const
{
  return (current_xic_->second).GetXIC();
}

void XIMController::FocusInXIC()
{
  (current_xic_->second).FocusInXIC();
}

void XIMController::FocusOutXIC()
{
  (current_xic_->second).FocusOutXIC();
}

bool XIMController::IsXICFocused() const
{
  return (current_xic_->second).IsFocused();
}

void XIMController::InitXIMCallback()
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
                                   XIMController::SetupXIMClientCallback,
                                   (XPointer)this);
  }
}

void XIMController::SetupXIMClientCallback(Display *dpy, XPointer client_data, XPointer call_data)
{
  XIMController* self = (XIMController*)client_data;
  self->SetupXIMClient();
}

void XIMController::EndXIMClientCallback(Display *dpy, XPointer client_data, XPointer call_data)
{
  XIMController* self = (XIMController*)client_data;
  self->xim_ = NULL;
  self->xim_style_ = 0;
  for (auto xic : self->xic_clients_)
    (xic.second).Destroy();
}

void XIMController::SetupXIMClient()
{
  SetupXIM();
  if (!xim_style_ && xim_)
    SetupRootStyle();
}

void XIMController::SetupXIM()
{
  xim_ = XOpenIM(display_, NULL, NULL, NULL);
  if (xim_)
  {
    XIMCallback destroy_callback;
    destroy_callback.client_data = (XPointer)this;
    destroy_callback.callback = (XIMProc)XIMController::EndXIMClientCallback;
    XSetIMValues (xim_, XNDestroyCallback, &destroy_callback, NULL);

    XUnregisterIMInstantiateCallback (display_, NULL, NULL, NULL,
                                      XIMController::SetupXIMClientCallback,
                                      (XPointer)this);
  }
  else
  {
    nuxDebugMsg("[GraphicsDisplay::XIMStartCallback] Failed to open IM.");
  }
}

void XIMController::SetupRootStyle()
{
  int i;
  XIMStyles *xim_styles = NULL;
  XIMStyle root_style = (XIMPreeditNothing|XIMStatusNothing);

  XGetIMValues(xim_, XNQueryInputStyle, &xim_styles, NULL);

  for (i = 0; i < xim_styles->count_styles; ++i)
    if (xim_styles->supported_styles[i] == root_style)
      break;

  if (i >= xim_styles->count_styles)
    xim_style_ = 0;
  xim_style_ = root_style;
}

