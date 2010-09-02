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


#include "Nux.h"

#include "WindowCompositor.h"
#include "FloatingWindow.h"
#include "Button.h"

#include "Dialog.h"
#include "SplineCurveEditor.h"
#include "SplineCurveDialog.h"

NAMESPACE_BEGIN_GUI

SplineCurveDialog::SplineCurveDialog()
{
    SetWindowTitle(TEXT("Spline Curve Editor"));
    SetWindowSizeMatchLayout(true);
    m_Vlayout = smptr(VLayout)(new VLayout(TEXT("Spline Curve Editor")));
    m_SplineCurve = smptr(SplineCurveEditor)(new SplineCurveEditor());

    m_Vlayout->AddActiveInterfaceObject(m_SplineCurve);
    m_Vlayout->AddLayout(m_ButtonLayout);

    m_Vlayout->SetStretchFactor(0);
    // Set layout for the window
    setLayout(m_Vlayout);

    m_SplineCurve->sigCurveChange.connect(sigc::mem_fun(this, &SplineCurveDialog::RecvCurveChange));

}


SplineCurveDialog::~SplineCurveDialog()
{
    //INL_SAFE_DELETE(m_Vlayout);
    //INL_SAFE_DELETE(m_SplineCurve);
}

void SplineCurveDialog::StartSplineCurve(bool StartModal)
{
    Dialog::Start(StartModal);
    //StartModal();
}

void SplineCurveDialog::RecvOk()
{
    //ShowWindow(FALSE);
    StopModal();
    sigOk.emit(m_SplineCurve);
}

void SplineCurveDialog::RecvCancel()
{
    //ShowWindow(FALSE);
    StopModal();
    sigCancel.emit(m_SplineCurve);
}

void SplineCurveDialog::RecvCurveChange(const smptr(SplineCurveEditor) splinecurve)
{
    sigChange.emit(splinecurve);
}
//void SplineCurveDialog::SetOkCallback(const sigc::slot_base& slot_)
//{
//    sigOk.connect(slot_);
//}
//
//void SplineCurveDialog::SetCancelCallback(const sigc::slot_base& slot_)
//{
//    sigCancel.connect(slot_);
//}

double SplineCurveDialog::Eval(double t)
{
    return m_SplineCurve->Eval(t);
}


NAMESPACE_END_GUI
