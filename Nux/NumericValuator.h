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


#ifndef NUMERICVALUATOR_H
#define NUMERICVALUATOR_H

#include "SpinBox_Logic.h"

namespace nux
{

  class HLayout;
  class EditTextBox;

  class NumericValuator : public SpinBox_Logic
  {
  public:
    NumericValuator();
    ~NumericValuator();

    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);

    virtual void ImplementIncrementBtn();
    virtual void ImplementDecrementBtn();
    virtual void ImplementValidateEntry();

    void SetValue(float value);
    float GetValue() const;
    void SetStep(float f);
    float GetStep();

    sigc::signal<void> sigIncrement;
    sigc::signal<void> sigDecrement;
    sigc::signal<void, float> sigValueChanged;

  protected:
    void InitializeWidgets();
    void InitializeLayout();

  private:
    HLayout *hlayout;

    DoubleValidator m_DoubleValidator;
    float m_fValue;
    float m_Step;
  };

}

#endif // NUMERICVALUATOR_H
