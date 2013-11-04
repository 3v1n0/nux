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
 * Authored by: Neil Jagdish Patel <neil.patel@canonical.com>
 *
 */


#ifndef LAYERED_LAYOUT_H
#define LAYERED_LAYOUT_H

#include "Layout.h"

namespace nux
{
  //! An layered layout.
  /*!
  LayeredLayout works either as a bin layout, showing only one layer at a time, or it works
  as a composite layout, drawing all children in order(and you are able to modify the order).

  The layout also allows two modes of input. In INPUT_MODE_ACTIVE, the layout will only send
  events to the active layer, even if in composite drawing mode. In INPUT_MODE_COMPOSITE, the
  layout will send events to all the layers, in the stacking order from top to bottom. This
  allows creation of complex widgets more easily than implementing the drawing and input modes
  manually.
  */
  class LayeredLayout: public Layout
  {
    NUX_DECLARE_OBJECT_TYPE(LayeredLayout, Layout);
  public:
    class LayeredChildProperties // : public LayeredLayout::LayoutProperties
    {
    public:
      LayeredChildProperties(bool expand, int x, int y, int width, int height);
      ~LayeredChildProperties();
      void Update(bool expand, int x, int y, int width, int height);

      bool m_expand;
      int  m_x;
      int  m_y;
      int  m_width;
      int  m_height;
      sigc::signal<void, Area *, bool>::iterator m_vis_it;
    };

    typedef enum
    {
      INPUT_MODE_ACTIVE = 0,
      INPUT_MODE_COMPOSITE

    } InputMode;

    LayeredLayout(NUX_FILE_LINE_PROTO);
    ~LayeredLayout();

    //! Add a layer to the layout
    /*!
    This method will add the layer with layout specific options
    \param area   the Area, Layout or View
    \param expand area should be expanded to all the available space of the layout. If this is set to false, all the following parameters must be set
    \param x      the horizontal position of the layer, expand must be false
    \param y      the vertical position of the layer, expand must be false
    \param width  the width of the layer inside the layout, expand must be false
    \param height the height of the layer inside the layout, expand must be false
    */
    void AddLayer    (Area *area, bool expand=true, int x=0, int y=0, int width=0, int height=0);

    //! Update properties of a layer in the layout
    /*!
    Allows updating properties of a layer after it has been added to the layout
    \param area   the Area, Layout or View to update
    \param expand area should be expanded to all the available space of the layout. If this is set to false, all the following parameters must be set
    \param x      the horizontal position of the layer, expand must be false
    \param y      the vertical position of the layer, expand must be false
    \param width  the width of the layer inside the layout, expand must be false
    \param height the height of the layer inside the layout, expand must be false
    */
    void UpdateLayer(Area *area, bool expand=true, int x=0, int y=0, int width=0, int height=0);

    //! Remove a layer
    /*!
    This method will remove a layer from the layout. It is here for completeness.
    \param area   the Area, Layout or View to remove
    */
    void RemoveLayer(Area *area);

    //! Set the active layer of the layout
    /*!
    The active layer will receives input in the input mode is INPUT_MODE_ACTIVE.
    \param index_ The index of the layer to make active
    */
    void SetActiveLayerN(int index_);

    //! Get the active layer of the layout
    /*!
    Returns the index of the active layer of the layout. This is only useful if input mode is INPUT_MODE_ACTIVE.
    \return the index of the active layer
    */
    int GetActiveLayerN();

    //! Set the active layer of the layout
    /*!
    The active layer will receives input in the input mode is INPUT_MODE_ACTIVE.
    \param area The area of the layer to make active
    */
    void SetActiveLayer(Area *area);

    //! Get the active layer of the layout
    /*!
    Returns the the active layer of the layout. This is only useful if input mode is INPUT_MODE_ACTIVE.
    \return the active layer
    */
    Area * GetActiveLayer  ();

    //! Set whether the layout will paint all the layers it contains. Default is false.
    /*!
    Normally, the layout will only paint the active layer. However, if you are using the layout in INPUT_COMPOSITE,
    or have just sized and positioned the layers that you'd like them to be drawn so that they they are composited
    inside the layout, this should be set to true.
    \param paint_all whether to paint all the layers in the layout
    */
    void SetPaintAll(bool paint_all);

    //! Get whether the layout is drawing all the layers it contains
    /*!
    Returns whether the layout is drawing all the layers it contains. See SetPaintAll.
    \return whether the layout is drawing all the layers
    */
    bool GetPaintAll();

    //! Sets the input mode of the layout
    /*!
    The layout is able to operate in two modes. INPUT_MODE_ACTIVE means that the layout will send events only to the
    active layout. In INPUT_MODE_COMPOSITE, the layout sends events to all the layouts it contains, starting from the
    topmost down to the bottom. A layer can stop propagation by returning the appropriate value in ProcessEvent.
    This can be mixed and matched with SetPaintAll depending on what you want to achieve. For instance, having
    paint all set to true but input_mode set to INPUT_MODE_ACTIVE allows you to create a composite view with one or
    many backgrounds, but with only one active view.
    \param input_mode the input mode
    */
    void SetInputMode(LayeredLayout::InputMode input_mode);

    //! Raise the paint and input depth of an area
    /*!
    Raises the paint and input depth of the area in the layout
    \param area area to raise
    \param above area to raise above
    */
    void Raise(Area *area, Area *above);

    //! Lower the paint and input depth of an area
    /*!
    Lowers the paint and input depth of the area in the layout
    \param area area to raise
    \param below area to lower below
    */
    void Lower(Area *area, Area *below);

    //! Raises the paint and input depth of area to the top of the layout
    /*!
    Area will be drawn above all other layers and will be the first receive events
    \param area area to raise
    */
    void RaiseTop(Area *area);

    //! Lowers the paint and input depth of area to the bottom of the layout
    /*!
    Area will be drawn below all other layers and will be the last receive events
    \param area area to lower
    */
    void LowerBottom(Area *area);

    //! Get which input mode is set on the layout
    /*!
    Returns the current input mode on the layout.
    \return the current input mode on the layout
    */
    LayeredLayout::InputMode GetInputMode();

//     //! Set the layout properties for this area
//     /*!
//         Allows the Layout managing this area to store the properties specifc to this area. Layouts
//         should create a sub-class of LayoutProperties. The LayoutProperties of an area will
//         be deleted upon destruction.
//         @param properties the LayoutProperties sub-class  associated with this area. Can be NULL to
//          unset.
//     */
//     void SetLayoutProperties(LayoutProperties *properties);
//
//     //! Get the layout properties for this area
//     /*!
//         Retrieves the LayoutProperties sub-class with this area. See SetLayoutProperties
//         @return LayoutProperties sub-class associated with this area.
//     */
//     LayoutProperties * GetLayoutProperties();

    void AddLayout(Layout                *layouy,
                    unsigned int           stretch_factor = 1,
                    MinorDimensionPosition position       = eAbove,
                    MinorDimensionSize     extend         = eFull,
                    float                  percentage     = 100.0f,
                    LayoutPosition = NUX_LAYOUT_END);
    void AddView(Area                  *view,
                  unsigned int           stretch_factor = 1,
                  MinorDimensionPosition positioning    = eAbove,
                  MinorDimensionSize     extend         = eFull,
                  float                  percentage     = 100.0f,
                  LayoutPosition = NUX_LAYOUT_END);
    void RemoveChildObject(Area *area);
    void Clear();

  protected:
    //
    // Overrides
    //
    long ComputeContentSize();
    void GetCompositeList(std::list<Area *> *ViewList);

    void ProcessDraw(GraphicsEngine &gfx_context, bool force_draw);
    Area* FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type);

    virtual Area* KeyNavIteration(KeyNavDirection direction);
    void OnLayerGeometryChanged(Area* area, Geometry geo);

  private:
    void PaintOne(Area *area, GraphicsEngine &graphics_engine, bool force_draw);
    void ChildQueueDraw(Area *area);
    void ChildVisibilityChanged(Area *area, bool visible);

  private:
    typedef std::map<Area*, std::shared_ptr<LayeredChildProperties> > AreaPropertyMap;
    AreaPropertyMap area_property_map_;
    int                       m_active_index;
    Area                      *m_active_area;
    bool                      m_paint_all;
    LayeredLayout::InputMode  m_input_mode;
    bool                      m_child_draw_queued;
  };
}

#endif // LAYERED_LAYOUT_H
