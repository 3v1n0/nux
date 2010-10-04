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


#ifndef VLAYOUT_H
#define VLAYOUT_H

#include "Layout.h"

namespace nux { //NUX_NAMESPACE_BEGIN

class VLayout: public Layout
{
    NUX_DECLARE_OBJECT_TYPE(VLayout, Layout);
public: 
    VLayout(NUX_FILE_LINE_PROTO);
    VLayout(NString name, NUX_FILE_LINE_PROTO);
    ~VLayout();

    virtual long ComputeLayout2();
    virtual void ComputePosition2(float offsetX, float offsetY);
    virtual void VLayoutManagement(t_s32 width, t_s32 height);
    virtual t_u32 getMaxStretchFactor();

    virtual void Draw();


    virtual t_s32 GetBaseWidth() const {return Area::GetBaseWidth();}
    virtual void SetBaseWidth(t_s32 w) {Area::SetBaseWidth(w);}

    virtual t_s32 GetBaseHeight() const {return Area::GetBaseHeight();}
    virtual void SetBaseHeight(t_s32 h) {Area::SetBaseHeight(h);}

    virtual t_s32 GetBaseX() const {return Area::GetBaseX();}
    virtual void SetBaseX(t_s32 x) {Area::SetBaseX(x);}
    virtual t_s32 GetBaseY() const {return Area::GetBaseY();}
    virtual void SetBaseY(t_s32 y) {Area::SetBaseY(y);}

    virtual Size GetMaximumSize() {return Area::GetMaximumSize();}
    virtual Size GetMinimumSize() {return Area::GetMinimumSize();}

    virtual void ApplyMinWidth() {Area::ApplyMinWidth();}
    virtual void ApplyMinHeight() {Area::ApplyMinHeight();}
    virtual void ApplyMaxWidth() {Area::ApplyMaxWidth();}
    virtual void ApplyMaxHeight() {Area::ApplyMaxHeight();}

    //virtual long ComputeLayout2(){return m_layout->ComputeLayout2();}
    //virtual void ComputePosition2() {m_layout->ComputePosition2();}

    virtual void GetCompositeList(std::list<Area*> *ViewList);
    
    //! Compute the how elements are spread inside the layout
    /*!
        @param remaining_height Size that remains after subtracting elements height, inner and outer margins from the content height.
        @param offset_space     The space at the top of all elements.
        @param element_margin   The margin between elements.
    */
    void ComputeStacking(t_s32 length, t_s32 &offset_space, t_s32 &element_margin);
};

} //NUX_NAMESPACE_END

#endif // VLAYOUT_H
