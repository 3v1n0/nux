

/*
  Copyright 2008 Google Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
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
  static const double kStrongCursorLineWidth = 1.2;
  static const double kStrongCursorBarWidth = 1.2;
  static const double kWeakCursorLineWidth = 3;
  static const double kWeakCursorBarWidth = 3;
  static const Color kStrongCursorColor(1.0f, 1.0f, 1.0f, 1.0f);
  static const Color kWeakCursorColor(1.0f, 1.0f, 1.0f, 0.5f);
  static const Color kDefaultTextColor(0, 0, 0, 1.0f);
  static const Color kDefaultBackgroundColor(1, 1, 1, 1.0f);
  static const Color kDefaultSelectionBackgroundColor(0.5, 0.5, 0.5, 1.0f);
  static const Color kDefaultSelectionTextColor(1, 1, 1, 1.0f);
  static const t_u64 kTripleClickTimeout = 500;
  static const std::string kDefaultFontName = "Ubuntu";

  static t_u64 GetCurrentTime()
  {
    GTimeVal tv;
    g_get_current_time(&tv);
    return static_cast<t_u64>(tv.tv_sec) * 1000 + tv.tv_usec / 1000;
  }

  static std::string CleanupLineBreaks(const char *source)
  {
    nuxAssert (source);
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

  CairoFont::CairoFont(const std::string &family,
    /*PangoFontDescription *font,*/
    double pt_size,
    Style style,
    Weight weight)
  {
    font_ = pango_font_description_new();

    pango_font_description_set_family(font_, family.c_str());
    // Calculate pixel size based on the Windows DPI of 96 for compatibility
    // reasons.
    
    size_ = pt_size * PANGO_SCALE * 96. / 72.;
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
    font_ = NULL;
  }

  NUX_IMPLEMENT_OBJECT_TYPE (TextEntry);

  TextEntry::TextEntry (const TCHAR* text, NUX_FILE_LINE_DECL)
    : View (NUX_FILE_LINE_PARAM)
  {
    canvas_ = NULL;
    //im_context_ = NULL;
    cached_layout_ = NULL;
    preedit_attrs_ = NULL;
    last_dblclick_time_ = 0;
    cursor_ = 0;
    preedit_cursor_ = 0;
    selection_bound_ = 0;
    scroll_offset_x_ = 0;
    scroll_offset_y_ = 0;
    cursor_blink_timer_ = 0;
    cursor_blink_status_ = 0;
    visible_ = true;
    focused_ = false;
    need_im_reset_ = false;
    overwrite_ = false;
    select_words_ = false;
    select_lines_ = false;
    button_ = false;
    bold_ = false;
    underline_ = false;
    strikeout_ = false;
    italic_ = false;
    multiline_ = false;
    wrap_ = false;
    cursor_visible_ = true;
    readonly_ = false;
    content_modified_ = false;
    //selection_changed_ = false;
    cursor_moved_ = false;
    update_canvas_ = false;
    //background_(new Texture(kDefaultBackgroundColor, 1)),
    _text_color = kDefaultTextColor;
    align_ = CairoGraphics::ALIGN_LEFT;

    _size_match_text = true;
    _text_color  = Color::White;
    _texture2D  = 0;

    font_family_ = "Ubuntu";
    font_size_ = 12;
    update_canvas_ = true;

    OnMouseDown.connect (sigc::mem_fun (this, &TextEntry::RecvMouseDown) );
    OnMouseDrag.connect (sigc::mem_fun (this, &TextEntry::RecvMouseDrag) );
    OnMouseUp.connect (sigc::mem_fun (this, &TextEntry::RecvMouseUp) );
    OnMouseDoubleClick.connect (sigc::mem_fun (this, &TextEntry::RecvMouseDoubleClick) );

    OnKeyEvent.connect (sigc::mem_fun (this, &TextEntry::RecvKeyEvent) );

    OnStartFocus.connect (sigc::mem_fun (this, &TextEntry::RecvStartKeyFocus) );
    OnEndFocus.connect (sigc::mem_fun (this, &TextEntry::RecvEndKeyFocus) );

    SetMinimumSize (DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    SetText (text);


    //MainDraw (canvas_);
  }

  TextEntry::~TextEntry ()
  {
    if (_texture2D)
      _texture2D->UnReference ();
  }

  void TextEntry::PreLayoutManagement ()
  {
    View::PreLayoutManagement ();
  }

  long TextEntry::PostLayoutManagement (long layoutResult)
  {
    long result = View::PostLayoutManagement (layoutResult);
    MainDraw ();
    return result;
  }

  long TextEntry::ProcessEvent (IEvent& event,
    long    traverseInfo,
    long    processEventInfo)
  {
    long ret = traverseInfo;

    ret = PostProcessEvent2 (event, ret, processEventInfo);
    return ret;
  }

  void TextEntry::ProcessMouseEvent (int event_type, int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    if (GetEventButton (button_flags) != 1)
      return;

    //ResetImContext();
    //Event::Type type = event.GetType();

    int X = static_cast<int>(x /*round(event.GetX())*/) - kInnerBorderX - scroll_offset_x_;
    int Y = static_cast<int>(y /*round(event.GetY())*/) - kInnerBorderY - scroll_offset_y_;
    int index = XYToTextIndex(X, Y);
    int sel_start, sel_end;
    GetSelectionBounds(&sel_start, &sel_end);

    t_u64 current_time = GetCurrentTime();

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
      SetSelectionBounds(selection_bound_, index);
    }

    QueueRefresh(false, true);
    //return EVENT_RESULT_HANDLED;
  }

  void TextEntry::ProcessKeyEvent (
    unsigned long    event_type  ,   /*event type*/
    unsigned long    keysym     ,   /*event keysym*/
    unsigned long    state      ,   /*event state*/
    const TCHAR*     character  ,   /*character*/
    unsigned short   keyCount       /*key repeat count*/)
  {
//     GdkEventKey *gdk_event = static_cast<GdkEventKey *>(event.GetOriginalEvent());
//     ASSERT(gdk_event);
// 
//     Event::Type type = event.GetType();
    // Cause the cursor to stop blinking for a while.
    cursor_blink_status_ = 4;

//     if (!readonly_ /*&& im_context_*/ && type != Event::EVENT_KEY_PRESS && 0/*&& gtk_im_context_filter_keypress(im_context_, gdk_event)*/)
//     {
//         need_im_reset_ = true;
//         QueueRefresh(false, true);
//         return EVENT_RESULT_HANDLED;
//     }

    if (event_type == NUX_KEYUP)
      return;

    if (character != 0 && (strlen (character) != 0))
    {
      EnterText(character);
      QueueRefresh(false, true);
      return;
    }

    unsigned int keyval = keysym;
    bool shift = (state & NUX_STATE_SHIFT);
    bool ctrl = (state & NUX_STATE_CTRL);

    // DLOG("TextEntry::OnKeyEvent(%d, shift:%d ctrl:%d)", keyval, shift, ctrl);

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
//       else if ((keyval == GDK_x && ctrl && !shift) ||
//         (keyval == GDK_Delete && shift && !ctrl))
//       {
//           CutClipboard();
//       }
//       else if ((keyval == GDK_c && ctrl && !shift) ||
//         (keyval == GDK_Insert && ctrl && !shift))
//       {
//           CopyClipboard();
//       }
//       else if ((keyval == GDK_v && ctrl && !shift) ||
//         (keyval == GDK_Insert && shift && !ctrl))
//       {
//           PasteClipboard();
//       }
      else if (keyval == NUX_VK_BACKSPACE)
      {
        BackSpace();
      }
      else if (keyval == NUX_VK_DELETE && !shift)
      {
        Delete();
      }
      else if (keyval == NUX_VK_INSERT && !shift && !ctrl)
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

    QueueRefresh(false, true);
    return;
  }

  void TextEntry::RecvMouseDoubleClick (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    ProcessMouseEvent(NUX_MOUSE_DOUBLECLICK, x, y, 0, 0, button_flags, key_flags);
  }

  void TextEntry::RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    ProcessMouseEvent(NUX_MOUSE_RELEASED, x, y, 0, 0, button_flags, key_flags);
  }

  void TextEntry::RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    ProcessMouseEvent(NUX_MOUSE_PRESSED, x, y, 0, 0, button_flags, key_flags);
  }

  void TextEntry::RecvMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    ProcessMouseEvent(NUX_MOUSE_MOVE, x, y, dx, dy, button_flags, key_flags);
  }

  void TextEntry::RecvKeyEvent (
    GraphicsEngine &GfxContext ,   /*Graphics Context for text operation*/
    unsigned long    eventType  ,   /*event type*/
    unsigned long    keysym     ,   /*event keysym*/
    unsigned long    state      ,   /*event state*/
    const TCHAR*     character  ,   /*character*/
    unsigned short   keyCount       /*key repeat count*/)
  {
    ProcessKeyEvent(eventType, keysym, state, character, keyCount);
  }

  void TextEntry::RecvStartKeyFocus ()
  {
    FocusInx ();
  }

  void TextEntry::RecvEndKeyFocus ()
  {
    FocusOutx ();
  }

  void TextEntry::Draw (GraphicsEngine& gfxContext, bool forceDraw)
  {
    MainDraw ();
    Geometry base = GetGeometry ();

    gfxContext.PushClippingRectangle (base);

    Color col = Color::Black;
    col.SetAlpha (0.0f);
    gfxContext.QRP_Color (base.x,
      base.y,
      base.width,
      base.height,
      col);

    TexCoordXForm texxform;
    texxform.SetWrap (TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    texxform.SetTexCoordType (TexCoordXForm::OFFSET_COORD);
    gfxContext.QRP_1Tex (base.x,
      base.y,
      base.width,
      base.height,
      _texture2D->GetDeviceTexture(),
      texxform,
      _text_color);

    gfxContext.PopClippingRectangle ();
  }

  void TextEntry::DrawContent (GraphicsEngine& gfxContext, bool forceDraw)
  {
    //MainDraw ();
  }

  void TextEntry::PostDraw (GraphicsEngine& gfxContext, bool forceDraw)
  {
    // intentionally left empty
  }

  void TextEntry::SetText (const char *text)
  {
    const char *end = NULL;
    g_utf8_validate(text, -1, &end);

    std::string txt((text && *text && end > text) ? std::string(text, end) : "");
    if (txt == _text)
      return; // prevent some redraws

    _text = multiline_ ? txt : CleanupLineBreaks(txt.c_str());
    cursor_ = 0;
    selection_bound_ = 0;
    need_im_reset_ = true;
    //ResetImContext();
    QueueRefresh(true, true);
    sigTextChanged.emit (this);
  }

  std::string TextEntry::GetText ()
  {
    return _text;
  }

  void TextEntry::SetTextColor (const Color &text_color)
  {
    if (_text_color != text_color)
    {
      _text_color = text_color;
    }
  }

  Color TextEntry::GetTextColor () const
  {
    return _text_color;
  }

  
  void TextEntry::MainDraw ()
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

    CairoGraphics* final_canvas = new CairoGraphics (CAIRO_FORMAT_ARGB32, edit_canvas->GetWidth (), edit_canvas->GetHeight ());

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

    NBitmapData* bitmap = final_canvas->GetBitmap ();
    delete final_canvas;

    if (_texture2D)
      _texture2D->UnReference ();

    _texture2D = GetGpuDevice ()->CreateSystemCapableTexture ();
    _texture2D->Update (bitmap);

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
        //gtk_im_context_focus_in(im_context_);
        //UpdateIMCursorLocation();
      }
      selection_changed_ = true;
      cursor_moved_ = true;
      // Don't adjust scroll.
      QueueRefresh(false, false);
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
        //gtk_im_context_focus_out(im_context_);
      }
      selection_changed_ = true;
      cursor_moved_ = true;
      // Don't adjust scroll.
      QueueRefresh(false, false);
    }
  }

  CairoGraphics* TextEntry::EnsureCanvas()
  {
    if (canvas_)
    {
      if ((GetBaseWidth () == canvas_->GetWidth()) && (GetBaseHeight () == canvas_->GetHeight()))
      {
        return canvas_;
      }
      else
      {
        nuxDebugMsg(TEXT("[TextEntry::EnsureCanvas] Recreate canvas"));
        delete canvas_;
        canvas_ = NULL;
      }
    }
    canvas_ = new CairoGraphics (CAIRO_FORMAT_ARGB32, GetBaseWidth (), GetBaseHeight ());
    nuxAssert (canvas_);
    return canvas_;
  }

  void TextEntry::AdjustScroll ()
  {
    int old_offset_x = scroll_offset_x_;
    int old_offset_y = scroll_offset_y_;
    int display_width = GetBaseWidth () - kInnerBorderX * 2;
    int display_height = GetBaseHeight () - kInnerBorderY * 2;

    PangoLayout *layout = EnsureLayout();
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
    //todo: QueueCursorBlink();
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
    if (_preedit.length())
      ResetLayout();

    _preedit.clear();
    preedit_cursor_ = 0;
    if (preedit_attrs_) {
      pango_attr_list_unref(preedit_attrs_);
      preedit_attrs_ = NULL;
    }
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
        _text_color.R(),
        _text_color.G(),
        _text_color.B());

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
        selection_color.R(),
        selection_color.G(),
        selection_color.B());
      cairo_paint(canvas->GetInternalContext());

      cairo_move_to(canvas->GetInternalContext(),
        scroll_offset_x_ + kInnerBorderX,
        scroll_offset_y_ + kInnerBorderY);
      cairo_set_source_rgb(canvas->GetInternalContext(),
        text_color.R(),
        text_color.G(),
        text_color.B());
      pango_cairo_show_layout(canvas->GetInternalContext(), layout);
      canvas->PopState();
    }
  }

  void TextEntry::ShowCursor()
  {
    if (!cursor_visible_)
    {
      cursor_visible_ = true;
      if (focused_ && !readonly_)
      {
        cursor_moved_ = true;
        QueueTextDraw();
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
        QueueTextDraw();
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
    if (!cursor_visible_ || !focused_)
      return;

    int strong_x, strong_y, strong_height;
    int weak_x, weak_y, weak_height;
    GetCursorLocationInLayout(&strong_x, &strong_y, &strong_height,
      &weak_x, &weak_y, &weak_height);

    // Draw strong cursor.
    canvas->DrawLine(strong_x + kInnerBorderX + scroll_offset_x_,
      strong_y + kInnerBorderY + scroll_offset_y_,
      strong_x + kInnerBorderX + scroll_offset_x_,
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

    if (_preedit.length())
    {
      size_t cursor_index = static_cast<size_t>(cursor_);
      size_t text_length = _text.length();
      size_t preedit_length = _preedit.length();
      if (visible_)
      {
        tmp_string = _text;
        tmp_string.insert(cursor_index, _preedit);
      }
      else
      {
        size_t nchars = g_utf8_strlen(_text.c_str(), text_length);
        size_t preedit_nchars = g_utf8_strlen(_preedit.c_str(), preedit_length);
        nchars += preedit_nchars;
        tmp_string.reserve(password_char_.length() * nchars);
        for (size_t i = 0; i < nchars; ++i)
          tmp_string.append(password_char_);
        size_t cursor_offset =
            g_utf8_pointer_to_offset(_text.c_str(), _text.c_str() + cursor_index);
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
      if(visible_)
      {
        tmp_string = _text;
      }
      else
      {
        size_t nchars = g_utf8_strlen(_text.c_str(), _text.length());
        tmp_string.reserve(password_char_.length() * nchars);
        for (size_t i = 0; i < nchars; ++i)
          tmp_string.append(password_char_);
      }
    }

    pango_layout_set_text(layout, tmp_string.c_str(),
                          static_cast<int>(tmp_string.length()));

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
    /* Set font desc */
    {
      /* safe to down_cast here, because we know the actual implementation. */
      CairoFont *font = new CairoFont (
              font_family_.empty() ? kDefaultFontName : font_family_.c_str(),
              font_size_,
              italic_ ? CairoFont::STYLE_ITALIC : CairoFont::STYLE_NORMAL,
              bold_ ? CairoFont::WEIGHT_BOLD : CairoFont::WEIGHT_NORMAL);
      nuxAssert(font);
      attr = pango_attr_font_desc_new(font->GetFontDescription());
      attr->start_index = 0;
      attr->end_index = static_cast<unsigned int>(tmp_string.length());
      pango_attr_list_insert(tmp_attrs, attr);
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

        return text_index + static_cast<int>(_preedit.length());
      }

      const char *text = _text.c_str();
      int offset = static_cast<int>(
        g_utf8_pointer_to_offset(text, text + text_index));
      int preedit_offset = 0;
      int preedit_chars = 0;
      if (_preedit.length())
      {
        const char *preedit_text = _preedit.c_str();
        preedit_offset = static_cast<int>(g_utf8_pointer_to_offset(
          preedit_text, preedit_text + preedit_cursor_));
        preedit_chars = static_cast<int>(g_utf8_strlen(
          preedit_text, _preedit.length()));
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

      int preedit_length = static_cast<int>(_preedit.length());
      if (layout_index >= cursor_ + preedit_length)
        return layout_index - preedit_length;

      return cursor_;
    }

    int password_char_length = static_cast<int>(password_char_.length());
    nuxAssert(layout_index % password_char_length == 0);

    int offset = layout_index / password_char_length;

    const char *text = _text.c_str();
    int cursor_offset = static_cast<int>(
      g_utf8_pointer_to_offset(text, text + cursor_));
    int preedit_chars = static_cast<int>(
      g_utf8_strlen(_preedit.c_str(), _preedit.length()));

    if (offset < cursor_offset)
      return static_cast<int>(g_utf8_offset_to_pointer(text, offset) - text);

    if (offset >= cursor_offset + preedit_chars)
      return static_cast<int>(
      g_utf8_offset_to_pointer(text, offset - preedit_chars) - text);

    return cursor_;
  }

  int TextEntry::GetCharLength(int index)
  {
    const char *text = _text.c_str();
    const char *ptr = text + index;
    const char *end = text + _text.length();
    const char *next = g_utf8_find_next_char(ptr, end);
    return static_cast<int>(next ? static_cast<int>(next - ptr) : end - ptr);
  }

  int TextEntry::GetPrevCharLength(int index)
  {
    const char *text = _text.c_str();
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
    else if (overwrite_ && cursor_ != static_cast<int>(_text.length()))
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
      _text.insert(cursor_, str, len);
      cursor_ += static_cast<int>(len);
      selection_bound_ += static_cast<int>(len);
    }

    ResetLayout();
    sigTextChanged.emit (this);
  }

  void TextEntry::DeleteText(int start, int end)
  {
    if (readonly_) return;

    int text_length = static_cast<int>(_text.length());
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

    _text.erase(start, end - start);

    if (cursor_ >= end)
      cursor_ -= (end - start);
    if (selection_bound_ >= end)
      selection_bound_ -= (end - start);

    ResetLayout();
    sigTextChanged.emit (this);
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
    int text_length = static_cast<int>(_text.length());
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
    SetSelectionBounds(0, static_cast<int>(_text.length()));
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

  void TextEntry::BackSpace()
  {
    if (GetSelectionBounds(NULL, NULL))
    {
      DeleteSelection();
    }
    else
    {
      if (cursor_ == 0)
        return;
      DeleteText(cursor_ - GetPrevCharLength(cursor_), cursor_);
    }
  }

  void TextEntry::Delete()
  {
    if (GetSelectionBounds(NULL, NULL))
    {
      DeleteSelection();
    }
    else
    {
      if (cursor_ == static_cast<int>(_text.length()))
        return;
      DeleteText(cursor_, cursor_ + GetCharLength(cursor_));
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

      for(int line_index = 0; line_index < n_lines; ++line_index)
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
        draw_start = Max<int>(start_index, line->start_index);
        draw_end = Min<int>(end_index, line->start_index + line->length);
        pango_layout_line_get_x_ranges(line, draw_start, draw_end,
          &ranges, &n_ranges);
        pango_layout_line_get_pixel_extents(line, NULL, &line_extents);
        pango_layout_index_to_pos(layout, line->start_index,  &pos);
        for(int i = 0; i < n_ranges; ++i) 
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
    if (cursor_ != selection_bound_ && !extend_selection)
      SetCursor(cursor_);

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
      new_cursor = static_cast<int>(count == -1 ? 0 : _text.length());
      break;
    }

    if (extend_selection)
      SetSelectionBounds(selection_bound_, new_cursor);
    else
      SetCursor(new_cursor);

    QueueRefresh(false, true);
  }

  int TextEntry::MoveVisually(int current_index, int count)
  {
    nuxAssert(current_index >= 0 &&
      current_index <= static_cast<int>(_text.length()));
    nuxAssert(count);
    nuxAssert(_preedit.length() == 0);

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
      current_index <= static_cast<int>(_text.length()));
    nuxAssert(count);
    nuxAssert(_preedit.length() == 0);

    if (!visible_)
    {
      return static_cast<int>(count > 0 ? _text.length() : 0);
    }

    // The cursor movement direction shall be determined by the direction of
    // current text line.
    PangoLayout *layout = EnsureLayout();
    int n_log_attrs;
    PangoLogAttr *log_attrs;
    pango_layout_get_log_attrs (layout, &log_attrs, &n_log_attrs);
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
    bool rtl = (line->resolved_dir == PANGO_DIRECTION_RTL);
    const char *ptr = text + index;
    int offset = static_cast<int>(g_utf8_pointer_to_offset(text, ptr));
    while (count != 0)
    {
      if (((rtl && count < 0) || (!rtl && count > 0)) && *ptr)
      {
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
      current_index <= static_cast<int>(_text.length()));
    nuxAssert(count);
    nuxAssert(_preedit.length() == 0);

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
      return static_cast<int>(_text.length());
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
      current_index <= static_cast<int>(_text.length()));
    nuxAssert(count);
    nuxAssert(_preedit.length() == 0);

    // Transfer pages to display lines.
    PangoLayout *layout = EnsureLayout();
    int layout_height;
    pango_layout_get_pixel_size(layout, NULL, &layout_height);
    int n_lines = pango_layout_get_line_count(layout);
    int line_height = layout_height / n_lines;
    int page_lines = (GetBaseHeight () - kInnerBorderY * 2) / line_height;
    return MoveDisplayLines(current_index, count * page_lines);
  }

  int TextEntry::MoveLineEnds(int current_index, int count)
  {
    nuxAssert(current_index >= 0 &&
      current_index <= static_cast<int>(_text.length()));
    nuxAssert(count);
    nuxAssert(_preedit.length() == 0);

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
      return static_cast<int>(_text.length());
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
    int preedit_length = static_cast<int>(_preedit.length());
    if (preedit_length && index > cursor_)
    {
      if (index >= cursor_ + preedit_length)
        index -= preedit_length;
      else
        index = cursor_;
    }
    return Clamp(index, 0, static_cast<int>(_text.length()));
  }

  bool TextEntry::GetSelectionBounds(int *start, int *end)
  {
    if (start)
      *start = Min<int>(selection_bound_, cursor_);
    if (end)
      *end = Max<int>(selection_bound_, cursor_);

    return(selection_bound_ != cursor_);
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
      }
      
      //ResetImContext();
    }
  }
}
