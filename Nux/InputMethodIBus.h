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
* Authored by: Brandon Schaefer <brandontschaefer@gmail.com>
*              Jay Taoko <jaytaoko@inalogic.com>
*
*/


#ifndef INPUTMETHODIBUS_H
#define INPUTMETHODIBUS_H

#include <Nux/TextEntry.h>
#include <ibus.h>

namespace nux
{

  class IBusIMEContext;
  class TextEntry;

  // FIXME This class should be reworked to replace the mouse_state
  // with the hardware key_code.
  class KeyEvent
  {
  public:

    KeyEvent(NuxEventType type,
      unsigned int key_sym,
      unsigned int key_code, unsigned int event_flags, const char* character)
      : type_(type)
      , key_sym_(key_sym)
      , key_code_(key_code)
      , key_modifiers_(event_flags)
      , character_(character ? character : "")
    {
    }

    NuxEventType type() const {return type_;}
    unsigned int key_sym() const {return key_sym_;}
    unsigned int key_code() const {return key_code_;}
    unsigned int flags() const {return key_modifiers_;}
    std::string character() const {return character_;}

    bool IsShiftDown() const { return (key_modifiers_ & KEY_MODIFIER_SHIFT) != 0; }
    bool IsControlDown() const { return (key_modifiers_ & KEY_MODIFIER_CTRL) != 0; }
    bool IsCapsLockDown() const { return (key_modifiers_ & KEY_MODIFIER_CAPS_LOCK) != 0; }
    bool IsAltDown() const { return (key_modifiers_ & KEY_MODIFIER_ALT) != 0; }

  private:
    EventType type_;
    unsigned int key_sym_;
    unsigned int key_code_;
    unsigned int key_modifiers_;
    std::string character_;

    KeyEvent(const KeyEvent&);
    void operator = (const KeyEvent&);
  };

  // Used for passing data to ProcessKeyEventDone function()
  class ProcessKeyEventData
  {
  public:
    ProcessKeyEventData(IBusIMEContext* context,
      const KeyEvent& event)
      : context(context)
      , event(event.type(), event.key_sym(), event.key_code(), event.flags(), event.character().c_str())
    {

    }
    IBusIMEContext* context;
    KeyEvent event;
  };

  // Implements IMEContext to integrate ibus input method framework
  class IBusIMEContext
  {
  public:
    explicit IBusIMEContext(TextEntry* text_entry);
    virtual ~IBusIMEContext();

    // views::IMEContext implementations:
    virtual void Focus();
    virtual void Blur();
    virtual void Reset();
    virtual bool FilterKeyEvent(const KeyEvent& event);
    virtual void SetSurrounding(const std::wstring& text, int cursor_pos);

    bool IsConnected() const;
    bool IsHotkeyEvent(EventType type, unsigned long keysym, unsigned long modifiers) const;

  protected:
    static std::vector<Event> ParseIBusHotkeys(const gchar** keybindings);

  private:
    void CreateContext();
    void DestroyContext();

    void UpdateCursorLocation();
    static void UpdateHotkeys();

    // Event handlers for IBusBus:
    //CHROMEG_CALLBACK_0(IBusIMEContext, void, OnConnected, IBusBus*);
    static void OnConnected_(IBusBus* bus, void* data) {nuxDebugMsg("***IBusIMEContext::OnConnected***"); static_cast<IBusIMEContext*>(data)->OnConnected(bus);}
    void OnConnected(IBusBus *bus);

    //CHROMEG_CALLBACK_0(IBusIMEContext, void, OnDisconnected, IBusBus*);
    static void OnDisconnected_(IBusBus* bus, void* data) {static_cast<IBusIMEContext*>(data)->OnDisconnected(bus);}
    void OnDisconnected(IBusBus *bus);

    static void OnConfigChanged_(IBusConfig* config, gchar* section, gchar* name, GVariant* value, gpointer data) {static_cast<IBusIMEContext*>(data)->OnConfigChanged(config, section, name, value);}
    void OnConfigChanged(IBusConfig* config, gchar* section, gchar* name, GVariant* value);

//     // Event handlers for IBusIMEContext:
    //CHROMEG_CALLBACK_1(IBusIMEContext, void, OnCommitText, IBusInputContext*, IBusText*);
    static void OnCommitText_(IBusInputContext* context, IBusText* text, void* data) {static_cast<IBusIMEContext*>(data)->OnCommitText(context, text);}
    void OnCommitText(IBusInputContext *context, IBusText* text);

    //CHROMEG_CALLBACK_3(IBusIMEContext, void, OnUpdatePreeditText, IBusInputContext*, IBusText*, guint, gboolean);
    static void OnUpdatePreeditText_(IBusInputContext* context, IBusText* text, guint cursor_pos, gboolean visible, void* data) {reinterpret_cast<IBusIMEContext*>(data)->OnUpdatePreeditText(context, text, cursor_pos, visible);}
    void OnUpdatePreeditText(IBusInputContext *context, IBusText* text, guint cursor_pos, gboolean visible);

    //CHROMEG_CALLBACK_0(IBusIMEContext, void, OnShowPreeditText, IBusInputContext*);
    static void OnShowPreeditText_(IBusInputContext* context, void* data) {static_cast<IBusIMEContext*>(data)->OnShowPreeditText(context);}
    void OnShowPreeditText(IBusInputContext *context);

//     CHROMEG_CALLBACK_0(IBusIMEContext, void, OnHidePreeditText, IBusInputContext*);
    static void OnHidePreeditText_(IBusInputContext* context, void* data) {static_cast<IBusIMEContext*>(data)->OnHidePreeditText(context);}
    void OnHidePreeditText(IBusInputContext *context);

//     CHROMEG_CALLBACK_0(IBusIMEContext, void, OnEnable, IBusInputContext*);
    static void OnEnable_(IBusInputContext* context, void* data) {static_cast<IBusIMEContext*>(data)->OnEnable(context);}
    void OnEnable(IBusInputContext *context);

//     CHROMEG_CALLBACK_0(IBusIMEContext, void, OnDisable, IBusInputContext*);
    static void OnDisable_(IBusInputContext* context, void* data) {static_cast<IBusIMEContext*>(data)->OnDisable(context);}
    void OnDisable(IBusInputContext *context);

//     CHROMEG_CALLBACK_0(IBusIMEContext, void, OnDestroy, IBusInputContext*);
    static void OnDestroy_(IBusInputContext* context, void* data) {static_cast<IBusIMEContext*>(data)->OnDestroy(context);}
    void OnDestroy(IBusInputContext *context);

    static void ProcessKeyEventDone(IBusInputContext* context,
      GAsyncResult* res,
      ProcessKeyEventData* data);

    TextEntry* text_entry_;
    IBusInputContext* context_;
    GCancellable* cancellable_;
    bool is_focused_;

    static IBusBus* bus_;
    static std::vector<Event> hotkeys_;

    IBusIMEContext(const IBusIMEContext&);
    void operator = (const IBusIMEContext&);
  };

}
#endif  // INPUTMETHODIBUS_H

