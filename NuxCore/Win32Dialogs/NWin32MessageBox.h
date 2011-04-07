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


#ifndef NWIN32MESSAGEBOX_H
#define NWIN32MESSAGEBOX_H

namespace nux
{

// Returns the last system error code in string form.  NOTE: Only one return value is valid at a time!
  const TCHAR *inlGetSystemErrorMessage ( unsigned int Error = 0 );
  void inlGetLastErrorMessageBox();
  void inlOKMessageBox ( const TCHAR *Title, const TCHAR *Fmt, ... );
// Message Box dialog type
  enum MessageBoxType
  {
    MBTYPE_Ok,
    MBTYPE_YesNo,
    MBTYPE_OkCancel,
    MBTYPE_YesNoCancel,
    MBTYPE_CancelTryContinue,
    MBTYPE_AbortRetryIgnore,
    MBTYPE_FORCE_DWORD    = 0x7fffffff
  };

  enum MessageBoxIcon
  {
    MBICON_Exclamation,
    MBICON_Warning,
    MBICON_Information,
    MBICON_Asterix,
    MBICON_Question,

    MBICON_Stop,
    MBICON_Error,
    MBICON_Hand,
    MBICON_FORCE_DWORD    = 0x7fffffff
  };

  enum MessageBoxModal
  {
    MBMODAL_ApplicationModal,
    MBMODAL_SystemModal,
    MBMODAL_TaskModal,
    MBMODAL_FORCE_DWORD   = 0x7fffffff
  };

  enum MessageBoxResult
  {
    MBRES_Ok        = 1,
    MBRES_Cancel,
    MBRES_Abort,
    MBRES_Retry,
    MBRES_Ignore,
    MBRES_Yes,
    MBRES_No,
    MBRES_Close,
    MBRES_Help,
    MBRES_TryAgain,
    MBRES_Continue,

    MBRES_FORCE_DWORD   = 0x7fffffff
  };

  VARARG_DECL (unsigned int, static BOOL, return, inlWin32MessageBox, VARARG_NONE, const TCHAR *,
               VARARG_EXTRA (HWND OwnerWindowHandle) VARARG_EXTRA (const TCHAR *Caption) VARARG_EXTRA (MessageBoxType Type) VARARG_EXTRA (MessageBoxIcon Icon) VARARG_EXTRA (MessageBoxModal Modal),
               VARARG_EXTRA (OwnerWindowHandle) VARARG_EXTRA (Caption) VARARG_EXTRA (Type) VARARG_EXTRA (Icon) VARARG_EXTRA (Modal) );

}

#endif // NWIN32MESSAGEBOX_H

