/*
* Copyright 2012-2013 Inalogic® Inc.
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

#include <string.h>

#include "XIMController.h"
#include "NuxCore/Logger.h"

DECLARE_LOGGER(logger, "xim.controller");

namespace nux
{

XIMController::XIMController(Display* display)
  : display_(display)
  , window_(0)
  , xim_(nullptr)
{
  InitXIMCallback();
}

XIMController::~XIMController()
{
  // The XIC must be destroyed before the XIM
  xic_client_.DestroyXIC();

  if (xim_)
    XCloseIM(xim_);
}

void XIMController::SetFocusedWindow(Window window)
{
  window_ = window;

  if (xim_)
    xic_client_.ResetXIC(xim_, window, display_);
}

void XIMController::SetCurrentTextEntry(TextEntry* text_entry)
{
  xic_client_.SetCurrentTextEntry(text_entry);

  if (xim_)
    xic_client_.ResetXIC(xim_, window_, display_);
}

void XIMController::RemoveFocusedWindow()
{
  window_ = 0;
  xic_client_.DestroyXIC();
}

bool XIMController::IsXICValid() const
{
  return xic_client_.HasXIC();
}

XIC XIMController::GetXIC() const
{
  return xic_client_.GetXIC();
}

void XIMController::FocusInXIC()
{
  xic_client_.FocusInXIC();
}

void XIMController::FocusOutXIC()
{
  xic_client_.FocusOutXIC();
  xic_client_.SetCurrentTextEntry(nullptr);
}

Window XIMController::GetCurrentWindow() const
{
  return window_;
}

void XIMController::InitXIMCallback()
{
  char* const xmodifier = getenv("XMODIFIERS");

  if (xmodifier && strstr(xmodifier,"ibus") != NULL)
  {
    LOG_WARN(logger) << "IBus natively supported.";
    return;
  }

  if (setlocale(LC_ALL, "") == NULL)
  {
    LOG_WARN(logger) << "Cannot setlocale.";
  }

  if (XSupportsLocale())
  {
    if (XSetLocaleModifiers("") == NULL)
    {
      LOG_WARN(logger) << "XSetLocalModifiers Failed.";
    }

    XRegisterIMInstantiateCallback(display_, NULL, NULL, NULL,
                                   XIMController::SetupXIMClientCallback,
                                   (XPointer)this);
  }
}

void XIMController::SetupXIMClientCallback(Display* dpy, XPointer client_data, XPointer call_data)
{
  XIMController* self = (XIMController*)client_data;
  self->SetupXIM();
}

void XIMController::EndXIMClientCallback(Display* dpy, XPointer client_data, XPointer call_data)
{
  XIMController* self = (XIMController*)client_data;
  self->xim_ = NULL;
  self->xic_client_.Reinitialize();
  self->InitXIMCallback();
}

void XIMController::SetupXIM()
{
  xim_ = XOpenIM(display_, NULL, NULL, NULL);

  if (xim_)
  {
    SetupXIMDestroyedCallback();

    if (window_)
      xic_client_.ResetXIC(xim_, window_, display_);

    XUnregisterIMInstantiateCallback (display_, NULL, NULL, NULL,
                                      XIMController::SetupXIMClientCallback,
                                      (XPointer)this);
  }
  else
  {
    LOG_WARN(logger) << "Failed to open IM.";
  }
}

void XIMController::SetupXIMDestroyedCallback()
{
  XIMCallback destroy_callback;
  destroy_callback.callback = (XIMProc)XIMController::EndXIMClientCallback;
  destroy_callback.client_data = (XPointer)this;
  XSetIMValues (xim_, XNDestroyCallback, &destroy_callback, NULL);
}

} //namespace nux
