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


#ifndef ABSTRACTSEPARATOR_H
#define ABSTRACTSEPARATOR_H

#include "Nux/View.h"

namespace nux
{
  class AbstractSeparator: public View
  {
  public:
    AbstractSeparator (NUX_FILE_LINE_PROTO);
    AbstractSeparator (const Color &color, float Alpha0, float Alpha1, int Border, NUX_FILE_LINE_PROTO);
    ~AbstractSeparator();
    void SetColor (const Color &color);
    void SetAlpha (float Alpha0, float Alpha1);
    void SetBorderSize (int Border);

  protected:
    Color m_Color;
    float m_Alpha0;
    float m_Alpha1;
    int m_BorderSize;
  };

}

#endif // ABSTRACTSEPARATOR_H
