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


#ifndef TABVIEW_H
#define TABVIEW_H

#include "Nux.h"
#include "InterfaceControl.h"
#include "TimerProc.h"
#include "HLayout.h"

NAMESPACE_BEGIN_GUI

class TabView : public ActiveInterfaceObject
{
public:
    TabView(NUX_FILE_LINE_PROTO);
    ~TabView();

    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    void AddTab(const char* tab_name, smptr(Layout) tab_layout);
    void SetActiveTad(int index);
    //! Return true if this object can break the layout.
    /*
        Return true if this object can break the layout, meaning, the layout can be done on the composition layout only without 
        recomputing the whole window layout.
        Inherited from ActiveInterfaceObject
    */
    virtual bool CanBreakLayout() { return false; }

    sigc::signal< void, const weaksmptr(TabView) > sigTabChanged;
    sigc::signal< void, int > sigTabIndexChanged;


private:
    class TabElement
    {
    public:
        TabElement()
        {
            index = 0;
        }
        ~TabElement()
        {
            index = 0;
            tab_layout = smptrnull(Layout);
        }
        std::string tab_name;
        smptr(Layout) tab_layout;
        smptr(CoreArea) tab_area;
        int index;
    };

public:
    void RecvTabMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags, TabElement*);
    void RecvTabMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags, TabElement*);

    void RecvMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags);

protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

private:

    void RecvTabRightTimerExpired(void* v);
    void RecvTabLeftTimerExpired(void* v);
    void RecvTabRightMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvTabLeftMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvTabButtonMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);

    virtual void PreLayoutManagement();
    virtual long PostLayoutManagement(long LayoutResult);
    virtual void PositionChildLayout(float offsetX, float offsetY);

    void TranslateLeft(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void TranslateRight(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void TranslateTabLayout(int offset);

    smptr(CoreArea) m_IncrTab;
    smptr(CoreArea) m_DecrTab;

    smptr(Layout) m_ClientLayout;
    smptr(HLayout) m_TabLayout;
    smptr(HLayout) m_TabControlLayout;

    int m_TabPositionOffset;
    int m_FocusTabIndex;

    TimerFunctor* tabright_callback;
    TimerFunctor* tableft_callback;
    TimerHandle m_TabRightTimerHandler;
    TimerHandle m_TabLeftTimerHandler;

    std::vector<TabElement*> m_TabVector;

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
NAMESPACE_END_GUI

#endif // TABVIEW_H
