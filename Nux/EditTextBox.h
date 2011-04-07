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


#ifndef EDITTEXTBOX_H
#define EDITTEXTBOX_H

#include "KeyboardHandler.h"
#include "TimerProc.h"

namespace nux
{
  class HLayout;
  class VLayout;
  class Layout;
  class BaseKeyboardHandler;
  class TextLine;
  class Validator;
  class TimerFunctor;

  class EditTextBox : public View
  {
  public:
    EditTextBox (const TCHAR *Caption, NUX_FILE_LINE_PROTO);
    ~EditTextBox();
    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);

    void SetText (const TCHAR &Caption);
    void SetText (const TCHAR *Caption);
    void SetText (const tstring &Caption);
    void SetText (const NString &Caption);
    const TCHAR *GetText() const;
    t_u32 GetTextSize() const
    {
      return (t_u32) m_Text.Length();
    }

    void SetDoubleValue (double d);
    void SetIntegerValue (int i);

    //! Return a caption string striping out the prefix and the suffix
    virtual NString GetCleanText() const;

    void SetTextBackgroundColor (const Color &color);
    Color GetTextBackgroundColor() const;

    void SetSelectedTextColor (Color color)
    {
      m_SelectedTextColor = color;
    }
    void SetSelectedTextBackgroundColor (Color color)
    {
      m_SelectedTextBackgroundColor = color;
    }
    void SetTextBlinkColor (Color color)
    {
      m_TextBlinkColor = color;
    }
    void SetCursorColor (Color color)
    {
      m_CursorColor = color;
    }

    Color GetSelectedTextColor() const
    {
      return m_SelectedTextColor;
    }
    Color GetSelectedTextBackgroundColor() const
    {
      return m_SelectedTextBackgroundColor;
    }
    Color GetTextBlinkColor() const
    {
      return m_TextBlinkColor;
    }
    Color GetCursorColor() const
    {
      return m_CursorColor;
    }

    void SetKeyEntryType (BaseKeyboardHandler::eKeyEntryType keytype)
    {
      m_KeyboardHandler.SetKeyEntryType (keytype);
    }
    BaseKeyboardHandler::eKeyEntryType GetKeyEntryType()
    {
      return m_KeyboardHandler.GetKeyEntryType();
    }

    //  Receivers
    void RecvMouseDoubleClick (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvKeyEvent (
      GraphicsEngine &GfxContext ,   /*Graphics Context for text operation*/
      unsigned long    eventType  ,   /*event type*/
      unsigned long    keysym     ,   /*event keysym*/
      unsigned long    state      ,   /*event state*/
      const TCHAR*     character  ,   /*character*/
      unsigned short   keyCount       /*key repeat count*/);

    void RecvStartKeyFocus();
    void RecvEndKeyFocus();


    void SetPrefix (const tstring &p)
    {
      m_Prefix = p;
    };
    void SetPrefix (const TCHAR *p)
    {
      m_Prefix = p;
    };
    void SetPrefix (const TCHAR &p)
    {
      m_Prefix = p;
    };
    void SetPrefix (const NString &p)
    {
      m_Prefix = p;
    };
    NString GetPrefix() const
    {
      return m_Prefix;
    };

    void SetSuffix (const tstring &s)
    {
      m_Suffix = s;
    };
    void SetSuffix (const TCHAR *s)
    {
      m_Suffix = s;
    };
    void SetSuffix (const TCHAR &s)
    {
      m_Suffix = s;
    };
    void SetSuffix (const NString &s)
    {
      m_Suffix = s;
    };
    NString Getsuffix() const
    {
      return m_Suffix;
    };

    void SetValidator (const Validator *validator);

    sigc::signal< void, EditTextBox *, unsigned int > sigCharacter; // Emitted every time a character typed
    sigc::signal< void, EditTextBox * > sigEditChange; // Emitted every time a character typed

    sigc::signal< void, EditTextBox *, const NString &> sigValidateKeyboardEntry;
    sigc::signal< void, EditTextBox * > sigValidateEntry;   // Emitted when the text is validated
    sigc::signal< void, EditTextBox * > sigSetText;     // Emitted when text is set with setCaption
    sigc::signal< void, EditTextBox * > sigEscapeKeyboardFocus;
    sigc::signal< void, EditTextBox * > sigStartKeyboardFocus;
    sigc::signal< void, EditTextBox * > sigEndKeyboardFocus;

    bool IsTextSelected()
    {
      return m_KeyboardHandler.IsTextSelected();
    }
    bool IsEmpty();

  private:
    bool ValidateKeyboardEntry (const TCHAR *text) const;
    void EscapeKeyboardFocus();
    void EnteringKeyboardFocus();
    void QuitingKeyboardFocus();

    virtual long PostLayoutManagement (long LayoutResult);

    NString m_Text;
    HLayout *hlayout;

    Color m_BackgroundColor;
    Color m_SelectedTextColor;
    Color m_SelectedTextBackgroundColor;
    Color m_TextBlinkColor;
    Color m_CursorColor;

    BaseKeyboardHandler m_KeyboardHandler;
    NString m_temporary_caption;
    BaseKeyboardHandler::eKeyEntryType m_keytype;

    Validator *m_Validator;
    NString m_Suffix;
    NString m_Prefix;

    bool BlinkCursor;
    void BlinkCursorTimerInterrupt (void *v);
    void StopBlinkCursor (bool BlinkState = false);
    void StartBlinkCursor (bool BlinkState = false);
    TimerFunctor *m_BlinkTimerFunctor;
    TimerHandle m_BlinkTimerHandler;

    void ScrollTimerInterrupt (void *v);
    TimerFunctor *m_ScrollTimerFunctor;
    TimerHandle m_ScrollTimerHandler;

    //! If true, blend the characters alpha value with the destination and write the result to the destination buffer.
    bool m_WriteAlpha;

    friend class RGBValuator;
  };

}

#endif // EDITTEXTBOX_H

