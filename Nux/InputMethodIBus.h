#ifndef INPUTMETHODIBUS_H
#define INPUTMETHODIBUS_H

#include "InputMethodContext.h"

namespace nux
{

  class IBusIMEContext;

  class ProcessKeyEventData
  {
  public:
    ProcessKeyEventData(IBusIMEContext* context,
      const KeyEvent& event)
      : context(context)
      , event(event.type(), event.key_code(), event.MouseState(), event.flags())
    {

    }

    IBusIMEContext* context;
    KeyEvent event;
  };

  // Implements IMEContext to integrate ibus input method framework
  class IBusIMEContext : public IMEContext
  {
  public:
    explicit IBusIMEContext(View* view);
    virtual ~IBusIMEContext();

    // views::IMEContext implementations:
    virtual void Focus();
    virtual void Blur();
    virtual void Reset();
    virtual bool FilterKeyEvent(const KeyEvent& event);
    virtual void SetCursorLocation(const Rect& caret_rect);
    virtual void SetSurrounding(const std::wstring& text, int cursor_pos);

  private:
    void CreateContext();
    void DestroyContext();

    // Event handlers for IBusBus:
    //CHROMEG_CALLBACK_0(IBusIMEContext, void, OnConnected, IBusBus*);
    static void OnConnected_(IBusBus* bus, void* data) {reinterpret_cast<IBusIMEContext*>(data)->OnConnected(bus);}
    void OnConnected(IBusBus *bus);

    //CHROMEG_CALLBACK_0(IBusIMEContext, void, OnDisconnected, IBusBus*);
    static void OnDisconnected_(IBusBus* bus, void* data) {reinterpret_cast<IBusIMEContext*>(data)->OnConnected(bus);}
    void OnDisconnected(IBusBus *bus);

//     // Event handlers for IBusIMEContext:
    //CHROMEG_CALLBACK_1(IBusIMEContext, void, OnCommitText, IBusInputContext*, IBusText*);
    static void OnCommitText_(IBusInputContext* context, IBusText* text, void* data) {reinterpret_cast<IBusIMEContext*>(data)->OnCommitText(context, text);}
    void OnCommitText(IBusInputContext *context, IBusText* text);

    //CHROMEG_CALLBACK_3(IBusIMEContext, void, OnForwardKeyEvent, IBusInputContext*, guint, guint, guint);
    static void OnForwardKeyEvent_(IBusInputContext* context, guint keyval, guint keycode, guint state, void* data) {reinterpret_cast<IBusIMEContext*>(data)->OnForwardKeyEvent(context, keyval, keycode, state);}
    void OnForwardKeyEvent(IBusInputContext *context, guint keyval, guint keycode, guint state);

    //CHROMEG_CALLBACK_3(IBusIMEContext, void, OnUpdatePreeditText, IBusInputContext*, IBusText*, guint, gboolean);
    static void OnUpdatePreeditText_(IBusInputContext* context, IBusText* text, guint cursor_pos, gboolean visible, void* data) {reinterpret_cast<IBusIMEContext*>(data)->OnUpdatePreeditText(context, text, cursor_pos, visible);}
    void OnUpdatePreeditText(IBusInputContext *context, IBusText* text, guint cursor_pos, gboolean visible);

    //CHROMEG_CALLBACK_0(IBusIMEContext, void, OnShowPreeditText, IBusInputContext*);
    static void OnShowPreeditText_(IBusInputContext* context, void* data) {reinterpret_cast<IBusIMEContext*>(data)->OnShowPreeditText(context);}
    void OnShowPreeditText(IBusInputContext *context);

//     CHROMEG_CALLBACK_0(IBusIMEContext, void, OnHidePreeditText, IBusInputContext*);
    static void OnHidePreeditText_(IBusInputContext* context, void* data) {reinterpret_cast<IBusIMEContext*>(data)->OnHidePreeditText(context);}
    void OnHidePreeditText(IBusInputContext *context);

//     CHROMEG_CALLBACK_0(IBusIMEContext, void, OnEnable, IBusInputContext*);
    static void OnEnable_(IBusInputContext* context, void* data) {reinterpret_cast<IBusIMEContext*>(data)->OnEnable(context);}
    void OnEnable(IBusInputContext *context);

//     CHROMEG_CALLBACK_0(IBusIMEContext, void, OnDisable, IBusInputContext*);
    static void OnDisable_(IBusInputContext* context, void* data) {reinterpret_cast<IBusIMEContext*>(data)->OnDisable(context);}
    void OnDisable(IBusInputContext *context);

//     CHROMEG_CALLBACK_0(IBusIMEContext, void, OnDestroy, IBusInputContext*);
    static void OnDestroy_(IBusInputContext* context, void* data) {reinterpret_cast<IBusIMEContext*>(data)->OnDestroy(context);}
    void OnDestroy(IBusInputContext *context);



    static void ProcessKeyEventDone(IBusInputContext* context,
      GAsyncResult* res,
      ProcessKeyEventData* data);

    IBusInputContext* context_;
    bool is_focused_;
    Rect caret_rect_;

    static IBusBus* bus_;

    IBusIMEContext(const IBusIMEContext&);
    void operator = (const IBusIMEContext&);
  };

}
#endif  // INPUTMETHODIBUS_H

