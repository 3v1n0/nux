/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef VALUATORDOUBLE_H
#define VALUATORDOUBLE_H

#include "EditTextBox.h"
#include "HLayout.h"
#include "VLayout.h"
#include "DoubleValidator.h"
#include "Valuator.h"

namespace nux
{
  // NUXTODO: Rename to DoubleSpinner
  class ValuatorDouble: public Valuator
  {
  public:
    ValuatorDouble (double Value = 0, double Step = 1, double MinValue = 0, double MaxValue = 100);
    ~ValuatorDouble();

    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);

    virtual void ImplementIncrementBtn();
    virtual void ImplementDecrementBtn();
    virtual void ImplementValidateEntry();

    void SetValue (double value);
    double GetValue() const;
    void SetStep (double i);
    double GetStep() const;
    double GetMinValue() const;
    double GetMaxValue() const;
    void SetRange (double MinValue, double Maxvalue);

    sigc::signal<void, ValuatorDouble *> sigIncrement;
    sigc::signal<void, ValuatorDouble *> sigDecrement;
    sigc::signal<void, ValuatorDouble *> sigValueChanged;
    sigc::signal<void, double> sigValue;
  private:
    HLayout *m_hlayout;

    DoubleValidator m_DoubleValidator;
    double m_Value;
    double m_Step;
  };

}

#endif // VALUATORDOUBLE_H

