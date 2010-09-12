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


#ifndef NWIN32CUSTOMDIALOG_H
#define NWIN32CUSTOMDIALOG_H


#define NUX_COLOR_DIALOG_CUSTOM_COLOR 16

NAMESPACE_BEGIN

class FileDialogOption
{
public:
    FileDialogOption();
    ~FileDialogOption();

    void    AddFilter(const TCHAR* Description, const TCHAR* Filter);
    TCHAR*  GetFormatedFilter();
    void    RemoveFilter();

    void            SetDialogTitle(const TCHAR* Title);
    const TCHAR*    GetDialogTitle() { return *DialogTitle; }
    void            SetInitialDirectory(const TCHAR* Directory);
    const TCHAR*    GetInitialDirectory() { return *InitialDirectory; }


public:
    NFileName ReturnFileName;
    std::vector<NString> FilterDesc;
    std::vector<NString> Filters;
    TCHAR* FormattedFilter;
    t_u32 FilterLength;
    t_u32 NumFilters;
    NString DialogTitle;
    NString InitialDirectory;
};


class ColorDialogOption
{
public:
    ColorDialogOption();
    ~ColorDialogOption();

    void SetCustomColor(t_u32 index, t_u32 RGBColor);
    void SetCustomColor(t_u32 index, BYTE R, BYTE G, BYTE B);


public:
    COLORREF m_CustomColors[NUX_COLOR_DIALOG_CUSTOM_COLOR];
    Color m_ReturnColor;
};


bool Win32ColorDialog(HWND hWnd, ColorDialogOption& cdo);

//! Open Win32 file Dialog
/*! 
    Open Win32 file Dialog
    Adding filter options
        FileDialogOption fdo;
        fdo.AddFilter(TEXT("All Files (*.*)"), TEXT("*.*"));
        fdo.AddFilter(TEXT("All Text (*.txt)"), TEXT("*.txt"));
        fdo.AddFilter(TEXT("All Images (*.bmp, *.tga, *.jpeg, *.png)"), TEXT("*.bmp;*.tga;*.jpeg;*.png"));
*/
bool Win32SaveFileDialog(HWND hWnd, FileDialogOption& fdo);
bool Win32OpenFileDialog(HWND hWnd, FileDialogOption& fdo);

NAMESPACE_END

#endif // NWIN32CUSTOMDIALOG_H
