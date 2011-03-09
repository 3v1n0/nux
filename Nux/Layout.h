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


#ifndef LAYOUT_H
#define LAYOUT_H

#include "Focusable.h"

namespace nux
{

#define DEBUG_LAYOUT 0
#define DEBUG_LAYOUT_COMPUTATION 0

  class Layout: public Area
  {
    NUX_DECLARE_OBJECT_TYPE (Layout, Area);
  public:
    Layout (NUX_FILE_LINE_PROTO);
    virtual ~Layout();

    virtual void AddLayout (Layout *, unsigned int stretchFactor = 1, MinorDimensionPosition = eAbove, MinorDimensionSize extend = eFull, float percentage = 100.0f);

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
    */
    virtual void AddView (Area *baseobject, unsigned int stretchFactor = 1, MinorDimensionPosition positioning = eAbove, MinorDimensionSize extend = eFull, float percentage = 100.0f);
    virtual void AddSpace (unsigned int width, unsigned int stretchFactor = 0);

    virtual void Clear();

    virtual unsigned int GetMaxStretchFactor();
    unsigned int GetMinStretchFactor();
    unsigned int GetNumStretchFactor (unsigned int sf);

    int GetContentWidth() const
    {
      return m_contentWidth;
    };
    int GetContentHeight() const
    {
      return m_contentHeight;
    };

    int GetHorizontalInternalMargin() const
    {
      return m_h_in_margin;
    }
    int GetHorizontalExternalMargin() const
    {
      return m_h_out_margin;
    }
    void SetHorizontalInternalMargin (int m)
    {
#if DEBUG_LAYOUT
      return;
#endif
      m_h_in_margin = m < 0 ? 0 : m;
    }
    void SetHorizontalExternalMargin (int m)
    {
#if DEBUG_LAYOUT
      return;
#endif
      m_h_out_margin = m < 0 ? 0 : m;
    }

    int GetVerticalInternalMargin() const
    {
      return m_v_in_margin;
    };
    int GetVerticalExternalMargin() const
    {
      return m_v_out_margin;
    };
    void SetVerticalInternalMargin (int m)
    {
#if DEBUG_LAYOUT
      return;
#endif
      m_v_in_margin = m < 0 ? 0 : m;
    }
    void SetVerticalExternalMargin (int m)
    {
#if DEBUG_LAYOUT
      return;
#endif
      m_v_out_margin = m < 0 ? 0 : m;
    }

  public:

    virtual bool IsLayout() const
    {
      return true;
    }
    virtual bool IsSpaceLayout() const
    {
      return false;
    }

    virtual void GetCompositeList (std::list<Area *> *ViewList)
    {

    }
    virtual void Draw() {}

    void removeParentLayout();
    void DoneRedraw();

    bool SearchInAllSubNodes (Area *bo);
    bool SearchInFirstSubNodes (Area *bo);

    //! Process Event
    /*!
    Propagate event to all element contained in the layout.
    \param ievent
    \param TraverseInfo
    \param ProcessEventInfo
    \return The state of the Process Event.
    */
    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);

    //! Draw Element
    /*!
    Draw all elements inside the layout.
    If force_draw is true then the system requests that all objects redraw themselves completely.
    \param force_draw
    \param TraverseInfo
    \param ProcessEventInfo
    \return The state of the Process Event.
    */
    virtual void ProcessDraw (GraphicsEngine &GfxContext, bool force_draw);

    //! Mark all element in the layout as dirty.
    /*!
        Mark all element in the layout as dirty. This will also mark all sub elements as dirty.
        InputArea element are not marked as dirty (they don't have the flags).
        Emits the signal \i OnQueueDraw.
    */
    virtual void QueueDraw ();
    virtual void NeedRedraw (); //!< deprecated

    //! Return true if a draw has been scheduled for this layout
    /*!
        @return True if a draw has been scheduled for this layout.
    */
    bool IsQueuedForDraw ();

    //! Define how elements are spread out inside the layout.
    /*!
        Typically, a layout stacks it elements from left to right (HLayout) or top to bottom (VLayout).
        When the elements don't uses all the space that is available, the content stacking policy allows
        alternatives ways to position the elements. This does not affect the elements size, only their position
        inside the layout.
        @param stacking_order
    */
    virtual void SetContentDistribution (LayoutContentDistribution stacking_order);
    virtual LayoutContentDistribution GetContentDistribution();

    virtual bool FindWidget (Area *WidgetObject) const;
    virtual bool IsEmpty() const;
    /*
        This function is reimplemented in Layout and View classes  they need to perform some special operations.
        It does nothing for Area classes (this class cannot have children).
    */
    virtual void RemoveChildObject (Area *);

    //! Request a Layout recompute after a change of size
    /*
        When an object size changes, it is necessary for its parent structure to initiate a layout
        re computation in order preserve the layout structure defined by the user through the API.
    */
    virtual void RequestBottomUpLayoutComputation (Area *bo_initiator);

    std::list<Area *>& GetChildren ()
    {
      return _layout_element_list;
    }

    virtual void ChildViewQueuedDraw (View *view);
    virtual void ChildLayoutQueuedDraw (Layout *layout);
    virtual void ChildLayoutChildQueuedDraw (Area *area);

    sigc::signal<void, Layout*> OnQueueDraw;  //!< Signal emitted when a layout is scheduled for a draw.
    sigc::signal<void, Area*>   OnChildQueueDraw;

    virtual void DoSetFocused (bool focused);
    virtual bool DoGetFocused ();
    virtual bool DoCanFocus ();
    virtual void DoActivateFocus ();

    bool FocusFirstChild ();
    bool FocusLastChild ();
    bool FocusNextChild (Area *child);
    bool FocusPreviousChild (Area *child);

    bool HasFocusableEntries ();

    sigc::signal <void, Layout *, Area*> ChildFocusChanged;

    // this should not be public, but has to be because of nux's object setup
    long ProcessFocusEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    bool _has_focus_control;
    void SetFocusControl (bool focus_control);
    bool HasFocusControl ();
    bool _ignore_focus;

  protected:
    bool _queued_draw; //<! The rendering of the layout needs to be refreshed.

    Size m_ContentSize;
    int m_contentWidth;
    int m_contentHeight;

    // Apply only to layout element. This is the width and height computed while sizing the child element.
    int m_fittingWidth;
    int m_fittingHeight;

    int m_h_in_margin;
    int m_h_out_margin;
    int m_v_in_margin;
    int m_v_out_margin;

    std::list<Area *> _layout_element_list;

    NString m_name;

    LayoutContentDistribution m_ContentStacking;

    long SendEventToArea (Area *area, IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
  };


// The Space layout is a layout object that is used to create fixed or resizable empty space.
  class SpaceLayout: public Layout
  {
  public:
    SpaceLayout()
    {
    };

    SpaceLayout (int minWidth, int maxWidth, int minHeight, int maxHeight)
    {
      SetMinimumSize (minWidth, minHeight);
      SetMaximumSize (maxWidth, maxHeight);
    };

    ~SpaceLayout()
    {
    };

    virtual bool FindWidget (Area *WidgetObject) const
    {
      return false;
    }
    virtual bool IsEmpty() const
    {
      return true;
    }

    virtual bool IsSpaceLayout() const
    {
      return true;
    }

    virtual void AddLayout (Layout *, unsigned int stretchFactor = 1, MinorDimensionPosition minor_position = eAbove, MinorDimensionSize minor_size = eFull, float percentage = 100.0f)
    {
      // Do not allow a WidgetLayout to encapsulate an object of type layout
    }

    virtual void AddView (Area *baseobject, unsigned int stretchFactor = 1, MinorDimensionPosition positioning = eAbove, MinorDimensionSize extend = eFull, float percentage = 100.0f)
    {
      // the baseObject is provided via the constructor.
    };

    virtual void AddSpace (unsigned int width, unsigned int stretchFactor = 0)
    {
      // Do not allow a WidgetLayout to encapsulate an object of type layout
    }

    virtual bool CanFocus ()
    {
      return false;
    }

    // Begin: Abstract virtual function member (inherited from class Layout) that must be implemented
    virtual long ComputeLayout2()
    {
      return 0;
    }
    virtual void ComputePosition2 (float offsetX, float offsetY)
    {
    }
    // End: Abstract virtual function member (inherited from class Layout) that must be implemented

  protected:
    Area *Find (long handle);
  };

}

#endif // LAYOUT_H
