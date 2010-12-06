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


#ifndef TABVIEW_H
#define TABVIEW_H

#include "Nux.h"
#include "View.h"
#include "TimerProc.h"
#include "HLayout.h"

namespace nux
{

  class TabView : public View
  {
  public:
    TabView (NUX_FILE_LINE_PROTO);
    ~TabView();

    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);

    void AddTab (const char *tab_name, Layout *tab_layout);
    void SetActiveTad (int index);
    //! Return true if this object can break the layout.
    /*
        Return true if this object can break the layout, meaning, the layout can be done on the composition layout only without
        recomputing the whole window layout.
        Inherited from View
    */
    virtual bool CanBreakLayout()
    {
      return false;
    }

    sigc::signal< void, TabView * > sigTabChanged;
    sigc::signal< void, int > sigTabIndexChanged;


  private:
    class TabElement
    {
    public:
      TabElement (NString TabName, Layout *TabLayout);
      ~TabElement();

      void SetIndex (int index);
      int GetIndex() const;
      void SetGeometry (const Geometry &geo);
      Geometry GetGeometry() const;
      const NString &GetName() const;

      NString     _tab_name;
      Layout     *_tab_layout;
      CoreArea   *_tab_area;
      int         _index;
    };

  public:
    void RecvTabMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags, TabElement *);
    void RecvTabMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags, TabElement *);

    void RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags);

  protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

  private:

    void RecvTabRightTimerExpired (void *v);
    void RecvTabLeftTimerExpired (void *v);
    void RecvTabRightMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvTabLeftMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvTabButtonMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags);

    virtual void PreLayoutManagement();
    virtual long PostLayoutManagement (long LayoutResult);
    virtual void PositionChildLayout (float offsetX, float offsetY);

    void TranslateLeft (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void TranslateRight (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void TranslateTabLayout (int offset);

    CoreArea   *m_IncrTab;
    CoreArea   *m_DecrTab;

    Layout     *m_ClientLayout;
    HLayout    *m_TabLayout;
    HLayout    *m_TabControlLayout;

    int m_TabPositionOffset;
    int m_FocusTabIndex;

    TimerFunctor   *tabright_callback;
    TimerFunctor   *tableft_callback;
    TimerHandle     m_TabRightTimerHandler;
    TimerHandle     m_TabLeftTimerHandler;

    std::vector<TabElement *> m_TabVector;

    static Color TAB_HEADER_BACKGROUND_COLOR;
    static Color TAB_BACKGROUND_COLOR;
    static Color TAB_HEADER_COLOR;
    static Color TAB_HEADER_FOCUS_COLOR;

    static int TAB_BUTTON_WIDTH;
    static int TAB_BUTTON_HEIGHT;
    static int TAB_HEIGHT;
    static int TAB_X_BORDER;
    static int TAB_Y_BORDER;

    // We need to draw the background on the previous size of the Table if its
    // size is set to match the content(IsSizeMatchContent) and an item is close.
    Geometry m_PreviousGeometry;
    bool m_DrawBackgroundOnPreviousGeometry;
  };
}

#endif // TABVIEW_H
