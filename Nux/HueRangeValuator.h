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


#ifndef HUERANGEVALUATOR_H
#define HUERANGEVALUATOR_H


#if defined(NUX_OS_WINDOWS)
    #include "NuxGraphics/Gfx_Events.h"
#elif defined(NUX_OS_LINUX)
    #include "NuxGraphics/GfxEventsX11.h"
#endif
#include "EditTextBox.h"
#include "StaticTextBox.h"
#include "HLayout.h"

namespace nux { //NUX_NAMESPACE_BEGIN
 
class HueRangeValuator : public RangeValue
{
public:
    HueRangeValuator(float value = 0.0f, float vmin = 0, float vmax = 1.0f);
    ~HueRangeValuator();

    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    void SetHLSLight(float light){m_HLSLight = light;}
    void SetHSVValue(float value){m_HSVValue = value;}
    void SetHLSSaturation(float saturation){m_HLSSaturation = saturation;}
    void SetHSVSaturation(float saturation){m_HSVSaturation = saturation;}
    void ModelHSV(){m_Model = CM_HSV;}
    void ModelHLS(){m_Model = CM_HLS;}
    eColorModel GetModel() const {return m_Model;}

protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

private:
    float m_HLSSaturation;
    float m_HLSLight;
    float m_HSVSaturation;
    float m_HSVValue;

    eColorModel m_Model;
};

} //NUX_NAMESPACE_END

#endif // HUERANGEVALUATOR_H


