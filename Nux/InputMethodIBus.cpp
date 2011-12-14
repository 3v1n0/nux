#include "Nux.h"
#include <ibus.h>

#include "InputMethodIBus.h"

// #include "base/logging.h"
// #include "base/utf_string_conversions.h"
// #include "third_party/skia/include/core/SkColor.h"
// #include "ui/base/gtk/gtk_signal.h"
// #include "ui/base/keycodes/keyboard_code_conversion_gtk.h"
// #include "ui/gfx/rect.h"
// #include "views/events/event.h"
// #include "views/ime/ime_context.h"

namespace nux
{
  IBusBus* IBusIMEContext::bus_ = NULL;

  IBusIMEContext::IBusIMEContext(View* view)
    : IMEContext(view),
    context_(NULL),
    is_focused_(false)
  {
      // init ibus
      if (!bus_)
      {
        ibus_init();
        bus_ = ibus_bus_new();
      }

      // connect bus signals
      g_signal_connect(bus_, "connected",     G_CALLBACK(OnConnected_),     this);
      g_signal_connect(bus_, "disconnected",  G_CALLBACK(OnDisconnected_),  this);

      if (ibus_bus_is_connected(bus_))
        CreateContext();
  }

  IBusIMEContext::~IBusIMEContext()
  {
    // disconnect bus signals
    g_signal_handlers_disconnect_by_func(bus_, reinterpret_cast<gpointer>(OnConnected_),    this);
    g_signal_handlers_disconnect_by_func(bus_, reinterpret_cast<gpointer>(OnDisconnected_), this);

    DestroyContext();
  }

  void IBusIMEContext::Focus()
  {
    if (is_focused_)
      return;
    is_focused_ = true;

    if (context_)
      ibus_input_context_focus_in(context_);
  }

  void IBusIMEContext::Blur()
  {
    if (!is_focused_)
      return;

    is_focused_ = false;

    if (context_)
      ibus_input_context_focus_out(context_);
  }

  void IBusIMEContext::Reset()
  {
    if (context_)
      ibus_input_context_reset(context_);
  }

  bool IBusIMEContext::FilterKeyEvent(const KeyEvent& event)
  {
    guint keyval = event.key_code(); // todo(jaytaoko): ui::GdkKeyCodeForWindowsKeyCode(event.key_code(), event.IsShiftDown() ^ event.IsCapsLockDown());

    if (context_) {
      guint modifiers = 0;

      if (event.type() == EVENT_KEY_UP)
        modifiers |= IBUS_RELEASE_MASK;

      if (event.IsShiftDown())
        modifiers |= IBUS_SHIFT_MASK;
      if (event.IsControlDown())
        modifiers |= IBUS_CONTROL_MASK;
      if (event.IsAltDown())
        modifiers |= IBUS_MOD1_MASK;
      if (event.IsCapsLockDown())
        modifiers |= IBUS_LOCK_MASK;

      ibus_input_context_process_key_event_async(context_,
        keyval, 0, modifiers,
        -1,
        NULL,
        reinterpret_cast<GAsyncReadyCallback>(ProcessKeyEventDone),
        new ProcessKeyEventData(this, event));
      return true;
    }

    return false;
  }

  void IBusIMEContext::SetCursorLocation(const Rect& caret_rect)
  {
    if (context_ && caret_rect_ != caret_rect)
    {
      caret_rect_ = caret_rect;
      ibus_input_context_set_cursor_location(context_,
        caret_rect_.x,
        caret_rect_.y,
        caret_rect_.width,
        caret_rect_.height);
    }
  }

  void IBusIMEContext::SetSurrounding(const std::wstring& text, int cursor_pos)
  {
      // TODO(penghuang) support surrounding
  }

  void IBusIMEContext::CreateContext() {
    nuxAssert(bus_ != NULL);
    nuxAssert(ibus_bus_is_connected(bus_));

    context_ = ibus_bus_create_input_context(bus_, "chrome");

    // connect input context signals
    g_signal_connect(context_, "commit-text",         G_CALLBACK(OnCommitText_),        this);
    g_signal_connect(context_, "forward-key-event",   G_CALLBACK(OnForwardKeyEvent_),   this);
    g_signal_connect(context_, "update-preedit-text", G_CALLBACK(OnUpdatePreeditText_), this);
    g_signal_connect(context_, "show-preedit-text",   G_CALLBACK(OnShowPreeditText_),   this);
    g_signal_connect(context_, "hide-preedit-text",   G_CALLBACK(OnHidePreeditText_),   this);
    g_signal_connect(context_, "enabled",             G_CALLBACK(OnEnable_),            this);
    g_signal_connect(context_, "disabled",            G_CALLBACK(OnDisable_),           this);
    g_signal_connect(context_, "destroy",             G_CALLBACK(OnDestroy_),           this);

    guint32 caps = IBUS_CAP_PREEDIT_TEXT |
      IBUS_CAP_FOCUS |
      IBUS_CAP_SURROUNDING_TEXT;
    ibus_input_context_set_capabilities(context_, caps);

    if (is_focused_)
      ibus_input_context_focus_in(context_);

    ibus_input_context_set_cursor_location(context_,
      caret_rect_.x,
      caret_rect_.y,
      caret_rect_.width,
      caret_rect_.height);
  }

  void IBusIMEContext::DestroyContext()
  {
    if (!context_)
      return;

    ibus_proxy_destroy(reinterpret_cast<IBusProxy *>(context_));

    nuxAssert(!context_);
  }

  void IBusIMEContext::OnConnected(IBusBus *bus)
  {
    nuxAssert(bus_ == bus);
    nuxAssert(ibus_bus_is_connected(bus_));

    if (context_)
      DestroyContext();
    CreateContext();
  }

  void IBusIMEContext::OnDisconnected(IBusBus *bus)
  {
  }

  void IBusIMEContext::OnCommitText(IBusInputContext *context, IBusText* text)
  {
    nuxAssert(context_ == context);
    IMEContext::CommitText(ANSICHAR_TO_UNICHAR(text->text));
  }

  void IBusIMEContext::OnForwardKeyEvent(IBusInputContext *context, guint keyval, guint keycode, guint state)
  {
    nuxAssert(context_ == context);

    int flags = 0;

    if (state & IBUS_LOCK_MASK)
      flags |= KEY_MODIFIER_CAPS_LOCK;
    if (state & IBUS_CONTROL_MASK)
      flags |= KEY_MODIFIER_CTRL;
    if (state & IBUS_SHIFT_MASK)
      flags |= KEY_MODIFIER_SHIFT;
    if (state & IBUS_MOD1_MASK)
      flags |= KEY_MODIFIER_ALT;

    int mouse_state = 0;
    if (state & IBUS_BUTTON1_MASK)
      mouse_state |= MOUSE_BUTTON1;
    if (state & IBUS_BUTTON2_MASK)
      mouse_state |= MOUSE_BUTTON2;
    if (state & IBUS_BUTTON3_MASK)
      mouse_state |= MOUSE_BUTTON3;

    ForwardKeyEvent(KeyEvent(state & IBUS_RELEASE_MASK ? EVENT_KEY_DOWN : EVENT_KEY_UP, keyval /* todo(jaytaoko): ui::WindowsKeyCodeForGdkKeyCode(keyval)*/, mouse_state, flags));
  }

  void IBusIMEContext::OnUpdatePreeditText(IBusInputContext *context, IBusText* text, guint cursor_pos, gboolean visible)
  {
    nuxAssert(context_ == context);
    nuxAssert(IBUS_IS_TEXT(text));

    if (visible)
    {
      CompositionAttributeList attributes;
      IBusAttrList *attrs = text->attrs;
      if (attrs)
      {
        int i = 0;
        while (1)
        {
          IBusAttribute *attr = ibus_attr_list_get(attrs, i++);
          if (!attr)
            break;
          if (attr->type == IBUS_ATTR_TYPE_UNDERLINE || attr->type == IBUS_ATTR_TYPE_BACKGROUND)
          {
            CompositionAttribute attribute(attr->start_index,
              attr->end_index,
              color::Black,
              false);
            if (attr->type == IBUS_ATTR_TYPE_BACKGROUND)
            {
              attribute.thick =  true;
            } else if (attr->type == IBUS_ATTR_TYPE_UNDERLINE)
            {
              if (attr->value == IBUS_ATTR_UNDERLINE_DOUBLE)
              {
                attribute.thick = true;
              } else if (attr->value == IBUS_ATTR_UNDERLINE_ERROR)
              {
                attribute.color = color::Red;
              }
            }
            attributes.push_back(attribute);
          }
        }
      }

      SetComposition(ANSICHAR_TO_UNICHAR(text->text), attributes, cursor_pos);
    }
    else
    {
      EndComposition();
    }
  }

  void IBusIMEContext::OnShowPreeditText(IBusInputContext *context)
  {
    nuxAssert(context_ == context);
  }

  void IBusIMEContext::OnHidePreeditText(IBusInputContext *context)
  {
    nuxAssert(context_ == context);

    EndComposition();
  }

  void IBusIMEContext::OnEnable(IBusInputContext *context)
  {
    nuxAssert(context_ == context);
  }

  void IBusIMEContext::OnDisable(IBusInputContext *context)
  {
    nuxAssert(context_ == context);
  }

  void IBusIMEContext::OnDestroy(IBusInputContext *context)
  {
    nuxAssert(context_ == context);

    g_object_unref(context_);
    context_ = NULL;

    EndComposition();
  }

  void IBusIMEContext::ProcessKeyEventDone(IBusInputContext *context, GAsyncResult* res, ProcessKeyEventData *data)
  {
      nuxAssert(data->context->context_ == context);

      gboolean processed = FALSE;

      if (!ibus_input_context_process_key_event_async_finish(context,
        res,
        &processed,
        NULL))
        processed = FALSE;

      if (processed == FALSE)
        data->context->ForwardKeyEvent(data->event);

      delete data;
  }

  IMEContext* IMEContext::Create(View* view)
  {
    return new IBusIMEContext(view);
  }

}
