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


#ifndef ABSTRACTOBJECTBASE_H
#define ABSTRACTOBJECTBASE_H

#include "Nux.h"
#include "NuxCore/Property.h"

#define NeedRedraw QueueDraw

namespace nux
{
  class Layout;
#ifdef NUX_GESTURES_SUPPORT
  class GestureEvent;
#endif

  class View: public InputArea
  {
    NUX_DECLARE_OBJECT_TYPE(View, InputArea);
  public:
    View(NUX_FILE_LINE_DECL);
    virtual ~View();

  public:

    /*
        If ComputeContentSize is called while outside of the layout process (\sa IsInsideLayoutCycle) then the parents of 
        this object maybe added to the layout queue if this view size changes. \sa Area::ReconfigureParentLayout.
    */
    virtual long ComputeContentSize();
    virtual void ComputeContentPosition(float offsetX, float offsetY);

    //! Enable a View.
    /*!
        Enable the view. The view cannot receive events. As for the rendering, each view handle 
        its own rendering while is enabled state.
    */
    virtual void EnableView();

    //! Disable a View.
    /*!
        Disable the view. The view cannot receive input events(keyboard, mouse, touch). As for the rendering, each view handle 
        its own rendering while is disabled state.
    */
    virtual void DisableView();

    //! Set the enable state of the view.
    /*!
        Set the enable state of the view.

        @param enable. The state of the view to be set.
    */
    virtual void SetEnableView(bool enable);

    //! Gets the enable state of the View.
    /*!
        @return True if the view is active.
    */
    bool IsViewEnabled() const;

  public:
    virtual void ProcessDraw(GraphicsEngine &graphics_engine, bool force_draw);
    //! Causes a redraw. The widget parameter draw_cmd_queued_ is set to true. The widget Draw() and DrawContent() are called.
    /*!
        Emits the signal \i queue_draw.
    */
    virtual void QueueDraw();

    //! Causes a soft redraw. The widget parameter draw_cmd_queued_ is set to false. The widget DrawContent() is called.
    virtual void NeedSoftRedraw();
    virtual bool IsRedrawNeeded();
    virtual void DoneRedraw();

    virtual void OverlayDrawing(GraphicsEngine & /* graphics_engine */) {}

    //Layout Bridge

    bool SearchInAllSubNodes(Area *bo);
    bool SearchInFirstSubNodes(Area *bo);

/*    void SetGeometry(int x, int y, int w, int h);*/

    //! Set Geometry
    /*
        Set the Geometry of the View and the geometry of the Default Background Area.
        For simple interface control UI classes(RGBValuator...), this is enough.
        For others, they have to overwrite the function and do the appropriate computations
        for their component.
    */
    virtual void SetGeometry(const Geometry &geo);

    //! Return true if this object can break the layout.
    /*
        Return true if this object can break the layout, meaning, the layout can be done on the composition layout only without
        recomputing the whole window layout.
    */
    virtual bool CanBreakLayout()
    {
      return false;
    }

    virtual void SetTextColor(const Color &color);
    virtual Color GetTextColor() const;

    //! Get the default layout of this view.
    /*!
        Get the default layout of this view.
        @return The default layout of this view.
    */
    virtual Layout* GetLayout();

    //! Set the default layout for this view.
    /*!
        Set the default layout for this view. Assigns a scale factor of 1 to the layout.

        @param layout A Layout object.
    */
    virtual bool SetLayout(Layout *layout);

    sigc::signal<void, View*, Layout*> LayoutAdded;
    sigc::signal<void, View*, Layout*> LayoutRemoved;

    void SetFont(ObjectPtr<FontTexture> font);
    ObjectPtr<FontTexture> GetFont();

    sigc::signal<void, View*> queue_draw;       //!< Signal emitted when a view is scheduled for a draw.
    sigc::signal<void, Area*> child_queue_draw; //!< Signal emitted when a child of this view is scheduled for a draw.

    virtual Area* KeyNavIteration(KeyNavDirection direction);
    virtual bool AcceptKeyNavFocus();

    virtual Area* FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type);

    virtual Area* FindKeyFocusArea(unsigned int key_symbol,
      unsigned long x11_key_code,
      unsigned long special_keys_state);

#ifdef NUX_GESTURES_SUPPORT
    virtual Area* GetInputAreaHitByGesture(const nux::GestureEvent &event);
#endif

  protected:
    virtual void ChildViewQueuedDraw(Area* area);

    void BeginBackupTextureRendering(GraphicsEngine& graphics_engine, bool force_draw);

    void EndBackupTextureRendering(GraphicsEngine& graphics_engine, bool force_draw);

    void OnChildFocusChanged(/*Area *parent,*/ Area *child);
    sigc::connection _on_focus_changed_handler;

    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw) = 0;
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void PreLayoutManagement();
    virtual long PostLayoutManagement(long LayoutResult);
    virtual void PreResizeGeometry();
    virtual void PostResizeGeometry();


    void InitializeWidgets();
    void InitializeLayout();

    Color m_TextColor;

    //! Deprecated. Use GetLayout();
    virtual Layout* GetCompositionLayout();

    //! Deprecated. Use SetLayout();
    virtual bool SetCompositionLayout(Layout *layout);

    void RemoveLayout();
    void RemoveCompositionLayout();

    /*!
        Accessed inside ContentDraw() to help determine if some parts needs to be rendered.
        Do not use it elsewhere.
        @return True if Draw() was called before ContentDraw().

    */
    bool IsFullRedraw() const;

    virtual void GeometryChangePending(bool position_about_to_change, bool size_about_to_change);
    virtual void GeometryChanged(bool position_has_changed, bool size_has_changed);

    Layout *view_layout_;

    bool draw_cmd_queued_; //<! The rendering of the view needs to be refreshed.
    bool child_draw_cmd_queued_; //<! A child of this view has requested a draw.

    bool full_view_draw_cmd_; //<! True if Draw is called before ContentDraw. It is read-only and can be accessed by calling IsFullRedraw();

  private:
    ObjectPtr<FontTexture> _font;

    friend class WindowCompositor;
    friend class Layout;
    friend class Area;
    friend class LayeredLayout;
    friend class Canvas;
    friend class VSplitter;
    friend class HSplitter;
  };
}

#endif // ABSTRACTOBJECTBASE_H
