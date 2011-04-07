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


#include "Nux.h"
#include "WidgetMetrics.h"

namespace nux
{

  const Color COLOR_BLACK = Color(0xFF000000);
  const Color COLOR_RED =   Color(0xFFFF0000);
  const Color COLOR_GREEN = Color(0xFF00FF00);
  const Color COLOR_BLUE =  Color(0xFF0000FF);
  const Color COLOR_WHITE = Color(0xFFFFFFFF);


  const Color COLOR_BACKGROUND_PRIMARY = Color(0xFF4D4D4D);
  const Color COLOR_BACKGROUND_SECONDARY = Color(0xFF868686);
  const Color COLOR_FOREGROUND_PRIMARY = Color(0xFF424242); //0xFFCFCFCF;
  const Color COLOR_FOREGROUND_SECONDARY = Color(0xFFEFEFEF);


  const Color COLOR_SELECTION_SECONDARY = Color(0xFFAAAAAA);

// Text color
  const Color COLOR_TEXTEDIT_BACKGROUNG = Color(0xFF9F9F9F);
  const Color COLOR_TEXTSTATIC_BACKGROUNG = Color(0xFF909090);

// General background color
  const Color  COLOR_WIDGET_BACKGROUND     = Color(0xFF6E6E6E);
  const Color  COLOR_WIDGET_MOUSEOVER      = Color(0xFF969664);
  const Color  COLOR_WINDOW_BACKGROUND     = Color(0xFFa0a0a0);

// Widget control background color
  const Color  COLOR_PRIMARY_LEFT          = Color(0xFFC0C0C0);
  const Color  COLOR_PRIMARY_RIGHT         = Color(0xFF7D7D7D);

// ScrollBar color
  const Color COLOR_SCROLLBAR_TRACK     = Color(0xFF636363);
  const Color COLOR_SCROLLBAR_SLIDER    = Color(0xFF373737);

  const Color COLOR_DARK_0     = Color(0xFF575757);
  const Color COLOR_LIGHT_0    = Color(0xFF737373);


  const int   AREA_MIN_WIDTH     = 1;
  const int   AREA_MAX_WIDTH     = 0x7FFFFFFF;
  const int   AREA_MIN_HEIGHT    = 1;
  const int   AREA_MAX_HEIGHT    = 0x7FFFFFFF;

  const int   DEFAULT_WIDGET_HEIGHT   = 8;
  const int   PRACTICAL_WIDGET_HEIGHT = 18;
  const int   DEFAULT_WIDGET_WIDTH    = 32;
  const int   PRACTICAL_WIDGET_WIDTH  = 32;
  const int   DEFAULT_TEXT_X_MARGIN   = 0;
  const int   DEFAULT_TEXT_Y_MARGIN   = 0;
  const float DEFAULT_REPEAT_DELAY    = 150; //milliseconds

}
