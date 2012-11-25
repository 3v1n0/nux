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
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef FLOATINGWINDOW_H
#define FLOATINGWINDOW_H

#include "NuxGraphics/Events.h"

#include "ScrollView.h"
#include "BaseWindow.h"
#include "BasicView.h"
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
    NUX_DECLARE_OBJECT_TYPE(FloatingWindow, BaseWindow);
  public:
    FloatingWindow(const char *WindowName = "", NUX_FILE_LINE_PROTO);
    ~FloatingWindow();

    void SetVisibleSizeGrip(bool b)
    {
      if (b && (m_bSizeMatchLayout))
        m_bIsVisibleSizeGrip = false;
      else
        m_bIsVisibleSizeGrip = b;
    }

    bool IsVisibleSizeGrip()
    {
      return m_bIsVisibleSizeGrip;
    }


    void EnableTitleBar(bool b);
    bool HasTitleBar() const;
    void OnSizeGrigMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnSizeGrigMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvCloseButtonClick(int x, int y, unsigned long button_flags, unsigned long key_flags);

    void RecvTitleBarMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvTitleBarMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    void SetWindowTitle(const char *title);
    std::string GetWindowTitle();

    //! Return true if this object can break the layout.
    /*
        Return true if this object can break the layout, meaning, the layout can be done on the composition layout only without
        recomputing the whole window layout.
        Inherited from View
    */
    virtual bool CanBreakLayout()
    {
      if (IsSizeMatchContent())
        return false;

      return true;
    }

  protected:
    virtual Area* FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type);
    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);

    virtual void PreLayoutManagement();
    virtual long PostLayoutManagement(long LayoutResult);
    virtual void ComputeContentPosition(float offsetX, float offsetY);

    //! Layout the window elements.
    /*!
        Layout elements such as button on the title bar, and the resize widget according to the current
        geometry of the window. Also initiate the computation of the child layout if there is one.
    */
    virtual void LayoutWindowElements();

    int m_SizeGripDragPositionX;
    int m_SizeGripDragPositionY;

  private:
    int _resize_handle_width;
    int _resize_handle_height;
    int _title_bar_height;

    BasicView *_resize_handle;
    BasicView *_title_bar;
    Point _title_bar_mouse_down_location;

    BasicView *_minimize_button;
    BasicView *_close_button;
    StaticTextBox *_window_title_bar;
    bool m_hasTitleBar;

    bool m_bIsVisible;
    bool m_bIsVisibleSizeGrip;
    bool m_bIsModal;
    //! If true then the FloatingWindow is resized to match the size of the layout.
    bool m_bSizeMatchLayout;


    BaseTexture* CloseIcon;
    BaseTexture* MinimizeIcon;

    std::list<View *> m_InterfaceObject;
    HLayout *_title_bar_layout;

    std::string _window_title;

    friend class PopUpWindow;
    friend class ComboBox_Logic_WindowView;
    friend class ComboBoxComplex;
    friend class WindowCompositor;
  };
}

#endif // FLOATINGWINDOW_H
