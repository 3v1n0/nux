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


#ifndef SPLINECURVEPICKER_H
#define SPLINECURVEPICKER_H

#include "TimerProc.h"

NAMESPACE_BEGIN_GUI

class SplineCurveDialog;
class SplineCurveEditor;
class SplineCurvePreview;
class TimerFunctor;
//struct TimerHandle;

class SplineCurveDialogProxy
{
public:
    SplineCurveDialogProxy(bool ModalWindow);
    ~SplineCurveDialogProxy();

    void RecvDialogOk(const weaksmptr(SplineCurveEditor) splinecurve);
    void RecvDialogCancel(const weaksmptr(SplineCurveEditor) splinecurve);
    void RecvDialogChange(const weaksmptr(SplineCurveEditor) splinecurve);
    
    void Start();
    bool IsActive();
    void StopThreadMonitoring();

    const SplineKnot& GetControlPoints() const;
    void SetControlPoints(const SplineKnot&);

private:
    SplineKnot m_PreviousControlPoints;
    SplineKnot m_ControlPoints;

    smptr(SplineCurveEditor) m_Curve;
    bool m_bDialogChange;
    bool m_bDialogRunning;
    unsigned int m_DialogThreadID;
    bool m_ModalWindow;
    NThread* m_Thread;

    friend class SplineCurvePreview;
};

class SplineCurvePreview : public ActiveInterfaceObject
{
public:
    SplineCurvePreview();
    ~SplineCurvePreview();
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    void SetXAxisBounds(float minX, float maxX);
    void SetYAxisBounds(float minY, float maxY);
    void UpdateGraph();

    int GetNumKnot() const;
    SplineKnot::Knot GetKnot(int i) const;

    sigc::signal<void, const weaksmptr(SplineCurveEditor) > sigSplineChanged;
    void RecvDialogChange(const weaksmptr(SplineCurveEditor) splinecurve);

    const SplineKnot& GetControlPoints() const;
    void Reset();

protected:
    // used by SplineCurvePropertyItem
    void AddKnot(double x, double y, bool bSelected);

private:
    void SetControlPoints(const SplineKnot&);

    double Eval(double t);
    void RecvTimer(void* v);
    void RecvClick(int x, int y, unsigned long button_flags, unsigned long key_flags);

    float m_minX, m_minY, m_maxX, m_maxY;

    TimerFunctor* m_ChangeDetectionTimer;
    TimerHandle* m_ChangeTimerHandler;

    TextureLayer* m_BackgroundLayer;
    TRefGL<IOpenGLTexture2D> Texture;
    GLSh_DrawFunction* m_DrawFunctionShader;
    WindowThread* m_WindowDialog;

    SplineCurveDialogProxy* m_DialogThreadProxy;

    SplineKnot m_control_knot;
    CubicSpline m_CubicSpline;
};


NAMESPACE_END_GUI

#endif // SPLINECURVEPICKER_H