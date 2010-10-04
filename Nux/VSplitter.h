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


#ifndef VSPLITTER_H
#define VSPLITTER_H

namespace nux { //NUX_NAMESPACE_BEGIN

class layout;

class VSplitter: public ActiveInterfaceObject
{
    NUX_DECLARE_OBJECT_TYPE(VSplitter, ActiveInterfaceObject);
public:
    VSplitter(NUX_FILE_LINE_PROTO);
    ~VSplitter();

    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    void AddWidget(Area* ic, float stretchfactor);
    void ResetSplitConfig();

    void clearContent();

    void OnSplitterMouseDown(t_s32 x, t_s32 y, unsigned long button_flags, unsigned long key_flags, t_s32 header_pos);
    void OnSplitterMouseUp(t_s32 x, t_s32 y, unsigned long button_flags, unsigned long key_flags, t_s32 header_pos);
    void OnSplitterMouseDrag(t_s32 x, t_s32 y, t_s32 dx, t_s32 dy, unsigned long button_flags, unsigned long key_flags, t_s32 header_pos);

    virtual void OverlayDrawing(GraphicsContext& GfxContext);

    //! Return true if this object can break the layout.
    /*
    Return true if this object can break the layout, meaning, the layout can be done on the composition layout only without 
    recomputing the whole window layout.
    Inherited from Area
    */
    virtual bool CanBreakLayout()
    {
        return true;
    }

protected:
    virtual long ComputeChildLayout();
    virtual void DoneRedraw();
    void ResizeSplitter(t_s32 header_pos);
    //void ContinuousSplitterAdjustment();
    void setResizeOnSplitterRelease(bool b) { m_ResizeOnSplitterRelease = b; }
    bool getResizeOnSplitterRelease() { return m_ResizeOnSplitterRelease; }

private:

    typedef CoreArea MySplitter;
    std::vector<Area*> m_InterfaceObject;
    std::vector<MySplitter*> m_SplitterObject;
    std::vector<float> m_SplitConfig;

    Layout *m_layout;
    Point m_point;
    bool new_addition;
    
    bool m_ResizeOnSplitterRelease;
    t_s32 m_current_width;
    t_s32 m_current_height;
    t_s32 m_current_x;
    t_s32 m_current_y;

    bool m_initial_config;

    // splitter bar differential position;
    t_s32 mvt_dx, mvt_dy;
    t_s32 m_focus_splitter_index;
};

} //NUX_NAMESPACE_END

#endif // VSPLITTER_H


