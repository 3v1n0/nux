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


#ifndef WIDGETMETRICS_H
#define WIDGETMETRICS_H

namespace nux
{

  extern const Color COLOR_BLACK ;
  extern const Color COLOR_RED;
  extern const Color COLOR_GREEN;;
  extern const Color COLOR_BLUE;
  extern const Color COLOR_WHITE;


  extern const Color COLOR_BACKGROUND_PRIMARY;
  extern const Color COLOR_BACKGROUND_SECONDARY;
  extern const Color COLOR_FOREGROUND_PRIMARY;
  extern const Color COLOR_FOREGROUND_SECONDARY;


  extern const Color COLOR_SELECTION_SECONDARY;

// Text color
  extern const Color COLOR_TEXTEDIT_BACKGROUNG;
  extern const Color COLOR_TEXTSTATIC_BACKGROUNG;

// General background color
  extern const Color  COLOR_WIDGET_BACKGROUND;
  extern const Color  COLOR_WIDGET_MOUSEOVER;
  extern const Color  COLOR_WINDOW_BACKGROUND;

// Widget control background color
  extern const Color  COLOR_PRIMARY_LEFT;
  extern const Color  COLOR_PRIMARY_RIGHT;

// ScrollBar color
  extern const Color COLOR_SCROLLBAR_TRACK;
  extern const Color COLOR_SCROLLBAR_SLIDER;

  extern const Color COLOR_DARK_0;
  extern const Color COLOR_LIGHT_0;


  extern const int   AREA_MIN_WIDTH;
  extern const int   AREA_MAX_WIDTH;
  extern const int   AREA_MIN_HEIGHT;
  extern const int   AREA_MAX_HEIGHT;

  extern const int   DEFAULT_WIDGET_HEIGHT;
  extern const int   PRACTICAL_WIDGET_HEIGHT;
  extern const int   DEFAULT_WIDGET_WIDTH;
  extern const int   PRACTICAL_WIDGET_WIDTH;
  extern const int   DEFAULT_TEXT_X_MARGIN;
  extern const int   DEFAULT_TEXT_Y_MARGIN;
  extern const float DEFAULT_REPEAT_DELAY;



}

#endif // WIDGETMETRICS_H
