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


#ifndef RANGEVALUEINTEGER_H
#define RANGEVALUEINTEGER_H

NAMESPACE_BEGIN_GUI

class HLayout;
class EditTextBox;

class RangeValueInteger : public ActiveInterfaceObject //public ValuatorAbstraction
{
public:
    RangeValueInteger(int Value = 0, int MinValue = 0, int MaxValue = 100);
    ~RangeValueInteger();

    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    void DrawMarker(GraphicsContext& GfxContext);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    /////////////////
    //  RECEIVERS  //
    /////////////////
    void SetRange(int min_value, int max_value);
    void SetValue(int value);
    int GetValue() const;
    int GetMinValue() const {return m_min;}
    int GetMaxValue() const {return m_max;}

    void SetBackgroundColor(const Color& color) { m_ValueString->SetTextBackgroundColor(color);}
    const Color GetBackgroundColor() const { return m_ValueString->GetTextBackgroundColor();}

    void setStartToEndColor(Color color_start, Color color_end) 
    {
        m_StartColor = color_start;
        m_EndColor = color_end;
    }
    void setStartColor(Color color) {m_StartColor = color;}
    void setEndColor(Color color) {m_EndColor = color;}
    void setProgressColor(Color color) {m_ProgressColor = color;}
    void EnableDrawProgress(bool b) {m_EnableDrawProgress = b;}

    ////////////////
    //  EMITTERS  //
    ////////////////
    void OnReceiveMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnReceiveMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnReceiveMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void OnKeyboardFocus();
    void OnLostKeyboardFocus();
    void OnValidateKeyboardEntry(const weaksmptr(EditTextBox) textbox, const NString& text);

    bool IsCtrlKeyPressed() const {return (m_CTRL_KEY? true: false);}

    // signals
    sigc::signal<void, const weaksmptr(RangeValueInteger)> sigValueChanged;
    sigc::signal<void, int> sigMouseDown;
    sigc::signal<void, int> sigMouseUp;
    sigc::signal<void, int> sigMouseDrag;
    sigc::signal<void, int> sigSetTypedValue;
    //sigc::signal<void, const char*> sigValidateKeyboarEntry;

protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

protected:
    smptr(HLayout) hlayout;
    smptr(EditTextBox) m_ValueString;
    smptr(CoreArea) m_Percentage;
    Color m_StartColor;
    Color m_EndColor;
    Color m_ProgressColor;
    Color m_BackgroundColor;
    bool m_EnableDrawProgress;

    long m_CTRL_KEY;

    float m_MarkerPosition;
    int m_Value;
    int m_min, m_max;

};

NAMESPACE_END_GUI

#endif // RANGEVALUEINTEGER_H


