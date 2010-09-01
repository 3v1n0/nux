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


#ifndef HLAYOUT_H
#define HLAYOUT_H

#include "Layout.h"

NAMESPACE_BEGIN_GUI

class HLayout: public Layout
{
    DECLARE_OBJECT_TYPE(HLayout, Layout);
public: 
    HLayout();
    HLayout(std::string name);
    ~HLayout();

    virtual long ComputeLayout2();
    virtual void ComputePosition2(float offsetX, float offsetY);
    virtual void HLayoutManagement(t_s32 width, t_s32 height);
    virtual t_u32 getMaxStretchFactor();

    virtual void Draw();


    virtual t_s32 GetBaseWidth() const {return BaseObject::GetBaseWidth();}
    virtual void SetBaseWidth(t_s32 w) {BaseObject::SetBaseWidth(w);}

    virtual t_s32 GetBaseHeight() const {return BaseObject::GetBaseHeight();}
    virtual void SetBaseHeight(t_s32 h) {BaseObject::SetBaseHeight(h);}

    virtual t_s32 GetBaseX() const {return BaseObject::GetBaseX();}
    virtual void SetBaseX(t_s32 x) {BaseObject::SetBaseX(x);}
    virtual t_s32 GetBaseY() const {return BaseObject::GetBaseY();}
    virtual void SetBaseY(t_s32 y) {BaseObject::SetBaseY(y);}

    virtual Size GetMaximumSize() {return BaseObject::GetMaximumSize();}
    virtual Size GetMinimumSize() {return BaseObject::GetMinimumSize();}

    virtual void ApplyMinWidth() {BaseObject::ApplyMinWidth();}
    virtual void ApplyMinHeight() {BaseObject::ApplyMinHeight();}
    virtual void ApplyMaxWidth() {BaseObject::ApplyMaxWidth();}
    virtual void ApplyMaxHeight() {BaseObject::ApplyMaxHeight();}

    //virtual long ComputeLayout2(){return m_layout->ComputeLayout2();}
    //virtual void ComputePosition2() {m_layout->ComputePosition2();}

    virtual void GetCompositeList(std::list<smptr(BaseObject)> *InterfaceControlList);

    //! Compute the how elements are spread inside the layout
    /*!
        @param remaining_width  Size that remains after subtracting elements width, inner and outer margins from the content width.
        @param offset_space     The space at the left of all elements.
        @param element_margin   The margin between elements.
    */
    void ComputeStacking(t_s32 remaining_width, t_s32 &offset_space, t_s32 &element_margin);
};

NAMESPACE_END_GUI

#endif // HLAYOUT_H
