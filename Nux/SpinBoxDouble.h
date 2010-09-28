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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef SPINBOXDOUBLE_H
#define SPINBOXDOUBLE_H

#include "EditTextBox.h"

namespace nux { //NUX_NAMESPACE_BEGIN

class SpinBoxDouble: public SpinBox_Logic
{
public:
    SpinBoxDouble(double Value = 0, double Step = 1, double MinValue = 0, double MaxValue = 100);
    ~SpinBoxDouble();

    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

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

    sigc::signal<void, const weaksmptr(SpinBoxDouble) > sigIncrement;
    sigc::signal<void, const weaksmptr(SpinBoxDouble) > sigDecrement;
    sigc::signal<void, const weaksmptr(SpinBoxDouble) > sigValueChanged;
    sigc::signal<void, double> sigValue;

protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

private:
    smptr(HLayout) m_hlayout;
    smptr(VLayout) m_vlayout;

    DoubleValidator m_DoubleValidator;
    double m_Value;
    double m_Step;
};

} //NUX_NAMESPACE_END

#endif // SPINBOXDOUBLE_H
