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


#ifndef EDITTEXTBOX_H
#define EDITTEXTBOX_H

#include "KeyboardHandler.h"
#include "TimerProc.h"

NAMESPACE_BEGIN_GUI
class HLayout;
class VLayout;
class Layout;
class BaseKeyboardHandler;
class TextLine;
class Validator;
class TimerFunctor;

class EditTextBox : public ActiveInterfaceObject
{
public:
    EditTextBox(const TCHAR* Caption = 0);
    ~EditTextBox();
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    void SetText(const TCHAR& Caption);
    void SetText(const TCHAR* Caption);
    void SetText(const tstring& Caption);
    void SetText(const NString& Caption);
    const TCHAR* GetText() const;
    t_u32 GetTextSize() const {return (t_u32)m_Text.Length();}

    void setDoubleValue(double d);
    void setIntegerValue(int i);

    //! Return a caption string striping out the prefix and the suffix 
    virtual NString GetCleanText() const;

    void SetTextBackgroundColor(const Color& color);
    Color GetTextBackgroundColor() const;

    void SetSelectedTextColor(Color color) {m_SelectedTextColor = color;}
    void SetSelectedTextBackgroundColor(Color color) {m_SelectedTextBackgroundColor = color;}
    void SetTextBlinkColor(Color color) {m_TextBlinkColor = color;}
    void SetCursorColor(Color color) {m_CursorColor = color;}

    Color GetSelectedTextColor() const {return m_SelectedTextColor;}
    Color GetSelectedTextBackgroundColor() const {return m_SelectedTextBackgroundColor;}
    Color GetTextBlinkColor() const {return m_TextBlinkColor;}
    Color GetCursorColor() const {return m_CursorColor;}

    void SetKeyEntryType(BaseKeyboardHandler::eKeyEntryType keytype) {m_KeyboardHandler.SetKeyEntryType(keytype);}
    BaseKeyboardHandler::eKeyEntryType GetKeyEntryType() {return m_KeyboardHandler.GetKeyEntryType();}

    //  Receivers
    void RecvMouseDoubleClick(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvKeyEvent(
        GraphicsContext& GfxContext ,   /*Graphics Context for text operation*/
        unsigned long    eventType  ,   /*event type*/
        unsigned long    keysym     ,   /*event keysym*/
        unsigned long    state      ,   /*event state*/
        const char*      character  ,   /*character*/
        bool             isRepeated ,   /*true if the key is repeated more than once*/
        unsigned short   keyCount       /*key repeat count*/);

    void RecvStartKeyFocus();
    void RecvEndKeyFocus();


    void SetPrefix(const tstring& p)    { m_Prefix = p; };
    void SetPrefix(const TCHAR* p)      { m_Prefix = p; };
    void SetPrefix(const TCHAR& p)      { m_Prefix = p; };
    void SetPrefix(const NString& p)    { m_Prefix = p; };
    NString GetPrefix() const           { return m_Prefix; };

    void SetSuffix(const tstring& s)    { m_Suffix = s; };
    void SetSuffix(const TCHAR* s)      { m_Suffix = s; };
    void SetSuffix(const TCHAR& s)      { m_Suffix = s; };
    void SetSuffix(const NString& s)    { m_Suffix = s; };
    NString Getsuffix() const           { return m_Suffix; };

    void SetValidator(const Validator* validator);
    
    sigc::signal< void, const weaksmptr(EditTextBox), unsigned int > sigCharacter; // Emitted every time a character typed
    sigc::signal< void, const weaksmptr(EditTextBox) > sigEditChange; // Emitted every time a character typed
    
    sigc::signal< void, const weaksmptr(EditTextBox), const NString&> sigValidateKeyboardEntry;
    sigc::signal< void, const weaksmptr(EditTextBox) > sigValidateEntry;    // Emitted when the text is validated
    sigc::signal< void, const weaksmptr(EditTextBox) > sigSetText;      // Emitted when text is set with setCaption
    sigc::signal< void, const weaksmptr(EditTextBox) > sigEscapeKeyboardFocus;
    sigc::signal< void, const weaksmptr(EditTextBox) > sigStartKeyboardFocus;
    sigc::signal< void, const weaksmptr(EditTextBox) > sigEndKeyboardFocus;

    bool IsTextSelected() {return m_KeyboardHandler.IsTextSelected();}
    bool IsEmpty();

private:
    UBOOL ValidateKeyboardEntry(const TCHAR* text) const;
    void EscapeKeyboardFocus();
    void EnteringKeyboardFocus();
    void QuitingKeyboardFocus();

    virtual long PostLayoutManagement(long LayoutResult);

    NString m_Text;
    smptr(HLayout) hlayout;

    Color m_BackgroundColor;
    Color m_SelectedTextColor;
    Color m_SelectedTextBackgroundColor;
    Color m_TextBlinkColor;
    Color m_CursorColor;

    BaseKeyboardHandler m_KeyboardHandler;
    NString m_temporary_caption;
    BaseKeyboardHandler::eKeyEntryType m_keytype;

    Validator* m_Validator;
    NString m_Suffix;
    NString m_Prefix;

    bool BlinkCursor;
    void BlinkCursorTimerInterrupt(void* v);
    void StopBlinkCursor(bool BlinkState = false);
    void StartBlinkCursor(bool BlinkState = false);
    TimerFunctor* m_BlinkTimerFunctor;
    TimerHandle* m_BlinkTimerHandler;

    void ScrollTimerInterrupt(void* v);
    TimerFunctor* m_ScrollTimerFunctor;
    TimerHandle* m_ScrollTimerHandler;

    //! If true, blend the characters alpha value with the destination and write the result to the destination buffer.
    bool m_WriteAlpha;

    friend class RGBValuator;
};

NAMESPACE_END_GUI

#endif // EDITTEXTBOX_H

