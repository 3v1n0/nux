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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "Nux.h"
#include "NuxGraphics/FontRenderer.h"
#include "TextView.h"
#include "TextViewInternal.h"
#include "racursor.h"

#include "HScrollBar.h"
#include "VScrollBar.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "Button.h"
#include "TimerProc.h"

namespace nux
{

//
//	Constructor for TextView class
//
  TextView::TextView (HWND hwnd)
    :   m_BlinkTimerFunctor (0)
    ,   m_BlinkTimerHandler (0)
    ,   m_HasFocus (true)
  {
    m_TextFontRenderer = new FontRenderer (*GetThreadGraphicsContext() );
    m_VBSize0 = 256 * 1024;
    m_VBSize1 = 256 * 1024;
    m_TextVertexBuffer0.resize (m_VBSize0);
    m_TextVertexBuffer1.resize (m_VBSize1);
    m_ColorQuadBuffer0.resize (m_VBSize0);
    m_ColorQuadBuffer1.resize (m_VBSize1);

    m_NumVBQuad0 = 0;
    m_NumVBQuad1 = 0;
    m_NumVBColorQuad0 = 0;
    m_NumVBColorQuad1 = 0;

    b_RenderToVertexBuffer = false;

    m_ShaderProg = GetGpuDevice()->CreateShaderProgram();
    m_ShaderProg->LoadIShaderFile (NUX_FINDRESOURCELOCATION (TEXT ("Shaders/TextViewShader.glsl") ) );
    m_ShaderProg->Link();

    m_CgColor               = m_ShaderProg->GetAttributeLocation (TEXT ("iColor") );
    m_CgPosition            = m_ShaderProg->GetAttributeLocation (TEXT ("iPosition") );
    m_CgTexUV               = m_ShaderProg->GetAttributeLocation (TEXT ("iTexUV") );

    m_CgFontTexture         = m_ShaderProg->GetUniformLocationARB (TEXT ("FontTexture") );
    m_ViewProjectionMatrix0   = m_ShaderProg->GetUniformLocationARB (TEXT ("ViewProjectionMatrix") );

    m_ColorQuadShaderProg = GetGpuDevice()->CreateShaderProgram();
    m_ColorQuadShaderProg->LoadIShaderFile (NUX_FINDRESOURCELOCATION (TEXT ("Shaders//TextViewColorQuadShader.glsl") ) );
    m_ColorQuadShaderProg->Link();

    m_CgQuadPosition        = m_ColorQuadShaderProg->GetAttributeLocation (TEXT ("iPosition") );
    m_CgQuadColor           = m_ColorQuadShaderProg->GetAttributeLocation (TEXT ("iColor") );
    m_ViewProjectionMatrix1   = m_ColorQuadShaderProg->GetUniformLocationARB (TEXT ("ViewProjectionMatrix") );

    m_hWnd = hwnd;
    m_MouseWheelAcc = 0;

    // Font-related data
    m_nNumFonts		= 1;
    m_nHeightAbove	= 0;
    m_nHeightBelow	= 0;

    // File-related data
    m_nLineCount   = 0;
    m_nLongestLine = 0;


    // Scrollbar related data
    m_nVScrollPos = 0;
    m_nHScrollPos = 0;
    m_nVScrollMax = 0;
    m_nHScrollMax = 0;

    // Display-related data
    m_nTabWidthChars = 4;
    m_uStyleFlags	 = 0;
    m_nCaretWidth	 = 0;
    m_nLongLineLimit = 80;
    m_nLineInfoCount = 0;
    m_nCRLFMode		 = TXL_ALL;

    m_nCaretWidth = 1; // texttodo: SystemParametersInfo(SPI_GETCARETWIDTH, 0, &m_nCaretWidth, 0);

    if (m_nCaretWidth == 0)
      m_nCaretWidth = 2;

    // Default display colours
    m_rgbColourList[TXC_FOREGROUND]		    = 0xFFFFFFFF; //    SYSCOL(COLOR_WINDOWTEXT);
    m_rgbColourList[TXC_BACKGROUND]		    = 0xFF4D4D4D; //    SYSCOL(COLOR_WINDOW);
    m_rgbColourList[TXC_HIGHLIGHTTEXT]	    = 0xFF000000; //    SYSCOL(COLOR_HIGHLIGHTTEXT);
    m_rgbColourList[TXC_HIGHLIGHT]		    = 0xFFbebebe; //    SYSCOL(COLOR_HIGHLIGHT);
    m_rgbColourList[TXC_HIGHLIGHTTEXT2]	    = 0xFFf4f4f4; //    SYSCOL(COLOR_INACTIVECAPTIONTEXT);
    m_rgbColourList[TXC_HIGHLIGHT2]		    = 0xFF747474; //    SYSCOL(COLOR_INACTIVECAPTION);
    m_rgbColourList[TXC_SELMARGIN1]		    = 0xFFe2e2e2; //    SYSCOL(COLOR_3DFACE);
    m_rgbColourList[TXC_SELMARGIN2]		    = 0xFFffffff; //    SYSCOL(COLOR_3DHIGHLIGHT);
    m_rgbColourList[TXC_LINENUMBERTEXT]	    = 0xFFb4b4b4; //    SYSCOL(COLOR_3DSHADOW);
    m_rgbColourList[TXC_LINENUMBER]		    = 0xFFe2e2e2; //    SYSCOL(COLOR_3DFACE);
    m_rgbColourList[TXC_LONGLINETEXT]	    = 0xFFb4b4b4; //    SYSCOL(COLOR_3DSHADOW);
    m_rgbColourList[TXC_LONGLINE]		    = 0xFFe2e2e2; //    SYSCOL(COLOR_3DFACE);
    m_rgbColourList[TXC_CURRENTLINETEXT]    = 0xFF000000; //    SYSCOL(COLOR_WINDOWTEXT);
    m_rgbColourList[TXC_CURRENTLINE]	    = 0xFFfff0e6; //    RGB(230,240,255);

    // Runtime data
    m_nSelectionMode	= SEL_NONE;
    m_nScrollTimer		= 0;
    m_fHideCaret		= false;
    m_fTransparent		= true;
    m_hImageList		= 0;

    m_nSelectionStart	= 0;
    m_nSelectionEnd		= 0;
    m_nCursorOffset		= 0;
    m_nCurrentLine		= 0;
    m_nPreviousLine     = 0;

    m_CharacterAtCursor = 0;
    m_CursorCharacterPosition = 0;

    m_nLinenoWidth		= 0;
    m_nCaretPosX		= 0;
    m_nAnchorPosX		= 0;

    m_RedrawCaret = true;

    m_pTextDoc = new TextDocument();

    m_hMarginCursor = CreateCursor (GetModuleHandle (0), 21, 5, 32, 32, XORMask, ANDMask);

    //
    //	The TextView state must be fully initialized before we
    //	start calling member-functions
    //

    hscrollbar->OnScrollLeft.connect ( sigc::mem_fun (this, &TextView::ScrollLeft) );
    hscrollbar->OnScrollRight.connect ( sigc::mem_fun (this, &TextView::ScrollRight) );
    vscrollbar->OnScrollUp.connect ( sigc::mem_fun (this, &TextView::ScrollUp) );
    vscrollbar->OnScrollDown.connect ( sigc::mem_fun (this, &TextView::ScrollDown) );

    OnMouseDown.connect (sigc::mem_fun (this, &TextView::OnLButtonDown) );
    OnMouseUp.connect (sigc::mem_fun (this, &TextView::OnLButtonUp) );
    OnMouseMove.connect (sigc::mem_fun (this, &TextView::RecvMouseMove) );
    OnMouseDrag.connect (sigc::mem_fun (this, &TextView::RecvMouseMove) );
    OnMouseEnter.connect (sigc::mem_fun (this, &TextView::RecvMouseEnter) );
    OnMouseLeave.connect (sigc::mem_fun (this, &TextView::RecvMouseLeave) );

    OnMouseWheel.connect (sigc::mem_fun (this, &TextView::RecvMouseWheel) );
    OnKeyEvent.connect (sigc::mem_fun (this, &TextView::RecvKeyEvent) );

    OnStartFocus.connect (sigc::mem_fun (this, &TextView::RecvStartFocus) );
    OnEndFocus.connect (sigc::mem_fun (this, &TextView::RecvEndFocus) );

    MouseAutoScrollTimer = new TimerFunctor;
    MouseAutoScrollTimer->OnTimerExpired.connect (sigc::mem_fun (this, &TextView::RecvTimer) );
    MouseAutoScrollHandle = 0;

    SetTextColor (Color (m_rgbColourList[TXC_FOREGROUND]) );
    SetTextBkColor (Color (m_rgbColourList[TXC_BACKGROUND]) );

    SetFont();
    RecalcLineHeight();
    OpenFile (TEXT ("C:\\Development\\Inalogic\\DIGITAL.txt") );
    SetStyle (0, TXS_SELMARGIN);
    SetStyle (0, TXS_LINENUMBERS);

    m_BlinkTimerFunctor = new TimerFunctor();
    m_BlinkTimerFunctor->OnTimerExpired.connect (sigc::mem_fun (this, &TextView::BlinkCursorTimerInterrupt) );
  }

//
//	Destructor for TextView class
//
  TextView::~TextView()
  {
    if (m_pTextDoc)
      delete m_pTextDoc;

    DestroyCursor (m_hMarginCursor);
  }


  void TextView::SetTextBkColor (const Color &color)
  {
    m_TextBkColor = color;
  }

  const Color &TextView::GetTextBkColor()
  {
    return m_TextBkColor;
  }


  VOID TextView::UpdateMetrics()
  {
    OnSize (0, m_ViewWidth, m_ViewHeight);
    RefreshWindow();

    RepositionCaret();
  }

  LONG TextView::OnSetFocus (HWND hwndOld)
  {
    return 0;
  }

  LONG TextView::OnKillFocus (HWND hwndNew)
  {
    return 0;
  }

  t_u32 TextView::SetStyle (t_u32 uMask, t_u32 uStyles)
  {
    t_u32 oldstyle = m_uStyleFlags;

    m_uStyleFlags  = (m_uStyleFlags & ~uMask) | uStyles;

    // update display here
    UpdateMetrics();
    RefreshWindow();

    return oldstyle;
  }

  t_u32 TextView::SetVar (t_u32 nVar, t_u32 nValue)
  {
    return 0;//oldval;
  }

  t_u32 TextView::GetVar (t_u32 nVar)
  {
    return 0;
  }

  t_u32 TextView::GetStyleMask (t_u32 uMask)
  {
    return m_uStyleFlags & uMask;
  }

  bool TextView::CheckStyle (t_u32 uMask)
  {
    return (m_uStyleFlags & uMask) ? true : false;
  }

  int TextView::SetCaretWidth (int nWidth)
  {
    int oldwidth = m_nCaretWidth;
    m_nCaretWidth  = nWidth;

    return oldwidth;
  }

  BOOL TextView::SetImageList (HIMAGELIST hImgList)
  {
    m_hImageList = hImgList;
    return TRUE;
  }

  LONG TextView::SetLongLine (int nLength)
  {
    int oldlen = m_nLongLineLimit;
    m_nLongLineLimit = nLength;
    return oldlen;
  }

  int CompareLineInfo (LINEINFO *elem1, LINEINFO *elem2)
  {
    if (elem1->nLineNo < elem2->nLineNo)
      return -1;

    if (elem1->nLineNo > elem2->nLineNo)
      return 1;
    else
      return 0;
  }

  int TextView::SetLineImage (t_u32 nLineNo, t_u32 nImageIdx)
  {
    LINEINFO *linfo = GetLineInfo (nLineNo);

    // if already a line with an image
    if (linfo)
    {
      linfo->nImageIdx = nImageIdx;
    }
    else
    {
      linfo = &m_LineInfo[m_nLineInfoCount++];
      linfo->nLineNo = nLineNo;
      linfo->nImageIdx = nImageIdx;

      // sort the array
      qsort (
        m_LineInfo,
        m_nLineInfoCount,
        sizeof (LINEINFO),
        (COMPAREPROC) CompareLineInfo
      );

    }

    return 0;
  }

  LINEINFO *TextView::GetLineInfo (t_u32 nLineNo)
  {
    LINEINFO key = { nLineNo, 0 };

    // perform the binary search
    return (LINEINFO *)	bsearch (
             &key,
             m_LineInfo,
             m_nLineInfoCount,
             sizeof (LINEINFO),
             (COMPAREPROC) CompareLineInfo
           );
  }


//
//	Public memberfunction
//
  LONG WINAPI TextView::WndProc (t_u32 msg, WPARAM wParam, LPARAM lParam)
  {
    switch (msg)
    {
      case WM_SIZE:
        return OnSize (wParam, LOWORD (lParam), HIWORD (lParam) );

      case WM_VSCROLL:
        return OnVScroll (LOWORD (wParam), HIWORD (wParam) );

      case WM_HSCROLL:
        return OnHScroll (LOWORD (wParam), HIWORD (wParam) );

      case WM_MOUSEACTIVATE:
        return OnMouseActivate ( (HWND) wParam, LOWORD (lParam), HIWORD (lParam) );

      case WM_MOUSEWHEEL:
        return OnMouseWheelFunc ( (short) HIWORD (wParam) );

      case WM_SETFOCUS:
        return OnSetFocus ( (HWND) wParam);

      case WM_KILLFOCUS:
        return OnKillFocus ( (HWND) wParam);

      case WM_SETCURSOR:

        if (LOWORD (lParam) == HTCLIENT)
          return TRUE;
        else
          break;

      case TXM_OPENFILE:
        return OpenFile ( (TCHAR *) lParam);

      case TXM_CLEAR:
        return ClearFile();
      default:
        break;
    }

    return DefWindowProc (m_hWnd, msg, wParam, lParam);
  }

  long TextView::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;

    ret = vscrollbar->ProcessEvent (ievent, ret, ProcessEventInfo);
    ret = hscrollbar->ProcessEvent (ievent, ret, ProcessEventInfo);

    ret = GetCompositionLayout()->ProcessEvent (ievent, ret, ProcessEventInfo);

    // PostProcessEvent2 must always have its last parameter set to 0
    // because the m_BackgroundArea is the real physical limit of the window.
    // So the previous test about IsPointInside do not prevail over m_BackgroundArea
    // testing the event by itself.
    ret = PostProcessEvent2 (ievent, ret, 0);

    if (ievent.e_event == NUX_WINDOW_ENTER_FOCUS)
    {
      m_HasFocus = true;
      StopBlinkCursor (false);
      StartBlinkCursor (false);
    }

    if (ievent.e_event == NUX_WINDOW_EXIT_FOCUS)
    {
      m_HasFocus = false;
      StopBlinkCursor (false);
    }

    return ret;
  }

  void TextView::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.PushClippingRectangle (GetGeometry() );

    b_RenderToVertexBuffer = true;
    m_NumVBQuad0 = 0;
    m_NumVBQuad1 = 0;
    m_NumVBColorQuad0 = 0;
    m_NumVBColorQuad1 = 0;

    OnPaint (GfxContext);

    if (b_RenderToVertexBuffer)
    {
      GetThreadGraphicsContext()->GetRenderStates().SetBlend (TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      GetThreadGraphicsContext()->GetRenderStates().SetColorMask (TRUE, TRUE, TRUE, FALSE); // Do not write the alpha of characters

      GfxContext.EnableTextureMode (GL_TEXTURE0, GL_TEXTURE_RECTANGLE_ARB);

      ObjectPtr< CachedTextureRectangle > glTexture = GfxContext.ResourceCache.GetCachedResource (m_TextFont->TextureArray[0]);

      GetThreadGraphicsContext()->SetTexture (GL_TEXTURE0, glTexture->m_Texture);

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      m_ColorQuadShaderProg->Begin();

      Matrix4 Quadmat = GetThreadGraphicsContext()->GetModelViewProjectionMatrix();

      if (m_ViewProjectionMatrix1 != -1)
        m_ColorQuadShaderProg->SetUniformLocMatrix4fv (m_ViewProjectionMatrix1, 1, false, (float *) &Quadmat);

      // Draw Line Number quad background
      {
        Geometry rect = GetTextAreaGeometry();
        rect.SetWidth (LeftMarginWidth() );
        GfxContext.PushClippingRectangle (rect);

        if (m_CgQuadPosition != -1)
        {
          CHECKGL ( glEnableVertexAttribArrayARB (m_CgQuadPosition) );
          CHECKGL ( glVertexAttribPointerARB (m_CgQuadPosition, 4, GL_FLOAT, GL_FALSE, 32, &m_ColorQuadBuffer1[0]) );
        }

        if (m_CgQuadColor != -1)
        {
          CHECKGL ( glEnableVertexAttribArrayARB (m_CgQuadColor) );
          CHECKGL ( glVertexAttribPointerARB (m_CgQuadColor, 4, GL_FLOAT, GL_FALSE, 32, &m_ColorQuadBuffer1[0] + 4) );
        }

        CHECKGL ( glDrawArrays ( GL_QUADS, 0, m_NumVBColorQuad1 * 4 ) );

        if (m_CgQuadPosition != -1)
          CHECKGL ( glDisableVertexAttribArrayARB (m_CgQuadPosition) );

        if (m_CgQuadColor != -1)
          CHECKGL ( glDisableVertexAttribArrayARB (m_CgQuadColor) );

        GfxContext.PopClippingRectangle();
      }

      // Draw Text quad background
      {
        Geometry rect = GetTextAreaGeometry();
        rect.OffsetPosition (LeftMarginWidth(), 0);
        rect.OffsetSize (-LeftMarginWidth(), 0);
        GfxContext.PushClippingRectangle (rect);

        if (m_CgQuadPosition != -1)
        {
          CHECKGL ( glEnableVertexAttribArrayARB (m_CgQuadPosition) );
          CHECKGL ( glVertexAttribPointerARB (m_CgQuadPosition, 4, GL_FLOAT, GL_FALSE, 32, &m_ColorQuadBuffer0[0]) );
        }

        if (m_CgQuadColor != -1)
        {
          CHECKGL ( glEnableVertexAttribArrayARB (m_CgQuadColor) );
          CHECKGL ( glVertexAttribPointerARB (m_CgQuadColor, 4, GL_FLOAT, GL_FALSE, 32, &m_ColorQuadBuffer0[0] + 4) );
        }

        CHECKGL ( glDrawArrays ( GL_QUADS, 0, m_NumVBColorQuad0 * 4 ) );

        if (m_CgQuadPosition != -1)
          CHECKGL ( glDisableVertexAttribArrayARB (m_CgQuadPosition) );

        if (m_CgQuadColor != -1)
          CHECKGL ( glDisableVertexAttribArrayARB (m_CgQuadColor) );

        GfxContext.PopClippingRectangle();
      }
      m_ColorQuadShaderProg->End();


      m_ShaderProg->Begin();

      Quadmat = GetThreadGraphicsContext()->GetModelViewProjectionMatrix();

      if (m_ViewProjectionMatrix0 != -1)
        m_ShaderProg->SetUniformLocMatrix4fv (m_ViewProjectionMatrix0, 1, false, (float *) &Quadmat);

      if (m_CgFontTexture != -1)
      {
        CHECKGL ( glUniform1iARB (m_CgFontTexture, 0) );
      }

      // Draw Line Number
      {
        Geometry rect = GetTextAreaGeometry();
        rect.SetWidth (LeftMarginWidth() );
        GfxContext.PushClippingRectangle (rect);

        if (m_CgPosition != -1)
        {
          CHECKGL ( glEnableVertexAttribArrayARB (m_CgPosition) );
          CHECKGL ( glVertexAttribPointerARB (m_CgPosition, 4, GL_FLOAT, GL_FALSE, 48, &m_TextVertexBuffer1[0]) );
        }

        if (m_CgTexUV != -1)
        {
          CHECKGL ( glEnableVertexAttribArrayARB (m_CgTexUV) );
          CHECKGL ( glVertexAttribPointerARB (m_CgTexUV, 4, GL_FLOAT, GL_FALSE, 48, &m_TextVertexBuffer1[0] + 4) );
        }

        if (m_CgColor != -1)
        {
          CHECKGL ( glEnableVertexAttribArrayARB (m_CgColor) );
          CHECKGL ( glVertexAttribPointerARB (m_CgColor, 4, GL_FLOAT, GL_FALSE, 48, &m_TextVertexBuffer1[0] + 8) );
        }

        CHECKGL ( glDrawArrays ( GL_QUADS, 0, m_NumVBQuad1 * 4 ) );

        if (m_CgPosition != -1)
          CHECKGL ( glDisableVertexAttribArrayARB (m_CgPosition) );

        if (m_CgTexUV != -1)
          CHECKGL ( glDisableVertexAttribArrayARB (m_CgTexUV) );

        if (m_CgColor != -1)
          CHECKGL ( glDisableVertexAttribArrayARB (m_CgColor) );

        GfxContext.PopClippingRectangle();
      }

      // Draw Text
      {
        Geometry rect = GetTextAreaGeometry();
        rect.OffsetPosition (LeftMarginWidth(), 0);
        rect.OffsetSize (-LeftMarginWidth(), 0);
        GfxContext.PushClippingRectangle (rect);

        if (m_CgPosition != -1)
        {
          CHECKGL ( glEnableVertexAttribArrayARB (m_CgPosition) );
          CHECKGL ( glVertexAttribPointerARB (m_CgPosition, 4, GL_FLOAT, GL_FALSE, 48, &m_TextVertexBuffer0[0]) );
        }

        if (m_CgTexUV != -1)
        {
          CHECKGL ( glEnableVertexAttribArrayARB (m_CgTexUV) );
          CHECKGL ( glVertexAttribPointerARB (m_CgTexUV, 4, GL_FLOAT, GL_FALSE, 48, &m_TextVertexBuffer0[0] + 4) );
        }

        if (m_CgColor != -1)
        {
          CHECKGL ( glEnableVertexAttribArrayARB (m_CgColor) );
          CHECKGL ( glVertexAttribPointerARB (m_CgColor, 4, GL_FLOAT, GL_FALSE, 48, &m_TextVertexBuffer0[0] + 8) );
        }

        CHECKGL ( glDrawArrays ( GL_QUADS, 0, m_NumVBQuad0 * 4 ) );

        if (m_CgPosition != -1)
          CHECKGL ( glDisableVertexAttribArrayARB (m_CgPosition) );

        if (m_CgTexUV != -1)
          CHECKGL ( glDisableVertexAttribArrayARB (m_CgTexUV) );

        if (m_CgColor != -1)
          CHECKGL ( glDisableVertexAttribArrayARB (m_CgColor) );

        GfxContext.PopClippingRectangle();
      }

      m_ShaderProg->End();

      GetThreadGraphicsContext()->GetRenderStates().SetColorMask (TRUE, TRUE, TRUE, TRUE);
      GetThreadGraphicsContext()->GetRenderStates().SetBlend (FALSE);

      CHECKGL ( glDisable (GL_TEXTURE_RECTANGLE_ARB) );
    }

    b_RenderToVertexBuffer = false;

    if (m_vertical_scrollbar_enable)
    {
      vscrollbar->ProcessDraw (GfxContext, force_draw);
    }

    if (m_horizontal_scrollbar_enable)
    {
      hscrollbar->ProcessDraw (GfxContext, force_draw);
    }

    GfxContext.PopClippingRectangle();
  }

  void TextView::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {
    static int n = 0;

    if (m_vertical_scrollbar_enable)
    {
      vscrollbar->ProcessDraw (GfxContext, force_draw);
    }

    if (m_horizontal_scrollbar_enable)
    {
      hscrollbar->ProcessDraw (GfxContext, force_draw);
    }

    std::vector<int>::iterator it;
    int NumDirtyLines = m_DirtyLines.size();

    for (it = m_DirtyLines.begin(); it != m_DirtyLines.end(); it++)
    {
      int LineNumber = * (it);
      OnPaintLine (GfxContext, LineNumber);
      //nuxDebugMsg(TEXT("Paint line (%d): %d"), n, LineNumber);
      ++n;
    }

    //if(m_RedrawCaret)
    {
      if ( (!m_fHideCaret) && m_HasFocus)
      {
        GfxContext.PushClippingRectangle (Geometry (m_ViewX /*+ LeftMarginWidth()*/, m_ViewY, m_ViewWidth /*- LeftMarginWidth()*/, m_ViewHeight) );

        //GetPainter().Draw2DLine(GfxContext, m_CaretPosition.x, m_CaretPosition.y, m_CaretPosition.x, m_CaretPosition.y + m_nLineHeight, Color(0xFFAA0000));
        if (!BlinkCursor)
          GetPainter().Paint2DQuadColor (GfxContext, m_CaretPosition.x, m_CaretPosition.y, 2, m_nLineHeight, Color (0xFFAA0000) );

        GfxContext.PopClippingRectangle();
        m_DirtyLines.clear();
      }

      m_RedrawCaret = false;
    }
  }

  void TextView::PreLayoutManagement()
  {
    TextViewWidget::PreLayoutManagement();
  }

  long TextView::PostLayoutManagement (long LayoutResult)
  {
    long ret = TextViewWidget::PostLayoutManagement (LayoutResult);

    UpdateMetrics();
    UpdateMarginWidth();
    return ret;
  }

  void TextView::PositionChildLayout (float offsetX, float offsetY)
  {
    TextViewWidget::PositionChildLayout (offsetX, offsetY);
  }

  Geometry TextView::GetTextAreaGeometry() const
  {
    Geometry	rect;
    int vscrollbarwidth = 0;
    int hscrollbarheight = 0;

    if (m_vertical_scrollbar_enable)
      vscrollbarwidth = vscrollbar->GetBaseWidth();

    if (m_horizontal_scrollbar_enable)
      hscrollbarheight = hscrollbar->GetBaseHeight();

    rect = GetGeometry();
    rect.OffsetPosition (getBorder() + GetViewContentLeftMargin(), getTopBorder() + GetViewContentTopMargin() );
    rect.OffsetSize (- (2 * getBorder() + GetViewContentLeftMargin() + GetViewContentRightMargin() + vscrollbarwidth),
                     - (getTopBorder() + getBorder() + GetViewContentTopMargin() + GetViewContentBottomMargin() + hscrollbarheight) );

    return rect;
  }

  void TextView::ScrollLeft (float stepx, int mousedx)
  {
    {
      m_ContentOffsetX += (float) stepx * (float) mousedx;;

      if (m_ContentOffsetX > 0)
      {
        m_nHScrollPos = 0;
        m_ContentOffsetX = 0;
      }
      else
      {
        m_nHScrollPos = Abs (m_ContentOffsetX / m_nFontWidth);
      }

      //nuxDebugMsg(TEXT("HScrollPos = %d"), m_nHScrollPos);
      hscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
    }
    hscrollbar->NeedRedraw();
    RepositionCaret();
    NeedRedraw();
  }

  void TextView::ScrollRight (float stepx, int mousedx)
  {
    {
      m_ContentOffsetX -= (float) stepx * (float) mousedx;

      if ( (m_ViewX + m_ContentOffsetX +  (m_nLongestLine * m_nFontWidth) < m_ViewX - LeftMarginWidth() + m_ViewWidth) && (m_ContentOffsetX < 0) )
      {
        m_ContentOffsetX = - ( (m_nLongestLine * m_nFontWidth) > m_ViewWidth - LeftMarginWidth() ? (m_nLongestLine * m_nFontWidth) - m_ViewWidth + LeftMarginWidth() : 0);
        m_nHScrollPos = Abs (m_ContentOffsetX / m_nFontWidth) + 1;
      }
      else
      {
        m_nHScrollPos = Abs ( (m_ContentOffsetX) / m_nFontWidth);
      }

      //nuxDebugMsg(TEXT("HScrollPos = %d"), m_nHScrollPos);
      hscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
    }
    hscrollbar->NeedRedraw();
    RepositionCaret();
    NeedRedraw();
  }

  void TextView::ScrollUp (float stepy, int mousedy)
  {
    {
      m_ContentOffsetY += stepy * (float) mousedy;

      if (m_ContentOffsetY > 0)
      {
        m_nVScrollPos = 0;
        m_ContentOffsetY = 0;
      }
      else
      {
        m_nVScrollPos = Abs (m_ContentOffsetY / m_nLineHeight);
      }

      //nuxDebugMsg(TEXT("VScrollPos = %d"), m_nVScrollPos);
      vscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
    }
    vscrollbar->NeedRedraw();
    RepositionCaret();
    NeedRedraw();
  }

  void TextView::ScrollDown (float stepy, int mousedy)
  {
    {
      m_ContentOffsetY -= (float) stepy * (float) mousedy;

      if ( (m_ViewY + m_ContentOffsetY + (m_nLineHeight * m_nLineCount) < m_ViewY + m_ViewHeight) && (m_ContentOffsetY < 0) )
      {
        m_ContentOffsetY = - ( (m_nLineHeight * m_nLineCount) > m_ViewHeight ? (m_nLineHeight * m_nLineCount) - m_ViewHeight : 0);
        m_nVScrollPos = Abs (m_ContentOffsetY / m_nLineHeight);
      }
      else
      {
        m_nVScrollPos = Abs (m_ContentOffsetY / m_nLineHeight);
      }

      //nuxDebugMsg(TEXT("VScrollPos = %d"), m_nVScrollPos);
      vscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
    }
    vscrollbar->NeedRedraw();
    RepositionCaret();
    NeedRedraw();
  }

  void TextView::RecvMouseWheel (int x, int y, int delta, t_u32 button_flags, t_u32 key_flags)
  {
    m_MouseWheelAcc += delta;
    int scrollunit = m_MouseWheelAcc / NUX_MOUSEWHEEL_DELTA;
    m_MouseWheelAcc = m_MouseWheelAcc % NUX_MOUSEWHEEL_DELTA;

    if (scrollunit > 0)
    {
//         m_nVScrollPos--;
      if (m_nVScrollPos > 0)
      {
        m_ContentOffsetY += scrollunit * m_nLineHeight;

        if (m_ContentOffsetY > 0)
          m_ContentOffsetY = 0;

        SetupScrollbars();
        vscrollbar->NeedRedraw();
        RepositionCaret();
        NeedRedraw();
      }
    }

    if (scrollunit < 0)
    {
//         m_nVScrollPos++;
      if (m_nVScrollPos < m_nLineCount)
      {
        m_ContentOffsetY += scrollunit * m_nLineHeight;

        if (m_ContentOffsetY < -m_nLineCount * m_nLineHeight)
          m_ContentOffsetY = -m_nLineCount * m_nLineHeight;

      }

      SetupScrollbars();
      vscrollbar->NeedRedraw();
      RepositionCaret();
      NeedRedraw();
    }
  }

  void TextView::RecvKeyEvent (
    GraphicsEngine &GfxContext , /*Graphics Context for text operation*/
    t_u32    eventType  , /*event type*/
    t_u32    keysym     , /*event keysym*/
    t_u32    state      , /*event state*/
    const TCHAR*  character  , /*character*/
    unsigned short   keyCount     /*key repeat count*/
  )
  {
    if (keyCount && ( (keysym == NUX_VK_PAGE_UP) || (keysym == NUX_KP_PAGE_UP) ) )
    {
      m_ContentOffsetY += m_nWindowLines * m_nLineHeight;

      if (m_ContentOffsetY > 0)
        m_ContentOffsetY = 0;

      SetupScrollbars();
      vscrollbar->NeedRedraw();
      RepositionCaret();
      NeedRedraw();
    }

    if (keyCount && ( (keysym == NUX_VK_PAGE_DOWN) || (keysym == NUX_KP_PAGE_DOWN) ) )
    {
      m_ContentOffsetY -= m_nWindowLines * m_nLineHeight;

      if (m_ContentOffsetY < -m_nLineCount * m_nLineHeight)
        m_ContentOffsetY = -m_nLineCount * m_nLineHeight;

      SetupScrollbars();
      vscrollbar->NeedRedraw();
      RepositionCaret();
      NeedRedraw();
    }
  }

  void TextView::RecvStartFocus()
  {
    RepositionCaret();
    m_HasFocus = true;
    StartBlinkCursor (true);
    NeedRedraw();
  }

  void TextView::RecvEndFocus()
  {
    // if we are making a selection when we lost focus then
    // stop the selection logic
    if (m_nSelectionMode != SEL_NONE)
    {
      OnLButtonUp (0, 0, 0, 0);
    }

    m_HasFocus = false;
    StopBlinkCursor (false);
    NeedRedraw();
  }

  bool TextView::IsKeyPressed (t_u32 virtualkey)
  {
    return GetThreadGLWindow()->GetCurrentEvent().GetVirtualKeyState (virtualkey);
  }

  void TextView::BlinkCursorTimerInterrupt (void *v)
  {
    GetTimer().RemoveTimerHandler (m_BlinkTimerHandler);
    m_BlinkTimerHandler = GetTimer().AddTimerHandler (500, m_BlinkTimerFunctor, this);
    BlinkCursor = !BlinkCursor;
    AddDirtyLine (m_nCurrentLine);
    NeedRedraw();
  }

  void TextView::StopBlinkCursor (bool BlinkState)
  {
    GetTimer().RemoveTimerHandler (m_BlinkTimerHandler);
    m_BlinkTimerHandler = 0;
    BlinkCursor = BlinkState;
    AddDirtyLine (m_nCurrentLine);
    NeedRedraw();
  }

  void TextView::StartBlinkCursor (bool BlinkState)
  {
    m_BlinkTimerHandler = GetTimer().AddTimerHandler (500, m_BlinkTimerFunctor, this);
    BlinkCursor = BlinkState;
    AddDirtyLine (m_nCurrentLine);
    NeedRedraw();
  }

}
