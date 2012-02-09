/*
  Copyright 2008 Google Inc.

  Licensed under the Apache License, Version 2.0(the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/


#include "Nux.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "Validator.h"

#include "cairo/cairo.h"
#include "pango/pango.h"
#include "pango/pangocairo.h"
#include "NuxImage/CairoGraphics.h"

#include "TextEntry.h"

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
  static const Color kTextUnderCursorColor(1, 1, 1, 1.0f);
  static const double kStrongCursorBarHeight = 1;
  static const double kWeakCursorBarHeight = 1;

  static const unsigned long long kTripleClickTimeout = 500;
  static const std::string kDefaultFontName = "Ubuntu";

  static unsigned long long GetCurrentTime()
  {
    GTimeVal tv;
    g_get_current_time(&tv);
    return static_cast<unsigned long long>(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
  }

  static std::string CleanupLineBreaks(const char *source)
  {
    nuxAssert(source);
    std::string result;
    while (*source)
    {
      if (*source == '\r')
      {
        result += ' ';
        if (source[1] == '\n')
          source++;
      }
      else if (*source == '\n')
      {
        result += ' ';
      }
      else
      {
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

  CairoFont::Style CairoFont::GetStyle() const
  {
    return style_;
  }
  
  CairoFont::Weight CairoFont::GetWeight() const
  {
    return weight_;
  }
  
  double CairoFont::GetPointSize() const
  {
    return size_;
  }

  void CairoFont::Destroy()
  {
    delete this;
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
    , last_dblclick_time_(0)
    , cursor_(0)
    , preedit_cursor_(0)
    , selection_bound_(0)
    , scroll_offset_x_(0)
    , scroll_offset_y_(0)
    , cursor_blink_status_(0)
    , characters_revealed_(true)
    , has_keyboard_focused_(false)
    , need_im_reset_(false)
    , overwrite_(false)
    , select_words_(false)
    , select_lines_(false)
    , button_(false)
    , bold_(false)
    , underline_(false)
    , strikeout_(false)
    , italic_(false)
    , cursor_visible_(false)
    , readonly_(false)
    , content_modified_(false)
    , selection_changed_(false)
    , cursor_moved_(false)
    , update_canvas_(true)
    , font_family_("Ubuntu")
    , font_size_(42)
    , _text_color(color::White)
    , align_(CairoGraphics::ALIGN_LEFT)
    , valign_(CairoGraphics::VALIGN_TOP)
    , text_input_mode_(false)
    , key_nav_mode_(false)
    , cursor_blink_signal_(NULL)
    , cursor_index_in_layout_(-1)
  {
    cursor_blink_signal_ = new TimeOutSignal();
    cursor_blink_signal_->time_expires.connect(sigc::mem_fun(this, &TextEntry::CursorBlinkCallback));

    // Set font options
    font_options_ = cairo_font_options_create();
    cairo_font_options_set_antialias      (font_options_, CAIRO_ANTIALIAS_SUBPIXEL);
    cairo_font_options_set_subpixel_order (font_options_, CAIRO_SUBPIXEL_ORDER_RGB);
    cairo_font_options_set_hint_style     (font_options_, CAIRO_HINT_STYLE_FULL);
    cairo_font_options_set_hint_metrics   (font_options_, CAIRO_HINT_METRICS_ON);

    // Set dpi
    font_dpi_ = 96.0;

    // Wrap mode
    wrap_ = false; // Single line text entry
    wrap_mode_ = PANGO_WRAP_WORD_CHAR;

    // Multi line
    multiline_ = false;

    // Password conceled character
    password_char_ = "*";

    ime_ = IMEContext::Create(this);

    mouse_down.connect(sigc::mem_fun(this, &TextEntry::RecvMouseDown));
    mouse_drag.connect(sigc::mem_fun(this, &TextEntry::RecvMouseDrag));
    mouse_up.connect(sigc::mem_fun(this, &TextEntry::RecvMouseUp));
    mouse_double_click.connect(sigc::mem_fun(this, &TextEntry::RecvMouseDoubleClick));

    key_down.connect(sigc::mem_fun(this, &TextEntry::RecvKeyEvent));

    begin_key_focus.connect(sigc::mem_fun(this, &TextEntry::RecvStartKeyFocus));
    end_key_focus.connect(sigc::mem_fun(this, &TextEntry::RecvEndKeyFocus));

    SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    SetText(text);

    SetAcceptKeyboardEvent(true);
    EnableDoubleClick(true);
  }

  TextEntry::~TextEntry()
  {
    StopCursorBlink();

    delete cursor_blink_signal_;

    cairo_font_options_destroy(font_options_);

    if (_texture2D)
      _texture2D->UnReference();
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

  void TextEntry::GeometryChanged()
  {

      update_canvas_ = true;
      View::GeometryChanged();

  }

  Area* TextEntry::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    Area* area = View::FindAreaUnderMouse(mouse_position, event_type);

    return area;
  }

  void TextEntry::ProcessMouseEvent(int event_type, int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    int X = static_cast<int>(x /*round(event.GetX())*/) - kInnerBorderX - scroll_offset_x_;
    int Y = static_cast<int>(y /*round(event.GetY())*/) - kInnerBorderY - scroll_offset_y_;
    int index = XYToTextIndex(X, Y);
    int sel_start, sel_end;
    GetSelectionBounds(&sel_start, &sel_end);

    unsigned long long current_time = GetCurrentTime();

    if ((event_type == NUX_MOUSE_PRESSED) && (current_time - last_dblclick_time_ <= kTripleClickTimeout))
    {
        SelectLine();
    }
    else if (event_type == NUX_MOUSE_DOUBLECLICK)
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
      // Prevent the cursor from blinking while typing text.
      cursor_blink_status_ = 3;
      SetSelectionBounds(selection_bound_, index);
    }

    QueueRefresh(false, MINIMAL_ADJUST);
    //return EVENT_RESULT_HANDLED;
  }

  void TextEntry::ProcessKeyEvent(
    unsigned long   event_type,   /*event type*/
    unsigned long   keysym,       /*event keysym*/
    unsigned long   state,        /*event state*/
    const char      *character,   /*character*/
    unsigned short  keyCount      /*key repeat count*/)
  {
    if (event_type == NUX_KEYDOWN)
    {
      text_input_mode_ = true;
    }

    // Prevent the cursor from blinking while typing text.
    cursor_blink_status_ = 3;

    if (!readonly_ /*&& im_context_*/ && (event_type != NUX_KEYDOWN) && 0 /*&& gtk_im_context_filter_keypress(im_context_, gdk_event)*/)
    {
      need_im_reset_ = true;
      QueueRefresh(false, MINIMAL_ADJUST);
      return;
    }

    if (event_type == NUX_KEYUP)
      return;

    // we need to ignore some characters
    if (keysym == NUX_VK_TAB)
      return;

    if (keysym == NUX_VK_ENTER || keysym == NUX_KP_ENTER)
    {
      activated.emit();
      return;
    }

    unsigned int keyval = keysym;
    bool shift = (state & NUX_STATE_SHIFT) ? true : false;
    bool ctrl = (state & NUX_STATE_CTRL) ? true : false;

    // DLOG("TextEntry::key_down(%d, shift:%d ctrl:%d)", keyval, shift, ctrl);

    if (event_type == NUX_KEYDOWN)
    {
      if (keyval == NUX_VK_LEFT)
      {
        if (!ctrl)
          MoveCursor(VISUALLY, -1, shift);
        else
          MoveCursor(WORDS, -1, shift);
      }
      else if (keyval == NUX_VK_RIGHT)
      {
        if (!ctrl)
          MoveCursor(VISUALLY, 1, shift);
        else
          MoveCursor(WORDS, 1, shift);
      }
      else if (keyval == NUX_VK_UP)
      {
        MoveCursor(DISPLAY_LINES, -1, shift);
      }
      else if (keyval == NUX_VK_DOWN)
      {
        MoveCursor(DISPLAY_LINES, 1, shift);
      }
      else if (keyval == NUX_VK_HOME)
      {
        if (!ctrl)
          MoveCursor(DISPLAY_LINE_ENDS, -1, shift);
        else
          MoveCursor(BUFFER, -1, shift);
      }
      else if (keyval == NUX_VK_END)
      {
        if (!ctrl)
          MoveCursor(DISPLAY_LINE_ENDS, 1, shift);
        else
          MoveCursor(BUFFER, 1, shift);
      }
      else if (keyval == NUX_VK_PAGE_UP)
      {
        if (!ctrl)
          MoveCursor(PAGES, -1, shift);
        else
          MoveCursor(BUFFER, -1, shift);
      }
      else if (keyval == NUX_VK_PAGE_DOWN)
      {
        if (!ctrl)
          MoveCursor(PAGES, 1, shift);
        else
          MoveCursor(BUFFER, 1, shift);
      }
      else if (((keyval == NUX_VK_x) && ctrl && !shift) || ((keyval == NUX_VK_DELETE) && shift && !ctrl))
      {
        CutClipboard();
      }
      else if (((keyval == NUX_VK_c) && ctrl && (!shift)) || ((keyval == NUX_VK_INSERT) && ctrl && (!shift)))
      {
          CopyClipboard();
      }
      else if (((keyval == NUX_VK_v) && ctrl && (!shift)) || ((keyval == NUX_VK_INSERT) && shift && (!ctrl)))
      {
          PasteClipboard();
      }
      else if ((keyval == NUX_VK_a) && ctrl)
      {
        // Select all
        int text_length = static_cast<int>(text_.length());
        SetSelectionBounds(0, text_length);
        QueueRefresh(false, MINIMAL_ADJUST);
        return;
      }
      else if (keyval == NUX_VK_BACKSPACE)
      {
        if (!ctrl)
          BackSpace(VISUALLY);
        else
          BackSpace(WORDS);
      }
      else if ((keyval == NUX_VK_DELETE) && (!shift))
      {
        if (!ctrl)
          Delete(VISUALLY);
        else
          Delete(WORDS);
      }
      else if ((keyval == NUX_VK_INSERT) && (!shift) && (!ctrl))
      {
        ToggleOverwrite();
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

    if (character != 0 && (strlen(character) != 0))
    {
      EnterText(character);
    }

    QueueRefresh(false, CENTER_CURSOR);
    return;
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
    text_input_mode_ = true;
    ProcessMouseEvent(NUX_MOUSE_PRESSED, x, y, 0, 0, button_flags, key_flags);
  }

  void TextEntry::RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    ProcessMouseEvent(NUX_MOUSE_MOVE, x, y, dx, dy, button_flags, key_flags);
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
    key_nav_mode_           = true;
    text_input_mode_        = true;

    StartKeyboardFocus();
  }

  void TextEntry::RecvEndKeyFocus()
  {
    key_nav_mode_     = false;
    text_input_mode_  = false;

    EndKeyboardFocus();
  }

  void TextEntry::Draw(GraphicsEngine& gfxContext, bool forceDraw)
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

  void TextEntry::DrawContent(GraphicsEngine& gfxContext, bool forceDraw)
  {
    //MainDraw();
  }

  void TextEntry::PostDraw(GraphicsEngine& gfxContext, bool forceDraw)
  {
    // intentionally left empty
  }

  void TextEntry::SetText(const char *text)
  {
    const char *end = NULL;
    g_utf8_validate(text, -1, &end);

    std::string txt((text && *text && end > text) ? std::string(text, end) : "");
    if (txt == text_)
      return; // prevent some redraws

    text_ = multiline_ ? txt : CleanupLineBreaks(txt.c_str());
    cursor_ = 0;
    selection_bound_ = 0;
    need_im_reset_ = true;
    //ResetImContext();
    QueueRefresh(true, MINIMAL_ADJUST);
    sigTextChanged.emit(this);
  }

  std::string const& TextEntry::GetText() const
  {
    return text_;
  }

  void TextEntry::SetTextColor(const Color &text_color)
  {
    if (_text_color != text_color)
    {
      _text_color = text_color;
      QueueRefresh(true, NO_SCROLL);
    }
  }

  Color const& TextEntry::GetTextColor() const
  {
    return _text_color;
  }


  void TextEntry::MainDraw()
  {

    CairoGraphics *edit_canvas = EnsureCanvas();

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

  void TextEntry::StartKeyboardFocus()
  {
    if (!has_keyboard_focused_)
    {
      has_keyboard_focused_ = true;
      if (!readonly_ /*&& im_context_*/)
      {
        StartCursorBlink();
        need_im_reset_ = true;
        //gtk_im_context_focus_in(im_context_);
        //UpdateIMCursorLocation();
      }
      cursor_visible_ = true; // show cursor when getting focus
      selection_changed_ = true;
      cursor_moved_ = true;
      // Don't adjust scroll.
      QueueRefresh(true, NO_SCROLL);
    }
  }

  void TextEntry::EndKeyboardFocus()
  {
    if (has_keyboard_focused_)
    {
      has_keyboard_focused_ = false;
      
      StopCursorBlink();

      if (!readonly_ /*&& im_context_*/)
      {
        need_im_reset_ = true;
        //gtk_im_context_focus_out(im_context_);
      }
      cursor_visible_ = false; // hide cursor when losing focus
      selection_changed_ = true;
      cursor_moved_ = true;
      // Don't adjust scroll.
      QueueRefresh(true, NO_SCROLL);
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

  void TextEntry::AdjustScroll(AdjustScrollPolicy policy)
  {
    if (policy == NO_SCROLL)
      return;

    int old_offset_x = scroll_offset_x_;
    int old_offset_y = scroll_offset_y_;
    int display_width = GetBaseWidth() - kInnerBorderX * 2;
    int display_height = GetBaseHeight() - kInnerBorderY * 2;

    PangoLayout *layout = EnsureLayout();
    int text_width, text_height;
    pango_layout_get_pixel_size(layout, &text_width, &text_height);

    PangoRectangle strong;
    PangoRectangle weak;
    GetCursorLocationInLayout(&strong, &weak);

    if (!wrap_ && display_width >= text_width) {
      PangoAlignment align = pango_layout_get_alignment(layout);
      if (align == PANGO_ALIGN_RIGHT)
        scroll_offset_x_ = display_width - text_width;
      else if (align == PANGO_ALIGN_CENTER)
        scroll_offset_x_ = (display_width - text_width) / 2;
      else
        scroll_offset_x_ = 0;
    } else {
      if (scroll_offset_x_ + strong.x > display_width) {
        if (policy == CENTER_CURSOR) {
          scroll_offset_x_ = std::max<int>(display_width - text_width,
            display_width / 2 - strong.x);
        } else {
          scroll_offset_x_ = display_width - strong.x;
        }
      }
      if (!wrap_ && scroll_offset_x_ + text_width < display_width) {
        scroll_offset_x_ = display_width - text_width;
      }
      if (scroll_offset_x_ + strong.x < 0) {
        if (policy == CENTER_CURSOR) {
          scroll_offset_x_ = std::min<int>(0, display_width / 2 - strong.x);
        } else {
          scroll_offset_x_ = -strong.x;
        }
      }

      if (std::abs(weak.x - strong.x) < display_width) {
        if (scroll_offset_x_ + weak.x < 0)
          scroll_offset_x_ = - weak.x;
        else if (scroll_offset_x_ + weak.x > display_width)
          scroll_offset_x_ = display_width - weak.x;
      }
    }

    if (display_height >= text_height)
    {
      if (valign_ == CairoGraphics::VALIGN_TOP)
        scroll_offset_y_ = 0;
      else if (valign_ == CairoGraphics::VALIGN_MIDDLE)
        scroll_offset_y_ = (display_height - text_height) / 2;
      else
        scroll_offset_y_ = display_height - text_height;
    }
    else
    {
      if (scroll_offset_y_ + strong.y + strong.height > display_height)
        scroll_offset_y_ = display_height - strong.y - strong.height;

      if (scroll_offset_y_ + text_height < display_height)
        scroll_offset_y_ = display_height - text_height;

      if (scroll_offset_y_ + strong.y < 0)
        scroll_offset_y_ = -strong.y;
    }

    if (old_offset_x != scroll_offset_x_ || old_offset_y != scroll_offset_y_)
      content_modified_ = true;
  }


  void TextEntry::QueueTextDraw()
  {
    if (content_modified_)
    {
      UpdateCursorRegion();

      if (!last_content_region_.empty())
        QueueDraw(); //owner_->QueueDrawRegion(last_content_region_);
      if (!content_region_.empty())
        QueueDraw(); //owner_->QueueDrawRegion(content_region_);

      content_modified_ = false;
      selection_changed_ = true;
      cursor_moved_ = true;
      update_canvas_ = true;
    }

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

  void TextEntry::QueueRefresh(bool relayout, AdjustScrollPolicy policy)
  {
    if (relayout)
      ResetLayout();

    if (policy != NO_SCROLL)
      AdjustScroll(policy);

    QueueTextDraw();
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

  void TextEntry::UpdateContentRegion()
  {
    content_region_.clear();

    PangoLayout *layout = EnsureLayout();
    PangoRectangle extents;
    double x, y, w, h;

    PangoLayoutIter *iter = pango_layout_get_iter(layout);
    do {
      pango_layout_iter_get_line_extents(iter, NULL, &extents);

#if PANGO_VERSION_CHECK(1,16,0)
      pango_extents_to_pixels(&extents, NULL);
#else
      extents.x = PANGO_PIXELS_FLOOR(extents.x);
      extents.y = PANGO_PIXELS_FLOOR(extents.y);
      extents.width = PANGO_PIXELS_CEIL(extents.width);
      extents.height = PANGO_PIXELS_CEIL(extents.height);
#endif

      x = kInnerBorderX + scroll_offset_x_ + extents.x;
      y = kInnerBorderY + scroll_offset_y_ + extents.y;
      w = extents.width;
      h = extents.height;

      if (x < GetBaseWidth() && x + w > 0 && y < GetBaseHeight() && y + h > 0) {
        content_region_.push_back(Rect(x, y, w, h));
      }
    } while(pango_layout_iter_next_line(iter));

    pango_layout_iter_free(iter);
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

  void TextEntry::CursorBlinkCallback(void *data)
  {
    if (cursor_blink_status_)
    {
      ShowCursor();
    }
    else
    {
      HideCursor();
    }

    if (--cursor_blink_status_ < 0)
    {
      cursor_blink_status_ = 2;
    }

    cursor_blink_handle_ = GetWindowThread()->GetTimerHandler().AddTimerHandler(kCursorBlinkTimeout, cursor_blink_signal_, this, NULL);
  }

  void TextEntry::StartCursorBlink()
  {
    if (!cursor_blink_handle_.IsValid())
    {
      cursor_blink_handle_ = GetWindowThread()->GetTimerHandler().AddTimerHandler(kCursorBlinkTimeout, cursor_blink_signal_, this, NULL);
    }
  }

  void TextEntry::StopCursorBlink()
  {
    if (cursor_blink_handle_.IsValid())
    {
      GetWindowThread()->GetTimerHandler().RemoveTimerHandler(cursor_blink_handle_);
    }
  }

  void TextEntry::ShowCursor()
  {
    if (!cursor_visible_)
    {
      cursor_visible_ = true;

      if (has_keyboard_focused_ && !readonly_)
      {
        cursor_moved_ = true;
        QueueRefresh(false, MINIMAL_ADJUST);
      }
    }
  }

  void TextEntry::HideCursor()
  {
    if (cursor_visible_)
    {
      cursor_visible_ = false;

      if (has_keyboard_focused_ && !readonly_)
      {
        cursor_moved_ = true;
        QueueRefresh(false, MINIMAL_ADJUST);
      }
    }
  }

  void TextEntry::GetCursorRects(Rect *strong, Rect *weak)
  {
    PangoRectangle strong_pos;
    PangoRectangle weak_pos;
    GetCursorLocationInLayout(&strong_pos, &weak_pos);

    strong->x =
      strong_pos.x + kInnerBorderX + scroll_offset_x_ - kStrongCursorBarWidth;
    strong->width = kStrongCursorBarWidth * 2 + strong_pos.width;
    strong->y = strong_pos.y + kInnerBorderY + scroll_offset_y_ - 1;
    strong->height = strong_pos.height + 2;

    if (weak_pos.x != strong_pos.x) {
      weak->x =
        weak_pos.x + kInnerBorderX + scroll_offset_x_ - kWeakCursorBarWidth;
      weak->width = kWeakCursorBarWidth * 2 + weak_pos.width;
      weak->y = weak_pos.y+ kInnerBorderY + scroll_offset_y_ - 1;
      weak->height = weak_pos.height + 2;
    } else {
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
    if (!cursor_visible_ || !has_keyboard_focused_)
    {
      return;
    }

    PangoRectangle strong;
    PangoRectangle weak;
    GetCursorLocationInLayout(&strong, &weak);
    canvas->PushState();
    canvas->TranslateCoordinates(kInnerBorderX + scroll_offset_x_, kInnerBorderY + scroll_offset_y_);

    if (strong.height == 0)
    {
      // Happens when there is no text. We still want to see the cursor.
      strong.height = GetBaseHeight();
    }

    // Draw strong cursor.
    // TODO: Is the color ok?
    canvas->DrawFilledRect(strong.x, strong.y, strong.width, strong.height, kStrongCursorColor);

    if (strong.width > 1)
    {
      // Block cursor, ignore weak cursor.
      PangoLayout *layout = EnsureLayout();
      CairoGraphics *cairo_canvas = canvas;
      cairo_t *cr = cairo_canvas->GetContext();
      cairo_rectangle(cr, strong.x, strong.y, strong.width, strong.height);
      cairo_clip(cr);
      cairo_set_source_rgb(cr,
        kTextUnderCursorColor.red,
        kTextUnderCursorColor.green,
        kTextUnderCursorColor.blue);
      pango_cairo_show_layout(cr, layout);
    }
    else
    {
      // Draw a small arrow towards weak cursor
      if (strong.x > weak.x)
      {
        canvas->DrawFilledRect(strong.x - kStrongCursorBarWidth, strong.y,
          kStrongCursorBarWidth, kStrongCursorBarHeight,
          kStrongCursorColor);
      }
      else if (strong.x < weak.x)
      {
        canvas->DrawFilledRect(strong.x + strong.width, strong.y,
          kStrongCursorBarWidth, kStrongCursorBarHeight,
          kStrongCursorColor);
      }

      if (strong.x != weak.x )
      {
        // Draw weak cursor.
        // TODO: Is the color ok?
        canvas->DrawFilledRect(weak.x, weak.y, weak.width, weak.height,
          kWeakCursorColor);
        // Draw a small arror towards strong cursor
        if (weak.x > strong.x)
        {
          canvas->DrawFilledRect(weak.x - kWeakCursorBarWidth, weak.y,
            kWeakCursorBarWidth, kWeakCursorBarHeight,
            kWeakCursorColor);
        }
        else
        {
          canvas->DrawFilledRect(weak.x + weak.width, weak.y,
            kWeakCursorBarWidth, kWeakCursorBarHeight,
            kWeakCursorColor);
        }
      }
    }

    canvas->PopState();
  }

  Color TextEntry::GetSelectionBackgroundColor()
  {
    return kDefaultSelectionBackgroundColor;
  }

  Color TextEntry::GetSelectionTextColor()
  {
    return kDefaultSelectionTextColor;
  }

  void TextEntry::GetCursorLocationInLayout(PangoRectangle *strong, PangoRectangle *weak)
  {
    if (cursor_index_in_layout_ < 0) {
      // Recalculate cursor position.
      PangoLayout *layout = EnsureLayout();
      int index = TextIndexToLayoutIndex(cursor_, true);
      cursor_index_in_layout_ = index;

      pango_layout_get_cursor_pos(layout, index, &strong_cursor_pos_,
        &weak_cursor_pos_);
      strong_cursor_pos_.width = PANGO_SCALE;
      weak_cursor_pos_.width = PANGO_SCALE;

      if (overwrite_) {
        PangoRectangle pos;
        pango_layout_index_to_pos(layout, index, &pos);
        if (pos.width != 0) {
          if (pos.width < 0) {
            pos.x += pos.width;
            pos.width = -pos.width;
          }
          strong_cursor_pos_ = pos;
        }
        weak_cursor_pos_ = strong_cursor_pos_;
      }
    }

    if (strong) {
      strong->x = PANGO_PIXELS(strong_cursor_pos_.x);
      strong->y = PANGO_PIXELS(strong_cursor_pos_.y);
      strong->width = PANGO_PIXELS(strong_cursor_pos_.width);
      strong->height = PANGO_PIXELS(strong_cursor_pos_.height);
    }
    if (weak) {
      weak->x = PANGO_PIXELS(weak_cursor_pos_.x);
      weak->y = PANGO_PIXELS(weak_cursor_pos_.y);
      weak->width = PANGO_PIXELS(weak_cursor_pos_.width);
      weak->height = PANGO_PIXELS(weak_cursor_pos_.height);
    }
  }

  void TextEntry::ResetLayout()
  {
    if (cached_layout_) {
      g_object_unref(cached_layout_);
      cached_layout_ = NULL;
      content_modified_ = true;
      cursor_index_in_layout_ = -1;
    }
  }

  PangoLayout* TextEntry::EnsureLayout()
  {
    if (!cached_layout_)
    {
      cached_layout_ = CreateLayout();
    }
    return cached_layout_;
  }

  PangoLayout* TextEntry::CreateLayout()
  {
    // Pango context
    PangoContext* pango_context = pango_font_map_create_context(pango_cairo_font_map_get_default());

    // Layout and attributes
    PangoLayout* layout = pango_layout_new(pango_context);

    pango_layout_set_width(layout, 1 * PANGO_SCALE);
    pango_layout_set_height(layout, 1 * PANGO_SCALE);

    PangoContext *pango_ctx = pango_layout_get_context(layout); // is not ref'ed

//     // Set font
//     PangoFontDescription* font_desc = pango_font_description_from_string("Ubuntu 10");
//     pango_layout_set_font_description(pangoLayout, font_desc);
//     pango_font_description_free(font_desc);

    ////////////////////////////////////////////////////////
//     // Creates the pango layout with a temporary canvas that is not zoomed.
//     CairoGraphics *canvas = new CairoGraphics(CAIRO_FORMAT_ARGB32, 1, 1);
//     PangoLayout *layout = pango_cairo_create_layout(canvas->GetInternalContext());
//     delete canvas;

    PangoAttrList *tmp_attrs = pango_attr_list_new();

    std::string tmp_string;

    /* Set necessary parameters */
    pango_cairo_context_set_font_options(pango_ctx, font_options_);
    pango_cairo_context_set_resolution(pango_ctx, font_dpi_);


    if (wrap_)
    {
      // Sets the width to which the lines of the PangoLayout should wrap or ellipsized.
      pango_layout_set_width(layout, (GetBaseWidth() - kInnerBorderX * 2) * PANGO_SCALE);
      pango_layout_set_wrap(layout, wrap_mode_);
    }
    else
    {
      // No width set.
      pango_layout_set_width(layout, -1);
    }

    pango_layout_set_single_paragraph_mode(layout, !multiline_);

    if (preedit_.length())
    {
      size_t cursor_index = static_cast<size_t>(cursor_);
      size_t text_length = text_.length();
      size_t preedit_length = preedit_.length();
      if (characters_revealed_)
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
        {
          tmp_string.append(password_char_);
        }
        
        size_t cursor_offset = g_utf8_pointer_to_offset(text_.c_str(), text_.c_str() + cursor_index);

        /* Fix cursor index and preedit_length */
        cursor_index = cursor_offset * password_char_.length();
        preedit_length = preedit_nchars * password_char_.length();
      }
      if (preedit_attrs_)
      {
        pango_attr_list_splice(tmp_attrs, preedit_attrs_,
          static_cast<int>(cursor_index),
          static_cast<int>(preedit_length));
      }
    }
    else
    {
      if (characters_revealed_)
      {
        tmp_string = text_;
      }
      else
      {
        size_t nchars = g_utf8_strlen(text_.c_str(), text_.length());
        tmp_string.reserve(password_char_.length() * nchars);
        for (size_t i = 0; i < nchars; ++i)
        {
          tmp_string.append(password_char_);
        }
      }
    }

    pango_layout_set_text(layout, tmp_string.c_str(), static_cast<int>(tmp_string.length()));

    /* Set necessary attributes */
    PangoAttribute *attr;
    if (underline_)
    {
      attr = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
      attr->start_index = 0;
      attr->end_index = static_cast<guint>(tmp_string.length());
      pango_attr_list_insert(tmp_attrs, attr);
    }

    if (strikeout_)
    {
      attr = pango_attr_strikethrough_new(TRUE);
      attr->start_index = 0;
      attr->end_index = static_cast<guint>(tmp_string.length());
      pango_attr_list_insert(tmp_attrs, attr);
    }

    // Set font desc
    {
      PangoFontDescription *font_desc = pango_font_description_new();
      pango_font_description_set_family(font_desc, font_family_.c_str());
      pango_font_description_set_absolute_size(font_desc, font_size_ * PANGO_SCALE * 96 / 72);

      if (bold_)
      {
        pango_font_description_set_weight(font_desc, PANGO_WEIGHT_BOLD);
      }

      if (italic_)
      {
        pango_font_description_set_style(font_desc, PANGO_STYLE_ITALIC);
      }

      attr = pango_attr_font_desc_new(font_desc);
      attr->start_index = 0;
      attr->end_index = static_cast<unsigned int>(tmp_string.length());

      pango_attr_list_insert(tmp_attrs, attr);
      pango_layout_set_font_description(layout, font_desc);
      
      pango_font_description_free(font_desc);
    }

    pango_layout_set_attributes(layout, tmp_attrs);
    pango_attr_list_unref(tmp_attrs);

    // Set alignment according to text direction. Only set layout's alignment
    // when it's not wrapped and in single line mode.

    if (!wrap_ && (pango_layout_get_line_count(layout) <= 1) && (align_ != CairoGraphics::ALIGN_CENTER))
    {
      PangoDirection dir;
      if (characters_revealed_)
        dir = pango_find_base_dir(tmp_string.c_str(), static_cast<int>(tmp_string.length()));
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
      PangoAlignment pango_align = (align_ == CairoGraphics::ALIGN_RIGHT ? PANGO_ALIGN_RIGHT : PANGO_ALIGN_LEFT);

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
      if (characters_revealed_)
      {
        if (text_index < cursor_)
        {
          return text_index;
        }

        if ((text_index == cursor_) && consider_preedit_cursor)
        {
          return text_index + preedit_cursor_;
        }

        return text_index + static_cast<int>(preedit_.length());
      }

      const char *text = text_.c_str();
      int offset = static_cast<int>(g_utf8_pointer_to_offset(text, text + text_index));

      int preedit_offset = 0;
      int preedit_chars = 0;

      if (preedit_.length())
      {
        const char *preedit_text = preedit_.c_str();
        preedit_offset = static_cast<int>(g_utf8_pointer_to_offset(preedit_text, preedit_text + preedit_cursor_));
        preedit_chars = static_cast<int>(g_utf8_strlen(preedit_text, preedit_.length()));
      }

      int password_char_length = static_cast<int>(password_char_.length());

      if (text_index < cursor_)
      {
        return offset * password_char_length;
      }

      if (text_index == cursor_ && consider_preedit_cursor)
      {
        return (offset + preedit_offset) * password_char_length;
      }

      return (offset + preedit_chars) * password_char_length;
  }


  int TextEntry::LayoutIndexToTextIndex(int layout_index)
  {
    if (characters_revealed_)
    {
      if (layout_index < cursor_)
      {
        return layout_index;
      }

      int preedit_length = static_cast<int>(preedit_.length());
      if (layout_index >= cursor_ + preedit_length)
      {
        return layout_index - preedit_length;
      }
      return cursor_;
    }

    // Invisible mode doesn't support preedit.
    int password_char_length = static_cast<int>(password_char_.length());
    nuxAssert(layout_index % password_char_length == 0);

    int offset = layout_index / password_char_length;
    const char *text = text_.c_str();

    return static_cast<int>(g_utf8_offset_to_pointer(text, offset) - text);
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
      DeleteText(cursor_, MoveLogically(cursor_, 1));
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
    sigTextChanged.emit(this);
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
    sigTextChanged.emit(this);
  }

  void TextEntry::SelectWord()
  {
    // Find the beginning of the word around the cursor position
    int selection_bound = MoveWords(cursor_, -1);
    // Find the end of the word around the cursor position
    int cursor = MoveWords(selection_bound, 1);
    // In LTR languages: The cursor ends up on the right side of the word (end of the word)
    // In RTL languages: The cursor ends up on the left side of the word (end of the word)
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
    QueueRefresh(false, MINIMAL_ADJUST);
  }

  void TextEntry::SelectAll() {
    SetSelectionBounds(0, static_cast<int>(text_.length()));
    QueueRefresh(false, MINIMAL_ADJUST);
  }

  CairoGraphics::Alignment TextEntry::GetAlign() const
  {
    return align_;
  }

  void TextEntry::SetAlign(CairoGraphics::Alignment align)
  {
    align_ = align;
    QueueRefresh(true, CENTER_CURSOR);
  }

  void TextEntry::DeleteSelection()
  {
    int start, end;
    if (GetSelectionBounds(&start, &end))
    {
      DeleteText(start, end);
    }
  }

  void TextEntry::CopyClipboard()
  {
  }

  void TextEntry::CutClipboard()
  {
    CopyClipboard();
    DeleteSelection();
  }

  void TextEntry::PasteClipboard()
  {
  }

  void TextEntry::BackSpace(MovementStep step)
  {
    if (GetSelectionBounds(NULL, NULL))
    {
      DeleteSelection();
    }
    else
    {
      if (cursor_ == 0) return;
      DeleteText(MoveLogically(cursor_, -1), cursor_);
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
      DeleteText(cursor_, MoveLogically(cursor_, 1));
    }
  }

  void TextEntry::ToggleOverwrite()
  {
    overwrite_ = !overwrite_;

    // Force recalculate the cursor position.
    cursor_index_in_layout_ = -1;
    cursor_moved_ = true;
    QueueRefresh(false, NO_SCROLL);
  }

  void TextEntry::UpdateSelectionRegion()
  {
    selection_region_.clear();

    // Selection in a single line may be not continual, so we use pango to
    // get the x-ranges of each selection range in one line, and draw them
    // separately.
    int start_index, end_index;
    if (GetSelectionBounds(&start_index, &end_index)) {
      PangoLayout *layout = EnsureLayout();
      PangoRectangle line_extents, pos;
      int draw_start, draw_end;
      int *ranges;
      int n_ranges;
      int n_lines = pango_layout_get_line_count(layout);
      double x, y, w, h;

      start_index = TextIndexToLayoutIndex(start_index, false);
      end_index = TextIndexToLayoutIndex(end_index, false);

      for(int line_index = 0; line_index < n_lines; ++line_index) {
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
        for(int i = 0; i < n_ranges; ++i) {
          x = kInnerBorderX + scroll_offset_x_ + PANGO_PIXELS(ranges[i * 2]);
          y = kInnerBorderY + scroll_offset_y_ + PANGO_PIXELS(pos.y);
          w = PANGO_PIXELS(ranges[i * 2 + 1] - ranges[i * 2]);
          h = line_extents.height;
          if (x < GetBaseWidth() && x + w > 0 && y < GetBaseHeight() && y + h > 0) {
            selection_region_.push_back(Rect(x, y, w, h));
          }
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

    QueueRefresh(true, MINIMAL_ADJUST);
  }

  int TextEntry::MoveVisually(int current_index, int count)
  {
    nuxAssert((current_index >= 0) && (current_index <= static_cast<int>(text_.length())));
    nuxAssert(count);
    nuxAssert(preedit_.length() == 0);

    PangoLayout *layout = EnsureLayout();
    const char *text = pango_layout_get_text(layout);
    int index = TextIndexToLayoutIndex(current_index, false);
    int new_index = 0;
    int new_trailing = 0;
    while (count != 0)
    {
      if (count > 0)
      {
        --count;
        pango_layout_move_cursor_visually(layout, true, index, 0, 1, &new_index, &new_trailing);
      }
      else if (count < 0)
      {
        ++count;
        pango_layout_move_cursor_visually(layout, true, index, 0, -1, &new_index, &new_trailing);
      }
      index = new_index;
      
      if (index < 0 || index == G_MAXINT)
      {
        // Reached the beginning or end of the text
        return current_index;
      }
      index = static_cast<int>(g_utf8_offset_to_pointer(text + index, new_trailing) - text);
    }
    return LayoutIndexToTextIndex(index);
  }

  int TextEntry::MoveLogically(int current_index, int count)
  {
    nuxAssert(current_index >= 0 && current_index <= static_cast<int>(text_.length()));
    nuxAssert(count);

    PangoLayout *layout = EnsureLayout();
    const char *text = pango_layout_get_text(layout);
    int index = TextIndexToLayoutIndex(current_index, false);

    if (characters_revealed_)
    {
      PangoLogAttr *log_attrs;
      gint n_attrs;
      pango_layout_get_log_attrs(layout, &log_attrs, &n_attrs);
      const char *ptr = text + index;
      const char *end = text + text_.length() + preedit_.length();
      int offset = static_cast<int>(g_utf8_pointer_to_offset(text, ptr));

      while (count > 0 && ptr < end)
      {
        do
        {
          ptr = g_utf8_find_next_char(ptr, NULL);
          ++offset;
        } while (ptr && *ptr && !log_attrs[offset].is_cursor_position);
        --count;
        if (!ptr) ptr = end;
      }
      while(count < 0 && ptr > text)
      {
        do
        {
          ptr = g_utf8_find_prev_char(text, ptr);
          --offset;
        } while (ptr && *ptr && !log_attrs[offset].is_cursor_position);
        ++count;
        if (!ptr) ptr = text;
      }
      index = static_cast<int>(ptr - text);
      g_free(log_attrs);
    }
    else
    {
      int password_char_length = static_cast<int>(password_char_.length());
      int text_len = static_cast<int>(strlen(text));
      index = Clamp(index + count * password_char_length, 0, text_len);
    }

    return LayoutIndexToTextIndex(index);
  }

  int TextEntry::MoveWords(int current_index, int count)
  {
    nuxAssert((current_index >= 0) && (current_index <= static_cast<int>(text_.length())));
    nuxAssert(count);

    if (!characters_revealed_)
    {
      return (count > 0 ? static_cast<int>(text_.length()) : 0);
    }

    PangoLayout *layout = EnsureLayout();
    const char *text = pango_layout_get_text(layout);
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
    PangoLayoutLine *line = pango_layout_get_line_readonly(layout, line_index);
#else
    PangoLayoutLine *line = pango_layout_get_line(layout, line_index);
#endif
    // The cursor movement direction shall be determined by the direction of
    // current text line.
    if (line->resolved_dir == PANGO_DIRECTION_RTL)
    {
      count = -count;
    }

    const char *ptr = text + index;
    const char *end = text + text_.length() + preedit_.length();
    int offset = static_cast<int>(g_utf8_pointer_to_offset(text, ptr));

    PangoLogAttr *log_attrs;
    gint n_attrs;
    pango_layout_get_log_attrs(layout, &log_attrs, &n_attrs);
    
    while (count > 0 && ptr < end)
    {
      do
      {
        ptr = g_utf8_find_next_char(ptr, NULL);
        ++offset;
      } while (ptr && *ptr && !log_attrs[offset].is_word_start &&
        !log_attrs[offset].is_word_end &&
        !log_attrs[offset].is_sentence_boundary);

      --count;
      if (!ptr)
      {
        ptr = end;
      }
    }
    while(count < 0 && ptr > text)
    {
      do
      {
        ptr = g_utf8_find_prev_char(text, ptr);
        --offset;
      } while (ptr && *ptr && !log_attrs[offset].is_word_start &&
        !log_attrs[offset].is_word_end &&
        !log_attrs[offset].is_sentence_boundary);

      ++count;
      if (!ptr)
      {
        ptr = text;
      }
    }
    index = static_cast<int>(ptr - text);
    g_free(log_attrs);

    return LayoutIndexToTextIndex(index);
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

    if (line_index < 0) {
      return 0;
    } else if (line_index >= n_lines) {
      return static_cast<int>(text_.length());
    }

#if PANGO_VERSION_CHECK(1,16,0)
    PangoLayoutLine *line = pango_layout_get_line_readonly(layout, line_index);
#else
    PangoLayoutLine *line = pango_layout_get_line(layout, line_index);
#endif

    // Find out the cursor x offset related to the new line position.
    pango_layout_index_to_pos(layout, line->start_index, &rect);

    if (line->resolved_dir == PANGO_DIRECTION_RTL) {
      PangoRectangle extents;
      pango_layout_line_get_extents(line, NULL, &extents);
      rect.x -= extents.width;
    }

    // rect.x is the left edge position of the line in the layout
    x_off -= rect.x;
    if (x_off < 0) x_off = 0;

    int trailing;
    pango_layout_line_x_to_index(line, x_off, &index, &trailing);

    index = static_cast<int>(
      g_utf8_offset_to_pointer(text + index, trailing) - text);
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
//     ResetImContext();
//     // If there was a selection range, then the selection range will be cleared.
//     // Then content_modified_ shall be set to true to force redrawing the text.
//     if (cursor_ != selection_bound_)
//     {
//       selection_changed_ = true;
//     }
// 
//     cursor_ = cursor;
//     selection_bound_ = cursor;
//     cursor_moved_ = true;

    if (cursor != cursor_) {
      ResetImContext();
      // If there was a selection range, then the selection range will be cleared.
      // Then content_modified_ shall be set to true to force redrawing the text.
      if (cursor_ != selection_bound_)
        selection_changed_ = true;
      cursor_ = cursor;
      selection_bound_ = cursor;
      cursor_moved_ = true;

      // Force recalculate the cursor position.
      cursor_index_in_layout_ = -1;

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
    pango_layout_xy_to_index(layout, x * PANGO_SCALE, y * PANGO_SCALE, &index, &trailing);
    index = static_cast<int>(g_utf8_offset_to_pointer(text + index, trailing) - text);

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

  bool TextEntry::GetSelectionBounds(int *start, int *end)
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
    if (selection_bound_ != selection_bound || cursor_ != cursor) {
      selection_changed_ = true;
      selection_bound_ = selection_bound;
      if (cursor_ != cursor) {
        cursor_ = cursor;
        cursor_moved_ = true;

        // Force recalculate the cursor position.
        cursor_index_in_layout_ = -1;
      }
      //ResetImContext();
    }
  }

  void TextEntry::SetFontFamily(const char *font)
  {
    font_family_ = font;
    QueueRefresh(true, MINIMAL_ADJUST);
  }

  void TextEntry::SetFontSize(double font_size)
  {
    font_size_ = font_size;
    QueueRefresh(true, MINIMAL_ADJUST);
  }

  void TextEntry::SetFontOptions(const cairo_font_options_t *options)
  {
    g_return_if_fail(options);

    cairo_font_options_destroy(font_options_);
    font_options_ = cairo_font_options_copy(options);

    QueueRefresh(true, MINIMAL_ADJUST);
  }

  bool TextEntry::InspectKeyEvent(unsigned int eventType,
    unsigned int key_sym,
    const char* character)
  {
    if ((eventType == NUX_KEYDOWN) && (key_nav_mode_ == true) && (text_input_mode_ == false))
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
        return false;
      }
    }

    if ((eventType == NUX_KEYDOWN) && (key_nav_mode_ == true) && (text_input_mode_ == true))
    {
      // Enable to exit the TextEntry when in write mode(hack for unity dash)
      if (key_sym == NUX_VK_UP ||
      key_sym == NUX_VK_DOWN ||
      key_sym == NUX_VK_ESCAPE)
      {
        return false;
      }
    }

    if ((eventType == NUX_KEYDOWN) && (key_nav_mode_ == false) && (text_input_mode_ == false))
    {
      return false;
    }

    return true;
  }
}
