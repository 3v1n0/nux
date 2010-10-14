#include "Nux/Nux.h"
#include "Nux/VLayout.h"
#include "Nux/HLayout.h"
#include "Nux/WindowThread.h"
#include "Nux/WindowCompositor.h"
#include "Nux/BaseWindow.h"
#include "Nux/Button.h"
#include "NuxGraphics/OpenGLEngine.h"
#include "NuxGraphics/GfxEventsX11.h"
#include "NuxGraphics/GfxSetupX11.h"
#include "Nux/TextureArea.h"
#include "NuxImage/CairoGraphics.h"

#include <pango/pango.h>
#include <pango/pangocairo.h>

#if defined(NUX_OS_LINUX)
#include <X11/Xlib.h>
#endif

#define ANCHOR_WIDTH   10.0f
#define ANCHOR_HEIGHT  18.0f
#define RADIUS         5.0f
#define BLUR_INTENSITY 8
#define LINE_WIDTH     1.0f
#define PADDING_SIZE   0
#define MARGIN         5

namespace nux
{
  class Tooltip : public BaseWindow
  {
    public:
      Tooltip (int     x,
               int     y,
               NString text);
      ~Tooltip ();

      long ProcessEvent (IEvent& iEvent,
                         long    traverseInfo,
                         long    processEventInfo);

      void Draw (GraphicsContext& gfxContext,
                 bool             forceDraw);

      void DrawContent (GraphicsContext& gfxContext,
                        bool             forceDraw);

    protected:
      void PreLayoutManagement ();

      long PostLayoutManagement (long layoutResult);

      void PositionChildLayout (float offsetX,
                                float offsetY);

      void LayoutWindowElements ();

      void NotifyConfigurationChange (int width,
                                      int height);
                                      
      nux::CairoGraphics*   _cairo_graphics;
      nux::NTexture*        _texture2D;
      int                   _anchorX;
      int                   _anchorY;
      nux::NString          _labelText;
      int                   _dpiX;
      int                   _dpiY;
      cairo_font_options_t* _fontOpts;

    private:
      void ComputeFullMaskPath (cairo_t* cr,
                                gfloat   anchor_width,
                                gfloat   anchor_height,
                                gint     width,
                                gint     height,
                                gint     upper_size,
                                gfloat   radius,
                                guint    pad);

      void DrawDraw (cairo_t* cr,
                     gboolean outline,
                     gfloat   line_width,
                     gfloat*  rgba,
                     gboolean negative,
                     gboolean stroke);

      void DrawTintDotHighlight (cairo_t* cr,
                                 gint     width,
                                 gint     height,
                                 gfloat   hl_x,
                                 gfloat   hl_y,
                                 gfloat   hl_size,
                                 gfloat*  rgba_tint,
                                 gfloat*  rgba_hl);

      void DrawOutlineShadow (cairo_t* cr,
                              gint     width,
                              gint     height,
                              gfloat   anchor_width,
                              gfloat   anchor_height,
                              gint     upper_size,
                              gfloat   corner_radius,
                              guint    blur_coeff,
                              gfloat*  rgba_shadow,
                              gfloat   line_width,
                              gint     padding_size,
                              gfloat*  rgba_line);

      void ComputeOutline (cairo_t* cr,
                           gfloat   line_width,
                           gfloat*  rgba_line);

      void DrawMask (cairo_t* cr,
                     gint     width,
                     gint     height,
                     gfloat   radius,
                     guint    shadow_radius,
                     gfloat   anchor_width,
                     gfloat   anchor_height,
                     gint     upper_size,
                     gboolean negative,
                     gboolean outline,
                     gfloat   line_width,
                     gint     padding_size,
                     gfloat*  rgba);

      void GetDPI ();

      void GetTextExtents (char* font,
                           int*  width,
                           int*  height);

      void DrawLabel (cairo_t* cr,
                      gint     width,
                      gint     height,
                      gfloat*  rgba);
  };

  void
  DrawCairo (cairo_t* cr,
             gboolean outline,
             gfloat   line_width,
             gfloat*  rgba,
             gboolean negative,
             gboolean stroke)
  {
    // prepare drawing
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);

    // actually draw the mask
    if (outline)
      {
        cairo_set_line_width (cr, line_width);
        cairo_set_source_rgba (cr, rgba[0], rgba[1], rgba[2], rgba[3]);
      }
    else
      {
        if (negative)
          cairo_set_source_rgba (cr, 1.0f, 1.0f, 1.0f, 1.0f);
        else
          cairo_set_source_rgba (cr, 0.0f, 0.0f, 0.0f, 0.0f);
      }

    // stroke or fill?
    if (stroke)
      cairo_stroke_preserve (cr);
    else
      cairo_fill_preserve (cr);
  }

  void
  Tooltip::ComputeFullMaskPath (cairo_t* cr,
                                gfloat   anchor_width,
                                gfloat   anchor_height,
                                gint     width,
                                gint     height,
                                gint     upper_size,
                                gfloat   radius,
                                guint    pad)
  {
    //     0  1        2  3
    //     +--+--------+--+
    //     |              |
    //     + 14           + 4
    //     |              |
    //     |              |
    //     |              |
    //     + 13           |
    //    /               |
    //   /                |
    //  + 12              |
    //   \                |
    //    \               |
    //  11 +              |
    //     |              |
    //     |              |
    //     |              |
    //  10 +              + 5
    //     |              |
    //     +--+--------+--+ 6
    //     9  8        7

    gfloat padding    = pad;
    int    ZEROPOINT5 = 0.0f;
  
    gfloat HeightToAnchor = 0.0f;
    HeightToAnchor = ((gfloat) height - 2.0f * radius - anchor_height -2*padding) / 2.0f;
    if (HeightToAnchor < 0.0f)
    {
      g_warning ("Anchor-height and corner-radius a higher than whole texture!");
      return;
    }

    //gint dynamic_size = height - 2*radius - 2*padding - anchor_height;
    //gint upper_dynamic_size = upper_size;
    //gint lower_dynamic_size = dynamic_size - upper_dynamic_size;
  
    if (upper_size >= 0)
      {
        if(upper_size > height - 2.0f * radius - anchor_height -2 * padding)
          {
            //g_warning ("[_compute_full_mask_path] incorrect upper_size value");
            HeightToAnchor = 0;
          }
        else
          {
            HeightToAnchor = height - 2.0f * radius - anchor_height -2 * padding - upper_size;
          }
      }
    else
      {
        HeightToAnchor = (height - 2.0f * radius - anchor_height -2*padding) / 2.0f;
      }

    cairo_translate (cr, -0.5f, -0.5f);
    
    // create path
    cairo_move_to (cr, padding + anchor_width + radius + ZEROPOINT5, padding + ZEROPOINT5); // Point 1
    cairo_line_to (cr, width - padding - radius, padding + ZEROPOINT5);   // Point 2
    cairo_arc (cr,
               width  - padding - radius + ZEROPOINT5,
               padding + radius + ZEROPOINT5,
               radius,
               -90.0f * G_PI / 180.0f,
               0.0f * G_PI / 180.0f);   // Point 4
    cairo_line_to (cr,
                   (gdouble) width - padding + ZEROPOINT5,
                   (gdouble) height - radius - padding + ZEROPOINT5); // Point 5
    cairo_arc (cr,
               (gdouble) width - padding - radius + ZEROPOINT5,
               (gdouble) height - padding - radius + ZEROPOINT5,
               radius,
               0.0f * G_PI / 180.0f,
               90.0f * G_PI / 180.0f);  // Point 7
    cairo_line_to (cr,
                   anchor_width + padding + radius + ZEROPOINT5,
                   (gdouble) height - padding + ZEROPOINT5); // Point 8

    cairo_arc (cr,
               anchor_width + padding + radius + ZEROPOINT5,
               (gdouble) height - padding - radius,
               radius,
               90.0f * G_PI / 180.0f,
               180.0f * G_PI / 180.0f); // Point 10

    cairo_line_to (cr,
                   padding + anchor_width + ZEROPOINT5,
                   (gdouble) height - padding - radius - HeightToAnchor + ZEROPOINT5 );  // Point 11
    cairo_line_to (cr,
                   padding + ZEROPOINT5,
                   (gdouble) height - padding - radius - HeightToAnchor - anchor_height / 2.0f + ZEROPOINT5); // Point 12
    cairo_line_to (cr,
                   padding + anchor_width + ZEROPOINT5,
                   (gdouble) height - padding - radius - HeightToAnchor - anchor_height + ZEROPOINT5);  // Point 13
  
    cairo_line_to (cr, padding + anchor_width + ZEROPOINT5, padding + radius  + ZEROPOINT5);  // Point 14
    cairo_arc (cr,
               padding + anchor_width + radius + ZEROPOINT5,
               padding + radius + ZEROPOINT5,
               radius,
               180.0f * G_PI / 180.0f,
               270.0f * G_PI / 180.0f);

    cairo_close_path (cr);
  }

  void
  ComputeMask (cairo_t* cr)
  {
    cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
    cairo_fill_preserve (cr);
  }

  void
  Tooltip::ComputeOutline (cairo_t* cr,
                           gfloat   line_width,
                           gfloat*  rgba_line)
  {
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgba (cr,
                         rgba_line[0],
                         rgba_line[1],
                         rgba_line[2],
                         rgba_line[3]);
    cairo_set_line_width (cr, line_width);
    cairo_stroke (cr);
  }

  void
  Tooltip::DrawTintDotHighlight (cairo_t* cr,
                                 gint     width,
                                 gint     height,
                                 gfloat   hl_x,
                                 gfloat   hl_y,
                                 gfloat   hl_size,
                                 gfloat*  rgba_tint,
                                 gfloat*  rgba_hl)
  {
    cairo_surface_t* dots_surf    = NULL;
    cairo_t*         dots_cr      = NULL;
    cairo_pattern_t* dots_pattern = NULL;
    cairo_pattern_t* hl_pattern   = NULL;

    // create context for dot-pattern
    dots_surf = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 4, 4);
    dots_cr = cairo_create (dots_surf);

    // clear normal context
    cairo_scale (cr, 1.0f, 1.0f);
    cairo_set_source_rgba (cr, 0.0f, 0.0f, 0.0f, 0.0f);
    cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint (cr);

    // prepare drawing for normal context
    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);

    // create path in normal context
    ComputeFullMaskPath (cr,
                         ANCHOR_WIDTH,
                         ANCHOR_HEIGHT,
                         width,
                         height,
                         -1,
                         RADIUS,
                         PADDING_SIZE);
    //cairo_rectangle (cr, 0.0f, 0.0f, (gdouble) width, (gdouble) height);  

    // fill path of normal context with tint
    cairo_set_source_rgba (cr,
                           rgba_tint[0],
                           rgba_tint[1],
                           rgba_tint[2],
                           rgba_tint[3]);
    cairo_fill_preserve (cr);

    // create pattern in dot-context
    cairo_set_operator (dots_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint (dots_cr);
    cairo_scale (dots_cr, 1.0f, 1.0f);
    cairo_set_operator (dots_cr, CAIRO_OPERATOR_OVER);
    cairo_set_source_rgba (dots_cr,
                           rgba_hl[0],
                           rgba_hl[1],
                           rgba_hl[2],
                           rgba_hl[3]);
    cairo_rectangle (dots_cr, 0.0f, 0.0f, 1.0f, 1.0f);
    cairo_fill (dots_cr);
    cairo_rectangle (dots_cr, 2.0f, 2.0f, 1.0f, 1.0f);
    cairo_fill (dots_cr);
    dots_pattern = cairo_pattern_create_for_surface (dots_surf);

    // fill path of normal context with dot-pattern
    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
    cairo_set_source (cr, dots_pattern);
    cairo_pattern_set_extend (dots_pattern, CAIRO_EXTEND_REPEAT);
    cairo_fill_preserve (cr);
    cairo_pattern_destroy (dots_pattern);
    cairo_surface_destroy (dots_surf);
    cairo_destroy (dots_cr);

    // draw highlight
    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
    hl_pattern = cairo_pattern_create_radial (hl_x,
                                              hl_y,
                                              0.0f,
                                              hl_x,
                                              hl_y,
                                              hl_size);
    cairo_pattern_add_color_stop_rgba (hl_pattern,
                                       0.0f,
                                       1.0f,
                                       1.0f,
                                       1.0f,
                                       0.65f);
    cairo_pattern_add_color_stop_rgba (hl_pattern,
                                       1.0f,
                                       1.0f,
                                       1.0f,
                                       1.0f,
                                       0.0f);
    cairo_set_source (cr, hl_pattern);
    cairo_fill (cr);
    cairo_pattern_destroy (hl_pattern);
  }

  void
  Tooltip::DrawMask (cairo_t* cr,
                     gint     width,
                     gint     height,
                     gfloat   radius,
                     guint    shadow_radius,
                     gfloat   anchor_width,
                     gfloat   anchor_height,
                     gint     upper_size,
                     gboolean negative,
                     gboolean outline,
                     gfloat   line_width,
                     gint     padding_size,
                     gfloat*  rgba)
  {
    ComputeFullMaskPath (cr,
                         anchor_width,
                         anchor_height,
                         width,
                         height,
                         upper_size,
                         radius,
                         padding_size);
  }

  void
  Tooltip::GetDPI ()
  {
#if defined(NUX_OS_LINUX)
    Display* display     = NULL;
    int      screen      = 0;
    double   dpyWidth    = 0.0;
    double   dpyHeight   = 0.0;
    double   dpyWidthMM  = 0.0;
    double   dpyHeightMM = 0.0;
    double   dpiX        = 0.0;
    double   dpiY        = 0.0;

    display = XOpenDisplay (NULL);
    screen = DefaultScreen (display);

    dpyWidth    = (double) DisplayWidth (display, screen);
    dpyHeight   = (double) DisplayHeight (display, screen);
    dpyWidthMM  = (double) DisplayWidthMM (display, screen);
    dpyHeightMM = (double) DisplayHeightMM (display, screen);

    dpiX = dpyWidth * 25.4 / dpyWidthMM;
    dpiY = dpyHeight * 25.4 / dpyHeightMM;

    _dpiX = (int) (dpiX + 0.5);
    _dpiY = (int) (dpiY + 0.5);

    XCloseDisplay (display);
#elif defined(NUX_OS_WINDOWS)
    _dpiX = 72;
    _dpiY = 72;
#endif
  }

  void
  Tooltip::GetTextExtents (char* font,
                           int*  width,
                           int*  height)
  {
    cairo_surface_t*      surface  = NULL;
    cairo_t*              cr       = NULL;
    PangoLayout*          layout   = NULL;
    PangoFontDescription* desc     = NULL;
    PangoContext*         pangoCtx = NULL;
    PangoRectangle        logRect  = {0, 0, 0, 0};

    // sanity check
    if (!font || !width || !height)
      return;

    surface = cairo_image_surface_create (CAIRO_FORMAT_A1, 1, 1);
    cr = cairo_create (surface);
    layout = pango_cairo_create_layout (cr);
    desc = pango_font_description_from_string (font);
    pango_font_description_set_weight (desc, PANGO_WEIGHT_NORMAL);
    pango_layout_set_font_description (layout, desc);
    pango_layout_set_wrap (layout, PANGO_WRAP_WORD_CHAR);
    pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_END);
    pango_layout_set_text (layout, _labelText.GetTCharPtr(), -1);
    pangoCtx = pango_layout_get_context (layout); // is not ref'ed
    pango_cairo_context_set_font_options (pangoCtx, _fontOpts);
    pango_cairo_context_set_resolution (pangoCtx, _dpiX);
    pango_layout_context_changed (layout);
    pango_layout_get_extents (layout, NULL, &logRect);

    *width  = logRect.width / PANGO_SCALE;
    *height = logRect.height / PANGO_SCALE;

    // clean up
    pango_font_description_free (desc);
    g_object_unref (layout);
    cairo_destroy (cr);
    cairo_surface_destroy (surface);
  }

  void
  Tooltip::DrawLabel (cairo_t* cr,
                      gint     width,
                      gint     height,
                      gfloat*  rgba)
  {
    int                   textWidth  = 0;
    int                   textHeight = 0;
    PangoLayout*          layout     = NULL;
    PangoFontDescription* desc       = NULL;
    PangoContext*         pangoCtx   = NULL;

    GetTextExtents ((char*) "Ubuntu 20",
                    &textWidth,
                    &textHeight);

    cairo_set_font_options (cr, _fontOpts);
    layout = pango_cairo_create_layout (cr);
    desc = pango_font_description_from_string ((char*) "Ubuntu 20");
    pango_font_description_set_weight (desc, PANGO_WEIGHT_NORMAL);
    pango_layout_set_font_description (layout, desc);
    pango_layout_set_wrap (layout, PANGO_WRAP_WORD_CHAR);
    pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_END);
    pango_layout_set_text (layout, _labelText.GetTCharPtr(), -1);
    pangoCtx = pango_layout_get_context (layout); // is not ref'ed
    pango_cairo_context_set_font_options (pangoCtx, _fontOpts);
    pango_cairo_context_set_resolution (pangoCtx, (double) _dpiX);
    pango_layout_context_changed (layout);

    cairo_move_to (cr,
                   ANCHOR_WIDTH + (float) ((width - ANCHOR_WIDTH) - textWidth) / 2.0f,
                   (float) (height - textHeight) / 2.0f);
    pango_cairo_show_layout (cr, layout);
    cairo_set_source_rgba (cr, rgba[0], rgba[1], rgba[2], rgba[3]);
    cairo_fill (cr);

    // clean up
    pango_font_description_free (desc);
    g_object_unref (layout);
  }

  void
  Tooltip::DrawOutlineShadow (cairo_t* cr,
                              gint     width,
                              gint     height,
                              gfloat   anchor_width,
                              gfloat   anchor_height,
                              gint     upper_size,
                              gfloat   corner_radius,
                              guint    blur_coeff,
                              gfloat*  rgba_shadow,
                              gfloat   line_width,
                              gint     padding_size,
                              gfloat*  rgba_line)
  {
    ComputeFullMaskPath (cr,
                         anchor_width,
                         anchor_height,
                         width,
                         height,
                         upper_size,
                         corner_radius,
                         padding_size);
    //DrawCairo (cr, TRUE, line_width, rgba_shadow, FALSE, FALSE);
    //ctk_surface_blur (surf, blur_coeff);
    //ComputeMask (cr);
    ComputeOutline (cr, line_width, rgba_line);
  }

  Tooltip::Tooltip (int     x,
                    int     y,
                    NString text)
  {
    _anchorX   = x;
    _anchorY   = y;
    _labelText = text;
    _fontOpts  = cairo_font_options_create ();

    // FIXME: hard-coding these for the moment, as we don't have
    // gsettings-support in place right now
    cairo_font_options_set_antialias (_fontOpts, CAIRO_ANTIALIAS_SUBPIXEL);
    cairo_font_options_set_hint_metrics (_fontOpts, CAIRO_HINT_METRICS_ON);
    cairo_font_options_set_hint_style (_fontOpts, CAIRO_HINT_STYLE_SLIGHT);
    cairo_font_options_set_subpixel_order (_fontOpts, CAIRO_SUBPIXEL_ORDER_RGB);

    // make sure _dpiX and _dpiY are initialized correctly
    GetDPI ();
  }

  Tooltip::~Tooltip ()
  {
    cairo_font_options_destroy (_fontOpts);
    delete(_cairo_graphics);
    delete(_texture2D);
  }

  long
  Tooltip::ProcessEvent (IEvent& ievent,
                         long    TraverseInfo,
                         long    ProcessEventInfo)
  {
    return 0;
  }

  void Tooltip::Draw (GraphicsContext& gfxContext,
                      bool             forceDraw)
  {
    Geometry base = GetGeometry();

    // the elements position inside the window are referenced to top-left window
    // corner. So bring base to (0, 0).
    base.SetX (0);
    base.SetY (0);
    gfxContext.PushClippingRectangle (base);

    gPainter.PushDrawShapeLayer (gfxContext,
                                 base,
                                 eSHAPE_CORNER_ROUND10,
                                 m_background_color, eAllCorners, true);

    TexCoordXForm texxform;
    texxform.SetWrap(TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    texxform.SetTexCoordType (TexCoordXForm::OFFSET_COORD);

    gfxContext.QRP_GLSL_1Tex (base.x,
                              base.y,
                              base.width,
                              base.height,
                              _texture2D->GetDeviceTexture(),
                              texxform,
                              Color(1.0f, 1.0f, 1.0f, 1.0f));

    gPainter.PopBackground ();
    gfxContext.PopClippingRectangle ();
  }

  void Tooltip::DrawContent (GraphicsContext &GfxContext, bool force_draw)
  {
    /*Geometry base = GetGeometry();
    int x = base.x;
    int y = base.y;
    // The elements position inside the window are referenced to top-left window corner. So bring base to (0, 0).
    base.SetX (0);
    base.SetY (0);

    if (UseBlurredBackground() )
    {
      TexCoordXForm texxform;
      texxform.uoffset = (float) x / (float) GetThreadWindowCompositor().GetScreenBlurTexture()->GetWidth();
      texxform.voffset = (float) y / (float) GetThreadWindowCompositor().GetScreenBlurTexture()->GetHeight();
      texxform.SetTexCoordType (TexCoordXForm::OFFSET_COORD);

      gPainter.PushTextureLayer (GfxContext, base, GetThreadWindowCompositor().GetScreenBlurTexture(), texxform, Color::White, true);
    }
    else
    {
      //nuxDebugMsg(TEXT("[BaseWindow::DrawContent]"));
      gPainter.PushShapeLayer (GfxContext, base, eSHAPE_CORNER_ROUND10, m_background_color, eAllCorners, true);
      //GfxContext.QRP_GLSL_Color(base.x, base.y, base.width, base.height, Color(1.0f, 0.0f, 0.0f, 1.0f));
      //GfxContext.QRP_GLSL_Color(base.x, base.y, base.width, base.height, Color(1.0f / (float) (std::rand () % 100), 1.0f / (float) (std::rand () % 100), 1.0f / (float) (std::rand () % 100), 0.5f));
    }

    if (m_layout)
    {
      GfxContext.PushClippingRectangle (base);
      m_layout->ProcessDraw (GfxContext, force_draw);
      GfxContext.PopClippingRectangle();
    }

    gPainter.PopBackground();*/
  }

  void Tooltip::PreLayoutManagement ()
  {
  }

  long
  Tooltip::PostLayoutManagement (long LayoutResult)
  {
    long     result           = BaseWindow::PostLayoutManagement (LayoutResult);
    int      textWidth        = 0;
    int      textHeight       = 0;
    Geometry base;
    float    rgbaTint[4]      = {0.0f, 0.0f, 0.0f, 0.7f};
    float    rgbaHighlight[4] = {1.0f, 1.0f, 1.0f, 0.15f};
    float    rgbaLine[4]      = {1.0f, 1.0f, 1.0f, 0.75f};
    float    rgbaShadow[4]    = {0.0f, 0.0f, 0.0f, 0.48f};
    float    rgbaText[4]      = {1.0f, 1.0f, 1.0f, 1.0f};

    GetTextExtents ((char*) "Ubuntu 20",
                    &textWidth,
                    &textHeight);

    base.x = _anchorX;
    base.y = _anchorY;
    base.width = textWidth + 2 * MARGIN + ANCHOR_WIDTH;
    base.height = textHeight + 2 * MARGIN;
    SetGeometry (base);

    _cairo_graphics = new CairoGraphics (CAIRO_FORMAT_ARGB32,
                                         base.GetWidth (),
                                         base.GetHeight ());
    cairo_t *cr = _cairo_graphics->GetContext ();

    DrawTintDotHighlight (cr,
                          base.GetWidth (),
                          base.GetHeight (),
                          base.GetWidth () / 2.0f,
                          15.0f,
                          base.GetWidth () / 2.0f,
                          rgbaTint,
                          rgbaHighlight);

    DrawOutlineShadow (cr,
                       base.GetWidth (),
                       base.GetHeight (),
                       ANCHOR_WIDTH,
                       ANCHOR_HEIGHT,
                       -1,
                       RADIUS,
                       BLUR_INTENSITY,
                       rgbaShadow,
                       LINE_WIDTH,
                       PADDING_SIZE,
                       rgbaLine);

    DrawLabel (cr,
               base.GetWidth (),
               base.GetHeight (),
               rgbaText);

    nux::NBitmapData* bitmap =  _cairo_graphics->GetBitmap();

    // NTexture2D is the high level representation of an image that is backed by
    // an actual opengl texture.
    _texture2D = GetThreadGLDeviceFactory()->CreateSystemCapableTexture ();
    _texture2D->Update(bitmap);
    
    delete _cairo_graphics;

    return result;
  }

  void
  Tooltip::PositionChildLayout (float offsetX,
                                float offsetY)
  {
  }

  void
  Tooltip::LayoutWindowElements ()
  {
  }

  void
  Tooltip::NotifyConfigurationChange (int width,
                                      int height)
  {
  }
}

void
initGUIThread (nux::NThread* thread,
               void*         data)
{
  nux::VLayout* layout  = new nux::VLayout (TEXT(""), NUX_TRACKER_LOCATION);
  nux::Tooltip* tooltip1 = new nux::Tooltip (5, 40, TEXT("Network"));
  nux::Tooltip* tooltip2 = new nux::Tooltip (5, 100, TEXT("Tools"));
  nux::Tooltip* tooltip3 = new nux::Tooltip (5, 160, TEXT("Games"));

  tooltip1->SetBackgroundColor(nux::Color(0xFF8914f7));
  tooltip2->SetBackgroundColor(nux::Color(0xFF8914f7));
  tooltip3->SetBackgroundColor(nux::Color(0xFF8914f7));
  tooltip1->ShowWindow(true);
  tooltip2->ShowWindow(true);
  tooltip3->ShowWindow(true);

  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(thread)->SetWindowBackgroundPaintLayer(&background);
  
  nux::GetGraphicsThread()->SetLayout (layout);
}

int main (int          argc,
          char const** argv)
{
  nux::WindowThread* thread = NULL;

  nux::NuxInitialize (0);

  thread = nux::CreateGUIThread (TEXT ("NUX/Unity Tooltips"),
                                 600,
                                 400,
                                 0,
                                 &initGUIThread,
                                 0);
  thread->Run (NULL);

  delete thread;

  return 0;
}

