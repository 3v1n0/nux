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


#ifndef UXTHEME_H
#define UXTHEME_H


namespace nux
{

  enum UXStyleImageRef
  {
    eIMAGE_STYLE_NONE = 0,
    eSTROKE_CORNER_SQUARE,
    eSTROKE_CORNER_ROUND1,
    eSTROKE_CORNER_ROUND2,
    eSTROKE_CORNER_ROUND4,
    eSTROKE_CORNER_ROUND10,

    eSHAPE_CORNER_SQUARE,
    eSHAPE_CORNER_ROUND1,
    eSHAPE_CORNER_ROUND2,
    eSHAPE_CORNER_ROUND4,
    eSHAPE_CORNER_ROUND10,

    eSHAPE_CORNER_ROUND10_SHADOW,
    eSHAPE_CORNER_ROUND4_SHADOW,
    eSHAPE_CORNER_SHADOW,

    eTRIANGLE_UP,
    eTRIANGLE_DOWN,
    eTRIANGLE_LEFT,
    eTRIANGLE_RIGHT,

    eWINDOW_SIZEGRIP,

    eSHAPE_CHECK_MARK,
    eSHAPE_CHECK_BOX_FOCUS,
    eSHAPE_CHECK_BOX_PRELIGHT,
    eSHAPE_CHECK_BOX,

    eCHECKBOX_NORMAL_ON,
    eCHECKBOX_NORMAL_OFF,
    eCHECKBOX_FOCUS_ON,
    eCHECKBOX_FOCUS_OFF,
    eCHECKBOX_PRELIGHT_ON,
    eCHECKBOX_PRELIGHT_OFF,

    eRADIO_NORMAL_ON,
    eRADIO_NORMAL_OFF,
    eRADIO_FOCUS_ON,
    eRADIO_FOCUS_OFF,
    eRADIO_PRELIGHT_ON,
    eRADIO_PRELIGHT_OFF,

    eBUTTON_NORMAL,
    eBUTTON_FOCUS,
    eBUTTON_PRELIGHT,

    eSPINNERUP,
    eSPINNERDOWN,

    eCOMBOBOX_OPEN_BUTTON,
    eTAB_LEFT,
    eTAB_RIGHT,
    eSPINER_UP,
    eSPINER_DOWN,
    eTREE_NODE_OPEN,
    eTREE_NODE_CLOSE,

    eSCROLLBAR_TRIANGLE_UP,
    eSCROLLBAR_TRIANGLE_DOWN,
    eSCROLLBAR_TRIANGLE_LEFT,
    eSCROLLBAR_TRIANGLE_RIGHT,

    eVECTORXLABEL,
    eVECTORYLABEL,
    eVECTORZLABEL,
    eVECTORWLABEL,

    eVALUATORVERTICALMOVE,
    eVALUATORHORIZONTALMOVE,
    eVALUATORMOVE,

    eHSCROLLBAR,
    eVSCROLLBAR,

    eMATRIX3PREVIEW,
    eMATRIX4PREVIEW,

    eDOT6x6,

    eGraphIcon,
    eGraphBarIcon,

    eWindowCloseButton,
  };

  struct PainterImage
  {
    BaseTexture *texture;
    UXStyleImageRef style;

    int border_left;
    int border_right;
    int border_top;
    int border_bottom;
    bool draw_borders_only;
  };

  class UXTheme
  {
  public:
    UXTheme();
    ~UXTheme();

    const PainterImage *GetImage (UXStyleImageRef style);
    Rect GetImageGeometry (UXStyleImageRef style);


  private:
    void LoadPainterImages();
    BaseTexture *Load2DTextureFile (const char *filename);
    //BaseTexture *Load2DRectangleTextureFile (const char *filename);
    BaseTexture *Load2DTextureFileGenerateAlpha (const char *filename, int red, int green, int blue);

    std::list<PainterImage *> m_PainterImageList;

  };


}

#endif // UXTHEME_H
