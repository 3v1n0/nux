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

#include "XICClient.h"

namespace nux
{

XICClient::XICClient()
  : xic_(nullptr)
  , xim_style_(0)
  , focused_(false)
{
}

void XICClient::ResetXIC(XIM xim, Window window, Display* display)
{
  if (!xim_style_)
    SetupXIMStyle(xim);

  SetupXIC(xim, window, display);
}

void XICClient::SetCurrentTextEntry(TextEntry* text_entry)
{
  text_entry_ = text_entry;
}

static int preedit_caret_callback(XIC xic,
                                  XPointer clientdata,
                                  XIMPreeditCaretCallbackStruct* call_data)
{
  return 0;
}

static int status_start_callback(XIC xic,
                                 XPointer clientdata,
                                 XIMPreeditDrawCallbackStruct* call_data)
{
  return 0;
}

static void status_draw_callback(XIC xic,
                                 XPointer clientdata,
                                 XPointer* call_data)
{
}

static void status_done_callback(XIC xic,
                                 XPointer clientdata,
                                 XPointer* call_data)
{
}

XVaNestedList XICClient::GetPreeditCallbacks()
{
  preedit_start_cb_.callback = (XIMProc)XICClient::PreeditStartCallback;
  preedit_start_cb_.client_data = nullptr;

  preedit_done_cb_.callback = (XIMProc)XICClient::PreeditDoneCallback;
  preedit_done_cb_.client_data = (XPointer)text_entry_;

  preedit_draw_cb_.callback = (XIMProc)XICClient::PreeditDrawCallback;
  preedit_draw_cb_.client_data = (XPointer)text_entry_;

  preedit_caret_cb_.callback = (XIMProc)preedit_caret_callback;
  preedit_caret_cb_.client_data = nullptr;

  XVaNestedList p_list = nullptr;
  p_list = XVaCreateNestedList(0,
                               XNPreeditStartCallback, &preedit_start_cb_,
                               XNPreeditDoneCallback,  &preedit_done_cb_,
                               XNPreeditDrawCallback,  &preedit_draw_cb_,
                               XNPreeditCaretCallback, &preedit_caret_cb_,
                               nullptr);

  return p_list;
}

XVaNestedList XICClient::GetStatusCallbacks()
{
  status_start_cb_.callback = (XIMProc)status_start_callback;
  status_start_cb_.client_data = nullptr;

  status_done_cb_.callback = (XIMProc)status_done_callback;
  status_done_cb_.client_data = nullptr;

  status_draw_cb_.callback = (XIMProc)status_draw_callback;
  status_draw_cb_.client_data = nullptr;

  XVaNestedList s_list = nullptr;
  s_list = XVaCreateNestedList(0,
                               XNStatusStartCallback, &status_start_callback,
                               XNStatusDoneCallback,  &status_done_callback,
                               XNStatusDrawCallback,  &status_draw_callback,
                               nullptr);

  return s_list;
}

XIMStyle XICClient::FilterXIMStyle()
{
  XIMStyle style = 0;

  if (xim_style_ & XIMPreeditCallbacks)
  {
    style |= XIMPreeditCallbacks;
  }
  else if (xim_style_ & XIMPreeditNone)
  {
    style |= XIMPreeditNone;
  }
  else
  {
    style |= XIMPreeditNothing;
  }

  if (xim_style_ & XIMStatusCallbacks)
  {
    style |= XIMStatusCallbacks;
  }
  else if (xim_style_ & XIMStatusNone)
  {
    style |= XIMStatusNone;
  }
  else
  {
    style |= XIMStatusNothing;
  }

  return style;
}

void XICClient::SetupXIC(XIM xim, Window window, Display* display)
{
  if (xic_)
    DestroyXIC();

  XIMStyle style = FilterXIMStyle();
  
  const char* p_name = nullptr;
  XVaNestedList p_list = nullptr;
  if (style & XIMPreeditCallbacks)
  {
    p_list = GetPreeditCallbacks();
    p_name = XNPreeditAttributes;
  }

  const char* s_name = nullptr;
  XVaNestedList s_list = nullptr;
  if (style & XIMStatusCallbacks)
  {
    s_list = GetStatusCallbacks();
    s_name = XNStatusAttributes;
  }

  xic_ = XCreateIC(xim, XNInputStyle, style,
                   XNClientWindow, window,
                   p_name, p_list,
                   s_name, s_list,
                   nullptr);

  if (p_list)
    XFree(p_list);
  if (s_list)
    XFree(s_list);

  xim_style_ = style;
}

XIMStyle ChooseBetterStyle(XIMStyle style1, XIMStyle style2)
{
  XIMStyle s,t;
  XIMStyle preedit = XIMPreeditArea | XIMPreeditCallbacks |
                     XIMPreeditPosition | XIMPreeditNothing | XIMPreeditNone;

  XIMStyle status = XIMStatusArea | XIMStatusCallbacks |
                    XIMStatusNothing | XIMStatusNone;

  if (style1 == 0)
    return style2;

  if (style2 == 0)
    return style1;

  if ((style1 & (preedit | status)) == (style2 & (preedit | status)))
    return style1;

  s = style1 & preedit;
  t = style2 & preedit;
  if (s != t)
  {
    if (s | t | XIMPreeditCallbacks)
      return (s == XIMPreeditCallbacks) ? style1 : style2;
    else if (s | t | XIMPreeditPosition)
      return (s == XIMPreeditPosition)  ? style1 : style2;
    else if (s | t | XIMPreeditArea)
      return (s == XIMPreeditArea)      ? style1 : style2;
    else if (s | t | XIMPreeditNothing)
      return (s == XIMPreeditNothing)   ? style1 : style2;
    else /*if (s | t | XIMPreeditNone)*/
      return (s == XIMPreeditNone)      ? style1 : style2;
  }
  else
  {
    s = style1 & status;
    t = style2 & status;

    if (s | t | XIMStatusCallbacks)
      return (s == XIMStatusCallbacks) ? style1 : style2;
    else if (s | t | XIMStatusArea)
      return (s == XIMStatusArea)      ? style1 : style2;
    else if (s | t | XIMStatusNothing)
      return (s == XIMStatusNothing)   ? style1 : style2;
    else /*if (s | t | XIMStatusNone)*/
      return (s == XIMStatusNone)      ? style1 : style2;
  }
}

void XICClient::SetupXIMStyle(XIM xim)
{
  int i;
  XIMStyles *xim_styles = nullptr;
  XIMStyle prefered_style = XIMPreeditCallbacks | XIMStatusCallbacks;

  XGetIMValues(xim, XNQueryInputStyle, &xim_styles, nullptr);

  XIMStyle best = 0, style = 0;
  for (i = 0; i < xim_styles->count_styles; ++i)
  {
    style = xim_styles->supported_styles[i];

    if ((style & prefered_style) == style)
    {
      best = ChooseBetterStyle(style, best);
      break;
    }
    else
    {
      best = ChooseBetterStyle(style, best);
    }
  }

  xim_style_ = best;

  XFree(xim_styles);
}

bool XICClient::HasXIC() const
{
  return xic_ != nullptr;
}

XIC XICClient::GetXIC() const
{
  return xic_;
}

void XICClient::Reinitialize()
{
  xic_ = nullptr;
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
    xic_ = nullptr;
  }
}

} //namespace nux
