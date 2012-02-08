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
#include "NWin32MessageBox.h"

namespace nux
{
//
// Get system error.
//
  const TCHAR *inlGetSystemErrorMessage ( unsigned int Error )
  {
    static TCHAR Msg[1024];
    *Msg = 0;

    if ( Error == 0 )
      Error = GetLastError();

#if UNICODE
    ANSICHAR ACh[1024];
    FormatMessageA ( FORMAT_MESSAGE_FROM_SYSTEM, NULL, Error, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), ACh, 1024, NULL );
    _TCSCPY_S ( Msg, 1024, ANSI_TO_TCHAR (ACh) );
#else
    FormatMessage ( FORMAT_MESSAGE_FROM_SYSTEM, NULL, Error, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), Msg, 1024, NULL );
#endif

    if (Strchr (Msg, TEXT ('\r') ) )
      *Strchr (Msg, TEXT ('\r') ) = 0;

    if (Strchr (Msg, TEXT ('\n') ) )
      *Strchr (Msg, TEXT ('\n') ) = 0;

    return Msg;
  }

  void inlOKMessageBox ( const TCHAR *Title, const TCHAR *Fmt, ... )
  {
    TCHAR TempStr[4096] = TEXT ("");
    GET_VARARGS ( TempStr, NUX_ARRAY_COUNT (TempStr), NUX_ARRAY_COUNT (TempStr) - 1, Fmt );

    if (GNoDialog == TRUE)
    {
      nuxDebugMsg (TempStr);
    }
    else
    {
      MessageBox (NULL, TempStr, Title, MB_OK | MB_SYSTEMMODAL);
    }
  }

  void inlGetLastErrorMessageBox()
  {
    TCHAR TempStr[4096] = TEXT ("");
    Snprintf ( TempStr, NUX_ARRAY_COUNT (TempStr), NUX_ARRAY_COUNT (TempStr) - 1, TEXT ("GetLastError : %d\n\n%s"), GetLastError(), inlGetSystemErrorMessage() );

    if ( GNoDialog == TRUE )
    {
      nuxError (TempStr);
    }
    else
    {
      MessageBox ( NULL, TempStr, TEXT ("System Error"), MB_OK | MB_SYSTEMMODAL );
    }
  }


//#define VARARG_BODY( FuncRet, FuncName, FmtType, ExtraParamDecl )
  VARARG_BODY (unsigned int, inlWin32MessageBox, const TCHAR *, VARARG_EXTRA (HWND OwnerWindowHandle) VARARG_EXTRA (const TCHAR *Caption) VARARG_EXTRA (MessageBoxType Type) VARARG_EXTRA (MessageBoxIcon Icon) VARARG_EXTRA (MessageBoxModal Modal) )
//BOOL inlWin32MessageBox(HWND OwnerWindowHandle, const TCHAR* Caption, const TCHAR* Message, MessageBoxType Type, unsigned int Modal)
  {
    //    TCHAR TempStr[4096]=TEXT("");
    //    GET_VARARGS( TempStr, NUX_ARRAY_COUNT(TempStr), Fmt, Fmt );

    TCHAR TempStr[4096] = TEXT ("");
    GET_VARARGS ( TempStr, NUX_ARRAY_COUNT (TempStr), NUX_ARRAY_COUNT (TempStr) - 1, Fmt);

    if (GNoDialog == TRUE)
    {
      nuxDebugMsg (TempStr);
      return 0;
    }
    else
    {
      unsigned int Win32Modal = 0;

      switch (Modal)
      {
        case MBMODAL_ApplicationModal:
          Win32Modal = MB_APPLMODAL;
          break;
        case MBMODAL_SystemModal:
          Win32Modal = MBMODAL_SystemModal;
          break;
        case MBMODAL_TaskModal:
          Win32Modal = MBMODAL_TaskModal;
          break;
        default:
          Win32Modal = MB_APPLMODAL;
          break;
      }

      unsigned int Win32Icon = 0;

      switch (Icon)
      {
        case MBICON_Exclamation:
          Win32Icon = MB_ICONEXCLAMATION;
          break;
        case MBICON_Warning:
          Win32Icon = MB_ICONWARNING;
          break;
        case MBICON_Information:
          Win32Icon = MB_ICONINFORMATION;
          break;
        case MBICON_Asterix:
          Win32Icon = MB_ICONASTERISK;
          break;
        case MBICON_Question:
          Win32Icon = MB_ICONQUESTION;
          break;
        case MBICON_Stop:
          Win32Icon = MB_ICONSTOP;
          break;
        case MBICON_Error:
          Win32Icon = MB_ICONERROR;
          break;
        case MBICON_Hand:
          Win32Icon = MB_ICONHAND;
          break;
        default:
          Win32Icon = MB_ICONINFORMATION;
          break;
      }

      int Result = 0;

      switch (Type)
      {
        case MBTYPE_Ok:
          Result = MessageBox ( (HWND) OwnerWindowHandle, TempStr, Caption, MB_OK | Win32Modal | Win32Icon);
          return (Result == IDOK) ? MBRES_Ok : 0;
          break;
        case MBTYPE_YesNo:
          Result = MessageBox ( (HWND) OwnerWindowHandle, TempStr, Caption, MB_YESNO | Win32Modal | Win32Icon);
          return (Result == IDYES) ? MBRES_Yes : MBRES_No;
          break;
        case MBTYPE_OkCancel:
          Result = MessageBox ( (HWND) OwnerWindowHandle, TempStr, Caption, MB_OKCANCEL | Win32Modal | Win32Icon);
          return (Result == IDOK) ? MBRES_Ok : MBRES_Cancel;
          break;
        case MBTYPE_YesNoCancel:
          Result = MessageBox ( (HWND) OwnerWindowHandle, TempStr, Caption, MB_YESNOCANCEL | Win32Modal | Win32Icon);
          return (Result == IDYES) ? MBRES_Yes : (Result == IDNO ? MBRES_No : MBRES_Cancel);
          break;
        case MBTYPE_CancelTryContinue:
          Result = MessageBox ( (HWND) OwnerWindowHandle, TempStr, Caption, MB_CANCELTRYCONTINUE | Win32Modal | Win32Icon);
          return (Result == IDCANCEL) ? MBRES_Cancel : (Result == IDTRYAGAIN ? MBRES_TryAgain : MBRES_Continue);
          break;
        case MBTYPE_AbortRetryIgnore:
          Result = MessageBox ( (HWND) OwnerWindowHandle, TempStr, Caption, MB_ABORTRETRYIGNORE | Win32Modal | Win32Icon);
          return (Result == IDABORT) ? MBRES_Abort : (Result == IDRETRY ? MBRES_Retry : MBRES_Ignore);
          break;
        default:
          Result = MessageBox ( (HWND) OwnerWindowHandle, TempStr, Caption, MB_OK | Win32Modal | Win32Icon);
          return (Result == IDOK) ? MBRES_Ok : 0;
          break;
      }
    }

    return 0;
  }

}

