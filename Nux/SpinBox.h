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

#ifndef SPINBOX_H
#define SPINBOX_H

#include "IntegerValidator.h"
#include "SpinBox_Logic.h"

namespace nux
{

  class SpinBox: public SpinBox_Logic
  {
  public:
    SpinBox(int Value = 0, int Step = 1, int MinValue = 0, int MaxValue = 100, NUX_FILE_LINE_PROTO);
    virtual ~SpinBox();

    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);

    virtual void ImplementIncrementBtn();
    virtual void ImplementDecrementBtn();
    virtual void ImplementValidateEntry();

    void SetValue(int value);
    int GetValue() const;
    void SetStep(int i);
    int GetStep() const;
    int GetMinValue() const;
    int GetMaxValue() const;
    void SetRange(int MinValue, int Maxvalue);

    sigc::signal<void, SpinBox * > sigIncrement;
    sigc::signal<void, SpinBox * > sigDecrement;
    sigc::signal<void, SpinBox * > sigValueChanged;
    sigc::signal<void, int> sigValue;

  protected:
    void InitializeWidgets();
    void InitializeLayout();

  private:
    HLayout *hlayout;
    VLayout *vlayout;

    IntegerValidator m_IntValidator;
    int m_iValue;
    int m_Step;
  };

}

#endif // SPINBOX_H
