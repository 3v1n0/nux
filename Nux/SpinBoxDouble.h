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


#ifndef SPINBOXDOUBLE_H
#define SPINBOXDOUBLE_H

#include "EditTextBox.h"

namespace nux
{

  class SpinBoxDouble: public SpinBox_Logic
  {
  public:
    SpinBoxDouble(double Value = 0, double Step = 1, double MinValue = 0, double MaxValue = 100, NUX_FILE_LINE_PROTO);
    virtual ~SpinBoxDouble();

    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);

    virtual void ImplementIncrementBtn();
    virtual void ImplementDecrementBtn();
    virtual void ImplementValidateEntry();

    void SetValue(double value);
    double GetValue() const;
    void SetStep(double i);
    double GetStep() const;
    double GetMinValue() const;
    double GetMaxValue() const;
    void SetRange(double MinValue, double Maxvalue);

    sigc::signal<void, SpinBoxDouble * > sigIncrement;
    sigc::signal<void, SpinBoxDouble * > sigDecrement;
    sigc::signal<void, SpinBoxDouble * > sigValueChanged;
    sigc::signal<void, double> sigValue;

  protected:
    void InitializeWidgets();
    void InitializeLayout();

  private:
    HLayout    *m_hlayout;
    VLayout    *m_vlayout;

    DoubleValidator m_DoubleValidator;
    double m_Value;
    double m_Step;
  };

}

#endif // SPINBOXDOUBLE_H
