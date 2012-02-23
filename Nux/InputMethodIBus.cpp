#include "Nux.h"

#include "InputMethodIBus.h"

namespace nux
{
  IBusBus* IBusIMEContext::bus_ = NULL;

  IBusIMEContext::IBusIMEContext(TextEntry* text_entry) 
    : text_entry_(text_entry),
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
    g_signal_connect(bus_, "connected",    G_CALLBACK(OnConnected_),     this);
    g_signal_connect(bus_, "disconnected", G_CALLBACK(OnDisconnected_),  this);

    if (ibus_bus_is_connected(bus_))
    {
      CreateContext();
    }
    else
    {
      nuxDebugMsg("[IBusIMEContext::IBusIMEContext] Can not connect to ibus");
    }
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
    guint keyval = event.key_sym(); // todo(jaytaoko): ui::GdkKeyCodeForWindowsKeyCode(event.key_code(), event.IsShiftDown() ^ event.IsCapsLockDown());

    if (context_) {
      guint modifiers = 0;

      if (event.flags() & IBUS_IGNORED_MASK)
        return false;

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
        keyval, event.key_code() - 8, modifiers,
        -1,
        NULL,
        reinterpret_cast<GAsyncReadyCallback>(ProcessKeyEventDone),
        new ProcessKeyEventData(this, event));
      return true;
    }
    return false;
  }

  void IBusIMEContext::SetSurrounding(const std::wstring& text, int cursor_pos)
  {
      // TODO(penghuang) support surrounding
  }

  void IBusIMEContext::CreateContext() {
    nuxAssert(bus_ != NULL);
    nuxAssert(ibus_bus_is_connected(bus_));

    if (!(context_ = ibus_bus_create_input_context(bus_, "nux")))
    {
      nuxDebugMsg("[IBusIMEContext::IBusIMEContext] Cannot create InputContext");
      return;
    }

    // connect input context signals
    g_signal_connect(context_, "commit-text",         G_CALLBACK(OnCommitText_),        this);
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

    UpdateCursorLocation();
  }

  void IBusIMEContext::DestroyContext()
  {
    //nuxDebugMsg("***IBusIMEContext::DestroyContext***");
    if (!context_)
      return;

    ibus_proxy_destroy(reinterpret_cast<IBusProxy *>(context_));

    nuxAssert(!context_);
  }

  void IBusIMEContext::UpdateCursorLocation()
  {
    nux::Rect strong, weak;
    text_entry_->GetCursorRects(&strong, &weak);
    
    // Get the position of the TextEntry in the Window.
    nux::Geometry geo = text_entry_->GetAbsoluteGeometry();

    // Get the Geometry of the window on the display.
    nux::Geometry window_geo = nux::GetGraphicsDisplay()->GetWindowGeometry();

    ibus_input_context_set_cursor_location(context_,
      geo.x + window_geo.x + strong.x,
      geo.y + window_geo.y,
      0,
      geo.height);
  }

  void IBusIMEContext::OnConnected(IBusBus *bus)
  {
    //nuxDebugMsg("***IBusIMEContext::OnConnected***");

    nuxAssert(bus_ == bus);
    nuxAssert(ibus_bus_is_connected(bus_));

    if (context_)
      DestroyContext();
    CreateContext();
  }

  void IBusIMEContext::OnDisconnected(IBusBus *bus)
  {
    //nuxDebugMsg("***IBusIMEContext::OnDisonnected***");

    if (context_)
      DestroyContext();
  }

  void IBusIMEContext::OnCommitText(IBusInputContext *context, IBusText* text)
  {
    //nuxDebugMsg("***IBusIMEContext::OnCommitText::%s***", text->text);
    nuxAssert(context_ == context);

    text_entry_->DeleteSelection();
    
    if (text->text)
    {
      int cursor = text_entry_->cursor_;
      std::string new_text(text_entry_->GetText());
      std::string commit_text (text->text);
      new_text.insert (cursor, commit_text);

      text_entry_->SetText(new_text.c_str());
      text_entry_->SetCursor(cursor + commit_text.length());
      UpdateCursorLocation();
    }
  }

  void IBusIMEContext::OnUpdatePreeditText(IBusInputContext* context, IBusText* text, guint cursor_pos, gboolean visible)
  {
    //nuxDebugMsg("***IBusIMEContext::OnUpdatePreeditText***");
    nuxAssert(context_ == context);
    nuxAssert(IBUS_IS_TEXT(text));

    if (text_entry_->preedit_.empty())
      UpdateCursorLocation();

    if (visible)
    {
      IBusAttrList* attrs = text->attrs;
      if (attrs)
      {
        PangoAttrList* preedit_attrs = pango_attr_list_new();
        int i = 0;
        while (1)
        {
          IBusAttribute* attr = ibus_attr_list_get(attrs, i++);
          PangoAttribute* pango_attr;
          if (!attr)
            break;
          switch (attr->type)
          {
             case IBUS_ATTR_TYPE_UNDERLINE:
              pango_attr = pango_attr_underline_new ((PangoUnderline)attr->value);
              break;
            case IBUS_ATTR_TYPE_FOREGROUND:
              pango_attr = pango_attr_foreground_new (
                            ((attr->value & 0xff0000) >> 8) | 0xff,
                            ((attr->value & 0x00ff00)) | 0xff,
                            ((attr->value & 0x0000ff) << 8) | 0xff);
              break;
            case IBUS_ATTR_TYPE_BACKGROUND:
              pango_attr = pango_attr_background_new (
                            ((attr->value & 0xff0000) >> 8) | 0xff,
                            ((attr->value & 0x00ff00)) | 0xff,
                            ((attr->value & 0x0000ff) << 8) | 0xff);
              break;
            default:
              continue;
          }
          pango_attr->start_index = g_utf8_offset_to_pointer (text->text, attr->start_index) - text->text;
          pango_attr->end_index = g_utf8_offset_to_pointer (text->text, attr->end_index) - text->text;
          pango_attr_list_insert (preedit_attrs, pango_attr);
        }
        text_entry_->preedit_attrs_ = preedit_attrs;
      }
      if (text->text)
      {
        std::string preedit(text->text);
        text_entry_->preedit_ = preedit;
        text_entry_->preedit_cursor_ = preedit.length();
        text_entry_->QueueRefresh (true, true);
        text_entry_->text_changed.emit(text_entry_);
      }
    }
    else
    {
      OnHidePreeditText(context_);
    }
  }

  void IBusIMEContext::OnShowPreeditText(IBusInputContext *context)
  {
    //nuxDebugMsg("***IBusIMEContext::OnShowPreeditText***");
    nuxAssert(context_ == context);
  }

  void IBusIMEContext::OnHidePreeditText(IBusInputContext *context)
  {
    //nuxDebugMsg("***IBusIMEContext::OnHidePreeditText***");
    nuxAssert(context_ == context);

    text_entry_->ResetPreedit();
    text_entry_->QueueRefresh (true, true);
    text_entry_->text_changed.emit(text_entry_);
  }

  void IBusIMEContext::OnEnable(IBusInputContext *context)
  {
    //nuxDebugMsg("***IBusIMEContext::OnEnable***");
    nuxAssert(context_ == context);

    text_entry_->ime_active_ = true;
    UpdateCursorLocation();
  }

  void IBusIMEContext::OnDisable(IBusInputContext *context)
  {
    //nuxDebugMsg("***IBusIMEContext::OnDisable***");
    nuxAssert(context_ == context);

    text_entry_->ime_active_ = false;
    text_entry_->ResetPreedit();
    text_entry_->QueueRefresh (true, true);
    text_entry_->text_changed.emit(text_entry_);
  }

  void IBusIMEContext::OnDestroy(IBusInputContext *context)
  {
    //nuxDebugMsg("***IBusIMEContext::OnDestroy***");
    nuxAssert(context_ == context);

    g_object_unref(context_);
    context_ = NULL;
  }

  void IBusIMEContext::ProcessKeyEventDone(IBusInputContext *context, GAsyncResult* res, ProcessKeyEventData *data)
  {
    //nuxDebugMsg("***IBusIMEContext::ProcessKeyEventDone***");
      nuxAssert(data->context->context_ == context);

      GError *error = NULL;
      gboolean processed = ibus_input_context_process_key_event_async_finish (
                            context,
                            res,
                            &error);

      if (error != NULL)
      {
        g_warning ("Process Key Event failed: %s.", error->message); 
        g_error_free (error);
      }

      if (processed == FALSE)
      {
        data->context->text_entry_->ProcessKeyEvent(data->event.type(),
                                                    data->event.key_sym(),
                                                    data->event.flags() | IBUS_IGNORED_MASK,
                                                    data->event.character().c_str(),
                                                    0);
      }

      delete data;
  }
}
