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


#ifndef ABSTRACTOBJECTBASE_H
#define ABSTRACTOBJECTBASE_H

#include "Nux/Nux.h"

namespace nux
{

  class Layout;

  enum eControlType
  {
    eNumericValuator,
    eSpinBox,
    eVector3Box,
    eBooleanBox,
    eStaticText,
    ePopupBox
  };

  class View: public InputArea //Area
  {
    NUX_DECLARE_OBJECT_TYPE (View, InputArea);
  public:
    View (NUX_FILE_LINE_DECL);
    virtual ~View();
    //virtual RemoveView(View *ic);

  public:

    long BaseProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);

    virtual long ComputeChildLayout();
    virtual void PositionChildLayout (float offsetX, float offsetY);

    // NUXTODO: Find better name
    virtual long ComputeLayout2();

    // NUXTODO: Find better name
    virtual void ComputePosition2 (float offsetX, float offsetY);

    virtual void PreLayoutManagement();
    virtual long PostLayoutManagement (long LayoutResult);
    virtual void PreResizeGeometry();
    virtual void PostResizeGeometry();

    // NUXTODO: Find better name
    virtual long PostProcessEvent2 (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);

    virtual bool IsLayout() const
    {
      return false;
    }
    virtual bool IsSpaceLayout() const
    {
      return false;
    }
    virtual bool IsArea() const
    {
      return false;
    }
    virtual bool IsView() const
    {
      return true;
    }

    void SetUsingStyleDrawing (bool b)
    {
      m_UseStyleDrawing = b;
    };
    bool IsUsingStyleDrawing() const
    {
      return m_UseStyleDrawing;
    };

    /*!
        Activate the View. The view cannot receive events. Its rendering is grayed (NUXTODO).
    */
    void DeactivateView ();

    /*!
        Activate the View. It can process them. The rendering of the View is normal.
    */
    void ActivateView ();

    /*!
        @return True if the view is active.
    */
    bool IsViewActive () const;

  public:
    virtual void ProcessDraw (GraphicsEngine &GfxContext, bool force_draw);
    //! Causes a redraw. The widget parameter _need_redraw is set to true. The widget Draw(), DrawContent() and PostDraw() are called.
    /*!
        Emits the signal \i OnQueueDraw.
    */
    virtual void QueueDraw ();
    virtual void NeedRedraw (); //!< Deprecated. Use QueueDraw.

    //! Causes a soft redraw. The widget parameter _need_redraw is set to false. The widget DrawContent() and PostDraw() are called.
    virtual void NeedSoftRedraw();
    virtual bool IsRedrawNeeded();
    virtual void DoneRedraw();

    virtual void OverlayDrawing (GraphicsEngine &GfxContext) {}

    //Layout Bridge

    bool SearchInAllSubNodes (Area *bo);
    bool SearchInFirstSubNodes (Area *bo);

    //! Set Geometry
    /*
        Set the Geometry of the View and the geometry of the Default Background Area.
        For simple interface control UI classes (RGBValuator...), this is enough.
        For others, they have to overwrite the function and do the appropriate computations
        for their component.
    */
    virtual void SetGeometry (const Geometry &geo);

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
    virtual Color GetTextColor();

    void SetFont(ObjectPtr<FontTexture> font);
    ObjectPtr<FontTexture> GetFont();

    sigc::signal<void, View*> OnQueueDraw;  //!< Signal emitted when a view is scheduled for a draw.

  protected:
    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo) = 0;
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw) = 0;
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);

    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

    Color m_TextColor;
    ObjectPtr<FontTexture> _font;

    //! Get the default layout of this view.
    /*!
        Get the default layout of this view.
        @return The default layout of this view.
    */
    virtual Layout* GetLayout();
    virtual Layout* GetCompositionLayout();

    //! Set the default layout for this view.
    /*!
        Set the default layout for this view.
        @param layout A Layout object.
    */
    virtual bool SetLayout (Layout *layout);
    virtual bool SetCompositionLayout (Layout *layout);

    void RemoveLayout();
    void RemoveCompositionLayout();

    /*!
        Accessed inside ContentDraw () to help determine if some parts needs to be rendered.
        Do not use it elsewhere.
        @return True if Draw () was called before ContentDraw ().
        
    */
    bool IsFullRedraw() const;

    virtual void GeometryChangePending ();
    virtual void GeometryChanged ();

    Layout *m_CompositionLayout;

    bool _need_redraw; //<! The rendering of the view needs to be refreshed.

    bool _full_redraw; //<! True if Draw is called before ContentDraw. It is read-only and can be accessed by calling IsFullRedraw();

    bool _is_active; //!< True if the view is enabled (it can receive events and process them).

  private:
    bool m_UseStyleDrawing;
    bool _is_view_active;

    friend class Layout;
    friend class Area;
    friend class LayeredLayout;
  };

}

#endif // ABSTRACTOBJECTBASE_H
