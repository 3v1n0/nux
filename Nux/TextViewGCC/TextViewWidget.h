/*
 * Copyright 2010 Inalogic® Inc.
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
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef TEXTVIEWWIDGET_H
#define TEXTVIEWWIDGET_H

#include "../View.h"

class HScrollBar;
class VScrollBar;

// Rename it to ScrollArea or ScrollWindow
class TextViewWidget: public View
{
public:
  TextViewWidget();
  virtual ~TextViewWidget();

  virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
  virtual void Draw (GraphicsContext &GfxContext, bool force_draw);
  virtual void DrawContent (GraphicsContext &GfxContext, bool force_draw);
  virtual void PostDraw (GraphicsContext &GfxContext, bool force_draw) {};

  // API
  void EnableVerticalScrollBar (bool b);
  void EnableHorizontalScrollBar (bool b);

public:
  //! Set the table size to be constrained by the size content.
  /*!
  Set the table size to be such that all the content items of the table are visible .
  The scrollbar will be useless as the content is entirely visible all the time. If the table is empty, then it assume its minimum size.
  This is needed for table inside ComboBox drop down memu.
  @param b If b is true, the size of the table is constrained by its content.
  @see IsSizeMatchLayout()
  */
  void SetSizeMatchContent (bool b);

  //! Check if the table size is constrained by the content of the table.
  /*!
  Check if the table size is constrained by its content.
  @return If the return value is true, the table size is constrained by its content.
  @see SetSizeMatchContent
  */
  bool IsSizeMatchContent() const;

  //    bool SetMaxSizeMatchLayout(inl::Size size)
  //    {
  //        m_MaxSizeMatchLayout = size;
  //        if(m_MaxSizeMatchLayout.GetWidth() < 0)
  //            m_MaxSizeMatchLayout.SetWidth(0);
  //        if(m_MaxSizeMatchLayout.GetBaseHeight() < 0)
  //            m_MaxSizeMatchLayout.SetHeight(0);
  //    }

private:
  bool m_bSizeMatchContent;

public:
  // Inherited from Area
  virtual void setGeometry (const Geometry &geo);

  /////////////////
  //  EMITERS    //
  /////////////////
  void OnSizeGrigMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
  void OnSizeGrigMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
  void EmitMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
  //void EmitInternalResize(int x, int y, int w, int h);


  /////////////////
  //  RECEIVERS  //
  /////////////////
//     virtual void ScrollLeft(float stepx, int mousedx);
//     virtual void ScrollRight(float stepx, int mousedx);
//     virtual void ScrollUp(float stepy, int mousedy);
//     virtual void ScrollDown(float stepy, int mousedy);

  virtual void ResetScrollToLeft();
  virtual void ResetScrollToRight();
  virtual void ResetScrollToUp();
  virtual void ResetScrollToDown();

  int m_ContentOffsetX;
  int m_ContentOffsetY;

  // Geometry of the layout that encompass the child layouts.
  int m_ViewContentX;
  int m_ViewContentY;
  int m_ViewContentWidth;
  int m_ViewContentHeight;

  //Client View Area
  int m_ViewX;
  int m_ViewY;
  int m_ViewWidth;
  int m_ViewHeight;
  Geometry m_ViewGeometry;
  Geometry m_ContentGeometry;

  // signals
  sigc::signal<void> SigTest;
  sigc::signal<void> sigMoveWindow;
  sigc::signal<void, int, int, int, int> sigResize;

protected:
  // Backup texture to speed up scrolling
  TRefGL<IOpenGLFrameBufferObject> m_FrameBufferObject;
  TRefGL<IOpenGLTexture2D> m_TextureBuffer[2];
  void SwapTextureIndex()
  {
    m_TextureIndex = (m_TextureIndex == 0) ? 1 : 0;
  }
  void SetTextureIndex (int index)
  {
    m_TextureIndex = index;
  }
  int GetTextureIndex()
  {
    return m_TextureIndex;
  }
  int m_TextureIndex;
  bool m_ReformatTexture;


  // ScrollBars
  HScrollBar *hscrollbar;
  VScrollBar *vscrollbar;
  bool m_horizontal_scrollbar_enable;
  bool m_vertical_scrollbar_enable;

  int m_SizeGripDragPositionX; //<<<--- remove this
  int m_SizeGripDragPositionY;

  //int viewx;
  // Internal function
  int getBorder() const;
  int getTopBorder() const;
  void setBorder (int border);
  void setTopBorder (int top_border);

  int m_top_border;
  int m_border;

public:
  void    SetViewContentLeftMargin (int margin)
  {
    m_ViewContentLeftMargin = margin;
  }
  int     GetViewContentLeftMargin() const
  {
    return m_ViewContentLeftMargin;
  }
  void    SetViewContentRightMargin (int margin)
  {
    m_ViewContentRightMargin = margin;
  }
  int     GetViewContentRightMargin() const
  {
    return m_ViewContentRightMargin;
  }
  void    SetViewContentTopMargin (int margin)
  {
    m_ViewContentTopMargin = margin;
  }
  int     GetViewContentTopMargin() const
  {
    return m_ViewContentTopMargin;
  }
  void    SetViewContentBottomMargin (int margin)
  {
    m_ViewContentBottomMargin = margin;
  }
  int     GetViewContentBottomMargin() const
  {
    return m_ViewContentBottomMargin;
  }
private:
  int m_ViewContentLeftMargin;
  int m_ViewContentRightMargin;
  int m_ViewContentTopMargin;
  int m_ViewContentBottomMargin;

protected:
  void FormatContent();
  virtual void PreLayoutManagement();
  virtual long PostLayoutManagement (long LayoutResult);
  virtual void PositionChildLayout (float offsetX, float offsetY);

  virtual void PreLayoutManagement2();
  virtual long PostLayoutManagement2 (long LayoutResult);

public:
  NFontPtr m_TextFont;

  ///////////////////////////////////////////////////////
  // AbstractInterfaceObject
  ///////////////////////////////////////////////////////
};

#endif // TEXTVIEWWIDGET_H
