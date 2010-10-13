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


#ifndef FLOATINGWINDOW_H
#define FLOATINGWINDOW_H

#include "ScrollView.h"
#include "BaseWindow.h"

#if defined(NUX_OS_WINDOWS)
#include "NuxGraphics/Gfx_Events.h"
#elif defined(NUX_OS_LINUX)
#include "NuxGraphics/GfxEventsX11.h"
#endif

#include "InputArea.h"
#include "MouseHandler.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "Validator.h"
#include "StaticTextBox.h"

namespace nux
{

  class HLayout;
  class PopUpWindow;

  class FloatingWindow: public BaseWindow
  {
    NUX_DECLARE_OBJECT_TYPE (FloatingWindow, BaseWindow);
  public:
    FloatingWindow (const TCHAR *WindowName = TEXT (""), NUX_FILE_LINE_PROTO);
    ~FloatingWindow();


    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsContext &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsContext &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsContext &GfxContext, bool force_draw);

    void SetVisibleSizeGrip (bool b)
    {
      if (b && (m_bSizeMatchLayout) )
        m_bIsVisibleSizeGrip = false;
      else
        m_bIsVisibleSizeGrip = b;
    }

    bool IsVisibleSizeGrip()
    {
      return m_bIsVisibleSizeGrip;
    }


    void EnableTitleBar (bool b);
    bool HasTitleBar() const;
    void OnSizeGrigMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnSizeGrigMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvCloseButtonClick (int x, int y, unsigned long button_flags, unsigned long key_flags);

    void RecvTitleBarMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvTitleBarMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    void SetWindowTitle (const char *title);
    const TCHAR *GetWindowTitle();

    //! Return true if this object can break the layout.
    /*
        Return true if this object can break the layout, meaning, the layout can be done on the composition layout only without
        recomputing the whole window layout.
        Inherited from View
    */
    virtual bool CanBreakLayout()
    {
      if (IsSizeMatchContent() )
        return false;

      return true;
    }
  protected:
    virtual void PreLayoutManagement();
    virtual long PostLayoutManagement (long LayoutResult);
    virtual void PositionChildLayout (float offsetX, float offsetY);
    //! Layout the window elements.
    /*!
        Layout elements such as button on the title bar, and the resize widget according to the current
        geometry of the window. Also initiate the computation of the child layout if there is one.
    */
    virtual void LayoutWindowElements();

    int m_SizeGripDragPositionX;
    int m_SizeGripDragPositionY;

  private:
    CoreArea *m_SizeGrip;
    CoreArea *m_TitleBar;
    Point m_TitleBarMouseDown;

    CoreArea *m_MinimizeButton;
    CoreArea *m_CloseButton;
    StaticTextBox *m_WindowTitleBar;
    bool m_hasTitleBar;

    bool m_bIsVisible;
    bool m_bIsVisibleSizeGrip;
    bool m_bIsModal;
    //! If true then the FloatingWindow is resized to match the size of the layout.
    bool m_bSizeMatchLayout;


    NTexture* CloseIcon;
    NTexture* MinimizeIcon;

    std::list<View *> m_InterfaceObject;
    HLayout *m_TitleBarLayout;

    TCHAR *m_WindowTitle;

    friend class PopUpWindow;
    friend class ComboBox_Logic_WindowView;
    friend class WindowCompositor;
  };

}
#endif // FLOATINGWINDOW_H
