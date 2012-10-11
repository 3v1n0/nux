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


#ifndef LAYOUT_H
#define LAYOUT_H

namespace nux
{

#define DEBUG_LAYOUT 0
#define DEBUG_LAYOUT_COMPUTATION 0

  typedef enum
  {
    NUX_LAYOUT_BEGIN = 0,
    NUX_LAYOUT_END = 0x7fffffff
  } LayoutPosition;

  class Layout: public Area
  {
    NUX_DECLARE_OBJECT_TYPE(Layout, Area);
  public:
    Layout(NUX_FILE_LINE_PROTO);
    virtual ~Layout();

    virtual void AddLayout(Layout *, unsigned int stretchFactor = 1, MinorDimensionPosition = eAbove, MinorDimensionSize extend = eFull, float percentage = 100.0f, LayoutPosition = NUX_LAYOUT_END);

    //! Add an object to the layout.
    /*! Add an object to the layout.
        A baseobject minor dimension with respect to a layout object is the dimension opposite to the layout flow.
        A baseobject major dimension with respect to a layout object is the dimension aligned with the layout flow.
        A layout object minor dimension is the dimension opposite to the layout flow.
        A layout object major dimension is the dimension aligned with the layout flow.

        Add an object to the layout. The added object get its size and position managed by the layout.
        When a baseobject is added with a stretches factor equal to 0, its major dimension assumes its minimum value.
        For instance, if the layout is a vertical layout and the added object has a stretch factor equal 0, then during the layout,
        the added object height will be set to its minimum value using ApplyMinHeight().

        The positioning parameter controls how the layout will place the object within itself. A vertical layout object controls the horizontal positioning
        of its children baseobject, While an horizontal layout object controls the vertical positioning of its children baseobject.

        The extend parameter controls how much size the baseobject minor dimension gets from the layout minor dimension. See MinorDimensionSize.

        /param baseobject The object that is being added.
        /param stretchFactor This value controls how the layout object share space between its children baseobject.
        /param positioning Controls how the layout position the object.
        /param extend Controls the object minor dimension size.
        /param percentage Controls the object minor dimension size in percentage of the layout minor dimension size.
        /param index Controls the object position in the layout.
    */
    virtual void AddView(Area *baseobject, unsigned int stretchFactor = 1, MinorDimensionPosition positioning = eAbove, MinorDimensionSize extend = eFull, float percentage = 100.0f, LayoutPosition index = NUX_LAYOUT_END);
    virtual void AddSpace(unsigned int width, unsigned int stretchFactor = 0, LayoutPosition index = NUX_LAYOUT_END);

    virtual void Clear();

    //! Set the left/right padding with the same value.
    /*!
        Set the left/right padding of the layout. \n
        Valid only for HLayout, VLayouts, HGridLayouts and VGridLayout.

        @param padding The left/right padding value of the layout.
    */
    void SetLeftAndRightPadding(int padding);

    //! Set the left/right padding independently.
    /*!
        Set the left/right padding of the layout. \n
        Valid only for HLayout, VLayouts, HGridLayouts and VGridLayout.

        @param left Left padding value of the layout.
        @param right Right padding value of the layout.
    */
    void SetLeftAndRightPadding(int left, int right);

    //! Set the top/bottom padding with the same value.
    /*!
        Set the top/bottom padding of the layout. \n
        Valid only for HLayout, VLayouts, HGridLayouts and VGridLayout.

        @param padding The top/bottom padding value of the layout.
    */
    void SetTopAndBottomPadding(int padding);


    //! Set the top/bottom padding independently.
    /*!
        Set the top/bottom padding of the layout. \n
        Valid only for HLayout, VLayouts, HGridLayouts and VGridLayout.

        @param top Top padding value of the layout.
        @param bottom Bottom padding value of the layout.
    */
    void SetTopAndBottomPadding(int top, int bottom);

    //! Set the left/right and top/bottom padding of the layout.
    /*!
        Set the left/right and top/bottom padding of the layout. \n
        Valid only for HLayout, VLayouts, HGridLayouts and VGridLayout.

        @param top_bottom_padding The top/bottom padding value of the layout.
        @param left_right_padding The left/right padding value of the layout.
    */
    void SetPadding(int top_bottom_padding, int left_right_padding);

    virtual unsigned int GetMaxStretchFactor();
    unsigned int GetMinStretchFactor();
    unsigned int GetNumStretchFactor(unsigned int sf);

    int GetContentWidth() const
    {
      return m_contentWidth;
    };

    int GetContentHeight() const
    {
      return m_contentHeight;
    };


    //! Deprecated. Use SetLeftAndRightPadding.
    void SetHorizontalExternalMargin(int m);

    //! Deprecated. Use SetTopAndBottomPadding,
    void SetVerticalExternalMargin(int m);

    //! Set the left/right/top/bottom padding of the layout.
    /*!
        Set the left/right and top/bottom padding of the layout. \n
        Valid only for HLayout, VLayouts, HGridLayouts and VGridLayout.

        @param top The top padding value of the layout.
        @param right The right padding value of the layout.
        @param bottom The bottom padding value of the layout.
        @param left The left padding value of the layout.
    */
    void SetPadding(int top, int right, int bottom, int left);

    //! Set the left/right/top/bottom padding of the layout.
    /*!
        Set the left/right/top/bottom padding of the layout. \n
        Valid only for HLayout, VLayouts, HGridLayouts and VGridLayout.

        @param padding The top/right/bottom/left padding value of the layout.
    */
    void SetPadding(int padding);

    int GetLeftPadding() const;
    int GetRightPadding() const;
    int GetTopPadding() const;
    int GetBottomPadding() const;

  public:

    virtual void GetCompositeList(std::list<Area *> * /* ViewList */)
    {

    }
    virtual void Draw() {}

    void DoneRedraw();

    bool SearchInAllSubNodes(Area *bo);
    bool SearchInFirstSubNodes(Area *bo);

    Area* FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type);

    //! Draw Element
    /*!
        Draw all elements inside the layout.
        If force_draw is true then the system requests that all objects redraw themselves completely.
        \param force_draw
        \param TraverseInfo
        \param ProcessEventInfo
        \return The state of the Process Event.
    */
    virtual void ProcessDraw(GraphicsEngine &graphics_engine, bool force_draw);

    //! Mark all element in the layout as dirty.
    /*!
        Mark all element in the layout as dirty. This will also mark all sub elements as dirty.
        InputArea element are not marked as dirty(they don't have the flags).
        Emits the signal \i queue_draw.
    */
    virtual void QueueDraw();

    //! Return true if a draw has been scheduled for this layout
    /*!
        @return True if a draw has been scheduled for this layout.
    */
    bool IsQueuedForDraw();

    //! Return true if a draw has been scheduled for a child of this layout
    /*!
        @return True if a draw has been scheduled for a child of this layout.
    */
    bool ChildQueuedForDraw();

    //! Define how elements are spread out inside the layout.
    /*!
        Typically, a layout stacks it elements from left to right(HLayout) or top to bottom(VLayout).
        When the elements don't uses all the space that is available, the content stacking policy allows
        alternatives ways to position the elements. This does not affect the elements size, only their position
        inside the layout.
        @param stacking_order
    */
    virtual void SetContentDistribution(LayoutContentDistribution stacking_order);
    virtual LayoutContentDistribution GetContentDistribution();

    virtual bool FindWidget(Area *WidgetObject) const;
    virtual bool IsEmpty() const;
    /*
        This function is reimplemented in Layout and View classes  they need to perform some special operations.
        It does nothing for Area classes(this class cannot have children).
    */
    virtual void RemoveChildObject(Area *);

    //! Request a Layout recompute after a change of size
    /*
        When an object size changes, it is necessary for its parent structure to initiate a layout
        re computation in order preserve the layout structure defined by the user through the API.
    */
    virtual void RequestBottomUpLayoutComputation(Area *bo_initiator);

    std::list<Area *>& GetChildren()
    {
      return _layout_element_list;
    }

    virtual void ChildQueueDraw(Area* area);

    sigc::signal<void, Layout*> queue_draw;  //!< Signal emitted when a layout is scheduled for a draw.
    sigc::signal<void, Area*>   child_queue_draw;
    sigc::signal<void, Layout*, Area*> ViewAdded;
    sigc::signal<void, Layout*, Area*> ViewRemoved;

#ifdef NUX_GESTURES_SUPPORT
    virtual Area* GetInputAreaHitByGesture(const GestureEvent &event);
#endif
    
    /*!
        When a layout goes through Layout::ProcessDraw, this call isn't necessary. Otherwise, call it
        to set the value of draw_cmd_queued_ to false. 
    */
    virtual void ResetQueueDraw(); 

  protected:
    void BeginBackupTextureRendering(GraphicsEngine& graphics_engine, bool force_draw);
    void EndBackupTextureRendering(GraphicsEngine& graphics_engine, bool force_draw);

    virtual void GeometryChangePending(bool position_about_to_change, bool size_about_to_change);
    virtual void GeometryChanged(bool position_has_changed, bool size_has_changed);

    virtual bool AcceptKeyNavFocus();
    
    bool draw_cmd_queued_; //<! The rendering of the layout needs to be refreshed.
    bool child_draw_cmd_queued_; //<! A child of this layout has requested a draw.

    Size m_ContentSize;
    int m_contentWidth;
    int m_contentHeight;

    // Apply only to layout element. This is the width and height computed while sizing the child element.
    int m_fittingWidth;
    int m_fittingHeight;

    //int m_h_in_margin;
    //int m_v_in_margin;

    int space_between_children_;
    int top_padding_;
    int bottom_padding_;
    int left_padding_;
    int right_padding_;

    std::list<Area *> _layout_element_list;

    std::string m_name;

    LayoutContentDistribution m_ContentStacking;
  };


// The Space layout is a layout object that is used to create fixed or re-sizable empty space.
  class SpaceLayout: public Layout
  {
    NUX_DECLARE_OBJECT_TYPE(SpaceLayout, Layout);
  public:
    SpaceLayout(NUX_FILE_LINE_PROTO)
      : Layout(NUX_FILE_LINE_PARAM)
    {
    };

    SpaceLayout(int minWidth, int maxWidth, int minHeight, int maxHeight, NUX_FILE_LINE_PROTO)
      : Layout(NUX_FILE_LINE_PARAM)
    {
      SetMinimumSize(minWidth, minHeight);
      SetMaximumSize(maxWidth, maxHeight);
    };

    ~SpaceLayout()
    {
    };

    virtual bool FindWidget(Area * /* WidgetObject */) const
    {
      return false;
    }
    virtual bool IsEmpty() const
    {
      return true;
    }


    virtual void AddLayout(Layout *, unsigned int /* stretchFactor */ = 1, MinorDimensionPosition /* minor_position */ = eAbove, MinorDimensionSize /* minor_size */ = eFull, float /* percentage */ = 100.0f, LayoutPosition /* index */ = NUX_LAYOUT_END)
    {
      // Do not allow a WidgetLayout to encapsulate an object of type layout
    }

    virtual void AddView(Area * /* baseobject */, unsigned int /* stretchFactor */ = 1, MinorDimensionPosition /* positioning */ = eAbove, MinorDimensionSize /* extend */ = eFull, float /* percentage */ = 100.0f, LayoutPosition /* index */ = NUX_LAYOUT_END)
    {
      // the baseObject is provided via the constructor.
    };

    virtual void AddSpace(unsigned int /* width */, unsigned int /* stretchFactor */ = 0, LayoutPosition /* index */ = NUX_LAYOUT_END)
    {
      // Do not allow a WidgetLayout to encapsulate an object of type layout
    }

    virtual bool CanFocus()
    {
      return false;
    }

    // Begin: Abstract virtual function member(inherited from class Layout) that must be implemented
    virtual long ComputeContentSize()
    {
      return 0;
    }
    virtual void ComputeContentPosition(float /* offsetX */, float /* offsetY */)
    {
    }
    // End: Abstract virtual function member(inherited from class Layout) that must be implemented

  protected:
    Area *Find(long handle);
  };
}

#endif // LAYOUT_H
