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


#ifndef NWIN32CUSTOMDIALOG_H
#define NWIN32CUSTOMDIALOG_H


#define NUX_COLOR_DIALOG_CUSTOM_COLOR 16

namespace nux
{

  class FileDialogOption
  {
  public:
    FileDialogOption();
    ~FileDialogOption();

    void    AddFilter (const TCHAR *Description, const TCHAR *Filter);
    TCHAR  *GetFormatedFilter();
    void    RemoveFilter();

    void            SetDialogTitle (const TCHAR *Title);
    const TCHAR    *GetDialogTitle()
    {
      return DialogTitle.c_str();
    }
    void            SetInitialDirectory (const TCHAR *Directory);
    const TCHAR    *GetInitialDirectory()
    {
      return InitialDirectory.c_str();
    }


  public:
    NFileName ReturnFileName;
    std::vector<std::string> FilterDesc;
    std::vector<std::string> Filters;
    TCHAR *FormattedFilter;
    unsigned int FilterLength;
    unsigned int NumFilters;
    std::string DialogTitle;
    std::string InitialDirectory;
  };


  class ColorDialogOption
  {
  public:
    ColorDialogOption();
    ~ColorDialogOption();

    void SetCustomColor (unsigned int index, unsigned int RGBColor);
    void SetCustomColor (unsigned int index, BYTE R, BYTE G, BYTE B);


  public:
    COLORREF m_CustomColors[NUX_COLOR_DIALOG_CUSTOM_COLOR];
    Color m_ReturnColor;
  };


  bool Win32ColorDialog (HWND hWnd, ColorDialogOption &cdo);

//! Open Win32 file Dialog
  /*!
      Open Win32 file Dialog
      Adding filter options
          FileDialogOption fdo;
          fdo.AddFilter(TEXT("All Files (*.*)"), TEXT("*.*"));
          fdo.AddFilter(TEXT("All Text (*.txt)"), TEXT("*.txt"));
          fdo.AddFilter(TEXT("All Images (*.bmp, *.tga, *.jpeg, *.png)"), TEXT("*.bmp;*.tga;*.jpeg;*.png"));
  */
  bool Win32SaveFileDialog (HWND hWnd, FileDialogOption &fdo);
  bool Win32OpenFileDialog (HWND hWnd, FileDialogOption &fdo);

}

#endif // NWIN32CUSTOMDIALOG_H
