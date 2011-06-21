/*
 * Copyright 2010 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef HUERANGEVALUATOR_H
#define HUERANGEVALUATOR_H


#if defined(NUX_OS_WINDOWS)
#include "NuxGraphics/Events.h"
#elif defined(NUX_OS_LINUX)
#include "NuxGraphics/Events.h"
#endif
#include "EditTextBox.h"
#include "StaticTextBox.h"
#include "HLayout.h"

namespace nux
{

  class HueRangeValuator : public RangeValue
  {
  public:
    HueRangeValuator (float value = 0.0f, float vmin = 0, float vmax = 1.0f);
    ~HueRangeValuator();

    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    void SetHLSLight (float light)
    {
      m_HLSLight = light;
    }
    void SetHSVValue (float value)
    {
      m_HSVValue = value;
    }
    void SetHLSSaturation (float saturation)
    {
      m_HLSSaturation = saturation;
    }
    void SetHSVSaturation (float saturation)
    {
      m_HSVSaturation = saturation;
    }
    void ModelHSV()
    {
      m_Model = color::HSV;
    }
    void ModelHLS()
    {
      m_Model = color::HLS;
    }
    color::Model GetModel() const
    {
      return m_Model;
    }

  protected:
    void InitializeWidgets();
    void InitializeLayout();

  private:
    float m_HLSSaturation;
    float m_HLSLight;
    float m_HSVSaturation;
    float m_HSVValue;

    color::Model m_Model;
  };

}

#endif // HUERANGEVALUATOR_H


