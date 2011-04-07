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


#ifndef COLORRANGEVALUATOR_H
#define COLORRANGEVALUATOR_H

namespace nux
{

  class ColorRangeValuator : public RangeValue
  {
  public:
    ColorRangeValuator (float value = 0.0f, Color StartColor = Color (0xFFFFFFFF), Color EndColor = Color (0x00000000) );
    ~ColorRangeValuator();

    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);

    //! Get the interpolated color.
    /*!
        @return The interpolated color.
    */
    Color GetColor() const;

  protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

  private:
    Color m_StartColor;
    Color m_EndColor;
  };

}

#endif // COLORRANGEVALUATOR_H


