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


#ifndef LAYOUT_H
#define LAYOUT_H

NAMESPACE_BEGIN_GUI

#define DEBUG_LAYOUT 0
#define DEBUG_LAYOUT_COMPUTATION 0

class Layout: public BaseObject
{
    DECLARE_OBJECT_TYPE(Layout, BaseObject);
public:
    Layout();
    virtual ~Layout();

    virtual void AddLayout(smptr(Layout), unsigned int stretchFactor = 1, eMinorPosition = eAbove, eMinorSize extend = eFull, float percentage = 100.0f);

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

        The extend parameter controls how much size the baseobject minor dimension gets from the layout minor dimension. See eMinorSize.

        /param baseobject The object that is being added.
        /param stretchFactor This value controls how the layout object share space between its children baseobject.
        /param positioning Controls how the layout position the object.
        /param extend Controls the object minor dimension size.
        /param percentage Controls the object minor dimension size in percentage of the layout minor dimension size.
    */
    virtual void AddActiveInterfaceObject(smptr(BaseObject) baseobject, unsigned int stretchFactor = 1, eMinorPosition positioning = eAbove, eMinorSize extend = eFull, float percentage = 100.0f);
    virtual void AddSpace(unsigned int width, unsigned int stretchFactor = 0);

    virtual void Clear();

    virtual unsigned int GetMaxStretchFactor();
    unsigned int GetMinStretchFactor();
    unsigned int GetNumStretchFactor(unsigned int sf);

    int GetContentWidth() const {return m_contentWidth;};
    int GetContentHeight() const {return m_contentHeight;};

    int GetHorizontalInternalMargin() const { return m_h_in_margin;}
    int GetHorizontalExternalMargin() const { return m_h_out_margin;}
    void SetHorizontalInternalMargin(int m)
    {
#if DEBUG_LAYOUT
        return;
#endif
        m_h_in_margin = m<0? 0 : m;
    }
    void SetHorizontalExternalMargin(int m) 
    {
#if DEBUG_LAYOUT
        return;
#endif
        m_h_out_margin = m<0? 0 : m;
    }

    int GetVerticalInternalMargin() const { return m_v_in_margin;};
    int GetVerticalExternalMargin() const { return m_v_out_margin;};
    void SetVerticalInternalMargin(int m)
    {
#if DEBUG_LAYOUT
        return;
#endif
        m_v_in_margin = m<0? 0 : m;
    }
    void SetVerticalExternalMargin(int m)
    {
#if DEBUG_LAYOUT
        return;
#endif
        m_v_out_margin = m<0? 0 : m;
    }

public:
    virtual int GetBaseWidth() const = 0;
    virtual void SetBaseWidth(int w) = 0;
    virtual int GetBaseHeight() const = 0;
    virtual void SetBaseHeight(int h) = 0;

    virtual int GetBaseX() const = 0;
    virtual void SetBaseX(int x) = 0;
    virtual int GetBaseY() const = 0;
    virtual void SetBaseY(int y) = 0;

    virtual Size GetMaximumSize() = 0;
    virtual Size GetMinimumSize() = 0;

    virtual void ApplyMinWidth() = 0;
    virtual void ApplyMinHeight() = 0;
    virtual void ApplyMaxWidth() = 0;
    virtual void ApplyMaxHeight() = 0;


    virtual bool IsLayout() const {return true;}
    virtual bool IsSpaceLayout() const {return false;}
    
    virtual void GetCompositeList(std::list<smptr(BaseObject)> *InterfaceControlList)
    {

    }
    virtual void Draw() {}

    void removeParentLayout();
    void DoneRedraw();

    bool SearchInAllSubNodes(smptr(BaseObject) bo);
    bool SearchInFirstSubNodes(smptr(BaseObject) bo);

    //! Process Event
    /*!
    Propagate event to all element contained in the layout.
    \param ievent
    \param TraverseInfo
    \param ProcessEventInfo
    \return The state of the Process Event.
    */
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);

    //! Draw Element
    /*!
    Draw all elements inside the layout. 
    If force_draw is true then the system requests that all objects redraw themselves completely.
    \param force_draw
    \param TraverseInfo
    \param ProcessEventInfo
    \return The state of the Process Event.
    */
    virtual void ProcessDraw(GraphicsContext& GfxContext, bool force_draw);

    //! Mark all element in the layout as dirty.
    /*!
        Mark all element in the layout as dirty. This will also mark all sub elements as dirty.
        BaseArea element are not marked as dirty (they don't have the flags).
    */
    virtual void NeedRedraw();

    //! Define how elements are spread out inside the layout.
    /*!
        Typically, a layout stacks it elements from left to right (HLayout) or top to bottom (VLayout).
        When the elements don't uses all the space that is available, the content stacking policy allows 
        alternatives ways to position the elements. This does not affect the elements size, only their position
        inside the layout.
        @param stacking_order 
    */
    virtual void SetContentStacking(eStacking stacking_order);
    virtual eStacking GetContentStacking();

    virtual bool FindWidget(smptr(BaseObject) WidgetObject) const;
    virtual bool IsEmpty() const;
    /* 
        This function is reimplemented in Layout and ActiveInterfaceObject classes  they need to perform some special operations.
        It does nothing for BaseObject classes (this class cannot have children).
    */
    virtual void RemoveChildObject(smptr(BaseObject));

    //! Request a Layout recompute after a change of size
    /*
        When an object size changes, it is necessary for its parent structure to initiate a layout
        re computation in order preserve the layout structure defined by the user through the API.
    */
    virtual void RequestBottomUpLayoutComputation(smptr(BaseObject) bo_initiator);

    virtual void SetApplication(WindowThread* Application);

    //! Set dirty flag
    void SetDirty(bool b) { m_bIsDirty = b; }
    //! Return true if the layout is dirty. 
    bool IsDirty() const { return m_bIsDirty; }

    //! Set draw dirty flag
    void SetDrawDirty(bool b) { m_bIsDrawDirty = b; }
    //! Return true if the children of the layout need to be redrawn. 
    bool IsDrawDirty() const { return m_bIsDrawDirty; }

    void Translate(int x, int y);

protected:
    bool m_bIsDirty;
    bool m_bIsDrawDirty;

protected:
    Matrix4x4<float> m_Matrix;

    Size m_ContentSize;
    int m_contentWidth;
    int m_contentHeight;

    // Apply only to layout element. This is the width and height computed while sizing the child element.
    int m_fittingWidth;
    int m_fittingHeight;

    smptr(BaseObject) m_compositeIC;

    int m_h_in_margin;
    int m_h_out_margin;
    int m_v_in_margin;
    int m_v_out_margin;

    std::list<smptr(BaseObject)> m_LayoutElementList;

    NString m_name;

    eStacking m_ContentStacking;
};


// The Space layout is a layout object that is used to create fixed or resizable empty space.
class SpaceLayout: public Layout
{
public: 
    SpaceLayout()
    {
    };

    SpaceLayout(int minWidth, int maxWidth, int minHeight, int maxHeight)
    {
        SetMinimumSize(minWidth, minHeight);
        SetMaximumSize(maxWidth, maxHeight);
    };

    ~SpaceLayout()
    {
    };

    virtual bool FindWidget(smptr(BaseObject) WidgetObject) const {return false;}
    virtual bool IsEmpty() const {return true;}
    virtual bool IsSpaceLayout() const {return true;}

    virtual void AddLayout(smptr(Layout), unsigned int stretchFactor = 1, eMinorPosition minor_position = eAbove, eMinorSize minor_size = eFull, float percentage = 100.0f)
    {
        // Do not allow a WidgetLayout to encapsulate an object of type layout
    }

    virtual void AddActiveInterfaceObject(smptr(BaseObject) baseobject, unsigned int stretchFactor = 1, eMinorPosition positioning = eAbove, eMinorSize extend = eFull, float percentage = 100.0f)
    {
        // the baseObject is provided via the constructor.
    };
 
    virtual void AddSpace(unsigned int width, unsigned int stretchFactor = 0)
    {
        // Do not allow a WidgetLayout to encapsulate an object of type layout
    }

    // Begin: Abstract virtual function member (inherited from class Layout) that must be implemented
    virtual long ComputeLayout2() 
    {
        return 0;
    }
    virtual void ComputePosition2(float offsetX, float offsetY)
    {
    }

    virtual int GetBaseWidth() const {return BaseObject::GetBaseWidth();}
    virtual void SetBaseWidth(int w) {BaseObject::SetBaseWidth(w);}

    virtual int GetBaseHeight() const {return BaseObject::GetBaseHeight();}
    virtual void SetBaseHeight(int h) {BaseObject::SetBaseHeight(h);}

    virtual int GetBaseX() const {return BaseObject::GetBaseX();}
    virtual void SetBaseX(int x) {BaseObject::SetBaseX(x);}
    virtual int GetBaseY() const {return BaseObject::GetBaseY();}
    virtual void SetBaseY(int y) {BaseObject::SetBaseY(y);}

    virtual Size GetMaximumSize() {return BaseObject::GetMaximumSize();}
    virtual Size GetMinimumSize() {return BaseObject::GetMinimumSize();}

    virtual void ApplyMinWidth() {BaseObject::ApplyMinWidth();}
    virtual void ApplyMinHeight() {BaseObject::ApplyMinHeight();}
    virtual void ApplyMaxWidth() {BaseObject::ApplyMaxWidth();}
    virtual void ApplyMaxHeight() {BaseObject::ApplyMaxHeight();}
    // End: Abstract virtual function member (inherited from class Layout) that must be implemented

protected:
    smptr(BaseObject) Find(long handle);
};

NAMESPACE_END_GUI

#endif // LAYOUT_H
