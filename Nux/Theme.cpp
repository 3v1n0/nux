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
#include "NuxMesh/NTextureArchiveManager.h"
#include "NuxCore/TinyXML/tinyxml.h"
#include "Theme.h"


namespace nux
{

#define INT_TO_ENUM_ELEMENT(a) {a, #a} // --->>> {a, "a"}

  typedef struct
  {
    UXStyleImageRef value;
    const char *style;
  } UXStyle;

  UXStyle UXStyleArray [] =
  {
    INT_TO_ENUM_ELEMENT (eIMAGE_STYLE_NONE),
    INT_TO_ENUM_ELEMENT (eSTROKE_CORNER_SQUARE),
    INT_TO_ENUM_ELEMENT (eSTROKE_CORNER_ROUND1),
    INT_TO_ENUM_ELEMENT (eSTROKE_CORNER_ROUND2),
    INT_TO_ENUM_ELEMENT (eSTROKE_CORNER_ROUND4),
    INT_TO_ENUM_ELEMENT (eSTROKE_CORNER_ROUND10),

    INT_TO_ENUM_ELEMENT (eSHAPE_CORNER_ROUND1),
    INT_TO_ENUM_ELEMENT (eSHAPE_CORNER_ROUND2),
    INT_TO_ENUM_ELEMENT (eSHAPE_CORNER_ROUND4),
    INT_TO_ENUM_ELEMENT (eSHAPE_CORNER_ROUND10),

    INT_TO_ENUM_ELEMENT (eSHAPE_CORNER_ROUND10_SHADOW),
    INT_TO_ENUM_ELEMENT (eSHAPE_CORNER_ROUND4_SHADOW),
    INT_TO_ENUM_ELEMENT (eSHAPE_CORNER_SHADOW),

    INT_TO_ENUM_ELEMENT (eTRIANGLE_UP),
    INT_TO_ENUM_ELEMENT (eTRIANGLE_DOWN),
    INT_TO_ENUM_ELEMENT (eTRIANGLE_LEFT),
    INT_TO_ENUM_ELEMENT (eTRIANGLE_RIGHT),

    INT_TO_ENUM_ELEMENT (eWINDOW_SIZEGRIP),

    INT_TO_ENUM_ELEMENT (eSHAPE_CHECK_MARK),
    INT_TO_ENUM_ELEMENT (eSHAPE_CHECK_BOX),

    INT_TO_ENUM_ELEMENT (eCHECKBOX_NORMAL_ON),
    INT_TO_ENUM_ELEMENT (eCHECKBOX_NORMAL_OFF),
    INT_TO_ENUM_ELEMENT (eCHECKBOX_FOCUS_ON),
    INT_TO_ENUM_ELEMENT (eCHECKBOX_FOCUS_OFF),
    INT_TO_ENUM_ELEMENT (eCHECKBOX_PRELIGHT_ON),
    INT_TO_ENUM_ELEMENT (eCHECKBOX_PRELIGHT_OFF),

    INT_TO_ENUM_ELEMENT (eRADIO_NORMAL_ON),
    INT_TO_ENUM_ELEMENT (eRADIO_NORMAL_OFF),
    INT_TO_ENUM_ELEMENT (eRADIO_FOCUS_ON),
    INT_TO_ENUM_ELEMENT (eRADIO_FOCUS_OFF),
    INT_TO_ENUM_ELEMENT (eRADIO_PRELIGHT_ON),
    INT_TO_ENUM_ELEMENT (eRADIO_PRELIGHT_OFF),

    INT_TO_ENUM_ELEMENT (eBUTTON_NORMAL),
    INT_TO_ENUM_ELEMENT (eBUTTON_FOCUS),
    INT_TO_ENUM_ELEMENT (eBUTTON_PRELIGHT),
    INT_TO_ENUM_ELEMENT (eSPINNERUP),
    INT_TO_ENUM_ELEMENT (eSPINNERDOWN),

    INT_TO_ENUM_ELEMENT (eCOMBOBOX_OPEN_BUTTON),
    INT_TO_ENUM_ELEMENT (eTAB_LEFT),
    INT_TO_ENUM_ELEMENT (eTAB_RIGHT),
    INT_TO_ENUM_ELEMENT (eSPINER_UP),
    INT_TO_ENUM_ELEMENT (eSPINER_DOWN),
    INT_TO_ENUM_ELEMENT (eTREE_NODE_OPEN),
    INT_TO_ENUM_ELEMENT (eTREE_NODE_CLOSE),

    INT_TO_ENUM_ELEMENT (eSCROLLBAR_TRIANGLE_UP),
    INT_TO_ENUM_ELEMENT (eSCROLLBAR_TRIANGLE_DOWN),
    INT_TO_ENUM_ELEMENT (eSCROLLBAR_TRIANGLE_LEFT),
    INT_TO_ENUM_ELEMENT (eSCROLLBAR_TRIANGLE_RIGHT),

    INT_TO_ENUM_ELEMENT (eVALUATORVERTICALMOVE),
    INT_TO_ENUM_ELEMENT (eVALUATORHORIZONTALMOVE),
    INT_TO_ENUM_ELEMENT (eVALUATORMOVE),

    INT_TO_ENUM_ELEMENT (eVECTORXLABEL),
    INT_TO_ENUM_ELEMENT (eVECTORYLABEL),
    INT_TO_ENUM_ELEMENT (eVECTORZLABEL),
    INT_TO_ENUM_ELEMENT (eVECTORWLABEL),


    INT_TO_ENUM_ELEMENT (eHSCROLLBAR),
    INT_TO_ENUM_ELEMENT (eVSCROLLBAR),

    INT_TO_ENUM_ELEMENT (eMATRIX3PREVIEW),
    INT_TO_ENUM_ELEMENT (eMATRIX4PREVIEW),
    INT_TO_ENUM_ELEMENT (eDOT6x6),

    INT_TO_ENUM_ELEMENT (eGraphIcon),
    INT_TO_ENUM_ELEMENT (eGraphBarIcon),

    INT_TO_ENUM_ELEMENT (eWindowCloseButton),

    {eIMAGE_STYLE_NONE, 0}
  };


  /*static unsigned long axtoi(const char *s)
  {
      int n = 0;         // position in string
      int m = 0;         // position in digit[] to shift
      int count;         // loop index
      unsigned long intValue = 0;  // integer value of hex string
      int digit[16];      // hold values to convert

      const char *hexStg = s;
      if((s[0] == '0') && ((s[1] == 'X') || (s[1] == 'x')))
      {
          hexStg = s+2;
      }

      while (n < 16)
      {
          if (hexStg[n]=='\0')
              break;
          if (hexStg[n] > 0x29 && hexStg[n] < 0x40 ) //if 0 to 9
              digit[n] = hexStg[n] & 0x0f;            //convert to int
          else if (hexStg[n] >='a' && hexStg[n] <= 'f') //if a to f
              digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
          else if (hexStg[n] >='A' && hexStg[n] <= 'F') //if A to F
              digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
          else break;
          n++;
      }
      count = n;
      m = n - 1;
      n = 0;
      while(n < count)
      {
          // digit[n] is value of hex digit at position n
          // (m << 2) is the number of positions to shift
          // OR the bits into return value
          intValue = intValue | (digit[n] << (m << 2));
          m--;   // adjust the position to set
          n++;   // next digit to process
      }
      return (intValue);
  }*/

  /*static unsigned int ReadXMLColorAttribute(TiXmlElement* element, const char* attribute_name)
  {
      unsigned int retvalue = 0;
      if(element)
      {
          retvalue = axtoi(element->Attribute(attribute_name));
      }
      return retvalue;
  }*/

  static UXStyleImageRef GetStyleImageRef (const TCHAR *style_name)
  {
    int i = 0;

    while (UXStyleArray[i].style != 0)
    {
      if (strcmp (UXStyleArray[i].style, style_name) == 0)
      {
        return UXStyleArray[i].value;
      }

      i++;
    }

    return eIMAGE_STYLE_NONE;
  }

  UXTheme::UXTheme()
  {
    LoadPainterImages();
  }

  UXTheme::~UXTheme()
  {
    std::list<PainterImage *>::iterator it;
    for (it = m_PainterImageList.begin(); it != m_PainterImageList.end(); it++)
    {
      (*it)->texture->UnReference ();
    }
    m_PainterImageList.clear ();
  }

  void UXTheme::LoadPainterImages()
  {
    NString file_search = TEXT ("Painter.xml");
    NString painter_filename = NUX_FIND_RESOURCE_LOCATION_NOFAIL (file_search.GetTCharPtr() );

    if (painter_filename == TEXT ("") )
    {
      nuxDebugMsg (TEXT ("[GraphicsEngine::LoadPainterImages] Can't find Painter.xml file.") );
      /*inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal,
          TEXT("Can't find User Interface xml file.\nThe program will exit."));
      exit(-1);*/
    }

    NUITextureArchiver UITextureArchive (NUX_FIND_RESOURCE_LOCATION_NOFAIL (TEXT ("UITextures/UIArchive.iar") ) );

    TiXmlDocument doc (painter_filename.GetTCharPtr() );
    doc.LoadFile();

    TiXmlHandle docHandle ( &doc );
    TiXmlElement *data = docHandle.FirstChild (TCHARToUTF8 (TEXT ("PaintData") ) ).Element();
    TiXmlElement *image = 0;

    for (image = data->FirstChildElement (TCHARToUTF8 (TEXT ("Image") ) ); image; image = image->NextSiblingElement (TCHARToUTF8 (TEXT ("Image") ) ) )
    {
      PainterImage *pimage = new PainterImage;
      Memset (pimage, 0, sizeof (PainterImage) );

      NString style = image->Attribute (TCHARToUTF8 (TEXT ("style") ) );

      pimage->style = GetStyleImageRef (style.GetTCharPtr() );

      // If the attributes border_left, border_right, border_top, border_bottom are not present, assume they are equal to 0;
      pimage->border_left = pimage->border_right = pimage->border_top = pimage->border_bottom = 0;

      image->Attribute (TCHARToUTF8 (TEXT ("border_left") ), &pimage->border_left);
      image->Attribute (TCHARToUTF8 (TEXT ("border_right") ), &pimage->border_right);
      image->Attribute (TCHARToUTF8 (TEXT ("border_top") ),  &pimage->border_top);
      image->Attribute (TCHARToUTF8 (TEXT ("border_bottom") ), &pimage->border_bottom);


      const char *draw_borders_only = image->Attribute (TCHARToUTF8 (TEXT ("border_only") ) );

      if (draw_borders_only == 0)
      {
        pimage->draw_borders_only = true;
      }
      else
      {
        if (strcmp (TCHARToUTF8 (TEXT ("false") ), draw_borders_only) == 0)
        {
          pimage->draw_borders_only = false;
        }
        else
        {
          pimage->draw_borders_only = true;
        }
      }

      if (1)
      {
        NBitmapData *uitexturedata = UITextureArchive.ExtractTextureStyle (style.GetTCharPtr() );
        BaseTexture* RectangleTexture = GetThreadGLDeviceFactory()->CreateSystemCapableTexture ();

        if (uitexturedata)
          RectangleTexture->Update (uitexturedata);
        else
        {
          NString filename = image->Attribute (TCHARToUTF8 (TEXT ("Name") ) );
          NString texture_filename = NUX_FIND_RESOURCE_LOCATION_NOFAIL (filename.GetTCharPtr() );
          RectangleTexture = Load2DTextureFile (texture_filename.GetTCharPtr() );
        }

        pimage->texture = RectangleTexture;
      }
      else
      {
        NString filename = image->Attribute (TCHARToUTF8 (TEXT ("Name") ) );
        NString texture_filename = NUX_FIND_RESOURCE_LOCATION_NOFAIL (filename.GetTCharPtr() );
        pimage->texture = Load2DTextureFile (texture_filename.GetTCharPtr() );
      }

      m_PainterImageList.push_back (pimage);
    }
  }

  const PainterImage *UXTheme::GetImage (UXStyleImageRef style)
  {
    std::list<PainterImage *>::iterator it;

    for (it = m_PainterImageList.begin(); it != m_PainterImageList.end(); it++)
    {
      if ( (*it)->style == style)
      {
        return (*it);
      }
    }

    return 0;
  }

  Rect UXTheme::GetImageGeometry (UXStyleImageRef style)
  {
    std::list<PainterImage *>::iterator it;

    for (it = m_PainterImageList.begin(); it != m_PainterImageList.end(); it++)
    {
      if ( (*it)->style == style)
      {
        t_u32 width = (*it)->texture->GetWidth();
        t_u32 height = (*it)->texture->GetHeight();
        return Rect (0, 0, width, height);
      }
    }

    nuxDebugMsg (TEXT ("[GraphicsEngine::GetImageGeometry] Cannot find UXStyleImageRef") );
    return Rect (0, 0, 0, 0);
  }

  BaseTexture *UXTheme::Load2DTextureFile (const char *filename)
  {
    BaseTexture* texture2D = GetThreadGLDeviceFactory()->CreateSystemCapableTexture ();
    NBitmapData *BitmapData = LoadImageFile (filename);

    if (BitmapData)
      texture2D->Update (BitmapData);

    return texture2D;
  }

//   BaseTexture *UXTheme::Load2DRectangleTextureFile (const char *filename)
//   {
//     TextureRectangle *RectangleTexture = new TextureRectangle();
//     NBitmapData *BitmapData = LoadImageFile (filename);
// 
//     if (BitmapData)
//       RectangleTexture->Update (BitmapData);
// 
// 
//     return RectangleTexture;
//   }

  BaseTexture *UXTheme::Load2DTextureFileGenerateAlpha (const char *filename, int red, int green, int blue)
  {
    return 0;
  }

}

