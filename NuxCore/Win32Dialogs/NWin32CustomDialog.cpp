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


#include "NuxCore.h"

namespace nux
{

  static enum eFileDialogType
  {
    FILE_OPEN_DIALOG,
    FILE_SAVE_DIALOG,
  };

  ColorDialogOption::ColorDialogOption()
    : m_ReturnColor (0, 0, 0, 0)
  {
    for (unsigned int i = 0; i < NUX_COLOR_DIALOG_CUSTOM_COLOR; i++)
    {
      m_CustomColors[i] = (COLORREF) (0xFFFFFF);
    }
  }

  ColorDialogOption::~ColorDialogOption()
  {

  }

  void ColorDialogOption::SetCustomColor (unsigned int index, unsigned int RGBColor)
  {
    nuxAssert (index < NUX_COLOR_DIALOG_CUSTOM_COLOR);
    m_CustomColors[index] = (COLORREF) RGBColor;
  }

  void ColorDialogOption::SetCustomColor (unsigned int index, BYTE R, BYTE G, BYTE B)
  {
    nuxAssert (index < NUX_COLOR_DIALOG_CUSTOM_COLOR);
    m_CustomColors[index] = (COLORREF) ((R<<16)|(G<<8)|B);
  }


#define MAX_FILE_CHARACTERS 2*260

  FileDialogOption::FileDialogOption()
    : NumFilters (0)
    , FormattedFilter (0)
  {
    DialogTitle = TEXT ("\0");
    InitialDirectory = TEXT ("\0");
  }

  FileDialogOption::~FileDialogOption()
  {

  }

  void FileDialogOption::AddFilter (const TCHAR *Description, const TCHAR *Filter)
  {
    nuxAssert (Description);
    nuxAssert (Filter);

    FilterDesc.push_back(std::string(Description));
    Filters.push_back(std::string(Filter));
    NumFilters++;
  }

  void FileDialogOption::RemoveFilter()
  {
    FilterDesc.clear();
    Filters.clear();
    NumFilters = 0;

    if (FormattedFilter)
    {
      delete [] FormattedFilter;
      FormattedFilter = 0;
    }
  }

  TCHAR *FileDialogOption::GetFormatedFilter()
  {
    size_t size = 0;

    for (unsigned int i = 0; i < NumFilters; i++)
    {
      size += FilterDesc[i].length() + Filters[i].length() + 2; // + 2 for for the NULL char at the end of each string
    }

    size += 1;  // + 1 for the final NULL char. The total string is terminated by two NULL char

    if (FormattedFilter)
      delete [] FormattedFilter;

    FormattedFilter = new TCHAR[size];
    size_t l = 0;

    for (unsigned int i = 0; i < NumFilters; i++)
    {
      Memcpy((void *) (FormattedFilter + l), FilterDesc[i].c_str(), FilterDesc[i].length());
      l += FilterDesc[i].length();
      FormattedFilter[l] = 0;
      l++;
      Memcpy ( (void *) (FormattedFilter + l), Filters[i].c_str(), Filters[i].length() );
      l += Filters[i].length();
      FormattedFilter[l] = 0;
      l++;
    }

    FormattedFilter[l] = 0;
    l++;
    nuxAssert (size == l);

    return FormattedFilter;
  }

  void FileDialogOption::SetDialogTitle (const TCHAR *Title)
  {
    DialogTitle = Title;
  }
  void FileDialogOption::SetInitialDirectory (const TCHAR *Directory)
  {
    InitialDirectory = Directory;
  }

  bool Win32ColorDialog (HWND hWnd, ColorDialogOption &cdo)
  {
    CHOOSECOLOR ColorOptions;

    Memset (&ColorOptions, 0, sizeof (CHOOSECOLOR) );
    ColorOptions.lStructSize = sizeof (CHOOSECOLOR);
    ColorOptions.hwndOwner = hWnd;
    ColorOptions.hInstance = 0;
    ColorOptions.lpCustColors = cdo.m_CustomColors;
    ColorOptions.Flags = CC_ANYCOLOR | CC_FULLOPEN;

    if ( ChooseColor (&ColorOptions) )
    {
      cdo.m_ReturnColor = Color (GetRValue (ColorOptions.rgbResult), GetGValue (ColorOptions.rgbResult), GetBValue (ColorOptions.rgbResult) );
      return TRUE;
    };

    return FALSE;
  }


  static bool _Win32FileDialog (HWND hWnd, eFileDialogType DialogType, FileDialogOption &fdo)
  {
    nuxAssert ( (DialogType == FILE_OPEN_DIALOG) || (DialogType == FILE_SAVE_DIALOG) );

    OPENFILENAME SaveFileOption;

    TCHAR DrivePathFileName[MAX_FILE_CHARACTERS];       // buffer for OpenFileName.lpstrFile
    TCHAR FileName[MAX_FILE_CHARACTERS];                // buffer for OpenFileName.lpstrFileTitle

    DrivePathFileName[0] = 0;
    Memset (&SaveFileOption, 0, sizeof (OPENFILENAME) );
    SaveFileOption.lStructSize = sizeof (OPENFILENAME);
    // (See remark at the end of this file!)
    SaveFileOption.hwndOwner = hWnd;

    SaveFileOption.lpstrFilter = fdo.GetFormatedFilter();
    SaveFileOption.nFilterIndex = 0;

    // On return, this buffer contains the drive designator, path, file name, and extension of the selected file.
    SaveFileOption.lpstrFile = DrivePathFileName;
    SaveFileOption.nMaxFile = MAX_FILE_CHARACTERS;

    // On return, this buffer receives the file name and extension (without path information) of the selected file.
    SaveFileOption.lpstrFileTitle = FileName;
    SaveFileOption.nMaxFileTitle = MAX_FILE_CHARACTERS;

    SaveFileOption.lpstrInitialDir = fdo.GetInitialDirectory();

    SaveFileOption.lpstrTitle = fdo.GetDialogTitle();

    SaveFileOption.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    SaveFileOption.lpstrDefExt = TEXT ("");


    if (DialogType == FILE_OPEN_DIALOG)
    {
      if (::GetOpenFileName (&SaveFileOption) )
      {
        fdo.ReturnFileName = NFileName (SaveFileOption.lpstrFile);
        return TRUE;
      }
    }

    if (DialogType == FILE_SAVE_DIALOG)
    {
      if (::GetSaveFileName (&SaveFileOption) )
      {
        fdo.ReturnFileName = NFileName (SaveFileOption.lpstrFile);
        return TRUE;
      }
    }

    unsigned int error = ::CommDlgExtendedError();
    return FALSE;
  }

  bool Win32SaveFileDialog (HWND hWnd, FileDialogOption &fdo)
  {
    return (bool) _Win32FileDialog (hWnd, FILE_SAVE_DIALOG, fdo);
  }

  bool Win32OpenFileDialog (HWND hWnd, FileDialogOption &fdo)
  {
    return (bool) _Win32FileDialog (hWnd, FILE_OPEN_DIALOG, fdo);
  }

}

