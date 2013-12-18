// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright 2013 Inalogic® Inc.
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
 * Authored by: Brandon Schaefer
 *
 */


#include "Nux.h"
#include "TextEntry.h"
#include "XICClient.h"

#include <string>

#include "NuxCore/Logger.h"

DECLARE_LOGGER(logger, "xim.callbacks");

using namespace std;

namespace nux
{

int const FEEDBACK_MASK = (XIMUnderline | XIMReverse);

int XICClient::PreeditStartCallback(XIC xic,
                                    XPointer clientdata,
                                    XPointer call_data)
{
  TextEntry* text_entry = (TextEntry*)clientdata;
  if (text_entry)
  {
    text_entry->PreeditStarted();
  }

  return 0;
}

int XICClient::PreeditDoneCallback(XIC xic,
                                   XPointer clientdata,
                                   XPointer call_data)
{
  TextEntry* text_entry = (TextEntry*)clientdata;
  if (text_entry)
  {
    text_entry->ClearPreedit();
  }

  return 0;
}

void add_feedback_attr(PangoAttrList* attrs,
                            char const* str,
                            XIMFeedback feedback,
                            int start,
                            int end)
{
  PangoAttribute* attr;

  int start_index = g_utf8_offset_to_pointer(str, start) - str;
  int end_index   = g_utf8_offset_to_pointer(str, end)   - str;

  if (feedback & XIMUnderline)
  {
    attr = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
    attr->start_index = start_index;
    attr->end_index = end_index;

    pango_attr_list_insert (attrs, attr);
  }

  if (feedback & XIMReverse)
  {
    attr = pango_attr_foreground_new(0xFFFF, 0xFFFF, 0xFFFF);
    attr->start_index = start_index;
    attr->end_index = end_index;

    pango_attr_list_insert (attrs, attr);

    attr = pango_attr_background_new(0x0, 0x0, 0x0);

    attr->start_index = start_index;
    attr->end_index = end_index;

    pango_attr_list_insert (attrs, attr);
  }

  if (feedback & ~FEEDBACK_MASK)
  {
    LOG_WARN(logger) << "Unrenderer feedback: " << (feedback & ~FEEDBACK_MASK);
  }
}

void feedback_to_pango_list(PangoAttrList** attrs,
                            char const* str,
                            int nfeedbacks,
                            XIMFeedback* feedbacks)
{
  XIMFeedback last_feedback = 0;
  int start = -1;
  int i = 0;

  if (attrs)
  {
    *attrs = pango_attr_list_new();

    for (i = 0; i < nfeedbacks; ++i)
    {
      XIMFeedback new_feedback = (feedbacks[i] & FEEDBACK_MASK);
      if (new_feedback != last_feedback)
      {
        last_feedback = new_feedback;
        start = i;
      }
      else
      {
        PangoAttribute* attr;

        int start_index = g_utf8_offset_to_pointer(str, start) - str;
        int end_index   = g_utf8_offset_to_pointer(str, i)   - str;

        attr = pango_attr_foreground_new(0x0, 0x0, 0x0);
        attr->start_index = start_index;
        attr->end_index = end_index;

        pango_attr_list_insert (*attrs, attr);

        attr = pango_attr_background_new(0xFFFF, 0xFFFF, 0xFFFF);

        attr->start_index = start_index;
        attr->end_index = end_index;

        pango_attr_list_insert (*attrs, attr);
      }
    }

    if (start >= 0)
      add_feedback_attr(*attrs, str, last_feedback, start, i);
  }
}

int XICClient::PreeditDrawCallback(XIC xic,
                                   XPointer clientdata,
                                   XIMPreeditDrawCallbackStruct* call_data)
{
  TextEntry* text_entry = (TextEntry*)clientdata;

  if (call_data->text && text_entry)
  {
    string preedit;

    // TODO Actually handle this correctly...
    if (call_data->text->encoding_is_wchar)
    {
      preedit = string("");
    }
    else
    {
      preedit = string(call_data->text->string.multi_byte);
    }

    PangoAttrList* preedit_attr;
    feedback_to_pango_list(&preedit_attr, preedit.c_str(),
                           call_data->text->length,
                           call_data->text->feedback);

    text_entry->UpdatePreeditAttribs(preedit_attr);
    text_entry->UpdatePreedit(preedit, call_data->caret);
  }

  return 0;
}

}
