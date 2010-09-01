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

//#include <commctrl.h>
#include "Basic/NArray.h"
#include "../TimerProc.h"
#include "TextViewWidget.h"
#include "TextDocument.h"

//
//	ATTR - text character attribute
//	
typedef struct
{
	t_color	fg;			// foreground colour
	t_color	bg;			// background colour
	inl::t_u32		style;		// possible font-styling information

} ATTR;

//
//	FONT - font attributes
//
// typedef struct
// {
// 	// Windows font information
// 	HFONT		hFont;		
// 	TEXTMETRIC	tm;
// 
// 	// dimensions needed for control-character 'bitmaps'
// 	int			nInternalLeading;
// 	int			nDescent;
// 
// } FONT;

//
//	LINEINFO - information about a specific line
//
typedef struct
{
	inl::t_u32	nLineNo;
	int		nImageIdx;

	// more here in the future?

} LINEINFO;

//typedef int (__cdecl * COMPAREPROC) (const void *, const void *);

// maximum number of lines that we can hold info for at one time
#define MAX_LINE_INFO 128	

// maximum fonts that a TextView can hold
#define MAX_FONTS 32

enum SELMODE { SEL_NONE, SEL_NORMAL, SEL_MARGIN, SEL_BLOCK };

typedef struct
{
    unsigned long	line;
    unsigned long	xpos;

} CURPOS;

//
//	TextView - internal window implementation
//
class TextView: public TextViewWidget
{
public:

	TextView(/*HWND hwnd*/);
	~TextView();

	//long WINAPI WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw) {};
    
    virtual void PreLayoutManagement();
    virtual long PostLayoutManagement(long LayoutResult);
    virtual void PositionChildLayout(float offsetX, float offsetY);

    virtual void ScrollLeft(float stepx, int mousedx);
    virtual void ScrollRight(float stepx, int mousedx);
    virtual void ScrollUp(float stepy, int mousedy);
    virtual void ScrollDown(float stepy, int mousedy);

    void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseWheel(int x, int y, int delta, unsigned long button_flags, unsigned long key_flags);
    void RecvKeyEvent(
        GraphicsContext& GfxContext , /*Graphics Context for text operation*/
        unsigned long    eventType  , /*event type*/
        unsigned long    keysym     , /*event keysym*/
        unsigned long    state      , /*event state*/
        const char*      character  , /*character*/
        bool             isRepeated , /*true if the key is repeated more than once*/
        unsigned short   keyCount     /*key repeat count*/
    );

    void RecvStartFocus();
    void RecvEndFocus();

private:

    Geometry GetTextAreaGeometry() const;
	//
	//	Message handlers
	//
	long OnPaint(GraphicsContext& GfxContext);
    long OnPaintLine(GraphicsContext& GfxContext, unsigned int LigneNumber);
	long OnSize(UINT nFlags, int width, int height);
	long OnVScroll(UINT nSBCode, UINT nPos);
	long OnHScroll(UINT nSBCode, UINT nPos);
	long OnMouseWheelFunc(int nDelta);
	void RecvTimer(void*);

	//long OnMouseActivate(HWND hwndTop, UINT nHitTest, UINT nMessage);
	void OnLButtonDown(int x, int y, unsigned long button_flags, unsigned long key_flags /*UINT nFlags, int x, int y*/);
	void OnLButtonUp(int x, int y, unsigned long button_flags, unsigned long key_flags /*UINT nFlags, int x, int y*/);
    void RecvMouseMove(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags /*UINT nFlags, int x, int y*/);
    void OnLButtonDblClick(int x, int y, unsigned long button_flags, unsigned long key_flags /*UINT nFlags, int mx, int my*/);

// 	long OnSetFocus(HWND hwndOld);
// 	long OnKillFocus(HWND hwndNew);

	inl::t_u32 OnKeyDown(UINT nVirtKey, UINT nFlags);
	
	//
	//	
	//
	inl::t_u32 OpenFile(const TCHAR *szFileName);
	bool ClearFile();

	void SetFont();
	long SetLineSpacing(int nAbove, int nBelow);
	inl::t_u32 SetLongLine(int nLength);
	t_color SetColour(UINT idx, t_color rgbColour);

	//
	//	Private Helper functions
	//
	void PaintLine(GraphicsContext& GfxContext, inl::t_u32 line);
	void PaintText(GraphicsContext& GfxContext, inl::t_u32 nLineNo, Geometry& rect);
	void PaintMargin(GraphicsContext& GfxContext, inl::t_u32 line, Geometry& margin);
    void RenderColorQuadToBuffer(float* VertexBuffer, int VBSize, inl::Rect geo, inl::Color color);

	int   ApplyTextAttributes(inl::t_u32 nLineNo, inl::t_u32 offset, inl::t_u32 &nColumn, TCHAR *szText, int nTextLen, ATTR *attr);
	int   NeatTextOut(GraphicsContext& GfxContext, int xpos, int ypos, TCHAR *szText, int nLen, int nTabOrigin, ATTR *attr);
	
	int  PaintCtrlChar(GraphicsContext& GfxContext, int xpos, int ypos, inl::t_u32 chValue);
	void InitCtrlCharFontAttr();

	void RefreshWindow();
	long InvalidateRange(inl::t_u32 nStart, inl::t_u32 nFinish);
	long InvalidateLine(inl::t_u32 nLineNo);
	void UpdateLine(inl::t_u32 nLineNo);
    void AddDirtyLine(inl::t_u32 nLineNo);

    void	PaintRect(GraphicsContext& GfxContext, int x, int y, int width, int height, t_color fill);
    //void	PaintRect(GraphicsContext& GfxContext, RECT *rect, t_color fill);
    //void	DrawCheckedRect(GraphicsContext& GfxContext, RECT *rect, t_color fg, t_color bg);

	//int  CtrlCharWidth(inl::t_u32 chValue, FONT *fa);
	//int  NeatTextYOffset(FONT *font);
	int  NeatTextWidth(const TCHAR *buf, int len, int nTabOrigin);
	int	 TabWidth();
	int  LeftMarginWidth();
	void UpdateMarginWidth();
	int	 SetCaretWidth(int nWidth);
	//bool SetImageList(HIMAGELIST hImgList);

	bool  MouseCoordToFilePos(int x, int y, inl::t_u32 *pnLineNo, inl::t_u32 *pnFileOffset, int *psnappedX, TCHAR* Character, inl::t_u32* CharacterPosition);//, inl::t_u32 *pnLineLen=0);
	void  RepositionCaret();
    void  UpdateCaretOffset(inl::t_u32 offset, bool fTrailing, int *outx, inl::t_u32 *outlineno);
    void  UpdateCaretXY(int xpos, inl::t_u32 lineno);
	void  MoveCaret(int x, inl::t_u32 lineno);

	inl::t_u32	SetStyle(inl::t_u32 uMask, inl::t_u32 uStyles);
	
	inl::t_u32	SetVar(inl::t_u32 nVar, inl::t_u32 nValue);
	inl::t_u32	GetVar(inl::t_u32 nVar);

	inl::t_u32	GetStyleMask(inl::t_u32 uMask);
	bool	CheckStyle(inl::t_u32 uMask);

	t_color GetColour(UINT idx);
	t_color LineColour(inl::t_u32 nLineNo);
	t_color LongColour(inl::t_u32 nLineNo);

	int		SetLineImage(inl::t_u32 nLineNo, inl::t_u32 nImageIdx);
	LINEINFO *GetLineInfo(inl::t_u32 nLineNo);
	int		StripCRLF(TCHAR *szText, int nLength, bool fAllow);

	void	SetupScrollbars();
	void	UpdateMetrics();
	void	RecalcLineHeight();
	bool    PinToBottomCorner();

	void	Scroll(int dx, int dy);
	//HRGN	ScrollRgn(int dx, int dy, bool fReturnUpdateRgn);

    inl::Color m_TextBkColor;
    virtual void SetTextBkColor(const inl::Color& color);
    virtual const inl::Color& GetTextBkColor();

    //! If true, then we can draw the blinking cursor.
    bool m_HasFocus;
	//HWND  m_hWnd;
	inl::t_u32	m_uStyleFlags;

	// Font-related data	
	//FONT  m_FontAttr[MAX_FONTS];
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
	inl::t_u32	m_nSelectionStart;
	inl::t_u32	m_nSelectionEnd;
	inl::t_u32	m_nCursorOffset;
    TCHAR   m_CharacterAtCursor;
    inl::t_u32   m_CursorCharacterPosition;
	DWORD	m_nCaretWidth;
	inl::t_u32	m_nCurrentLine;
    inl::t_u32	m_nPreviousLine;
    std::vector<int> m_DirtyLines;
	int		m_nLongLineLimit;
	int		m_nCRLFMode;
    bool    m_RedrawCaret;
	
	LINEINFO	m_LineInfo[MAX_LINE_INFO];
	int			m_nLineInfoCount;

	int			m_nLinenoWidth;
#if defined (INL_OS_WINDOWS)
    HCURSOR     m_hMarginCursor;
#endif
	//RECT		m_rcBorder;

	t_color	m_rgbColourList[TXC_MAX_COLOURS];

	// Runtime data
	SELMODE		m_nSelectionMode;
    SELMODE		m_nSelectionType;
    CURPOS		m_cpBlockStart;
    CURPOS		m_cpBlockEnd;

    inl::t_u32		m_nSelMarginOffset1;
	inl::t_u32		m_nSelMarginOffset2;
	UINT		m_nScrollTimer;
	int			m_nScrollCounter;
	bool		m_fHideCaret;
    inl::Point  m_CaretPosition;
	bool		m_fTransparent;
	//HIMAGELIST	m_hImageList;

	// File-related data
	int		m_nLineCount;

    FontRenderer* m_TextFontRenderer;
    NArray<float> m_TextVertexBuffer0;
    NArray<float> m_TextVertexBuffer1;
    NArray<float> m_ColorQuadBuffer0;
    NArray<float> m_ColorQuadBuffer1;
    int m_NumVBQuad0;
    int m_NumVBQuad1;
    int m_NumVBColorQuad0;
    int m_NumVBColorQuad1;
    int m_VBSize0;
    int m_VBSize1;
    bool b_RenderToVertexBuffer;

    TRefGL<ICgPixelShader> m_PixelShaderProg;
    TRefGL<ICgVertexShader> m_VertexShaderProg;
    TRefGL<ICgPixelShader> m_ColorQuadPixelShader;
    TRefGL<ICgVertexShader> m_ColorQuadVertexShader;
    CGparameter m_CgPosition;
    CGparameter m_CgTexUV;
    CGparameter m_CgColor;
    CGparameter m_CgFontTexture;
    CGparameter m_CgQuadPosition;
    CGparameter m_CgQuadColor;

    TimerFunctor* MouseAutoScrollTimer;
    TimerHandle* MouseAutoScrollHandle;
    void MouseAutoScrollHandler(void* v);

    // Caret blinking
    bool BlinkCursor;
    void BlinkCursorTimerInterrupt(void* v);
    void StopBlinkCursor(bool BlinkState = false);
    void StartBlinkCursor(bool BlinkState = false);
    TimerFunctor* m_BlinkTimerFunctor;
    TimerHandle* m_BlinkTimerHandler;

	TextDocument *m_pTextDoc;

    int			m_nCaretPosX;
    int			m_nAnchorPosX;
    void MoveLineUp(int numLines);

    bool IsKeyPressed(unsigned long virtualkey);

    void UspSnapXToOffset(
        int     lineno,
        int     xpos,
        int     *snappedX,          // out, optional
        inl::t_u32   *CharacterColumn,   // out
        bool    *fRTL               // out, optional
        );

    void UspXToOffset(
        int     lineno,
        int     xpos,
        int		*px,                // [out] adjusted x coord of caret
        inl::t_u32   *CharacterColumn,   // [out]
        inl::t_u32	*pnFileOffset,      // [out] zero-based file-offset (in chars)
        TCHAR   *Character,         // [out]
        bool    *trailing           // [out]
        );

    bool UspOffsetToX(
        int     lineno,
        int     charPos,
        bool    trailing,
        int     *px         // [out]
        );
};

#endif
