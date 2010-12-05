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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef TEXTVIEW_INTERNAL_INCLUDED
#define TEXTVIEW_INTERNAL_INCLUDED

#define TEXTBUFSIZE  128
#define LINENO_FMT  _T(" %d ")
#define LINENO_PAD	 8

#include <commctrl.h>
#include "../TimerProc.h"
#include "TextViewWidget.h"
#include "TextDocument.h"

namespace nux
{

//
//	ATTR - text character attribute
//
  typedef struct
  {
    COLORREF	fg;			// foreground colour
    COLORREF	bg;			// background colour
    t_u32		style;		// possible font-styling information

  } ATTR;

//
//	FONT - font attributes
//
  typedef struct
  {
    // Windows font information
    HFONT		hFont;
    TEXTMETRIC	tm;

    // dimensions needed for control-character 'bitmaps'
    int			nInternalLeading;
    int			nDescent;

  } FONT;

//
//	LINEINFO - information about a specific line
//
  typedef struct
  {
    t_u32	nLineNo;
    int		nImageIdx;

    // more here in the future?

  } LINEINFO;

  typedef int (__cdecl *COMPAREPROC) (const void *, const void *);

// maximum number of lines that we can hold info for at one time
#define MAX_LINE_INFO 128

// maximum fonts that a TextView can hold
#define MAX_FONTS 32

  enum SELMODE { SEL_NONE, SEL_NORMAL, SEL_MARGIN, SEL_BLOCK };

  typedef struct
  {
    t_u32	line;
    t_u32	xpos;

  } CURPOS;

//
//	TextView - internal window implementation
//
  class TextView: public TextViewWidget
  {
  public:

    TextView (HWND hwnd);
    ~TextView();

    long WINAPI WndProc (t_u32 msg, WPARAM wParam, LPARAM lParam);

    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw) {};

    virtual void PreLayoutManagement();
    virtual long PostLayoutManagement (long LayoutResult);
    virtual void PositionChildLayout (float offsetX, float offsetY);

    virtual void ScrollLeft (float stepx, int mousedx);
    virtual void ScrollRight (float stepx, int mousedx);
    virtual void ScrollUp (float stepy, int mousedy);
    virtual void ScrollDown (float stepy, int mousedy);

    void RecvMouseDown (int x, int y, t_u32 button_flags, t_u32 key_flags);
    void RecvMouseUp (int x, int y, t_u32 button_flags, t_u32 key_flags);
    void RecvMouseDrag (int x, int y, int dx, int dy, t_u32 button_flags, t_u32 key_flags);
    void RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseWheel (int x, int y, int delta, t_u32 button_flags, t_u32 key_flags);
    void RecvKeyEvent (
      GraphicsEngine &GfxContext , /*Graphics Context for text operation*/
      t_u32    eventType  , /*event type*/
      t_u32    keysym     , /*event keysym*/
      t_u32    state      , /*event state*/
      TCHAR    character  , /*character*/
      unsigned short   keyCount     /*key repeat count*/
    );

    void RecvStartFocus();
    void RecvEndFocus();

  private:

    Geometry GetTextAreaGeometry() const;
    //
    //	Message handlers
    //
    long OnPaint (GraphicsEngine &GfxContext);
    long TextView::OnPaintLine (GraphicsEngine &GfxContext, unsigned int LigneNumber);
    long OnSize (t_u32 nFlags, int width, int height);
    long OnVScroll (t_u32 nSBCode, t_u32 nPos);
    long OnHScroll (t_u32 nSBCode, t_u32 nPos);
    long OnMouseWheelFunc (int nDelta);
    void RecvTimer (void *);

    LONG OnMouseActivate (HWND hwndTop, t_u32 nHitTest, t_u32 nMessage);
    void OnLButtonDown (int x, int y, unsigned long button_flags, unsigned long key_flags /*t_u32 nFlags, int x, int y*/);
    void OnLButtonUp (int x, int y, unsigned long button_flags, unsigned long key_flags /*t_u32 nFlags, int x, int y*/);
    void RecvMouseMove (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags /*t_u32 nFlags, int x, int y*/);
    void OnLButtonDblClick (int x, int y, unsigned long button_flags, unsigned long key_flags /*t_u32 nFlags, int mx, int my*/);

    long OnSetFocus (HWND hwndOld);
    long OnKillFocus (HWND hwndNew);

    long OnKeyDown (t_u32 nVirtKey, t_u32 nFlags);

    //
    //
    //
    long OpenFile (TCHAR *szFileName);
    long ClearFile();

    void SetFont();
    long SetLineSpacing (int nAbove, int nBelow);
    long SetLongLine (int nLength);
    COLORREF SetColour (t_u32 idx, COLORREF rgbColour);

    //
    //	Private Helper functions
    //
    void PaintLine (GraphicsEngine &GfxContext, t_u32 line);
    void PaintText (GraphicsEngine &GfxContext, t_u32 nLineNo, Geometry &rect);
    void PaintMargin (GraphicsEngine &GfxContext, t_u32 line, Geometry &margin);
    void RenderColorQuadToBuffer (float *VertexBuffer, int VBSize, Rect geo, Color color);

    int   ApplyTextAttributes (t_u32 nLineNo, t_u32 offset, t_u32 &nColumn, TCHAR *szText, int nTextLen, ATTR *attr);
    int   NeatTextOut (GraphicsEngine &GfxContext, int xpos, int ypos, TCHAR *szText, int nLen, int nTabOrigin, ATTR *attr);

    int  PaintCtrlChar (GraphicsEngine &GfxContext, int xpos, int ypos, t_u32 chValue);
    void InitCtrlCharFontAttr();

    void RefreshWindow();
    long InvalidateRange (t_u32 nStart, t_u32 nFinish);
    long InvalidateLine (t_u32 nLineNo);
    void UpdateLine (t_u32 nLineNo);
    void AddDirtyLine (t_u32 nLineNo);

    void	PaintRect (GraphicsEngine &GfxContext, int x, int y, int width, int height, COLORREF fill);
    void	PaintRect (GraphicsEngine &GfxContext, RECT *rect, COLORREF fill);
    void	DrawCheckedRect (GraphicsEngine &GfxContext, RECT *rect, COLORREF fg, COLORREF bg);

    int  CtrlCharWidth (t_u32 chValue, FONT *fa);
    int  NeatTextYOffset (FONT *font);
    int  NeatTextWidth (const TCHAR *buf, int len, int nTabOrigin);
    int	 TabWidth();
    int  LeftMarginWidth();
    void UpdateMarginWidth();
    int	 SetCaretWidth (int nWidth);
    BOOL SetImageList (HIMAGELIST hImgList);

    BOOL  MouseCoordToFilePos (int x, int y, t_u32 *pnLineNo, t_u32 *pnFileOffset, int *psnappedX, TCHAR *Character, t_u32 *CharacterPosition); //, t_u32 *pnLineLen=0);
    void  RepositionCaret();
    void  UpdateCaretOffset (t_u32 offset, BOOL fTrailing, int *outx, t_u32 *outlineno);
    void  UpdateCaretXY (int xpos, t_u32 lineno);
    void  MoveCaret (int x, t_u32 lineno);

    t_u32	SetStyle (t_u32 uMask, t_u32 uStyles);

    t_u32	SetVar (t_u32 nVar, t_u32 nValue);
    t_u32	GetVar (t_u32 nVar);

    t_u32	GetStyleMask (t_u32 uMask);
    bool	CheckStyle (t_u32 uMask);

    COLORREF GetColour (t_u32 idx);
    COLORREF LineColour (t_u32 nLineNo);
    COLORREF LongColour (t_u32 nLineNo);

    int		SetLineImage (t_u32 nLineNo, t_u32 nImageIdx);
    LINEINFO *GetLineInfo (t_u32 nLineNo);
    int		StripCRLF (TCHAR *szText, int nLength, bool fAllow);

    VOID	SetupScrollbars();
    VOID	UpdateMetrics();
    VOID	RecalcLineHeight();
    bool    PinToBottomCorner();

    void	Scroll (int dx, int dy);
    HRGN	ScrollRgn (int dx, int dy, bool fReturnUpdateRgn);

    Color m_TextBkColor;
    virtual void SetTextBkColor (const Color &color);
    virtual const Color &GetTextBkColor();

    //! If true, then we can draw the blinking cursor.
    bool m_HasFocus;
    HWND	m_hWnd;
    t_u32	m_uStyleFlags;

    // Font-related data
    FONT	m_FontAttr[MAX_FONTS];
    int		m_nNumFonts;
    int		m_nFontWidth;
    int		m_nMaxAscent;
    int		m_nLineHeight;
    int		m_nHeightAbove;
    int		m_nHeightBelow;

    // Scrollbar-related data
    int	    m_nVScrollPos;
    int	    m_nVScrollMax;
    int		m_nHScrollPos;
    int		m_nHScrollMax;

    int		m_nLongestLine;
    int		m_nWindowLines;
    int		m_nWindowColumns;

    int m_MouseWheelAcc;
    // Display-related data
    int		m_nTabWidthChars;
    t_u32	m_nSelectionStart;
    t_u32	m_nSelectionEnd;
    t_u32	m_nCursorOffset;
    TCHAR   m_CharacterAtCursor;
    t_u32   m_CursorCharacterPosition;
    DWORD	m_nCaretWidth;
    t_u32	m_nCurrentLine;
    t_u32	m_nPreviousLine;
    std::vector<int> m_DirtyLines;
    int		m_nLongLineLimit;
    int		m_nCRLFMode;
    bool    m_RedrawCaret;

    LINEINFO	m_LineInfo[MAX_LINE_INFO];
    int			m_nLineInfoCount;

    int			m_nLinenoWidth;
    HCURSOR		m_hMarginCursor;
    RECT		m_rcBorder;

    COLORREF	m_rgbColourList[TXC_MAX_COLOURS];

    // Runtime data
    SELMODE		m_nSelectionMode;
    SELMODE		m_nSelectionType;
    CURPOS		m_cpBlockStart;
    CURPOS		m_cpBlockEnd;

    t_u32		m_nSelMarginOffset1;
    t_u32		m_nSelMarginOffset2;
    t_u32		m_nScrollTimer;
    int			m_nScrollCounter;
    bool		m_fHideCaret;
    Point  m_CaretPosition;
    bool		m_fTransparent;
    HIMAGELIST	m_hImageList;

    // File-related data
    t_u32		m_nLineCount;

    FontRenderer *m_TextFontRenderer;
    std::vector<float> m_TextVertexBuffer0;
    std::vector<float> m_TextVertexBuffer1;
    std::vector<float> m_ColorQuadBuffer0;
    std::vector<float> m_ColorQuadBuffer1;
    int m_NumVBQuad0;
    int m_NumVBQuad1;
    int m_NumVBColorQuad0;
    int m_NumVBColorQuad1;
    int m_VBSize0;
    int m_VBSize1;
    bool b_RenderToVertexBuffer;

    IntrusiveSP<IOpenGLPixelShader> m_PixelShaderProg;
    IntrusiveSP<IOpenGLVertexShader> m_VertexShaderProg;
    IntrusiveSP<IOpenGLShaderProgram> m_ShaderProg;
    IntrusiveSP<IOpenGLPixelShader> m_ColorQuadPixelShader;
    IntrusiveSP<IOpenGLVertexShader> m_ColorQuadVertexShader;
    IntrusiveSP<IOpenGLShaderProgram> m_ColorQuadShaderProg;
    int m_CgPosition;
    int m_CgTexUV;
    int m_CgColor;
    int m_CgFontTexture;
    int m_CgQuadPosition;
    int m_CgQuadColor;

    int m_ViewProjectionMatrix0;
    int m_ViewProjectionMatrix1;

    TimerFunctor *MouseAutoScrollTimer;
    TimerHandle MouseAutoScrollHandle;
    void MouseAutoScrollHandler (void *v);

    // Caret blinking
    bool BlinkCursor;
    void BlinkCursorTimerInterrupt (void *v);
    void StopBlinkCursor (bool BlinkState = false);
    void StartBlinkCursor (bool BlinkState = false);
    TimerFunctor *m_BlinkTimerFunctor;
    TimerHandle m_BlinkTimerHandler;

    TextDocument *m_pTextDoc;

    int			m_nCaretPosX;
    int			m_nAnchorPosX;
    void MoveLineUp (int numLines);

    bool IsKeyPressed (t_u32 virtualkey);

    void UspSnapXToOffset (
      int     lineno,
      int     xpos,
      int     *snappedX,          // out, optional
      t_u32   *CharacterColumn,   // out
      BOOL    *fRTL               // out, optional
    );

    void UspXToOffset (
      int     lineno,
      int     xpos,
      int		*px,                // [out] adjusted x coord of caret
      t_u32   *CharacterColumn,   // [out]
      t_u32	*pnFileOffset,      // [out] zero-based file-offset (in chars)
      TCHAR   *Character,         // [out]
      bool    *trailing           // [out]
    );

    bool UspOffsetToX (
      int     lineno,
      int     charPos,
      bool    trailing,
      int     *px         // [out]
    );
  };

}

#endif

