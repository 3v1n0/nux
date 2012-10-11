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


#ifndef HSPLITTER_H
#define HSPLITTER_H

namespace nux
{
  class Layout;

  class HSplitter: public View
  {
    NUX_DECLARE_OBJECT_TYPE(HSplitter, View);
  public:
    HSplitter(NUX_FILE_LINE_PROTO);
    ~HSplitter();

    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);

    void AddWidget(Area *ic, float stretchfactor);
    void ResetSplitConfig();

    void clearContent();

    void OnSplitterMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags, int header_pos);
    void OnSplitterMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags, int header_pos);
    void OnSplitterMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags, int header_pos);

    virtual void OverlayDrawing(GraphicsEngine &graphics_engine);

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

    virtual Area* FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type);

  protected:
    virtual long ComputeContentSize();
    virtual void DoneRedraw();
    void ResizeSplitter(int header_pos);
    //void ContinuousSplitterAdjustment();
    void setResizeOnSplitterRelease(bool b)
    {
      m_ResizeOnSplitterRelease = b;
    }
    bool getResizeOnSplitterRelease()
    {
      return m_ResizeOnSplitterRelease;
    }

    virtual bool AcceptKeyNavFocus();
    virtual Area* KeyNavIteration(KeyNavDirection direction);

  private:
    typedef BasicView MySplitter;
    std::vector<Area *> m_InterfaceObject;
    std::vector<MySplitter *> m_SplitterObject;
    std::vector<float> m_SplitConfig;

    Point m_point;
    bool new_addition;

    bool m_ResizeOnSplitterRelease;
    int m_current_width;
    int m_current_height;
    int m_current_x;
    int m_current_y;

    bool m_initial_config;

    // splitter bar differential position;
    int mvt_dx, mvt_dy;
    int m_focus_splitter_index;
  };

}

#endif // HSPLITTER_H


