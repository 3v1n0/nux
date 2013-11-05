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
#include "NWin32Clipboard.h"

namespace nux
{

  void inlCopyTextToClipboard (const TCHAR *text)
  {
    // test to see if we can open the clipboard first before
    // wasting any cycles with the memory allocation
    if (::OpenClipboard (::GetActiveWindow() ) )
    {
      // Empty the Clipboard. This also has the effect
      // of allowing Windows to free the memory associated
      // with any data that is in the Clipboard
      ::EmptyClipboard();

      // Ok. We have the Clipboard locked and it's empty.
      // Now let's allocate the global memory for our data.

      // Here I'm simply using the GlobalAlloc function to
      // allocate a block of data equal to the text in the
      // "to clipboard" edit control plus one character for the
      // terminating null character required when sending
      // ANSI text to the Clipboard.
      HGLOBAL hClipboardData;
      hClipboardData = ::GlobalAlloc (GMEM_MOVEABLE, StringLength (text) + 1);
      // The actual size of the memory allocated can be larger than the requested size.
      // To determine the actual number of bytes allocated, use the GlobalSize or LocalSize function.
      // If the amount allocated is greater than the amount requested, the process can use the entire amount.
      unsigned int Size = ::GlobalSize (hClipboardData);

      // Calling GlobalLock returns to me a pointer to the
      // data associated with the handle returned from
      // GlobalAlloc
      TCHAR *pData;
      pData = (TCHAR *) ::GlobalLock (hClipboardData);

      // At this point, all I need to do is use the standard
      // C/C++ strcpy function to copy the data from the local
      // variable to the global memory.
      inlTCharStringCopy (pData, Size, text);

      // Once done, I unlock the memory - remember you
      // don't call GlobalFree because Windows will free the
      // memory automatically when EmptyClipboard is next
      // called.
      ::GlobalUnlock (hClipboardData);

      // Now, set the Clipboard data by specifying that
      // ANSI text is being used and passing the handle to
      // the global memory.
#if UNICODE

      if (::SetClipboardData (CF_UNICODETEXT, hClipboardData) == NULL)
      {
        nuxError (TEXT ("SetClipboardData failed with error code %i"), ::GetLastError() );
      }

#else

      if (::SetClipboardData (CF_TEXT, hClipboardData) == NULL)
      {
        nuxError (TEXT ("SetClipboardData failed with error code %i"), ::GetLastError() );
      }

#endif
      // Finally, when finished I simply close the Clipboard
      // which has the effect of unlocking it so that other
      // applications can examine or modify its contents.
      ::CloseClipboard();
    }
  }

  std::string inlReadTextToClipboard()
  {
    std::string ResultString;

    if (::OpenClipboard (::GetActiveWindow() ) )
    {
      HGLOBAL GlobalMem = NULL;
      BOOL IsUnicodeText = 0;

      if ( (GlobalMem = GetClipboardData ( CF_UNICODETEXT )) )
      {
        IsUnicodeText = 1;
      }
      else if ( (GlobalMem = GetClipboardData ( CF_TEXT )) )
      {
        IsUnicodeText = 0;
      }

      if ( GlobalMem == 0)
        ResultString = TEXT ("");
      else
      {
        void *Data = ::GlobalLock ( GlobalMem );
        nuxAssert ( Data );

        if ( IsUnicodeText )
#if UNICODE
          ResultString = (TCHAR *) Data;

#else
          ResultString = (ANSICHAR *) UNICHAR_TO_ANSICHAR (Data);
#endif
        else
        {
#if UNICODE
          ResultString = (UNICHAR *) ANSICHAR_TO_UNICHAR (Data);
#else
          ResultString = (TCHAR *) Data;
#endif
//                ANSICHAR* AnsiChar = (ANSICHAR*) Data;
//                INT i;
//                // This for loop count the number of characters in ACh.
//                for( i=0; AnsiChar[i]; i++ );
//
//                INT n = i+1;
//                TCHAR* CharArray = new TCHAR[n];
//                for( i=0; i < n; i++ )
//                    CharArray[i]=FromAnsi(AnsiChar[i]);
//                ResultString = CharArray;
//                delete [] CharArray;
        }

        ::GlobalUnlock ( GlobalMem );
      }

      nuxAssert (::CloseClipboard() );
    }
    else
      ResultString = TEXT ("");

    return ResultString;
  }

}

