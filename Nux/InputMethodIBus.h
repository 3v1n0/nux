#ifndef INPUTMETHODIBUS_H
#define INPUTMETHODIBUS_H

#include "InputMethodContext.h"

namespace nux
{

  class IBusIMEContext;

  class ProcessKeyEventData
  {
    ProcessKeyEventData(IBusIMEContext* context,
      const KeyEvent& event)
      : context(context),
      event(event.type(), event.key_code(), event.flags())
    {

    }

    IBusIMEContext* context;
    KeyEvent event;
  };

  // Implements IMEContext to integrate ibus input method framework
  class IBusIMEContext : public IMEContext
  {
  public:
    explicit IBusIMEContext(views::View* view);
    virtual ~IBusIMEContext();

    // views::IMEContext implementations:
    virtual void Focus();
    virtual void Blur();
    virtual void Reset();
    virtual bool FilterKeyEvent(const views::KeyEvent& event);
    virtual void SetCursorLocation(const gfx::Rect& caret_rect);
    virtual void SetSurrounding(const std::wstring& text, int cursor_pos);

  private:
    void CreateContext();
    void DestroyContext();

    // Event handlers for IBusBus:
    CHROMEG_CALLBACK_0(IBusIMEContext, void, OnConnected, IBusBus*);
    CHROMEG_CALLBACK_0(IBusIMEContext, void, OnDisconnected, IBusBus*);

    // Event handlers for IBusIMEContext:
    CHROMEG_CALLBACK_1(IBusIMEContext, void, OnCommitText, IBusInputContext*, IBusText*);
    CHROMEG_CALLBACK_3(IBusIMEContext, void, OnForwardKeyEvent, IBusInputContext*, guint, guint, guint);
    CHROMEG_CALLBACK_3(IBusIMEContext, void, OnUpdatePreeditText, IBusInputContext*, IBusText*, guint, gboolean);
    CHROMEG_CALLBACK_0(IBusIMEContext, void, OnShowPreeditText, IBusInputContext*);
    CHROMEG_CALLBACK_0(IBusIMEContext, void, OnHidePreeditText, IBusInputContext*);
    CHROMEG_CALLBACK_0(IBusIMEContext, void, OnEnable, IBusInputContext*);
    CHROMEG_CALLBACK_0(IBusIMEContext, void, OnDisable, IBusInputContext*);
    CHROMEG_CALLBACK_0(IBusIMEContext, void, OnDestroy, IBusInputContext*);

    static void ProcessKeyEventDone(IBusInputContext* context,
      GAsyncResult* res,
      ProcessKeyEventData *data);

    IBusInputContext* context_;
    bool is_focused_;
    gfx::Rect caret_rect_;

    static IBusBus* bus_;

    DISALLOW_COPY_AND_ASSIGN(IBusIMEContext);
  };

}
#endif  // INPUTMETHODIBUS_H

