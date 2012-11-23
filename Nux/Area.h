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


#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <string>
#include <sigc++/sigc++.h>
#include "NuxCore/InitiallyUnownedObject.h"
#include "NuxGraphics/Events.h"
#include "Utils.h"
#include "WidgetMetrics.h"

namespace nux
{
  class WindowThread;
  class GraphicsEngine;
  class IOpenGLBaseTexture;
  class IOpenGLFrameBufferObject;

#ifdef NUX_GESTURES_SUPPORT
  class GestureEvent;
#endif // NUX_GESTURES_SUPPORT

// In a Horizontal/Vertical Layout, the following enum have the respective meanings:
// eFull: the object has the full height/width of the parent layout(minus margins)
// ePercentage: the object has a height/width that is a percentage of the parent layout(minus margins)
// eFix: the object has a fixed height/width

// Another way to see it
// eFix = The minor dimension of the object will remain what it is. But the positioning in the
//        minor dimension inside the layout can be controlled with MinorDimensionPosition.
// eFull = The minor dimension of the object will take the entire size that is offered by the parent layout.
//         MinorDimensionPosition has no effect if MinorDimensionSize = eFull
// eContent = The minor dimension of the object will be set to 1 by its parent and later on, the minor dimension will be
//         resized larger by the children of the element if necessary.
//


  typedef enum
  {
    MINOR_SIZE_FULL,
    MINOR_SIZE_PERCENTAGE,
    MINOR_SIZE_FIX,
    MINOR_SIZE_MATCHCONTENT,
    eFull = MINOR_SIZE_FULL,                  //!< Deprecated.
    ePercentage = MINOR_SIZE_PERCENTAGE,      //!< Deprecated.
    eFix  = MINOR_SIZE_FIX,                   //!< Deprecated.
    eMatchContent = MINOR_SIZE_MATCHCONTENT,  //!< Deprecated.
  } MinorDimensionSize;

//! Policy for and element position in the minor dimension of a layout.
  typedef enum
  {
    MINOR_POSITION_START,         //!< Place the element at the start of the layout(Hlayout and VLayout)
    MINOR_POSITION_CENTER,        //!< Place the element at the center of the layout(Hlayout and VLayout)
    MINOR_POSITION_END,           //!< Place the element at the end of the layout(Hlayout and VLayout)
    MINOR_POSITION_TOP = MINOR_POSITION_START,   //!< Deprecated.
    MINOR_POSITION_LEFT = MINOR_POSITION_START,  //!< Deprecated.
    MINOR_POSITION_BOTTOM = MINOR_POSITION_END,  //!< Deprecated.
    MINOR_POSITION_RIGHT = MINOR_POSITION_END,   //!< Deprecated.

    eAbove = MINOR_POSITION_START,      //!< Deprecated.
    eBelow = MINOR_POSITION_END,   //!< Deprecated.
    eLeft = MINOR_POSITION_START,      //!< Deprecated.
    eRight = MINOR_POSITION_END,    //!< Deprecated.
    eCenter = MINOR_POSITION_CENTER,  //!< Deprecated.
  } MinorDimensionPosition;

  //! Control how a layout distribute its children position.
  /*!
      Applies to layouts that have more space than their children can use.
      These options control how a layouts places its children inside of itself.
  */
  typedef enum
  {
    MAJOR_POSITION_CENTER,   //!< Stack elements in the center of the layout(for HLayout and VLayout).
    MAJOR_POSITION_START,    //!< Stack elements at the begining of the layout(for HLayout and VLayout).
    MAJOR_POSITION_END,      //!< Stack elements at the end of the layout(for HLayout and VLayout).
    MAJOR_POSITION_SPREAD,   //!< Spread elements evenly inside the layout(for HLayout and VLayout).

    MAJOR_POSITION_TOP = MAJOR_POSITION_START,    //!< Deprecated.
    MAJOR_POSITION_BOTTOM = MAJOR_POSITION_END,   //!< Deprecated.
    MAJOR_POSITION_LEFT = MAJOR_POSITION_START,   //!< Deprecated.
    MAJOR_POSITION_RIGHT = MAJOR_POSITION_END,    //!< Deprecated.


    eStackTop = MAJOR_POSITION_START,         //!< Deprecated.
    eStackBottom = MAJOR_POSITION_END,   //!< Deprecated.
    eStackLeft = MAJOR_POSITION_START,       //!< Deprecated.
    eStackRight = MAJOR_POSITION_END,     //!< Deprecated.
    eStackCenter = MAJOR_POSITION_CENTER,   //!< Deprecated.
    eStackExpand = MAJOR_POSITION_SPREAD,   //!< Deprecated.
  }  LayoutContentDistribution;

  //! For internal use only.
  /*!
      For internal use only.
  */
  typedef enum
  {
    SIZE_EQUAL_WIDTH     = (1L),
    SIZE_EQUAL_HEIGHT    = (1L) << 1,
    SIZE_SMALLER_WIDTH   = (1L) << 2,
    SIZE_SMALLER_HEIGHT  = (1L) << 3,
    SIZE_LARGER_WIDTH    = (1L) << 4,
    SIZE_LARGER_HEIGHT   = (1L) << 5,
    SIZE_FORCE_COMPLY    = (1L) << 6,

    eCompliantWidth     = SIZE_EQUAL_WIDTH,     //!< Deprecated.
    eCompliantHeight    = SIZE_EQUAL_HEIGHT,    //!< Deprecated.
    eSmallerWidth       = SIZE_SMALLER_WIDTH,   //!< Deprecated.
    eSmallerHeight      = SIZE_SMALLER_HEIGHT,  //!< Deprecated.
    eLargerWidth        = SIZE_LARGER_WIDTH,    //!< Deprecated.
    eLargerHeight       = SIZE_LARGER_HEIGHT,   //!< Deprecated.
    eForceComply        = SIZE_FORCE_COMPLY,    //!< Deprecated.
  } SizeCompliance;

  enum KeyNavDirection
  {
    KEY_NAV_NONE,
    KEY_NAV_UP,
    KEY_NAV_DOWN,
    KEY_NAV_RIGHT,
    KEY_NAV_LEFT,
    KEY_NAV_TAB_NEXT,
    KEY_NAV_TAB_PREVIOUS,
    KEY_NAV_ENTER,
  };

  class Layout;
  class View;
  class Area;

  class Area: public InitiallyUnownedObject
  {
    NUX_DECLARE_OBJECT_TYPE(Area, InitiallyUnownedObject);
  public:
    Area(NUX_FILE_LINE_DECL);
    virtual ~Area();

    int GetX() const;
    int GetY() const;
    int GetWidth() const;
    int GetHeight() const;

    void SetX(int x);
    void SetY(int y);
    void SetXY(int x, int y);
    void SetWidth(int w);
    void SetHeight(int h);

    int GetBaseX() const;
    int GetBaseY() const;
    int GetBaseWidth() const;
    int GetBaseHeight() const;
    void SetBaseX(int x);
    void SetBaseY(int y);
    void SetBaseXY(int x, int y);
    void SetBaseWidth(int w);
    void SetBaseHeight(int h);

    //! Set the size of the object.
    /*
        Set the size of the object.
        The size is adjusted to respect the min and max size policy
        \sa SetWidth(), SetHeight(), SetMinimumSize(), SetMaximumSize().
    */
    virtual void SetSize(int w, int h);
    virtual void SetBaseSize(int w, int h);

    virtual void SetMinimumSize(int w, int h);
    virtual void SetMaximumSize(int w, int h);
    virtual void SetMinMaxSize(int w, int h);

    virtual void SetMinimumWidth(int w);
    virtual void SetMaximumWidth(int w);
    virtual void SetMinimumHeight(int h);
    virtual void SetMaximumHeight(int h);

    virtual int GetMinimumWidth() const;
    virtual int GetMaximumWidth() const;
    virtual int GetMinimumHeight() const;
    virtual int GetMaximumHeight() const;

    virtual void ApplyMinWidth();
    virtual void ApplyMinHeight();
    virtual void ApplyMaxWidth();
    virtual void ApplyMaxHeight();

    virtual Size GetMinimumSize() const;
    virtual Size GetMaximumSize() const;

    //! Get the geometry of the object.
    /*!
        @return The Geometry of the object.
        @sa GetBaseWidth(), GetBaseHeight(), GetBaseX(), GetBaseY().
    */
    Geometry const& GetGeometry() const;

    //! Set the geometry of the object.
    /*!
        Set the width, height, and x, y position of the object on the screen.
        @param x Horizontal position.
        @param y Vertical position.
        @param w Width.
        @param h Height.

        \sa SetBaseWidth(), SetBaseHeight(), SetBaseX(), SetBaseY().
    */
    virtual void SetGeometry(int x, int y, int w, int h);

    //! Set the geometry of the object.
    /*!
        This is an overloaded member function, provided for convenience.
        It behaves essentially like SetGeometry(int x, int y, int w, int h).

        @param geo Geometry object.
        \sa SetWidth(), SetHeight(), SetX(), SetY().
    */

    void SetBaseString(std::string const& caption);
    std::string const& GetBaseString() const;

    virtual void SetGeometry(const Geometry& geo);

    //! Deprecated. Use GetToplevel.
    Area* GetToplevel();

    //! Return the root parent of the rendering tree for this area.
    /*!
        The root parent of the rendering tree is either a BaseWindow or the main layout.
        If the object isn't hooked to the rendering tree the function returns NULL.

        @return The root parent of the rendering tree or NULL.
    */
    Area* GetRootParent();

    //! Return the Top level BaseWindow of this area.
    /*!
        @return The top level BaseWindow or NULL.
    */
    Area* GetTopLevelViewWindow();

    //! Return true is this area has a top level parent.
    /*!
        @return True if this area has a top level parent.
    */
    bool HasTopLevelParent();

    //! Return true is area is a child of the given parent in the widget tree.
    /*!
        @param parent Area to test if it is a parent of this area.
        @return True if this area is area is a child of parent in the widget tree.
    */
    bool IsChildOf(Area* parent);

    /*!
        Test if a point is inside the area.

        @param p A 2D point.
        @param event_type The type of mouse event(a parameter of FindAreaUnderMouse).

        @return True if p is located inside the Area.
    */
    bool TestMousePointerInclusion(const Point& mouse_position, NuxEventType event_type);

    /*!
        Test if a point is inside the area and if the area accepts mouse wheel events.

        @param p A 2D point.
        @param event_type The type of mouse event(a parameter of FindAreaUnderMouse).
        @param filter_mouse_wheel_event If the event type is NUX_MOUSE_WHEEL and the mouse is over this area and this
        area does not accept mouse wheel events, then return false.

        @return True if p is located inside the Area.
    */
    bool TestMousePointerInclusionFilterMouseWheel(const Point& mouse_position, NuxEventType event);

    //! Test if the mouse pointer is inside the area.
    /*!
        Return true if the mouse pointer is inside the area.
        
        @return True if the mouse pointer is inside the area.
    */
    bool IsMousePointerInside() const;

    virtual long ComputeContentSize();
    virtual void ComputeContentPosition(float offsetX, float offsetY);

    virtual bool IsArea() const;
    virtual bool IsInputArea() const;
    virtual bool IsView() const;
    virtual bool IsLayout() const;
    virtual bool IsSpaceLayout() const;
    virtual bool IsViewWindow() const;

    Area * GetParentObject() const;

    //! Set visibility of the area
    /*!
        If visible, an area will be drawn. Default: true.
        @param visible if the area is visible to the user
    */
    void SetVisible(bool visible);

    //! Get the visibility of the area
    /*!
        Gets whether the area is visible to the user and will be visible to the user. Default is true.
        @return whether the area is visible
    */
    bool IsVisible();

    //! Deprecated. Use SetInputEventSensitivity.
    void SetSensitive(bool);

    //! Set input event sensitivity of the area.
    /*!
        A insensitive Area will not receive input events.\n
        If the Area has a layout, the event will be passed down to it. Sensitivity only control an area's ability to receive input events(keyboard, mouse, touch).
        An area that is not sensitive will return false in \a TestMousePointerInclusion, \a TestMousePointerInclusionFilterMouseWheel and \a AcceptKeyNavFocus.\n
        Sensitivity does not affect layouts since they do not process events.

        By default, an area is sensitive.

        @param sensitive If the area should receive input events
    */
    void SetInputEventSensitivity(bool sensitive);

    //! Deprecated. Use GetInputEventSensitivity.
    bool IsSensitive() const;

    //! Get input event sensitivity of the area is sensitive
    /*!
        Gets whether the area is sensitive to input events
        @return whether the area is visible
    */
    bool GetInputEventSensitivity() const;

    sigc::signal<void, Area*> ChildFocusChanged; // sends parent + child

    /*!
        This signal is received whether the area receiving or loosing the keyboard focus.
        If the second parameter is true, it means the area is receiving the focus.
        The third parameter of this signal indicates the keyboard action that triggered this area 
        to receive or loose the keyboard focus.
    */
    sigc::signal<void, Area*, bool, KeyNavDirection> key_nav_focus_change;

    /*!
        This signal is received when the area has the key focus and the ENTER key has been pressed.
    */
    sigc::signal<void, Area*> key_nav_focus_activate;

    //! Queue a relayout
    /*!
        Queues a relayout before the next paint cycle. This is safe to call multiple times within a cycle.
    */
    void QueueRelayout();

    //! Get the area scale factor.
    /*!
        The scale factor is used to control the layout of area objects inside HLayout and VLayout.
        \sa HLayout, VLayout.

        @return the Area scale factor.
    */
    virtual unsigned int GetScaleFactor();

    //! Set the area scale factor.
    /*!
        The scale factor is used to control the layout of area objects inside HLayout and VLayout.
        \sa HLayout, VLayout.

        @param the scale factor.
    */
    virtual void SetScaleFactor(unsigned int sf);

    virtual MinorDimensionPosition GetPositioning();
    virtual void SetPositioning(MinorDimensionPosition p);

    virtual MinorDimensionSize GetExtend();
    virtual void SetExtend(MinorDimensionSize ext);

    virtual float GetPercentage();
    virtual void SetPercentage(float f);
    virtual bool IsLayoutDone();
    virtual void SetLayoutDone(bool b);

    void Set2DMatrix(const Matrix4 &mat);
    void Set2DTranslation(float tx, float ty, float tz);
    Matrix4 Get2DMatrix() const;

    Matrix4 Get3DMatrix() const;
    bool Is3DArea() const;

    //! Return the position of this object with regard to the top left corner of the physical window.
    /*!
        Return the position of the Area inside the physical window.
        For the main layout set in WindowThread, The following functions are equivalent:
        \li GetGeometry()
        \li GetRootGeometry()
        \li GetAbsoluteGeometry()
    */
    virtual Geometry GetAbsoluteGeometry() const;

    //! Return the area absolute x coordinate.
    int GetAbsoluteX() const;
    
    //! Return the area absolute y coordinate.
    int GetAbsoluteY() const;
    
    //! Return the area absolute width.
    /*!
        As long as _2d_xform contains only translations, the absolute width is the same as value returned by GetBaseWidth();
    */
    int GetAbsoluteWidth() const;
    
    //! Return the area absolute height.
    /*!
        As long as _2d_xform contains only translations, the absolute height is the same as value returned by GetBaseHeight();
    */
    int GetAbsoluteHeight() const;

    //! Return the position of this object with regard to its top level parent(the main layout or a BaseWindow).
    /*!
        Return the position of the Area inside the physical window.
        For the main layout set in WindowThread or for a BaseWindow, GetRootGeometry() is equivalent to GetGeometry().
    */
    virtual Geometry GetRootGeometry() const;

    //! Return the area root x coordinate.
    int GetRootX() const;

    //! Return the area root y coordinate.
    int GetRootY() const;

    //! Return the area root width.
    /*!
        As long as _2d_xform contains only translations, the root width is the same as value returned by GetBaseWidth();
    */
    int GetRootWidth() const;

    //! Return the area root height.
    /*!
        As long as _2d_xform contains only translations, the root width is the same as value returned by GetBaseWidth();
    */
    int GetRootHeight() const;

    sigc::signal<void, int, int, int, int> OnResize; //!< Signal emitted when an area is resized.
    sigc::signal<void, Area *, bool> OnVisibleChanged;
    sigc::signal<void, Area *, bool> OnSensitiveChanged;

    /*!
        This signal is only meant to inform of a change of size. When receiving this signal don't do anything
        that could change the size of this object. Or you risk creating an infinite loop.
    */
    sigc::signal<void, Area*, Geometry&> geometry_changed;

    /*!
        This signal emitted when the size of the area has changed. It is emitted after geometry_changed.
    */
    sigc::signal<void, Area*, int, int> size_changed;

    /*!
        This signal emitted when the position of the area has changed. It is emitted after geometry_changed.
    */
    sigc::signal<void, Area*, int, int> position_changed;

    /*!
        SetParentObject/UnParentObject are protected API. They are not meant to be used directly by users.
        Users add widgets to layouts and layout have to be attached to a composition for objects to be rendered.
        Setting a parent to and child widget does not mean that when the parent is rendered, the child is also rendered.
        For instance, setting a button to be the child of a check-box means absolutely nothing is terms of rendering.
        The check-box draw function would have to be aware of the existence of the button in order to render it.\n
        A view with a parent cannot be parented to another area for rendering. It has to be un-parented first.\n
        A layout with a parent cannot be parented to another area for rendering. It has to be un-parented first.\n
        In essence only View and Layouts should be calling SetParentObject/UnParentObject.\n
        SetParentObject returns true if it was successful.
        

        @param parent The object that will become the parent of this area.
        @return True if the object is successfully parented. 
    */
    virtual bool SetParentObject(Area *parent);

    //! Un-parent and area
    /*!
        For un-parented areas the following functions are equivalent:
          \li GetGeometry()
          \li GetRootGeometry()
          \li GetAbsoluteGeometry()
    */
    virtual void UnParentObject();

    /*!
        Return the area under the mouse pointer.

        @return The Area under the mouse pointer.
    */
    virtual Area* FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type);


    virtual Area* FindKeyFocusArea(unsigned int key_symbol,
        unsigned long x11_key_code,
        unsigned long special_keys_state);

    /*!
        Mark the path from this area to its top level parent as the one to follow \n
        to get to the area that has the keyboard.
    */
    void SetPathToKeyFocusArea();

    /*!
        Erase the path from this area to the child area that has the keyboard.
    */
    void ResetDownwardPathToKeyFocusArea();

    /*!
        Erase the path that marks this object as part of the keyboard focus chain, \n
        from this area to its top level parent.
    */
    void ResetUpwardPathToKeyFocusArea();

    //! Return True if the the area knows what to do with the key event.
    /*!
        For a View to receive the key_up and key_down signal, it must override this function and return true.
        
        @param even_type Event type is either EVENT_KEY_DOWN or EVENT_KEY_UP.
        @param keysym The key symbol.
        @param characters The character string of the key.

        @return bool True if the View wants to received the key events signals.
    */
    virtual bool InspectKeyEvent(unsigned int event_type,
      unsigned int keysym,
      const char* character);

    virtual Area* KeyNavIteration(KeyNavDirection direction);

    bool HasKeyFocus() const;

    //! Set to True to initiate a layout reconfiguration when the geometry of this widget changes.
    /*!
        When the geometry of an area changes, the new geometry can be recursively propagated to all its 
        parent so a layout reconfiguration is initiated.
        \sa ReconfigureParentLayout()
        \sa on_geometry_change_reconfigure_parent_layout_

        @param reconfigure_parent_layout Set it to True to reconfigure this area parent layouts.
    */
    void SetReconfigureParentLayoutOnGeometryChange(bool reconfigure_parent_layout);

    //! Return True if the the parent layouts of this area should be reconfigured on geometry changed.
    /*!
        @return True if the parent layouts of this area must be reconfigured on a geometry change.
    */
    bool ReconfigureParentLayoutOnGeometryChange();

    //! Enable keyboard event processing.
    /*!
        @param accept_key_event Set it to true if the area accepts keyboard events.
    */
    void SetAcceptKeyboardEvent(bool accept_key_event);

    //! Return true if the Area is interested in keyboard events.
    /*!
        @return True if the area accepts in keyboard events.
    */
    bool AcceptKeyboardEvent() const;

    //! Enable mouse wheel event processing.
    /*!
        @param accept_mouse_wheel_event Set it to true if the area accepts mouse wheel events.
    */
    void SetAcceptMouseWheelEvent(bool accept_mouse_wheel_event);

    //! Return true if the area is accepts mouse wheel events.
    /*!
        @return True if the area accepts mouse wheel events.
    */
    bool AcceptMouseWheelEvent() const;

  protected:
    /*
        This function is reimplemented in Layout as it need to perform some special operations.
        It does nothing for Area and View classes.
    */
    //virtual void RemoveChildObject(smptr(Area));

    /*!
        This signal is only meant to inform that the size is about to change. When overriding this function,
        don't do anything that could change the size of this object. Or you risk creating an infinite loop.
    */
    virtual void GeometryChangePending(bool /* position_about_to_change */, bool /* size_about_to_change */) {}
    
    /*!
        This signal is only meant to inform that the size has changed. When overriding this function,
        don't do anything that could change the size of this object. Or you risk creating an infinite loop.
    */
    virtual void GeometryChanged(bool /* position_has_changed */, bool /* size_has_changed */) {}

    //! Request a Layout recompute after a change of size
    /*
        When an object size changes, it is necessary for its parent structure to initiate a layout
        re computation in order preserve the layout structure defined by the user through the API.
    */
    virtual void RequestBottomUpLayoutComputation(Area *bo_initiator);

    //! Return the absolute geometry starting with a relative geometry passed as argument.
    void InnerGetAbsoluteGeometry(Geometry &geometry);

    //! Return the absolute geometry starting with a relative geometry passed as argument.
    void InnerGetRootGeometry(Geometry &geometry);

    bool on_geometry_change_reconfigure_parent_layout_;

    //! If this variable is true, then this area has the keyboard focus.
    bool has_key_focus_;

    void SetNextObjectToKeyFocusArea(Area*);

    //! Gets the next object in the chain that ends with the area that has the keyboard focus.
    /*!
        Gets the next object in the chain that ends with the area that has the keyboard focus.
        The next object is a child of this area.

        @return An area that is the next object in the chain that leads to the area that has the keyboard focus.
    */
    Area* GetNextObjectToKeyFocusArea();

    //! If this variable is not NULL, then this area is part of the keyboard focus chain.
    Area* next_object_to_key_focus_area_;

    
    /**********************************************/
    /*** Begin support for redirected rendering ***/
    /**********************************************/
public:
    //! Redirect the rendering of this view to a texture.
    /*!
        Redirect the rendering of this view to a texture. \sa BackupTexture().
        @param redirect If true, redirect the rendering of this view to a texture.
    */
    virtual void SetRedirectRenderingToTexture(bool redirect);

    /*!
        @return True if the rendering of this view is done in a texture.
    */
    virtual bool RedirectRenderingToTexture() const;

    //! Return the texture of this View if RedirectRenderingToTexture is enabled.
    /*
        Return the texture of this View if RedirectRenderingToTexture is enabled.
        If RedirectRenderingToTexture() is false, then backup_texture_ is not a valid smart pointer.

        @return the device texture that contains the rendering of this view.
    */
    ObjectPtr<IOpenGLBaseTexture> BackupTexture() const;

    /*!
        The use of this function is a bit arcan but it gives more rendering
        options to redirected areas.
    */
    void SetCopyPreviousFboTexture(bool copy);

    /*!
        Activate/Deactivate the presentation of the redirected texture in the rendering tree.
    */
    void SetPresentRedirectedView(bool present_redirected_view);

    /*!
        @return True if the redirected texture is displayed in the rendering tree.
    */
    bool PresentRedirectedView() const;

protected:
    //! Redirect the rendering of the view to a texture.
    bool redirect_rendering_to_texture_;
    bool update_backup_texture_;
    bool present_redirected_view_;
    //! The texture that holds the rendering of this view.
    ObjectPtr<IOpenGLBaseTexture> backup_texture_;
    ObjectPtr<IOpenGLBaseTexture> backup_depth_texture_;
    ObjectPtr<IOpenGLBaseTexture> background_texture_;
    ObjectPtr<IOpenGLFrameBufferObject> backup_fbo_;
    ObjectPtr<IOpenGLFrameBufferObject> prev_fbo_;
    Geometry prev_viewport_;
    Matrix4 model_view_matrix_;
    Matrix4 perspective_matrix_;
    /*!
        If true, copy the area in the previous fbo texture
        into background_texture_.
    */
    bool copy_previous_fbo_for_background_;

    /*!
        Implemented in nux::View and nux::Layout.
        Report to a parent view with redirect_rendering_to_texture_ set to true that one of its children
        needs to be redrawn.
    */
    virtual void PrepareParentRedirectedView();

    virtual bool HasParentRedirectedView();
    Area* RedirectedAncestor();

    /*!
        Inform this view that one of its children has requested a draw. This view must have its rendering redirected to a texture.
        @param update True if this view is redirected and one of its children has requested a draw.
    */
    virtual void SetUpdateBackupTextureForChildRendering(bool update);
    virtual bool UpdateBackupTextureForChildRendering() const;

    /********************************************/
    /*** End support for redirected rendering ***/
    /********************************************/

#ifdef NUX_GESTURES_SUPPORT
    //! Returns the InputArea hit by the given gesture
    virtual Area* GetInputAreaHitByGesture(const GestureEvent &event);

    //! Returns whether the gesture from the given event is fully inside this area.
    /*!
      For getures from direct devices (e.g. touchscreens), it checks whether all
      touch points lie within this area's boundaries. For gestures from indirect
      devices (e.g. touchpads) it checks whether the gesture's focus point lies
      inside this area.
     */
    bool IsGestureInsideArea(const GestureEvent &event) const;
#endif // NUX_GESTURES_SUPPORT

  private:
    void ReconfigureParentLayout(Area *child = 0);
    void CheckMinSize();
    void CheckMaxSize();


    Geometry                geometry_;        //!< The area geometry.

    //! Define a parent child structure
    /*
        An object of the class Area may have another of the class Layout as Parent.
        An object of the class View may have an object of the class Layout as parent.
        An object of the class Layout may have a parent of the class Layout or View as parent.
        A Area cannot have children(that may change later).
    */
    Area*             parent_area_;

    bool              visible_;       //!< Visible state of the area.
    bool              sensitive_;     //!< Input sensitive state of the area
    bool              view_enabled_;  //!< The enable state of a view.

    std::string             base_string_;     //!< A text string property for this area.

    Size                    min_size_;        //!< A text string property for this area.
    Size                    max_size_;        //!< A text string property for this area.

    // Parameters used in layouts
    unsigned int            scale_factor_;          //!< Factor for element expansion.
    MinorDimensionPosition  minor_axis_position_;   //!< Area position hint
    MinorDimensionSize      minor_axis_size_;       //!< Area dimension hint
    float                   minor_axis_size_scale_; //!< Area size percentage value.
    bool                    layout_done_;           //!< Area layout status flag.


    Matrix4                 _2d_xform;        //!< 2D transformation matrix for this area and its children. Contains only translations.
    Matrix4                 _3d_xform;        //!< 3D transformation matrix for the area in a perspective space.
    bool                    _3d_area;         //!< True if the area is resides in a 3D space.

    std::list<Area*>        _children_list;

    bool                    accept_mouse_wheel_event_;
    bool                    accept_keyboard_event_;

    WindowThread*           window_thread_;

    friend class Layout;
    friend class View;
    friend class WindowCompositor;
    friend class WindowThread;
#if !defined(NUX_MINIMAL)
    friend class HSplitter;
    friend class VSplitter;
#endif
  };
}
#endif // BASEOBJECT_H

