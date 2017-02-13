/*
 * Copyright 2011-2013 Canonical Ltd.
 * Copyright 2010 Inalogic Inc.
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
 * Copyright 2008 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0(the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Nux.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "Validator.h"

#include "cairo/cairo.h"
#include "pango/pango.h"
#include "pango/pangocairo.h"
#include "NuxGraphics/CairoGraphics.h"

#include "TextEntry.h"

#if defined(NUX_OS_LINUX)
# include "TextEntryComposeSeqs.h"
# if defined(USE_X11)
#  include <X11/cursorfont.h>
#  include "InputMethodIBus.h"
# endif
#endif


namespace nux
{
  static const int kInnerBorderX = 2;
  static const int kInnerBorderY = 0; //1;
  static const int kCursorBlinkTimeout = 400;
  static const double kStrongCursorLineWidth = 1;
  static const double kStrongCursorBarWidth = 1;
  static const double kWeakCursorLineWidth = 3;
  static const double kWeakCursorBarWidth = 3;
  static const Color kStrongCursorColor(0.9f, 0.9f, 0.9f, 1.0f);
  static const Color kWeakCursorColor(1.0f, 1.0f, 1.0f, 0.5f);
  static const Color kDefaultTextColor(0, 0, 0, 1.0f);
  static const Color kDefaultBackgroundColor(1, 1, 1, 1.0f);
  static const Color kDefaultSelectionBackgroundColor(0.5, 0.5, 0.5, 1.0f);
  static const Color kDefaultSelectionTextColor(1, 1, 1, 1.0f);
  static const unsigned long long kTripleClickTimeout = 500;
  static const std::string kDefaultFontName = "Ubuntu";

  static unsigned long long GetCurrentTime()
  {
    gint64 micro_secs = g_get_real_time();
    return static_cast<unsigned long long>(micro_secs / 1000);
  }

  static std::string CleanupLineBreaks(const char *source)
  {
    nuxAssert(source);
    std::string result;
    while (*source) {
      if (*source == '\r') {
        result += ' ';
        if (source[1] == '\n')
          source++;
      } else if (*source == '\n') {
        result += ' ';
      } else {
        result += *source;
      }
      source++;
    }
    return result;
  }

  // Calculate pixel size based on the Windows DPI of 96 for compatibility
  // reasons.
  CairoFont::CairoFont(const std::string &family,
                       /*PangoFontDescription *font,*/
                       double pt_size,
                       Style style,
                       Weight weight)
    : font_(pango_font_description_new())
    , size_(pt_size * PANGO_SCALE * 96 / 72)
    , style_(style)
    , weight_(weight)
  {
    pango_font_description_set_family(font_, family.c_str());
    pango_font_description_set_absolute_size(font_, size_);

    if (weight_ == CairoFont::WEIGHT_BOLD)
    {
      pango_font_description_set_weight(font_, PANGO_WEIGHT_BOLD);
    }

    if (style_ == CairoFont::STYLE_ITALIC)
    {
      pango_font_description_set_style(font_, PANGO_STYLE_ITALIC);
    }
  }

  CairoFont::~CairoFont()
  {
    pango_font_description_free(font_);
  }

  NUX_IMPLEMENT_OBJECT_TYPE(TextEntry);

  TextEntry::TextEntry(const char* text, NUX_FILE_LINE_DECL)
    : View(NUX_FILE_LINE_PARAM)
    , _size_match_text(true)
    , _texture2D(nullptr)
    , canvas_(nullptr)
    , cached_layout_(nullptr)
    , preedit_attrs_(nullptr)
    , completion_color_(color::Gray)
    , last_dblclick_time_(0)
    , cursor_(0)
    , preedit_cursor_(0)
    , selection_bound_(0)
    , scroll_offset_x_(0)
    , scroll_offset_y_(0)
    , cursor_blink_timer_(0)
    , cursor_blink_status_(0)
    , visible_(true)
    , focused_(false)
    , need_im_reset_(false)
    , overwrite_(false)
    , select_words_(false)
    , select_lines_(false)
    , button_(false)
    , bold_(false)
    , underline_(false)
    , strikeout_(false)
    , italic_(false)
    , multiline_(false)
    , wrap_(false)
    , cursor_visible_(false)
    , readonly_(false)
    , content_modified_(false)
    , selection_changed_(false)
    , cursor_moved_(false)
    , update_canvas_(true)
    , font_family_("Ubuntu")
    , font_size_(12)
    , font_options_(cairo_font_options_create())
    , font_dpi_(96.0)
    , _text_color(color::White)
    , align_(CairoGraphics::ALIGN_LEFT)
#if defined(USE_X11)
    , caret_cursor_(None)
    , ime_(new IBusIMEContext(this))
#endif
    , ime_active_(false)
    , text_input_mode_(false)
    , key_nav_mode_(false)
    , lose_key_focus_on_key_nav_direction_up_(true)
    , lose_key_focus_on_key_nav_direction_down_(true)
    , cursor_visible_on_key_focus_only_(false)
  {
    cairo_font_options_set_antialias(font_options_, CAIRO_ANTIALIAS_SUBPIXEL);
    cairo_font_options_set_hint_style(font_options_, CAIRO_HINT_STYLE_FULL);
    cairo_font_options_set_hint_metrics(font_options_, CAIRO_HINT_METRICS_ON);
    cairo_font_options_set_subpixel_order(font_options_, CAIRO_SUBPIXEL_ORDER_RGB);

    mouse_down.connect(sigc::mem_fun(this, &TextEntry::RecvMouseDown));
    mouse_drag.connect(sigc::mem_fun(this, &TextEntry::RecvMouseDrag));
    mouse_up.connect(sigc::mem_fun(this, &TextEntry::RecvMouseUp));
    mouse_double_click.connect(sigc::mem_fun(this, &TextEntry::RecvMouseDoubleClick));
    mouse_enter.connect(sigc::mem_fun(this, &TextEntry::RecvMouseEnter));
    mouse_leave.connect(sigc::mem_fun(this, &TextEntry::RecvMouseLeave));

    key_down.connect(sigc::mem_fun(this, &TextEntry::RecvKeyEvent));
    key_up.connect([this] (unsigned int keysym, unsigned long, unsigned long state) {
      RecvKeyEvent(NUX_KEYUP, keysym, state, nullptr, 0);
    });

    geometry_changed.connect([this] (Area* /*area*/, Geometry& /*geo*/) {
      QueueRefresh(true, false);
    });

    begin_key_focus.connect(sigc::mem_fun(this, &TextEntry::RecvStartKeyFocus));
    end_key_focus.connect(sigc::mem_fun(this, &TextEntry::RecvEndKeyFocus));

    SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    SetText(text);

    SetAcceptKeyboardEvent(true);
    EnableDoubleClick(true);
    SetPasswordChar("*");
  }

  TextEntry::~TextEntry()
  {
    if (cursor_blink_timer_)
      g_source_remove(cursor_blink_timer_);

    cairo_font_options_destroy(font_options_);
    if (_texture2D)
      _texture2D->UnReference();

#if defined(USE_X11)
    if (ime_)
      delete ime_;
#endif
    delete canvas_;
    ResetLayout();
  }

  void TextEntry::PreLayoutManagement()
  {
    View::PreLayoutManagement();
  }

  long TextEntry::PostLayoutManagement(long layoutResult)
  {
    long result = View::PostLayoutManagement(layoutResult);
    MainDraw();
    return result;
  }

  void TextEntry::GeometryChanged(bool /*position_has_changed*/, bool /*size_has_changed*/)
  {
      update_canvas_ = true;
      View::GeometryChanged(true, true);
  }

  Area* TextEntry::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    Area* area = View::FindAreaUnderMouse(mouse_position, event_type);

    return area;
  }

  void TextEntry::ProcessMouseEvent(int event_type, int x, int y, int /* dx */, int /* dy */, unsigned long button_flags, unsigned long key_flags)
  {
    int X = static_cast<int>(x /*round(event.GetX())*/) - kInnerBorderX - scroll_offset_x_;
    int Y = static_cast<int>(y /*round(event.GetY())*/) - kInnerBorderY - scroll_offset_y_;
    int index = XYToTextIndex(X, Y);
    MouseButton button = GetEventButton(button_flags);

    if (event_type == NUX_MOUSE_PRESSED && (button == 2 || button == 3))
    {
      SetCursor(index);
#if defined(NUX_OS_LINUX)
      if (button == 2)
        PastePrimaryClipboard();
#endif
      QueueRefresh(false, true);

      return;
    }

    if (button != 1 && event_type != NUX_MOUSE_MOVE)
      return;

    int sel_start, sel_end;
    GetSelectionBounds(&sel_start, &sel_end);

    unsigned long long current_time = GetCurrentTime();

    if ((event_type == NUX_MOUSE_PRESSED) && (current_time - last_dblclick_time_ <= kTripleClickTimeout))
    {
      SelectLine();
    }
    else if (event_type == NUX_MOUSE_DOUBLECLICK && !ime_active_)
    {
      SelectWord();
      last_dblclick_time_ = current_time;
    }
    else if (event_type == NUX_MOUSE_PRESSED)
    {
      if (key_flags & NUX_STATE_SHIFT)
      {
        // If current click position is inside the selection range, then just
        // cancel the selection.
        if (index > sel_start && index < sel_end)
          SetCursor(index);
        else if (index <= sel_start)
          SetSelectionBounds(sel_end, index);
        else if (index >= sel_end)
          SetSelectionBounds(sel_start, index);
      }
      else
      {
        SetCursor(index);
      }
    }
    else if (event_type == NUX_MOUSE_MOVE)
    {
      SetSelectionBounds(selection_bound_, index);
    }

    QueueRefresh(false, true);
    //return EVENT_RESULT_HANDLED;
  }

  void TextEntry::ProcessKeyEvent(
    unsigned long    event_type,   /*event type*/
    unsigned long    keysym    ,   /*event keysym*/
    unsigned long    state     ,   /*event state*/
    const char*      character ,   /*character*/
    unsigned short   /* keyCount */      /*key repeat count*/)
  {
#if defined(USE_X11)
    if (im_running())
    {
      // FIXME Have to get the x11_keycode for ibus-hangul/korean input
      KeyCode keycode = XKeysymToKeycode(GetGraphicsDisplay()->GetX11Display(), keysym);
      KeyEvent event(static_cast<EventType>(event_type), keysym, keycode, state, character);

      if (ime_->FilterKeyEvent(event))
        return;
    }
#endif

    /* Ignore all the keyup events to make Composition and Dead keys to work,
     * as no one (IBus a part) needs them */
    if (event_type == NUX_KEYUP)
      return;

#if !defined(NO_X11)
    if (IsInCompositionMode() || IsInitialCompositionKeySym(keysym))
    {
      if (HandleComposition(keysym))
        return;
    }

#endif

    if (event_type == NUX_KEYDOWN)
      text_input_mode_ = true;

    cursor_blink_status_ = 4;

    if ((!multiline_) && (!lose_key_focus_on_key_nav_direction_up_) && (keysym == NUX_VK_UP))
    {
      // Ignore key navigation direction 'up' if we are not in multi-line.
      return;
    }

    if ((!multiline_) && (!lose_key_focus_on_key_nav_direction_up_) && (keysym == NUX_VK_DOWN))
    {
      // Ignore key navigation direction 'down' if we are not in multi-line.
      return;
    }

    // we need to ignore some characters
    if (keysym == NUX_VK_TAB)
      return;

    if ((keysym == NUX_VK_ENTER || keysym == NUX_KP_ENTER))
    {
      activated.emit();
      return;
    }

    unsigned int keyval = keysym;
    bool shift = (state & NUX_STATE_SHIFT);
    bool ctrl = (state & NUX_STATE_CTRL);
    bool handled = false;

    // DLOG("TextEntry::key_down(%d, shift:%d ctrl:%d)", keyval, shift, ctrl);
    if (event_type == NUX_KEYDOWN)
    {
      if (keyval == NUX_VK_LEFT)
      {
        if (!ctrl)
          MoveCursor(VISUALLY, -1, shift);
        else
          MoveCursor(WORDS, -1, shift);

        handled = true;
      }
      else if (keyval == NUX_VK_RIGHT)
      {
        if (!ctrl)
          MoveCursor(VISUALLY, 1, shift);
        else
          MoveCursor(WORDS, 1, shift);

        handled = true;
      }
      else if (keyval == NUX_VK_UP)
      {
        // move cursor to start of line
        MoveCursor(DISPLAY_LINES, -1, shift);
        handled = true;
      }
      else if (keyval == NUX_VK_DOWN)
      {
        // move cursor to end of line
        MoveCursor(DISPLAY_LINES, 1, shift);
        handled = true;
      }
      else if (keyval == NUX_VK_HOME)
      {
        if (!ctrl)
          MoveCursor(DISPLAY_LINE_ENDS, -1, shift);
        else
          MoveCursor(BUFFER, -1, shift);

        handled = true;
      }
      else if (keyval == NUX_VK_END)
      {
        if (!ctrl)
          MoveCursor(DISPLAY_LINE_ENDS, 1, shift);
        else
          MoveCursor(BUFFER, 1, shift);

        handled = true;
      }
      else if (keyval == NUX_VK_PAGE_UP)
      {
        if (!ctrl)
          MoveCursor(PAGES, -1, shift);
        else
          MoveCursor(BUFFER, -1, shift);

        handled = true;
      }
      else if (keyval == NUX_VK_PAGE_DOWN)
      {
        if (!ctrl)
          MoveCursor(PAGES, 1, shift);
        else
          MoveCursor(BUFFER, 1, shift);

        handled = true;
      }
      else if (((keyval == NUX_VK_x || keyval == NUX_VK_X) && ctrl && !shift) ||
               ((keyval == NUX_VK_DELETE) && shift && !ctrl))
      {
        CutClipboard();
        handled = true;
      }
      else if (((keyval == NUX_VK_c || keyval == NUX_VK_C) && ctrl && !shift) ||
               ((keyval == NUX_VK_INSERT) && ctrl && !shift))
      {
        CopyClipboard();
        handled = true;
      }
      else if (((keyval == NUX_VK_v || keyval == NUX_VK_V) && ctrl && !shift) ||
               ((keyval == NUX_VK_INSERT) && shift && !ctrl))
      {
        PasteClipboard();
        handled = true;
      }
      else if ((keyval == NUX_VK_a || keyval == NUX_VK_A) && ctrl)
      {
        SelectAll();
        handled = true;
      }
      else if (keyval == NUX_VK_BACKSPACE)
      {
        if (!ctrl)
          BackSpace(VISUALLY);
        else
          BackSpace(WORDS);

        handled = true;
      }
      else if ((keyval == NUX_VK_DELETE) && (!shift))
      {
        if (!ctrl)
          Delete(VISUALLY);
        else
          Delete(WORDS);

        handled = true;
      }
      else if ((keyval == NUX_VK_INSERT) && (!shift) && (!ctrl))
      {
        ToggleOverwrite();
        handled = true;
      }
//       else
//       {
//         return EVENT_RESULT_UNHANDLED;
//       }
    }
    else
    { // EVENT_KEY_PRESS
//       if (keyval == GDK_Return || keyval == GDK_KP_Enter)
//       {
//         // If multiline_ is unset, just ignore new_line.
//         if (multiline_)
//           EnterText("\n");
//         else
//           return;
//       }
//       else
//       {
//         return;
//       }
    }

    if (!handled && character)
    {
      unsigned int utf_char = g_utf8_get_char(character);

      if (g_unichar_isprint(utf_char))
        EnterText(character);
    }

    QueueRefresh(false, true);
  }

  void TextEntry::RecvMouseDoubleClick(int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    ProcessMouseEvent(NUX_MOUSE_DOUBLECLICK, x, y, 0, 0, button_flags, key_flags);
  }

  void TextEntry::RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    ProcessMouseEvent(NUX_MOUSE_RELEASED, x, y, 0, 0, button_flags, key_flags);
  }

  void TextEntry::RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    ProcessMouseEvent(NUX_MOUSE_PRESSED, x, y, 0, 0, button_flags, key_flags);
  }

  void TextEntry::RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    ProcessMouseEvent(NUX_MOUSE_MOVE, x, y, dx, dy, button_flags, key_flags);
  }

  void TextEntry::RecvMouseEnter(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
#if defined(USE_X11)
    if (caret_cursor_ == None)
    {
      Display* display = nux::GetGraphicsDisplay()->GetX11Display();
      nux::BaseWindow* window = static_cast<nux::BaseWindow*>(GetTopLevelViewWindow());

      if (display && window)
      {
        caret_cursor_ = XCreateFontCursor(display, XC_xterm);
        XDefineCursor(display, window->GetInputWindowId(), caret_cursor_);
      }
    }
#endif
  }

  void TextEntry::RecvMouseLeave(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
#if defined(USE_X11)
    if (caret_cursor_ != None)
    {
      Display* display = nux::GetGraphicsDisplay()->GetX11Display();
      nux::BaseWindow* window = static_cast<nux::BaseWindow*>(GetTopLevelViewWindow());

      if (display && window)
      {
        XUndefineCursor(display, window->GetInputWindowId());
        XFreeCursor(display, caret_cursor_);
        caret_cursor_ = None;
      }
    }
#endif
  }

  void TextEntry::RecvKeyEvent(
    unsigned long    eventType  ,   /*event type*/
    unsigned long    keysym     ,   /*event keysym*/
    unsigned long    state      ,   /*event state*/
    const char*     character  ,   /*character*/
    unsigned short   keyCount       /*key repeat count*/)
  {
    ProcessKeyEvent(eventType, keysym, state, character, keyCount);
  }

  void TextEntry::RecvStartKeyFocus()
  {
    key_nav_mode_     = true;
    text_input_mode_  = false;
    composition_list_.clear();

    FocusInx();
  }

  void TextEntry::RecvEndKeyFocus()
  {
    key_nav_mode_     = false;
    text_input_mode_  = false;
    composition_list_.clear();

    FocusOutx();
  }

  void TextEntry::Draw(GraphicsEngine& gfxContext, bool /* forceDraw */)
  {
    MainDraw();
    Geometry base = GetGeometry();

    gfxContext.PushClippingRectangle(base);

    nux::GetPainter().PaintBackground(gfxContext, base);

    Color col = color::Black;
    col.alpha = 0;
    gfxContext.QRP_Color(base.x,
      base.y,
      base.width,
      base.height,
      col);

    TexCoordXForm texxform;
    texxform.SetWrap(TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    texxform.SetTexCoordType(TexCoordXForm::OFFSET_COORD);
    gfxContext.QRP_1Tex(base.x,
      base.y,
      base.width,
      base.height,
      _texture2D->GetDeviceTexture(),
      texxform,
      _text_color);

    gfxContext.PopClippingRectangle();
  }

  void TextEntry::DrawContent(GraphicsEngine& /* gfxContext */, bool /* forceDraw */)
  {
    //MainDraw();
  }

  TextEntry::SearchState TextEntry::GetCompositionForList(std::vector<unsigned long> const& input, std::string& composition)
  {
    SearchState search_state = SearchState::NO_MATCH;

#if defined(NUX_OS_LINUX)
    if (input.size() >= ComposeSequence::MAX_SYMBOLS)
      return search_state;

    for (unsigned i = 0; i < COMPOSE_SEQUENCES_SIZE; ++i)
    {
      int j = 0;
      bool valid_seq = false;
      ComposeSequence const& seq = COMPOSE_SEQUENCES[i];

      for (auto key : input)
      {
        if (key != seq.symbols[j])
        {
          if (seq.symbols[j] != XK_VoidSymbol)
            valid_seq = false;

          break;
        }

        valid_seq = true;
        ++j;
      }

      if (valid_seq)
      {
        if (seq.symbols[j] == XK_VoidSymbol)
        {
          search_state = SearchState::MATCH;
          composition = seq.result;
          return search_state;
        }
        else
        {
          search_state = SearchState::PARTIAL;
        }
      }
    }
#endif

    return search_state;
  }

  bool TextEntry::IsInCompositionMode() const
  {
    return !composition_list_.empty();
  }

  bool TextEntry::IsInitialCompositionKeySym(unsigned long keysym) const
  {
#if defined(USE_X11)
    /* Checks if a keysym is a valid initial composition key */
    if (keysym == XK_Multi_key ||
        (keysym >= XK_dead_grave && keysym <= XK_dead_currency) ||
        keysym == XK_Greek_accentdieresis)
    {
      return true;
    }
#endif
    return false;
  }

  bool TextEntry::HandleComposition(unsigned long keysym)
  {
#if defined(USE_X11)
    bool composition_mode = IsInCompositionMode();

    if (composition_mode && IsModifierKey(keysym))
      return true;

    composition_list_.push_back(keysym);
    std::string composition_match;
    auto match = GetCompositionForList(composition_list_, composition_match);

    if (match == SearchState::PARTIAL)
    {
      return true;
    }
    else if (match == SearchState::MATCH)
    {
      EnterText(composition_match.c_str());
      composition_list_.clear();
      QueueRefresh(false, true);
      return true;
    }
    else
    {
      composition_list_.clear();
      return composition_mode;
    }
#endif

    return false;
  }

  void TextEntry::SetText(const char* text)
  {
    const char* end = NULL;
    g_utf8_validate(text, -1, &end);

    std::string txt((text && *text && end > text) ? std::string(text, end) : "");
    if (txt == text_)
      return; // prevent some redraws

    text_ = multiline_ ? txt : CleanupLineBreaks(txt.c_str());
    cursor_ = 0;
    selection_bound_ = 0;
    need_im_reset_ = true;
    //ResetImContext();

    text_input_mode_ = true;
    QueueRefresh(true, true);
    text_changed.emit(this);
  }

  std::string const& TextEntry::GetText() const
  {
    return text_;
  }

  std::string TextEntry::GetTextSelection() const
  {
    if (text_.size() == 0)
    {
      return std::string("");
    }

    int selection_start = 0;
    int selection_end = 0;
    if (GetSelectionBounds(&selection_start, &selection_end))
    {
      return text_.substr(selection_start, selection_end);
    }
    else
    {
      return std::string("");
    }
  }

  void TextEntry::SetCompletion(const char* text)
  {
    const char* end = NULL;
    g_utf8_validate(text, -1, &end);
    std::string txt((text && *text && end > text) ? std::string(text, end) : "");
    if (txt == completion_)
      return;

    completion_ = txt;
    QueueRefresh(true, true);
  }

  std::string const& TextEntry::GetCompletion() const
  {
    return completion_;
  }

  void TextEntry::SetCompletionColor(const Color &color)
  {
    completion_color_ = color;
    QueueRefresh(true, true);
  }

  Color const& TextEntry::GetCompletionColor() const
  {
    return completion_color_;
  }

  void TextEntry::SetTextColor(const Color &text_color)
  {
    if (_text_color != text_color)
    {
      _text_color = text_color;
      QueueRefresh(true, true);
    }
  }

  Color TextEntry::GetTextColor() const
  {
    return _text_color;
  }

  void TextEntry::MainDraw()
  {

    CairoGraphics* edit_canvas = EnsureCanvas();

    if (update_canvas_ || !last_selection_region_.empty() || !selection_region_.empty())
    {
      edit_canvas->PushState();
      DrawText(edit_canvas);
      edit_canvas->PopState();
    }

//     if (background_)
//       background_->Draw(canvas, 0, 0, GetBaseWidth, GetBaseHeight);

    CairoGraphics* final_canvas = new CairoGraphics(CAIRO_FORMAT_ARGB32, edit_canvas->GetWidth(), edit_canvas->GetHeight());

    final_canvas->PushState();
    final_canvas->IntersectRectClipRegion(kInnerBorderX,
      kInnerBorderY,
      GetBaseWidth() - kInnerBorderX,
      GetBaseHeight() - kInnerBorderY);
    final_canvas->DrawCanvas(0, 0, edit_canvas);
    final_canvas->PopState();
    DrawCursor(final_canvas);

    update_canvas_ = false;
    last_selection_region_ = selection_region_;
    last_cursor_region_ = cursor_region_;

    NBitmapData* bitmap = final_canvas->GetBitmap();
    delete final_canvas;

    if (!_texture2D || _texture2D->GetWidth() != bitmap->GetWidth() || _texture2D->GetHeight() != bitmap->GetHeight())
    {
      if (_texture2D)
        _texture2D->UnReference();
      _texture2D = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture();
    }

    _texture2D->Update(bitmap);
    delete bitmap;
  }

  void TextEntry::FocusInx()
  {
    if (!focused_)
    {
      focused_ = true;
      if (!readonly_ /*&& im_context_*/)
      {
        need_im_reset_ = true;
#if defined(USE_X11)
        ime_->Focus();
        nux::GetWindowThread()->XICFocus(this);
#endif
        //gtk_im_context_focus_in(im_context_);
        //UpdateIMCursorLocation();
      }
      cursor_visible_ = true; // show cursor when getting focus
      selection_changed_ = true;
      cursor_moved_ = true;
      // Don't adjust scroll.
      QueueRefresh(true, false);
    }
  }

  void TextEntry::FocusOutx()
  {
    if (focused_)
    {
      focused_ = false;
      if (!readonly_ /*&& im_context_*/)
      {
        need_im_reset_ = true;
#if defined(USE_X11)
        ime_->Blur();
        nux::GetWindowThread()->XICUnFocus();
#endif
        //gtk_im_context_focus_out(im_context_);
      }
      cursor_visible_ = false; // hide cursor when losing focus
      selection_changed_ = true;
      cursor_moved_ = true;

      if (cursor_visible_on_key_focus_only_ && cursor_blink_timer_)
      {
        g_source_remove(cursor_blink_timer_);
        cursor_blink_timer_ = 0;
      }

      // Don't adjust scroll.
      QueueRefresh(true, false);
    }
  }

  CairoGraphics* TextEntry::EnsureCanvas()
  {
    if (canvas_)
    {
      if ((GetBaseWidth() == canvas_->GetWidth()) && (GetBaseHeight() == canvas_->GetHeight()))
      {
        return canvas_;
      }
      else
      {
        nuxDebugMsg("[TextEntry::EnsureCanvas] Recreate canvas");
        delete canvas_;
        canvas_ = NULL;
      }
    }
    canvas_ = new CairoGraphics(CAIRO_FORMAT_ARGB32, GetBaseWidth(), GetBaseHeight());
    nuxAssert(canvas_);
    return canvas_;
  }

  void TextEntry::AdjustScroll()
  {
    int old_offset_x = scroll_offset_x_;
    int old_offset_y = scroll_offset_y_;
    int display_width = GetBaseWidth() - kInnerBorderX * 2;
    int display_height = GetBaseHeight() - kInnerBorderY * 2;

    PangoLayout* layout = EnsureLayout();
    int text_width, text_height;
    pango_layout_get_pixel_size(layout, &text_width, &text_height);

    int strong_x, strong_y, strong_height;
    int weak_x, weak_y, weak_height;
    GetCursorLocationInLayout(&strong_x, &strong_y, &strong_height,
      &weak_x, &weak_y, &weak_height);

    if (!wrap_ && display_width > text_width)
    {
      PangoAlignment align = pango_layout_get_alignment(layout);
      if (align == PANGO_ALIGN_RIGHT)
        scroll_offset_x_ = display_width - text_width;
      else if (align == PANGO_ALIGN_CENTER)
        scroll_offset_x_ = (display_width - text_width) / 2;
      else
        scroll_offset_x_ = 0;
    }
    else
    {
      if (scroll_offset_x_ + strong_x < 0)
        scroll_offset_x_ = -strong_x;
      else if (scroll_offset_x_ + strong_x > display_width)
        scroll_offset_x_ = display_width - strong_x;

      if (std::abs(weak_x - strong_x) < display_width)
      {
        if (scroll_offset_x_ + weak_x < 0)
          scroll_offset_x_ = - weak_x;
        else if (scroll_offset_x_ + weak_x > display_width)
          scroll_offset_x_ = display_width - weak_x;
      }
    }

    if (display_height > text_height)
    {
      scroll_offset_y_ = 0;
    }
    else
    {
      if (scroll_offset_y_ + strong_y + strong_height > display_height)
        scroll_offset_y_ = display_height - strong_y - strong_height;
      if (scroll_offset_y_ + strong_y < 0)
        scroll_offset_y_ = -strong_y;
    }

    if (old_offset_x != scroll_offset_x_ || old_offset_y != scroll_offset_y_)
      content_modified_ = true;
  }

  void TextEntry::QueueRefresh(bool relayout, bool adjust_scroll)
  {
    if (relayout)
      ResetLayout();

    if (adjust_scroll)
      AdjustScroll();

    QueueTextDraw();
    QueueCursorBlink();
  }

  void TextEntry::ResetImContext()
  {
    if (need_im_reset_)
    {
      need_im_reset_ = false;
//       if (im_context_)
//         gtk_im_context_reset(im_context_);
      ResetPreedit();
    }
  }

  void TextEntry::ResetPreedit() {
    // Reset layout if there were some content in preedit string
    if (preedit_.length())
      ResetLayout();

    preedit_.clear();
    preedit_cursor_ = 0;
    if (preedit_attrs_) {
      pango_attr_list_unref(preedit_attrs_);
      preedit_attrs_ = NULL;
    }
  }

  void TextEntry::PreeditStarted()
  {
    ime_active_ = true;
  }

  void TextEntry::UpdatePreedit(std::string const& preedit, int cursor)
  {
    preedit_ = preedit;
    preedit_cursor_ = cursor;
    QueueRefresh(true, true);
  }

  void TextEntry::UpdatePreeditAttribs(PangoAttrList* list)
  {
    if (preedit_attrs_)
    {
      pango_attr_list_unref(preedit_attrs_);
      preedit_attrs_ = NULL;
    }

    preedit_attrs_ = list;
  }

  void TextEntry::ClearPreedit()
  {
    ResetPreedit();
    QueueRefresh(true, true);
  }

  void TextEntry::DrawText(CairoGraphics *canvas)
  {
    PangoLayout *layout = EnsureLayout();

    bool redraw_text = false;
    if (update_canvas_)
    {
      canvas->ClearCanvas();
      canvas->PushState();
      redraw_text = true;
    }
    else if (!last_selection_region_.empty())
    {
      //last_selection_region_.Integerize();
      canvas->PushState();
      canvas->IntersectGeneralClipRegion(last_selection_region_);
      canvas->ClearRect(0, 0, GetBaseWidth(), GetBaseHeight());
      redraw_text = true;
    }

    if (redraw_text)
    {
      cairo_set_source_rgb(canvas->GetInternalContext(),
        _text_color.red,
        _text_color.green,
        _text_color.blue);

      cairo_move_to(canvas->GetInternalContext(),
        scroll_offset_x_ + kInnerBorderX,
        scroll_offset_y_ + kInnerBorderY);

      pango_cairo_show_layout(canvas->GetInternalContext(), layout);

      canvas->PopState();
    }

    // Draw selection background.
    // Selection in a single line may be not continual, so we use pango to
    // get the x-ranges of each selection range in one line, and draw them
    // separately.
    if (!selection_region_.empty())
    {
      canvas->PushState();
      //selection_region_.Integerize();
      canvas->IntersectGeneralClipRegion(selection_region_);

      Color selection_color = GetSelectionBackgroundColor();
      Color text_color = GetSelectionTextColor();

      cairo_set_source_rgb(canvas->GetInternalContext(),
        selection_color.red,
        selection_color.green,
        selection_color.blue);
      cairo_paint(canvas->GetInternalContext());

      cairo_move_to(canvas->GetInternalContext(),
        scroll_offset_x_ + kInnerBorderX,
        scroll_offset_y_ + kInnerBorderY);
      cairo_set_source_rgb(canvas->GetInternalContext(),
        text_color.red,
        text_color.green,
        text_color.blue);
      pango_cairo_show_layout(canvas->GetInternalContext(), layout);
      canvas->PopState();
    }
  }

  bool TextEntry::CursorBlinkCallback(TextEntry *self)
  {
    if (self->cursor_blink_status_)
      self->ShowCursor();
    else
      self->HideCursor();

    if (--self->cursor_blink_status_ < 0)
      self->cursor_blink_status_ = 2;

    return true;
  }

  void TextEntry::QueueCursorBlink()
  {
    if (cursor_visible_on_key_focus_only_ && !focused_)
      return;

    if (!cursor_blink_timer_)
      cursor_blink_timer_ = g_timeout_add(kCursorBlinkTimeout,
                                          (GSourceFunc)&CursorBlinkCallback,
                                          this);
  }

  void TextEntry::ShowCursor()
  {
    if (!cursor_visible_)
    {
      cursor_visible_ = true;
      if (focused_ && !readonly_)
      {
        cursor_moved_ = true;
        QueueRefresh(false, false);
      }
    }
  }

  void TextEntry::HideCursor()
  {
    if (cursor_visible_)
    {
      cursor_visible_ = false;
      if (focused_ && !readonly_)
      {
        cursor_moved_ = true;
        QueueRefresh(false, false);
      }
    }
  }

  void TextEntry::GetCursorRects(Rect *strong, Rect *weak)
  {
    int strong_x, strong_y, strong_height;
    int weak_x, weak_y, weak_height;
    GetCursorLocationInLayout(&strong_x, &strong_y, &strong_height,
      &weak_x, &weak_y, &weak_height);

    strong->x =
      strong_x + kInnerBorderX + scroll_offset_x_ - kStrongCursorBarWidth;
    strong->width = kStrongCursorBarWidth * 2;
    strong->y = strong_y + kInnerBorderY + scroll_offset_y_;
    strong->height = strong_height;

    if (weak_x != strong_x)
    {
      weak->x = weak_x+ kInnerBorderX + scroll_offset_x_ - kWeakCursorBarWidth;
      weak->width = kWeakCursorBarWidth * 2;
      weak->y = weak_y+ kInnerBorderY + scroll_offset_y_;
      weak->height = weak_height;
    }
    else
    {
      *weak = *strong;
    }
  }

  void TextEntry::UpdateCursorRegion()
  {
    cursor_region_.clear();

    Rect strong, weak;
    GetCursorRects(&strong, &weak);

    cursor_region_.push_back(strong);
    cursor_region_.push_back(weak);
  }


  void TextEntry::DrawCursor(CairoGraphics *canvas)
  {
    if (!cursor_visible_)
      return;

    int strong_x, strong_y, strong_height;
    int weak_x, weak_y, weak_height;
    GetCursorLocationInLayout(&strong_x, &strong_y, &strong_height,
      &weak_x, &weak_y, &weak_height);

    // Draw strong cursor.
    // 0.5 is for cairo drawing between the grid
    canvas->DrawLine(strong_x + kInnerBorderX + scroll_offset_x_ + 0.5,
                     strong_y + kInnerBorderY + scroll_offset_y_,
                     strong_x + kInnerBorderX + scroll_offset_x_ + 0.5,
                     strong_y + strong_height + kInnerBorderY + scroll_offset_y_,
                     kStrongCursorLineWidth, kStrongCursorColor);
    // Draw a small arror towards weak cursor
    if (strong_x > weak_x)
    {
      canvas->DrawLine(
        strong_x + kInnerBorderX + scroll_offset_x_ - kStrongCursorBarWidth,
        strong_y + kInnerBorderY + scroll_offset_y_ + kStrongCursorLineWidth,
        strong_x + kInnerBorderX + scroll_offset_x_,
        strong_y + kInnerBorderY + scroll_offset_y_ + kStrongCursorLineWidth,
        kStrongCursorLineWidth, kStrongCursorColor);
    }
    else if (strong_x < weak_x)
    {
      canvas->DrawLine(
        strong_x + kInnerBorderX + scroll_offset_x_,
        strong_y + kInnerBorderY + scroll_offset_y_ + kStrongCursorLineWidth,
        strong_x + kInnerBorderX + scroll_offset_x_ + kStrongCursorBarWidth,
        strong_y + kInnerBorderY + scroll_offset_y_ + kStrongCursorLineWidth,
        kStrongCursorLineWidth, kStrongCursorColor);
    }

    if (strong_x != weak_x )
    {
      // Draw weak cursor.
      canvas->DrawLine(weak_x + kInnerBorderX + scroll_offset_x_,
        weak_y + kInnerBorderY + scroll_offset_y_,
        weak_x + kInnerBorderX + scroll_offset_x_,
        weak_y + weak_height + kInnerBorderY + scroll_offset_y_,
        kWeakCursorLineWidth, kWeakCursorColor);
      // Draw a small arror towards strong cursor
      if (weak_x > strong_x)
      {
        canvas->DrawLine(
          weak_x + kInnerBorderX + scroll_offset_x_ - kWeakCursorBarWidth,
          weak_y + kInnerBorderY + scroll_offset_y_ + kWeakCursorLineWidth,
          weak_x + kInnerBorderX + scroll_offset_x_,
          weak_y + kInnerBorderY + scroll_offset_y_ + kWeakCursorLineWidth,
          kWeakCursorLineWidth, kWeakCursorColor);
      }
      else
      {
        canvas->DrawLine(
          weak_x + kInnerBorderX + scroll_offset_x_,
          weak_y + kInnerBorderY + scroll_offset_y_ + kWeakCursorLineWidth,
          weak_x + kInnerBorderX + scroll_offset_x_ + kWeakCursorBarWidth,
          weak_y + kInnerBorderY + scroll_offset_y_ + kWeakCursorLineWidth,
          kWeakCursorLineWidth, kWeakCursorColor);
      }
    }
  }

  Color TextEntry::GetSelectionBackgroundColor()
  {
    return kDefaultSelectionBackgroundColor;
  }

  Color TextEntry::GetSelectionTextColor()
  {
    return kDefaultSelectionTextColor;
  }

  void TextEntry::GetCursorLocationInLayout(int *strong_x, int *strong_y,
    int *strong_height,
    int *weak_x, int *weak_y,
    int *weak_height)
  {
    PangoLayout *layout = EnsureLayout();
    int cursor_index = TextIndexToLayoutIndex(cursor_, true);

    PangoRectangle strong, weak;
    pango_layout_get_cursor_pos(layout, cursor_index, &strong, &weak);

    if (strong_x)
      *strong_x = PANGO_PIXELS(strong.x);
    if (strong_y)
      *strong_y = PANGO_PIXELS(strong.y);
    if (strong_height)
      *strong_height = PANGO_PIXELS(strong.height);
    if (weak_x)
      *weak_x = PANGO_PIXELS(weak.x);
    if (weak_y)
      *weak_y = PANGO_PIXELS(weak.y);
    if (weak_height)
      *weak_height = PANGO_PIXELS(weak.height);
  }

  PangoLayout* TextEntry::EnsureLayout()
  {
    if (!cached_layout_)
    {
      cached_layout_ = CreateLayout();
    }
    return cached_layout_;
  }

  void TextEntry::QueueTextDraw()
  {
    if (content_modified_)
    {
      UpdateSelectionRegion();
      UpdateCursorRegion();
      QueueDraw(); //owner->QueueDraw();
      content_modified_ = false;
      update_canvas_ = true;
    }
    else
    {
      if (selection_changed_)
      {
        UpdateSelectionRegion();
        if (!last_selection_region_.empty())
          QueueDraw(); //owner_->QueueDrawRegion(last_selection_region_);
        if (!selection_region_.empty())
          QueueDraw(); //owner_->QueueDrawRegion(selection_region_);
        selection_changed_ = false;
      }
      if (cursor_moved_)
      {
        UpdateCursorRegion();
        if (!last_cursor_region_.empty())
          QueueDraw(); //owner_->QueueDrawRegion(last_cursor_region_);
        if (!cursor_region_.empty())
          QueueDraw(); //owner_->QueueDrawRegion(cursor_region_);
        cursor_moved_ = false;
      }
    }
  }

  void TextEntry::ResetLayout()
  {
    if (cached_layout_)
    {
      g_object_unref(cached_layout_);
      cached_layout_ = NULL;
      content_modified_ = true;
    }
  }

  PangoLayout* TextEntry::CreateLayout()
  {
    // Creates the pango layout with a temporary canvas that is not zoomed.
    CairoGraphics *canvas = new CairoGraphics(CAIRO_FORMAT_ARGB32, 1, 1);
    PangoLayout *layout = pango_cairo_create_layout(canvas->GetInternalContext());
    delete canvas;
    PangoAttrList *tmp_attrs = pango_attr_list_new();
    std::string tmp_string;

    /* Set necessary parameters */
    pango_cairo_context_set_font_options(pango_layout_get_context(layout),
                                          font_options_);
    pango_cairo_context_set_resolution(pango_layout_get_context(layout),
                                        font_dpi_);

    if (wrap_)
    {
      pango_layout_set_width(layout, (GetBaseWidth() - kInnerBorderX * 2) * PANGO_SCALE);
      pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
    }
    else
    {
      pango_layout_set_width(layout, -1);
    }

    pango_layout_set_single_paragraph_mode(layout, !multiline_);

    if (preedit_.length())
    {
      size_t cursor_index = static_cast<size_t>(cursor_);
      size_t text_length = text_.length();
      size_t preedit_length = preedit_.length();
      if (visible_)
      {
        tmp_string = text_;
        tmp_string.insert(cursor_index, preedit_);
      }
      else
      {
        size_t nchars = g_utf8_strlen(text_.c_str(), text_length);
        size_t preedit_nchars = g_utf8_strlen(preedit_.c_str(), preedit_length);
        nchars += preedit_nchars;
        tmp_string.reserve(password_char_.length() * nchars);
        for (size_t i = 0; i < nchars; ++i)
          tmp_string.append(password_char_);
        size_t cursor_offset =
            g_utf8_pointer_to_offset(text_.c_str(), text_.c_str() + cursor_index);
        /* Fix cursor index and preedit_length */
        cursor_index = cursor_offset * password_char_.length();
        preedit_length = preedit_nchars * password_char_.length();
      }
      if (preedit_attrs_)
        pango_attr_list_splice(tmp_attrs, preedit_attrs_,
                               static_cast<int>(cursor_index),
                               static_cast<int>(preedit_length));
    }
    else
    {
      if (visible_)
      {
        tmp_string = text_;
      }
      else
      {
        size_t nchars = g_utf8_strlen(text_.c_str(), text_.length());
        tmp_string.reserve(password_char_.length() * nchars);
        for (size_t i = 0; i < nchars; ++i)
          tmp_string.append(password_char_);
      }
    }

    int pre_completion_length = tmp_string.length();

    if (!completion_.empty() && !wrap_ && preedit_.empty())
    {
      tmp_string = text_ + completion_;
    }

    pango_layout_set_text(layout, tmp_string.c_str(),
                          static_cast<int>(tmp_string.length()));

    /* Set necessary attributes */
    PangoAttribute *attr;
    if (underline_)
    {
      attr = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
      attr->start_index = 0;
      attr->end_index = static_cast<guint>(pre_completion_length);
      pango_attr_list_insert(tmp_attrs, attr);
    }
    if (strikeout_)
    {
      attr = pango_attr_strikethrough_new(TRUE);
      attr->start_index = 0;
      attr->end_index = static_cast<guint>(pre_completion_length);
      pango_attr_list_insert(tmp_attrs, attr);
    }
    if (!completion_.empty() && !wrap_)
    {
      attr = pango_attr_foreground_new(65535 * completion_color_.red,
                                       65535 * completion_color_.green,
                                       65535 * completion_color_.blue);
      attr->start_index = static_cast<guint>(pre_completion_length);
      attr->end_index = static_cast<guint>(tmp_string.length());
      pango_attr_list_insert(tmp_attrs, attr);
    }
    /* Set font desc */
    {
      /* safe to down_cast here, because we know the actual implementation. */
      CairoFont *font = new CairoFont(
              font_family_.empty() ? kDefaultFontName : font_family_.c_str(),
              font_size_,
              italic_ ? CairoFont::STYLE_ITALIC : CairoFont::STYLE_NORMAL,
              bold_ ? CairoFont::WEIGHT_BOLD : CairoFont::WEIGHT_NORMAL);
      nuxAssert(font);
      attr = pango_attr_font_desc_new(font->GetFontDescription());
      attr->start_index = 0;
      attr->end_index = static_cast<unsigned int>(tmp_string.length());
      pango_attr_list_insert(tmp_attrs, attr);
      pango_layout_set_font_description(layout, font->GetFontDescription());
      font->Destroy();
    }
    pango_layout_set_attributes(layout, tmp_attrs);
    pango_attr_list_unref(tmp_attrs);

    /* Set alignment according to text direction. Only set layout's alignment
     * when it's not wrapped and in single line mode.
     */
    if (!wrap_ && pango_layout_get_line_count(layout) <= 1 &&
        align_ != CairoGraphics::ALIGN_CENTER)
    {
      PangoDirection dir;
      if (visible_)
        dir = pango_find_base_dir(tmp_string.c_str(),
                                  static_cast<int>(tmp_string.length()));
      else
        dir = PANGO_DIRECTION_NEUTRAL;

      if (dir == PANGO_DIRECTION_NEUTRAL)
      {
//         GtkWidget *widget = GetWidgetAndCursorLocation(NULL);
//         if (widget && gtk_widget_get_direction(widget) == GTK_TEXT_DIR_RTL)
//           dir = PANGO_DIRECTION_RTL;
//         else
//           dir = PANGO_DIRECTION_LTR;

        dir = PANGO_DIRECTION_LTR;
      }

      // If wordWrap is false then "justify" alignment has no effect.
      PangoAlignment pango_align = (align_ == CairoGraphics::ALIGN_RIGHT ?
                                    PANGO_ALIGN_RIGHT : PANGO_ALIGN_LEFT);

      // Invert the alignment if text direction is right to left.
      if (dir == PANGO_DIRECTION_RTL)
      {
        pango_align = (align_ == CairoGraphics::ALIGN_RIGHT ?
                       PANGO_ALIGN_LEFT : PANGO_ALIGN_RIGHT);
      }

      pango_layout_set_alignment(layout, pango_align);
      pango_layout_set_justify(layout, FALSE);
    }
    else if (align_ == CairoGraphics::ALIGN_JUSTIFY)
    {
      pango_layout_set_justify(layout, TRUE);
      pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
    }
    else if (align_ == CairoGraphics::ALIGN_RIGHT)
    {
      pango_layout_set_justify(layout, FALSE);
      pango_layout_set_alignment(layout, PANGO_ALIGN_RIGHT);
    }
    else if (align_ == CairoGraphics::ALIGN_CENTER)
    {
      pango_layout_set_justify(layout, FALSE);
      pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
    }
    else
    {
      pango_layout_set_justify(layout, FALSE);
      pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
    }

    {
      PangoContext *context;
      PangoFontMetrics *metrics;
      int ascent, descent;

      context = pango_layout_get_context(layout);
      metrics = pango_context_get_metrics(context,
                                           pango_layout_get_font_description(layout),
                                           pango_context_get_language(context));

      ascent = pango_font_metrics_get_ascent(metrics);
      descent = pango_font_metrics_get_descent(metrics);

      int full_height = PANGO_PIXELS(ascent + descent) + (kInnerBorderY * 2);
      SetMinimumHeight(full_height);

      pango_font_metrics_unref(metrics);
    }

    return layout;
  }

  int TextEntry::TextIndexToLayoutIndex(int text_index, bool consider_preedit_cursor)
  {
      if (visible_)
      {
        if (text_index < cursor_)
          return text_index;

        if (text_index == cursor_ && consider_preedit_cursor)
          return text_index + preedit_cursor_;

        return text_index + static_cast<int>(preedit_.length());
      }

      const char *text = text_.c_str();
      int offset = static_cast<int>(
        g_utf8_pointer_to_offset(text, text + text_index));
      int preedit_offset = 0;
      int preedit_chars = 0;
      if (preedit_.length())
      {
        const char *preedit_text = preedit_.c_str();
        preedit_offset = static_cast<int>(g_utf8_pointer_to_offset(
          preedit_text, preedit_text + preedit_cursor_));
        preedit_chars = static_cast<int>(g_utf8_strlen(
          preedit_text, preedit_.length()));
      }

      int password_char_length = static_cast<int>(password_char_.length());

      if (text_index < cursor_)
        return offset * password_char_length;

      if (text_index == cursor_ && consider_preedit_cursor)
        return (offset + preedit_offset) * password_char_length;

      return (offset + preedit_chars) * password_char_length;
  }


  int TextEntry::LayoutIndexToTextIndex(int layout_index)
  {
    if (visible_)
    {
      if (layout_index < cursor_)
        return layout_index;

      int preedit_length = static_cast<int>(preedit_.length());
      if (layout_index >= cursor_ + preedit_length)
        return layout_index - preedit_length;

      return cursor_;
    }

    int password_char_length = static_cast<int>(password_char_.length());
    nuxAssert(layout_index % password_char_length == 0);

    int offset = layout_index / password_char_length;

    const char *text = text_.c_str();
    int cursor_offset = static_cast<int>(
      g_utf8_pointer_to_offset(text, text + cursor_));
    int preedit_chars = static_cast<int>(
      g_utf8_strlen(preedit_.c_str(), preedit_.length()));

    if (offset < cursor_offset)
      return static_cast<int>(g_utf8_offset_to_pointer(text, offset) - text);

    if (offset >= cursor_offset + preedit_chars)
      return static_cast<int>(
      g_utf8_offset_to_pointer(text, offset - preedit_chars) - text);

    return cursor_;
  }

  int TextEntry::GetCharLength(int index)
  {
    const char *text = text_.c_str();
    const char *ptr = text + index;
    const char *end = text + text_.length();
    const char *next = g_utf8_find_next_char(ptr, end);
    return static_cast<int>(next ? static_cast<int>(next - ptr) : end - ptr);
  }

  int TextEntry::GetPrevCharLength(int index)
  {
    const char *text = text_.c_str();
    const char *ptr = text + index;
    const char *prev = g_utf8_find_prev_char(text, ptr);
    return static_cast<int>(prev ? static_cast<int>(ptr - prev) : ptr - text);
  }

  void TextEntry::EnterText(const char *str)
  {
    if (readonly_ || !str || !*str) return;

    if (GetSelectionBounds(NULL, NULL))
    {
      DeleteSelection();
    }
    else if (overwrite_ && cursor_ != static_cast<int>(text_.length()))
    {
      DeleteText(cursor_, cursor_ + GetCharLength(cursor_));
    }

    std::string tmp_text;
    if (!multiline_)
    {
      tmp_text = CleanupLineBreaks(str);
      str = tmp_text.c_str();
    }

    const char *end = NULL;
    g_utf8_validate(str, -1, &end);
    if (end > str)
    {
      size_t len = end - str;

      text_.insert(cursor_, str, len);
      cursor_ += static_cast<int>(len);
      selection_bound_ += static_cast<int>(len);
    }

    ResetLayout();
    text_input_mode_ = true;
    text_changed.emit(this);
  }

  void TextEntry::DeleteText(int start, int end)
  {
    if (readonly_) return;

    int text_length = static_cast<int>(text_.length());
    if (start < 0)
      start = 0;
    else if (start > text_length)
      start = text_length;

    if (end < 0)
      end = 0;
    else if (end > text_length)
      end = text_length;

    if (start > end)
      std::swap(start, end);
    else if (start == end)
      return;

    text_.erase(start, end - start);

    if (cursor_ >= end)
      cursor_ -= (end - start);
    if (selection_bound_ >= end)
      selection_bound_ -= (end - start);

    ResetLayout();
    text_input_mode_ = true;
    text_changed.emit(this);
  }

  void TextEntry::SelectWord()
  {
    int selection_bound = MoveWords(cursor_, -1);
    int cursor = MoveWords(selection_bound, 1);
    SetSelectionBounds(selection_bound, cursor);
  }

  void TextEntry::SelectLine()
  {
    int selection_bound = MoveLineEnds(cursor_, -1);
    int cursor = MoveLineEnds(selection_bound, 1);
    SetSelectionBounds(selection_bound, cursor);
  }

  void TextEntry::Select(int start, int end) {
    int text_length = static_cast<int>(text_.length());
    if (start == -1)
      start = text_length;
    if (end == -1)
      end = text_length;

    start = Clamp(start, 0, text_length);
    end = Clamp(end, 0, text_length);
    SetSelectionBounds(start, end);
    QueueRefresh(false, true);
  }

  void TextEntry::SelectAll() {
    SetSelectionBounds(0, static_cast<int>(text_.length()));
    QueueRefresh(false, true);
  }

  CairoGraphics::Alignment TextEntry::GetAlign() const
  {
    return align_;
  }

  void TextEntry::SetAlign(CairoGraphics::Alignment align)
  {
    align_ = align;
    QueueRefresh(true, true);
  }

  bool TextEntry::im_active()
  {
    return ime_active_;
  }

  bool TextEntry::im_running()
  {
#if defined(USE_X11)
    return ime_->IsConnected();
#else
    return false;
#endif
  }

  void TextEntry::DeleteSelection()
  {
    int start, end;
    if (GetSelectionBounds(&start, &end))
      DeleteText(start, end);
  }

  void TextEntry::CopyClipboard()
  {
//     int start, end;
//     if (GetSelectionBounds(&start, &end))
//     {
//       GtkWidget *widget = GetWidgetAndCursorLocation(NULL);
//       if (widget)
//       {
//         if (visible_)
//         {
//           gtk_clipboard_set_text(
//             gtk_widget_get_clipboard(widget, GDK_SELECTION_CLIPBOARD),
//             text_.c_str() + start, end - start);
//         }
//         else
//         {
//           // Don't copy real content if it's in invisible.
//           std::string content;
//           int nchars = static_cast<int>(
//             g_utf8_strlen(text_.c_str() + start, end - start));
//           for (int i = 0; i < nchars; ++i)
//             content.append(password_char_);
//           gtk_clipboard_set_text(
//             gtk_widget_get_clipboard(widget, GDK_SELECTION_CLIPBOARD),
//             content.c_str(), static_cast<int>(content.length()));
//         }
//       }
//     }
  }

  void TextEntry::CutClipboard()
  {
    CopyClipboard();
    DeleteSelection();
    QueueRefresh(true, true);
  }

  void TextEntry::PasteClipboard()
  {
//     GtkWidget *widget = GetWidgetAndCursorLocation(NULL);
//     if (widget)
//     {
//       gtk_clipboard_request_text(
//         gtk_widget_get_clipboard(widget, GDK_SELECTION_CLIPBOARD),
//         PasteCallback, this);
//     }
  }

#if defined(NUX_OS_LINUX)
  void TextEntry::PastePrimaryClipboard()
  {
    // TODO
  }
#endif

  void TextEntry::BackSpace(MovementStep step)
  {
    if (GetSelectionBounds(NULL, NULL))
    {
      DeleteSelection();
    }
    else
    {
      if (cursor_ == 0)
        return;
      if (step == VISUALLY)
      {
        DeleteText(cursor_ - GetPrevCharLength(cursor_), cursor_);
      }
      else if (step == WORDS)
      {
        int new_cursor;
        new_cursor = MoveWords(cursor_, -1);
        DeleteText(new_cursor, cursor_);
      }
    }
  }

  void TextEntry::Delete(MovementStep step)
  {
    if (GetSelectionBounds(NULL, NULL))
    {
      DeleteSelection();
    }
    else
    {
      if (cursor_ == static_cast<int>(text_.length()))
        return;
      if (step == VISUALLY)
      {
        DeleteText(cursor_, cursor_ + GetCharLength(cursor_));
      }
      else if (step == WORDS)
      {
        int new_cursor;
        new_cursor = MoveWords(cursor_, 1);
        DeleteText(cursor_, new_cursor);
      }
    }
  }

  void TextEntry::ToggleOverwrite()
  {
    overwrite_ = !overwrite_;
  }

  void TextEntry::UpdateSelectionRegion()
  {
    selection_region_.clear();

    // Selection in a single line may be not continual, so we use pango to
    // get the x-ranges of each selection range in one line, and draw them
    // separately.
    int start_index, end_index;
    if (GetSelectionBounds(&start_index, &end_index))
    {
      PangoLayout *layout = EnsureLayout();
      PangoRectangle line_extents, pos;
      int draw_start, draw_end;
      int *ranges;
      int n_ranges;
      int n_lines = pango_layout_get_line_count(layout);

      start_index = TextIndexToLayoutIndex(start_index, false);
      end_index = TextIndexToLayoutIndex(end_index, false);

      for (int line_index = 0; line_index < n_lines; ++line_index)
      {
#if PANGO_VERSION_CHECK(1,16,0)
        PangoLayoutLine *line = pango_layout_get_line_readonly(layout, line_index);
#else
        PangoLayoutLine *line = pango_layout_get_line(layout, line_index);
#endif
        if (line->start_index + line->length < start_index)
          continue;
        if (end_index < line->start_index)
          break;
        draw_start = std::max<int>(start_index, line->start_index);
        draw_end = std::min<int>(end_index, line->start_index + line->length);
        pango_layout_line_get_x_ranges(line, draw_start, draw_end,
          &ranges, &n_ranges);
        pango_layout_line_get_pixel_extents(line, NULL, &line_extents);
        pango_layout_index_to_pos(layout, line->start_index,  &pos);
        for (int i = 0; i < n_ranges; ++i)
        {
          selection_region_.push_back(Rect(
            kInnerBorderX + scroll_offset_x_ + PANGO_PIXELS(ranges[i * 2]),
            kInnerBorderY + scroll_offset_y_ + PANGO_PIXELS(pos.y),
            PANGO_PIXELS(ranges[i * 2 + 1] - ranges[i * 2]),
            line_extents.height));
        }
        g_free(ranges);
      }
    }
  }

  void TextEntry::MoveCursor(MovementStep step, int count, bool extend_selection)
  {
    ResetImContext();
    int new_cursor = 0;
    // Clear selection first if not extend it.
    if (!extend_selection)
    {
      selection_changed_ = true;
      cursor_moved_ = true;
      selection_bound_ = cursor_;
      cursor_moved.emit(cursor_);
    }

    // Calculate the new offset after motion.
    switch(step)
    {
    case VISUALLY:
      new_cursor = MoveVisually(cursor_, count);
      break;
    case WORDS:
      new_cursor = MoveWords(cursor_, count);
      break;
    case DISPLAY_LINES:
      new_cursor = MoveDisplayLines(cursor_, count);
      break;
    case DISPLAY_LINE_ENDS:
      new_cursor = MoveLineEnds(cursor_, count);
      break;
    case PAGES:
      new_cursor = MovePages(cursor_, count);
      break;
    case BUFFER:
      nuxAssert(count == -1 || count == 1);
      new_cursor = static_cast<int>(count == -1 ? 0 : text_.length());
      break;
    }

    if (extend_selection)
      SetSelectionBounds(selection_bound_, new_cursor);
    else
      SetCursor(new_cursor);

    QueueRefresh(true, true);
  }

  int TextEntry::MoveVisually(int current_index, int count)
  {
    nuxAssert(current_index >= 0 &&
      current_index <= static_cast<int>(text_.length()));
    nuxAssert(count);
    nuxAssert(preedit_.length() == 0);

    PangoLayout* layout = EnsureLayout();
    const char* text = pango_layout_get_text(layout);
    int index = TextIndexToLayoutIndex(current_index, false);
    int new_index = 0;
    int new_trailing = 0;
    while (count != 0)
    {
      if (count > 0)
      {
        --count;
        pango_layout_move_cursor_visually(layout, true, index, 0, 1,
          &new_index, &new_trailing);
      }
      else if (count < 0)
      {
        ++count;
        pango_layout_move_cursor_visually(layout, true, index, 0, -1,
          &new_index, &new_trailing);
      }
      index = new_index;
      if (index < 0 || index == G_MAXINT)
        return current_index;
      index = static_cast<int>(g_utf8_offset_to_pointer(text + index, new_trailing) - text);
    }
    return LayoutIndexToTextIndex(index);
  }

  int TextEntry::MoveWords(int current_index, int count)
  {
    nuxAssert(current_index >= 0 &&
      current_index <= static_cast<int>(text_.length()));
    nuxAssert(count);
    nuxAssert(preedit_.length() == 0);

    if (!visible_)
    {
      return static_cast<int>(count > 0 ? text_.length() : 0);
    }

    // The cursor movement direction shall be determined by the direction of
    // current text line.
    PangoLayout* layout = EnsureLayout();
    int n_log_attrs;
    PangoLogAttr* log_attrs;
    pango_layout_get_log_attrs(layout, &log_attrs, &n_log_attrs);
    const char* text = pango_layout_get_text(layout);
    int index = TextIndexToLayoutIndex(current_index, false);
    int line_index;
    pango_layout_index_to_line_x(layout, index, FALSE, &line_index, NULL);

    // Weird bug: line_index here may be >= than line count?
    int line_count = pango_layout_get_line_count(layout);
    if (line_index >= line_count)
    {
      line_index = line_count - 1;
    }

#if PANGO_VERSION_CHECK(1,16,0)
    PangoLayoutLine* line = pango_layout_get_line_readonly(layout, line_index);
#else
    PangoLayoutLine* line = pango_layout_get_line(layout, line_index);
#endif
    bool rtl = (line->resolved_dir == PANGO_DIRECTION_RTL);
    const char* ptr = text + index;
    int offset = static_cast<int>(g_utf8_pointer_to_offset(text, ptr));
    while (count != 0)
    {
      if (((rtl && count < 0) || (!rtl && count > 0)) && *ptr)
      {
        if (log_attrs[offset].is_white)
        {
          while (ptr && *ptr && log_attrs[offset].is_white)
          {
            ptr = g_utf8_find_next_char(ptr, NULL);
            ++offset;
          }
        }
        else
        {
          if (ptr && *ptr)
          {
            ptr = g_utf8_find_next_char(ptr, NULL);
            ++offset;
          }
        }
        while (ptr && *ptr)
        {
          ptr = g_utf8_find_next_char(ptr, NULL);
          ++offset;
          if (log_attrs[offset].is_word_start || log_attrs[offset].is_word_end)
            break;
        }
        if (!ptr)
        {
          ptr = text;
          while (*ptr) ++ptr;
        }
      }
      else if (((rtl && count > 0) || (!rtl && count < 0)) && (ptr > text))
      {
        if (offset > 0 && log_attrs[offset - 1].is_white)
        {
          while (ptr && offset > 0 && log_attrs[offset - 1].is_white)
          {
            ptr = g_utf8_find_prev_char(text, ptr);
            --offset;
          }
        }
        else
        {
          if (ptr)
          {
            ptr = g_utf8_find_prev_char(text, ptr);
            --offset;
          }
        }
        while (ptr /*&& *ptr*/) //fix: when at the end of the string, allow ctrl+left arrow to move backward to the start/end of the previous word.
        {
          ptr = g_utf8_find_prev_char(text, ptr);
          --offset;
          if (log_attrs[offset].is_word_start || log_attrs[offset].is_word_end)
            break;
        }
        if (!ptr)
          ptr = text;
      }
      else
      {
        break;
      }
      if (count > 0)
        --count;
      else
        ++count;
    }
    return LayoutIndexToTextIndex(static_cast<int>(ptr - text));
  }

  int TextEntry::MoveDisplayLines(int current_index, int count)
  {
    nuxAssert(current_index >= 0 &&
      current_index <= static_cast<int>(text_.length()));
    nuxAssert(count);
    nuxAssert(preedit_.length() == 0);

    PangoLayout *layout = EnsureLayout();
    const char *text = pango_layout_get_text(layout);
    int index = TextIndexToLayoutIndex(current_index, false);
    int n_lines = pango_layout_get_line_count(layout);
    int line_index = 0;
    int x_off = 0;
    PangoRectangle rect;

    // Find the current cursor X position in layout
    pango_layout_index_to_line_x(layout, index, FALSE, &line_index, &x_off);

    // Weird bug: line_index here may be >= than line count?
    if (line_index >= n_lines)
    {
      line_index = n_lines - 1;
    }

    pango_layout_get_cursor_pos(layout, index, &rect, NULL);
    x_off = rect.x;

    line_index += count;

    if (line_index < 0)
    {
      return 0;
    }
    else if (line_index >= n_lines)
    {
      return static_cast<int>(text_.length());
    }

    int trailing;
#if PANGO_VERSION_CHECK(1,16,0)
    PangoLayoutLine *line = pango_layout_get_line_readonly(layout, line_index);
#else
    PangoLayoutLine *line = pango_layout_get_line(layout, line_index);
#endif
    // Find out the cursor x offset related to the new line position.
    if (line->resolved_dir == PANGO_DIRECTION_RTL)
    {
      pango_layout_get_cursor_pos(layout, line->start_index + line->length,
        &rect, NULL);
    }
    else
    {
      pango_layout_get_cursor_pos(layout, line->start_index, &rect, NULL);
    }

    // rect.x is the left edge position of the line in the layout
    x_off -= rect.x;
    if (x_off < 0) x_off = 0;
    pango_layout_line_x_to_index(line, x_off, &index, &trailing);

    index = static_cast<int>(g_utf8_offset_to_pointer(text + index, trailing) - text);
    return LayoutIndexToTextIndex(index);
  }

  int TextEntry::MovePages(int current_index, int count)
  {
    nuxAssert(current_index >= 0 &&
      current_index <= static_cast<int>(text_.length()));
    nuxAssert(count);
    nuxAssert(preedit_.length() == 0);

    // Transfer pages to display lines.
    PangoLayout *layout = EnsureLayout();
    int layout_height;
    pango_layout_get_pixel_size(layout, NULL, &layout_height);
    int n_lines = pango_layout_get_line_count(layout);
    int line_height = layout_height / n_lines;
    int page_lines = (GetBaseHeight() - kInnerBorderY * 2) / line_height;
    return MoveDisplayLines(current_index, count * page_lines);
  }

  int TextEntry::MoveLineEnds(int current_index, int count)
  {
    nuxAssert(current_index >= 0 &&
      current_index <= static_cast<int>(text_.length()));
    nuxAssert(count);
    nuxAssert(preedit_.length() == 0);

    PangoLayout *layout = EnsureLayout();
    int index = TextIndexToLayoutIndex(current_index, false);
    int line_index = 0;

    // Find current line
    pango_layout_index_to_line_x(layout, index, FALSE, &line_index, NULL);

    // Weird bug: line_index here may be >= than line count?
    int line_count = pango_layout_get_line_count(layout);
    if (line_index >= line_count)
    {
      line_index = line_count - 1;
    }

// #if PANGO_VERSION_CHECK(1,16,0)
//     PangoLayoutLine *line = pango_layout_get_line_readonly(layout, line_index);
// #else
    PangoLayoutLine *line = pango_layout_get_line(layout, line_index);
// #endif

    if (line->length == 0)
      return current_index;

    if ((line->resolved_dir == PANGO_DIRECTION_RTL && count < 0) ||
      (line->resolved_dir != PANGO_DIRECTION_RTL && count > 0))
    {
        index = line->start_index + line->length;
    }
    else
    {
      index = line->start_index;
    }
    return LayoutIndexToTextIndex(index);
  }

  void TextEntry::SetCursor(int cursor)
  {
    if (cursor != cursor_)
    {
      ResetImContext();
      // If there was a selection range, then the selection range will be cleared.
      // Then content_modified_ shall be set to true to force redrawing the text.
      if (cursor_ != selection_bound_)
        selection_changed_ = true;
      cursor_ = cursor;
      selection_bound_ = cursor;
      cursor_moved_ = true;

      cursor_moved.emit(cursor);
    }
  }

  int TextEntry::XYToTextIndex(int x, int y)
  {
    int width, height;
    PangoLayout *layout = EnsureLayout();
    const char *text = pango_layout_get_text(layout);
    pango_layout_get_pixel_size(layout, &width, &height);

    if (y < 0)
    {
      return 0;
    }
    else if (y >= height)
    {
      return static_cast<int>(text_.length());
    }

    int trailing;
    int index;
    pango_layout_xy_to_index(layout, x * PANGO_SCALE, y * PANGO_SCALE,
      &index, &trailing);
    index = static_cast<int>(
      g_utf8_offset_to_pointer(text + index, trailing) - text);

    index = LayoutIndexToTextIndex(index);

    // Adjust the offset if preedit is not empty and if the offset is after
    // current cursor.
    int preedit_length = static_cast<int>(preedit_.length());
    if (preedit_length && index > cursor_)
    {
      if (index >= cursor_ + preedit_length)
        index -= preedit_length;
      else
        index = cursor_;
    }
    return Clamp(index, 0, static_cast<int>(text_.length()));
  }

  bool TextEntry::GetSelectionBounds(int* start, int* end) const
  {
    if (start)
    {
      *start = std::min<int>(selection_bound_, cursor_);
    }

    if (end)
    {
      *end = std::max<int>(selection_bound_, cursor_);
    }

    return (selection_bound_ != cursor_);
  }

  void TextEntry::SetSelectionBounds(int selection_bound, int cursor)
  {
    if (selection_bound_ != selection_bound || cursor_ != cursor)
    {
      selection_changed_ = true;
      selection_bound_ = selection_bound;
      if (cursor_ != cursor)
      {
        cursor_ = cursor;
        cursor_moved_ = true;
        cursor_moved.emit(cursor);
      }

      //ResetImContext();
    }
  }

  void TextEntry::SetFontFamily(const char *font)
  {
    font_family_ = font;
    QueueRefresh(true, true);
  }

  void TextEntry::SetFontSize(double font_size)
  {
    font_size_ = font_size;
    QueueRefresh(true, true);
  }

  void TextEntry::SetFontOptions(const cairo_font_options_t *options)
  {
    g_return_if_fail(options);

    cairo_font_options_destroy(font_options_);
    font_options_ = cairo_font_options_copy(options);

    QueueRefresh(true, true);
  }

  bool TextEntry::InspectKeyEvent(unsigned int /* eventType */, unsigned int /* key_sym */, const char* /* character */)
  {
    nux::Event const& cur_event = GetGraphicsDisplay()->GetCurrentEvent();

    return InspectKeyEvent(cur_event);
  }

  bool TextEntry::InspectKeyEvent(nux::Event const& event)
  {
    unsigned int eventType = event.type;
    unsigned int key_sym = event.GetKeySym();

#if defined(USE_X11)
    if (im_running())
    {
      // Always allow IBus hotkey events
      if (ime_->IsHotkeyEvent(static_cast<EventType>(eventType), key_sym, event.key_modifiers))
        return true;
    }
#endif

    // Ignore events when Alt or Super are pressed
    if (event.GetKeyModifierState(KEY_MODIFIER_SUPER) || event.GetKeyModifierState(KEY_MODIFIER_ALT))
      return false;

    if ((eventType == NUX_KEYDOWN) && (key_nav_mode_ == true) && (text_input_mode_ == false) && (ime_active_ == false))
    {
      if (key_sym == NUX_VK_ENTER ||
        key_sym == NUX_KP_ENTER ||
        key_sym == NUX_VK_UP ||
        key_sym == NUX_VK_DOWN ||
        key_sym == NUX_VK_LEFT ||
        key_sym == NUX_VK_RIGHT ||
        key_sym == NUX_VK_LEFT_TAB ||
        key_sym == NUX_VK_TAB ||
        key_sym == NUX_VK_ESCAPE)
      {
        if (key_sym == NUX_VK_LEFT ||
            key_sym == NUX_VK_RIGHT ||
            key_sym == NUX_VK_ENTER ||
            key_sym == NUX_KP_ENTER)
        {
          return true;
        }

        if (multiline_ && (key_sym == NUX_VK_UP))
        {
          // Navigate between the lines of the text entry.
          // This will move the cursor one line up.
          return true;
        }

        if (multiline_ && (key_sym == NUX_VK_DOWN))
        {
          // Navigate between the lines of the text entry.
          // This will move the cursor one line down.
          return true;
        }

        if ((!multiline_) && (!lose_key_focus_on_key_nav_direction_up_) && (key_sym == NUX_VK_UP))
        {
          // By returning true, the text entry signals that it want to receive the signal for this event.
          // Otherwise, the parent view of the text entry would be looking for another view to receive keynav focus to.
          return true;
        }

        if ((!multiline_) && (!lose_key_focus_on_key_nav_direction_down_) && (key_sym == NUX_VK_DOWN))
        {
          // By returning true, the text entry signals that it want to receive the signal for this event.
          // Otherwise, the parent view of the text entry would be looking for another view to receive keynav focus to.
          return true;
        }

        return false;
      }
    }

    if ((eventType == NUX_KEYDOWN) && (key_nav_mode_ == true) && (text_input_mode_ == true) && (ime_active_ == false))
    {
      // Enable to exit the TextEntry when in write mode(hack for unity dash)
      if (key_sym == NUX_VK_UP ||
      key_sym == NUX_VK_DOWN ||
      key_sym == NUX_VK_ESCAPE)
      {
        if ((!multiline_) && (!lose_key_focus_on_key_nav_direction_up_) && NUX_VK_UP)
        {
          // By returning true, the text entry signals that it want to receive the signal for this event.
          // Otherwise, the parent view of the text entry would be looking for another view to receive keynav focus to.
          return true;
        }

        if ((!multiline_) && (!lose_key_focus_on_key_nav_direction_down_) && NUX_VK_DOWN)
        {
          // By returning true, the text entry signals that it want to receive the signal for this event.
          // Otherwise, the parent view of the text entry would be looking for another view to receive keynav focus to.
          return true;
        }

        return false;
      }
    }

    if ((eventType == NUX_KEYDOWN) && (key_nav_mode_ == false) && (text_input_mode_ == false))
    {
      return false;
    }

    return true;
  }

  /// Public API

  void TextEntry::MoveCursorToLineStart()
  {
    MoveCursor(DISPLAY_LINE_ENDS, -1, 0);
  }

  void TextEntry::MoveCursorToLineEnd()
  {
    MoveCursor(DISPLAY_LINE_ENDS, 1, 0);
  }

  void TextEntry::SetLoseKeyFocusOnKeyNavDirectionUp(bool b)
  {
    lose_key_focus_on_key_nav_direction_up_ = b;
  }

  bool TextEntry::GetLoseKeyFocusOnKeyNavDirectionUp() const
  {
    return lose_key_focus_on_key_nav_direction_up_;
  }

  void TextEntry::SetLoseKeyFocusOnKeyNavDirectionDown(bool b)
  {
    lose_key_focus_on_key_nav_direction_down_ = b;
  }

  bool TextEntry::GetLoseKeyFocusOnKeyNavDirectionDown() const
  {
    return lose_key_focus_on_key_nav_direction_down_;
  }

  void TextEntry::SetToggleCursorVisibilityOnKeyFocus(bool b)
  {
    if (cursor_visible_on_key_focus_only_ == b)
      return;

    cursor_visible_on_key_focus_only_ = b;

    if (cursor_visible_on_key_focus_only_)
    {
      if (!focused_)
      {
        if (cursor_blink_timer_)
        {
          g_source_remove(cursor_blink_timer_);
          cursor_blink_timer_ = 0;
        }

        cursor_visible_ = false;
        QueueDraw();
      }
    }
    else
    {
      if (!focused_)
      {
        QueueCursorBlink();
      }
    }
  }

  bool TextEntry::GetToggleCursorVisibilityOnKeyFocus() const
  {
    return cursor_visible_on_key_focus_only_;
  }

  bool TextEntry::IsInTextInputMode() const
  {
    return text_input_mode_;
  }

  void TextEntry::SetVisibility(bool visible)
  {
    if (visible_ != visible)
    {
      visible_ = visible;

      if (!readonly_)
      {
        if (focused_)
        {
          //gtk_im_context_focus_out(im_context_);
        }

        //InitImContext();
        ResetPreedit();

        if (focused_)
        {
          //gtk_im_context_focus_in(im_context_);
        }
      }

      ResetLayout();
    }
  }

  typedef unsigned short  UTF16Char;
  typedef unsigned int    UTF32Char;
  typedef	unsigned char   UTF8Char;
  static const UTF8Char kTrailingBytesForUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
  };


  // http://google-gadgets-for-linux.googlecode.com/svn-history/r97/trunk/ggadget/unicode_utils.cc
  size_t GetUTF8CharLength(const char *src)
  {
    return src ? (kTrailingBytesForUTF8[static_cast<UTF8Char>(*src)] + 1) : 0;
  }

  bool IsLegalUTF8Char(const char *src, size_t length)
  {
    if (!src || !length) return false;

    const UTF8Char *srcptr = reinterpret_cast<const UTF8Char*>(src);
    UTF8Char a;
    UTF8Char ch = *srcptr;
    srcptr += length;
    switch (length)
    {
    default: return false;
      // Everything else falls through when "true"...
    case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
    case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
    case 2: if ((a = (*--srcptr)) > 0xBF) return false;
      switch (ch) {
        // No fall-through in this inner switch
      case 0xE0: if (a < 0xA0) return false; break;
      case 0xED: if (a > 0x9F) return false; break;
      case 0xF0: if (a < 0x90) return false; break;
      case 0xF4: if (a > 0x8F) return false; break;
      default:   if (a < 0x80) return false;
      }
    case 1: if (ch >= 0x80 && ch < 0xC2) return false;
    }

    if (ch > 0xF4) return false;
    return true;
  }

  void TextEntry::SetPasswordChar(const char* c)
  {
    if (c == NULL || *c == 0 || !IsLegalUTF8Char(c, GetUTF8CharLength(c)))
    {
      password_char_.clear();
    }
    else
    {
      password_char_.assign(c, GetUTF8CharLength(c));
    }
    QueueRefresh(true, true);
  }

  std::string TextEntry::GetPasswordChar()
  {
    return password_char_;
  }

  void TextEntry::SetPasswordMode(bool visible)
  {
    SetVisibility(!visible);
  }

  bool TextEntry::PasswordMode() const
  {
    return !visible_;
  }
}
