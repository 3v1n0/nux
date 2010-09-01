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


#ifndef NUMERICVALUATOR_H
#define NUMERICVALUATOR_H

#include "SpinBox_Logic.h"

NAMESPACE_BEGIN_GUI

class HLayout;
class EditTextBox;

class NumericValuator : public SpinBox_Logic
{
public:
    NumericValuator();
    ~NumericValuator();

    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);
    
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
    void DestroyLayout();

private:
    smptr(HLayout) hlayout;

    DoubleValidator m_DoubleValidator;
    float m_fValue;
    float m_Step;
};

NAMESPACE_END_GUI

#endif // NUMERICVALUATOR_H
