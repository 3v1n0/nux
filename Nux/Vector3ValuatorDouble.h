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


#ifndef VECTOR3DOUBLEVALUATOR_H
#define VECTOR3DOUBLEVALUATOR_H

#include "DoubleValidator.h"
#include "Vector3Valuator.h"

NAMESPACE_BEGIN_GUI

class EditTextBox;
class StaticTextBox;
class HLayout;

class Vector3DoubleValuator: public Vector3Valuator
{
public:
    Vector3DoubleValuator(double X = 0, double Y = 0, double Z = 0, double Step = 1, double MinValue = -1.7E308, double MaxValue = 1.7E308);
    ~Vector3DoubleValuator();

    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    virtual void ImplementIncrementX();
    virtual void ImplementDecrementX();
    virtual void ImplementIncrementY();
    virtual void ImplementDecrementY();
    virtual void ImplementIncrementZ();
    virtual void ImplementDecrementZ();
    virtual void ImplementValidateEntryX();
    virtual void ImplementValidateEntryY();
    virtual void ImplementValidateEntryZ();

    void SetVectorX(double value);
    void SetVectorY(double value);
    void SetVectorZ(double value);
    void SetValue(double x, double y, double z);
    double GetVectorX() const;
    double GetVectorY() const;
    double GetVectorZ() const;
    void SetStep(double i);
    double GetStep() const;
    double GetMinValue() const;
    double GetMaxValue() const;
    void SetRange(double MinValue, double Maxvalue);

    sigc::signal<void, const weaksmptr(Vector3DoubleValuator)> sigIncrement;
    sigc::signal<void, const weaksmptr(Vector3DoubleValuator)> sigDecrement;
    sigc::signal<void, const weaksmptr(Vector3DoubleValuator)> sigValueChanged;
    sigc::signal<void, double, double, double> sigValue;
private:
    smptr(HLayout) m_hlayout;

    DoubleValidator m_DoubleValidator;
    double m_X;
    double m_Y;
    double m_Z;
    double m_Step;

    smptr(StaticTextBox) m_XLabel;
    smptr(StaticTextBox) m_YLabel;
    smptr(StaticTextBox) m_ZLabel;
};

NAMESPACE_END_GUI

#endif // VECTOR3DOUBLEVALUATOR_H
