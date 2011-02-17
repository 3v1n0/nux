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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "NuxCore/NuxCore.h"
#include "GLResource.h"
#include "IniFile.h"
#include "UIColorTheme.h"

namespace nux
{

  UIColorThemeImpl *UIColorThemeImpl::m_pInstance = 0;

  static bool IsHexadecimalNumber (const char *s)
  {
    if (s == 0)
      return false;

    const char *HexStr = s;

    if ( (s[0] == '0') && ( (s[1] == 'X') || (s[1] == 'x') ) )
    {
      HexStr = s + 2;
    }

    int n = 0;

    if (HexStr[0] == 0)
      return false;

    while (HexStr[n] != '\0')
    {
      if (HexStr[n] > 0x29 && HexStr[n] < 0x40 ) //if 0 to 9
      {
      }
      else if (HexStr[n] >= 'a' && HexStr[n] <= 'f') //if a to f
      {
      }
      else if (HexStr[n] >= 'A' && HexStr[n] <= 'F') //if A to F
      {
      }
      else
        return false;

      n++;
    }

    return true;
  }

  static unsigned long axtoi (const char *s)
  {
    int n = 0;         // position in string
    int m = 0;         // position in digit[] to shift
    int count;         // loop index
    unsigned long intValue = 0;  // integer value of hex string
    int digit[16];      // hold values to convert

    const char *hexStg = s;

    if ( (s[0] == '0') && ( (s[1] == 'X') || (s[1] == 'x') ) )
    {
      hexStg = s + 2;
    }

    while (n < 16)
    {
      if (hexStg[n] == '\0')
        break;

      if (hexStg[n] > 0x29 && hexStg[n] < 0x40 ) //if 0 to 9
      {
        digit[n] = hexStg[n] & 0x0f;            //convert to int
      }
      else if (hexStg[n] >= 'a' && hexStg[n] <= 'f') //if a to f
      {
        digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
      }
      else if (hexStg[n] >= 'A' && hexStg[n] <= 'F') //if A to F
      {
        digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
      }
      else
        break;

      n++;
    }

    count = n;
    m = n - 1;
    n = 0;

    while (n < count)
    {
      // digit[n] is value of hex digit at position n
      // (m << 2) is the number of positions to shift
      // OR the bits into return value
      intValue = intValue | (digit[n] << (m << 2) );
      m--;   // adjust the position to set
      n++;   // next digit to process
    }

    return (intValue);
  }


  void ReadColorFromINI (std::string section_name, std::string key_name, Color &color)
  {
    FilePath m_FilePath;
    m_FilePath.AddSearchPath (""); // for case where fully qualified path is given
    m_FilePath.AddSearchPath ("./ini");
    m_FilePath.AddSearchPath ("../ini");
    m_FilePath.AddSearchPath ("../../ini");
    m_FilePath.AddSearchPath ("../../../ini");

    NString file_search = "inalogic.ini";
    NString FileName = m_FilePath.GetFile (file_search.GetTCharPtr() );

    if (FileName == "")
    {
      std::cout << "Can't find file inalogic.ini. Using default options..." << std::endl;
    }

    if (FileName != "")
    {
      if (CIniFile::RecordExists (key_name, section_name, FileName.GetTCharPtr() ) == false)
      {
        std::cout << "INI key " << "\\" << section_name << "\\" << key_name << " does not exist" << std::endl;
      }
      else
      {
        std::string value = CIniFile::GetValue (key_name, section_name, FileName.GetTCharPtr() );
        const char *s = value.c_str();

        if (IsHexadecimalNumber (s) )
        {
          color = Color (axtoi (s) );
        }
      }
    }
  }

  UIColorThemeImpl::UIColorThemeImpl()
  {
    mUI_BackgroundColor0 = Color (0xFF4D4D4D);
    mUI_BackgroundColor1 = Color (0xFF868686);

    mUI_ForegroundColor0 = Color (0xFF424242);
    mUI_ForegroundColor1 = Color (0xFFEFEFEF);

    mUI_TextColor = Color (0xFF000000);
    mUI_DisabledText_Color = Color (0xFFAAAAAA);
    mUI_EditableText_Color = Color (0xFF000000);

    mUI_EditableText_BackgroundColor = Color (0xFFFFFFFF);
    mUI_DisableEditableText_BackgroundColor = Color (0xFFEEEEEE);
  }

  UIColorThemeImpl &UIColorThemeImpl::Instance()
  {
    if (m_pInstance == 0)
    {
      m_pInstance = new UIColorThemeImpl();
    }

    return *m_pInstance;
  }

  void UIColorThemeImpl::Initialize()
  {
    ReadColorFromINI ("UIColors", "BackgroundColor0", mUI_BackgroundColor0);
    ReadColorFromINI ("UIColors", "BackgroundColor1", mUI_BackgroundColor1);

    ReadColorFromINI ("UIColors", "ForegroundColor0", mUI_ForegroundColor0);
    ReadColorFromINI ("UIColors", "ForegroundColor1", mUI_ForegroundColor1);
  }

}
