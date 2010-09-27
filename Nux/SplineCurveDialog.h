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


#ifndef SPLINCURVEDIALOG_H
#define SPLINCURVEDIALOG_H

#include "SplineCurveEditor.h"

namespace nux { //NUX_NAMESPACE_BEGIN

class SplineCurveDialog : public Dialog
{
public:
    SplineCurveDialog();
    ~SplineCurveDialog();

    void StartSplineCurve(bool StartModal/* = false*/);
    double Eval(double t);

    virtual void RecvOk();
    virtual void RecvCancel();

    sigc::signal<void, const smptr(SplineCurveEditor) > sigOk;
    sigc::signal<void, const smptr(SplineCurveEditor) > sigCancel;    
    sigc::signal<void, const smptr(SplineCurveEditor) > sigChange;    

    //void SetOkCallback(const sigc::internal::slot_type& slot_);
    //void SetCancelCallback(const sigc::slot_type& slot_);

    INT GetNumKnot() const {return (INT) m_SplineCurve->GetNumKnot();}
    SplineKnot::Knot GetKnot(int i) const {return m_SplineCurve->GetKnot(i);}
    SplineCurveEditor& GetSplineCurve() {return *m_SplineCurve;}

private:
    void RecvCurveChange(const smptr(SplineCurveEditor) splinecurve);

    smptr(VLayout) m_Vlayout;
    smptr(SplineCurveEditor) m_SplineCurve;
    //nux::Color& m_Color;
};

} //NUX_NAMESPACE_END

#endif // SPLINCURVEDIALOG_H
