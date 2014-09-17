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


#include "Nux.h"
#include "Area.h"
#include "NuxGraphics/GraphicsEngine.h"
#include "Layout.h"
#include "BaseWindow.h"
#if !defined(NUX_MINIMAL)
#  include "VSplitter.h"
#  include "HSplitter.h"
#  include "MenuPage.h"
#endif

#ifdef NUX_GESTURES_SUPPORT
#include "NuxGraphics/GestureEvent.h"
#endif

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(Area);

  Area::Area(NUX_FILE_LINE_DECL)
    : InitiallyUnownedObject(NUX_FILE_LINE_PARAM)
    , redirect_rendering_to_texture_(false)
    , update_backup_texture_(false)
    , present_redirected_view_(true)
    , copy_previous_fbo_for_background_(true)
    , geometry_(0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT)
    , min_size_(AREA_MIN_WIDTH, AREA_MIN_HEIGHT)
    , max_size_(AREA_MAX_WIDTH, AREA_MAX_HEIGHT)
    , layout_done_(true)
  {
    window_thread_ = GetWindowThread();
    visible_ = true;
    view_enabled_ = true;
    parent_area_ = NULL;

    next_object_to_key_focus_area_ = NULL;
    has_key_focus_ = false;

    scale_factor_ = 1;
    sensitive_ = true;

    on_geometry_change_reconfigure_parent_layout_ = true;
    accept_mouse_wheel_event_ = false;
    accept_keyboard_event_ = false;

    _2d_xform.Identity();
    _3d_xform.Identity();
    _3d_area = false;
  }


  Area::~Area()
  {
    // It is possible that the window thread has been deleted before the area
    // itself, so check prior to calling RemoveObjectFromLayoutQueue.
    WindowThread* wt = GetWindowThread();
    if (wt)
    {
      // It is possible that the object is in the refresh list. Remove it here
      // before it is deleted.
      wt->RemoveObjectFromLayoutQueue(this);
    }
    ResetDownwardPathToKeyFocusArea();
    ResetUpwardPathToKeyFocusArea();
  }

  std::string const& Area::GetBaseString() const
  {
    return base_string_;
  }

  void Area::SetBaseString(std::string const& caption)
  {
    base_string_ = caption;
  }

  void Area::CheckMinSize()
  {
    int w = min_size_.width;
    w = Max<int>(AREA_MIN_WIDTH, w);
    int h = min_size_.height;
    h = Max<int>(AREA_MIN_HEIGHT, h);

    min_size_ = Size(w, h);

    if (min_size_.width > max_size_.width)
    {
      max_size_.width = min_size_.width;
    }

    if (min_size_.height > max_size_.height)
    {
      max_size_.height = min_size_.height;
    }

    if (geometry_.width < min_size_.width)
    {
      geometry_.width = min_size_.width;
    }

    if (geometry_.height < min_size_.height )
    {
      geometry_.height = min_size_.height;
    }
  }

  void Area::CheckMaxSize()
  {
    int w = max_size_.width;
    w = w <= AREA_MAX_WIDTH ? w : AREA_MAX_WIDTH;
    int h = max_size_.height;
    h = h <= AREA_MAX_HEIGHT ? h: AREA_MAX_HEIGHT;

    max_size_ = Size(w, h);

    if (min_size_.width > max_size_.width)
    {
      min_size_.width = max_size_.width;
    }

    if (min_size_.height > max_size_.height)
    {
      min_size_.height = max_size_.height;
    }

    if (geometry_.width > max_size_.width)
    {
      geometry_.width = max_size_.width;
    }

    if (geometry_.height > max_size_.height)
    {
      geometry_.height = max_size_.height;
    }
  }

  void Area::SetMinimumSize(int w, int h)
  {
    nuxAssert(w >= 0);
    nuxAssert(h >= 0);
    min_size_ = Size(w, h);

    CheckMinSize();

    ReconfigureParentLayout();
  }

  void Area::SetMaximumSize(int w, int h)
  {
    nuxAssert(w >= 0);
    nuxAssert(h >= 0);
    max_size_ = Size(w, h);

    CheckMaxSize();

    ReconfigureParentLayout();
  }

  void Area::SetMinMaxSize(int w, int h)
  {
    nuxAssert(w >= 0);
    nuxAssert(h >= 0);
    SetMinimumSize(w, h);
    SetMaximumSize(w, h);

    //ReconfigureParentLayout();
  }

  void Area::ApplyMinWidth()
  {
    geometry_.width = min_size_.width;

    ReconfigureParentLayout();
  }

  void Area::ApplyMinHeight()
  {
    geometry_.height = min_size_.height;

    ReconfigureParentLayout();
  }

  void Area::ApplyMaxWidth()
  {
    geometry_.width = max_size_.width;

    ReconfigureParentLayout();
  }

  void Area::ApplyMaxHeight()
  {
    geometry_.height = max_size_.height;

    ReconfigureParentLayout();
  }

  Size Area::GetMinimumSize() const
  {
    return min_size_;
  }

  Size Area::GetMaximumSize() const
  {
    return max_size_;
  }

  void Area::SetMinimumWidth(int w)
  {
    nuxAssert(w >= 0);
    min_size_.width = w;
    CheckMinSize();
    ReconfigureParentLayout();
  }

  void Area::SetMaximumWidth(int w)
  {
    nuxAssert(w >= 0);
    max_size_.width = w;
    CheckMaxSize();
    ReconfigureParentLayout();
  }

  void Area::SetMinimumHeight(int h)
  {
    nuxAssert(h >= 0);
    min_size_.height = h;
    CheckMinSize();
    ReconfigureParentLayout();
  }

  void Area::SetMaximumHeight(int h)
  {
    nuxAssert(h >= 0);
    max_size_.height = h;
    CheckMaxSize();
    ReconfigureParentLayout();
  }

  int Area::GetMinimumWidth() const
  {
    return min_size_.width;
  }

  int Area::GetMaximumWidth() const
  {
    return max_size_.width;
  }

  int Area::GetMinimumHeight() const
  {
    return min_size_.height;
  }

  int Area::GetMaximumHeight() const
  {
    return max_size_.height;
  }

  unsigned int Area::GetScaleFactor()
  {
    return scale_factor_;
  }

  void Area::SetScaleFactor(unsigned int sf)
  {
    // re implemented by Layout
    scale_factor_ = sf;
  }

  bool Area::SetParentObject(Area *parent)
  {
    if (parent == 0)
    {
      nuxAssertMsg(0, "[Area::SetParentObject] Invalid parent obejct.");
      return false;
    }

    if (parent_area_ && (parent_area_ != parent))
    {
      nuxAssertMsg(0, "[Area::SetParentObject] Object already has a parent. You must UnParent the object before you can parenting again.");
      return false;
    }

    if (parent_area_)
    {
      // Already parented to the same area. Return.
      return true;
    }

    parent_area_ = parent;
    Reference();

    return true;
  }

  void Area::UnParentObject()
  {
    // If the area is on the path to the key focus area, or has focus, reset it.
    if (has_key_focus_ || next_object_to_key_focus_area_)
    {
      GetWindowThread()->GetWindowCompositor().SetKeyFocusArea(NULL, KEY_NAV_NONE);
    }

    if (parent_area_)
    {
      parent_area_ = 0;
      UnReference();
    }
  }

  Area *Area::GetParentObject() const
  {
    return parent_area_;
  }

  int Area::GetX() const
  {
    return geometry_.x;
  }

  int Area::GetBaseX() const
  {
    return GetX();
  }

  int Area::GetY() const
  {
    return geometry_.y;
  }

  int Area::GetBaseY() const
  {
    return GetY();
  }

  int Area::GetWidth() const
  {
    return geometry_.width;
  }

  int Area::GetBaseWidth() const
  {
    return GetWidth();
  }

  int Area::GetHeight() const
  {
    return geometry_.height;
  }

  int Area::GetBaseHeight() const
  {
    return GetHeight();
  }

  void Area::SetGeometry(int x, int y, int w, int h)
  {
    h = nux::Clamp<int> (h, min_size_.height, max_size_.height);
    w = nux::Clamp<int> (w, min_size_.width, max_size_.width);

    bool detected_size_change = false;
    bool detected_position_change = false;

    if (geometry_.x != x || geometry_.y != y)
      detected_position_change = true;

    if (geometry_.width != w || geometry_.height != h)
      detected_size_change = true;

    if (!detected_position_change && !detected_size_change)
      return;

    GeometryChangePending(detected_position_change, detected_size_change);
    geometry_.Set(x, y, w, h);
    ReconfigureParentLayout();
    GeometryChanged(detected_position_change, detected_size_change);

    geometry_changed.emit(this, geometry_);

    if (detected_position_change)
      position_changed.emit(this, x, y);

    if (detected_size_change)
      size_changed.emit(this, w, h);
  }

  void Area::SetGeometry(const Geometry &geo)
  {
    SetGeometry(geo.x, geo.y, geo.width, geo.height);
  }

  Geometry const& Area::GetGeometry() const
  {
    return geometry_;
  }

  void Area::SetX(int x)
  {
    SetGeometry(x, geometry_.y, geometry_.width, geometry_.height);
  }

  void Area::SetY    (int y)
  {
    SetGeometry(geometry_.x, y, geometry_.width, geometry_.height);
  }

  void Area::SetBaseX(int x)
  {
    SetX(x);
  }

  void Area::SetBaseY(int y)
  {
    SetY(y);
  }

  void Area::SetXY(int x, int y)
  {
    SetGeometry(x, y, geometry_.width, geometry_.height);
  }

  void Area::SetBaseXY(int x, int y)
  {
    SetXY(x, y);
  }

  void Area::SetSize(int w, int h)
  {
    SetGeometry(geometry_.x, geometry_.y, w, h);
  }

  void Area::SetBaseSize(int w, int h)
  {
    SetSize(w, h);
  }

  void Area::SetWidth(int w)
  {
    SetGeometry(geometry_.x, geometry_.y, w, geometry_.height);
  }

  void Area::SetBaseWidth(int w)
  {
    SetWidth(w);
  }

  void Area::SetHeight(int h)
  {
    SetGeometry(geometry_.x, geometry_.y, geometry_.width, h);
  }

  void Area::SetBaseHeight(int h)
  {
    SetHeight(h);
  }

  long Area::ComputeContentSize()
  {
    return (eCompliantWidth | eCompliantHeight);
  }

  void Area::ComputeContentPosition(float /* offsetX */, float /* offsetY */)
  {

  }

  void Area::SetReconfigureParentLayoutOnGeometryChange(bool reconfigure_parent_layout)
  {
    on_geometry_change_reconfigure_parent_layout_ = reconfigure_parent_layout;
  }
  
  bool Area::ReconfigureParentLayoutOnGeometryChange()
  {
    return on_geometry_change_reconfigure_parent_layout_;
  }

  void Area::ReconfigureParentLayout(Area *child)
  {
    if (on_geometry_change_reconfigure_parent_layout_ == false)
      return;

    if (window_thread_ && window_thread_->IsComputingLayout())
    {
      // there is no need to do the following while we are already computing the layout.
      // If we do, we will end up in an infinite loop.
      return;
    }

    if (this->Type().IsDerivedFromType(View::StaticObjectType))
    {
      // The object that is being resized is a View object and it has a parent.
      if (this->OwnsTheReference() == false && this->GetParentObject())
      {
        // Only reference parented areas.
        this->Reference();
      }

      View *ic = static_cast<View *>(this);

      if (ic->CanBreakLayout())
      {
#if !defined(NUX_MINIMAL)
        if ((child != 0) && (ic->Type().IsObjectType(VSplitter::StaticObjectType) || ic->Type().IsObjectType(HSplitter::StaticObjectType)))
        {
          // If this element is a Splitter, then we submit its child to the refresh list. We don't want to submit the
          // splitter because this will cause a redraw of all parts of the splitter(costly and unnecessary).
          window_thread_->QueueObjectLayout(child);
        }
        else
#endif
        {
          window_thread_->QueueObjectLayout(ic);
        }
      }
      else if (ic->parent_area_)
      {
        ic->parent_area_->ReconfigureParentLayout(this);
      }
      else
      {
        window_thread_->QueueObjectLayout(ic);
      }
    }
    else if (this->Type().IsDerivedFromType(Layout::StaticObjectType))
    {
      // The object that is being resized is a View object and it has a parent.
      if (this->OwnsTheReference() == false && this->GetParentObject())
      {
        // Only reference parented areas.
        this->Reference();
      }

      Layout *layout = static_cast<Layout *>(this);

      if (layout->parent_area_)
      {
        if (layout->parent_area_->Type().IsDerivedFromType(View::StaticObjectType))
        {
          View *ic = (View *) (layout->parent_area_);

          if (ic->CanBreakLayout())
          {
#if !defined(NUX_MINIMAL)
            if ((child != 0) &&
                (ic->Type().IsObjectType(VSplitter::StaticObjectType) ||
                 ic->Type().IsObjectType(HSplitter::StaticObjectType)))
            {
              // If the parent of this element is a splitter, then we submit
              // its child to the refresh list. We don't want to submit the
              // splitter because this will cause a redraw of all parts of the
              // splitter(costly and unnecessary).
              window_thread_->QueueObjectLayout(this);
            }
            else
#endif
            {
              window_thread_->QueueObjectLayout(ic);
            }
          }
          else
          {
            // The parent object of an object of type View is a Layout object type.
            layout->parent_area_->ReconfigureParentLayout(this);
          }
        }
        else
        {
          layout->parent_area_->ReconfigureParentLayout(this);
        }
      }
      else
      {
        // This is possibly the Main layout or the layout of a floating object(popup for example) unless the layout is not part of the object tree.
        window_thread_->QueueObjectLayout(layout);
      }
    }
    else
    {
      // The object that is being resized is a InputArea object.
      if (this->parent_area_)
      {
        // The object that is being resized is a View object and it has a parent.
        if (this->OwnsTheReference() == false && this->GetParentObject())
        {
          // Only reference parented areas.
          this->Reference();
        }

        // The parent object of an object of type InputArea is a Layout object type.
        this->parent_area_->ReconfigureParentLayout(this);
      }
    }
  }

  void Area::RequestBottomUpLayoutComputation(Area *bo_initiator)
  {
    if (parent_area_ && parent_area_->IsLayout())
    {
      parent_area_->RequestBottomUpLayoutComputation(bo_initiator);
    }
  }

  void Area::SetVisible(bool visible)
  {
    if (IsLayout())
      return;

    if (visible_ == visible)
      return;

    visible_ = visible;

    OnVisibleChanged.emit(this, visible_);
  }

  bool Area::IsVisible()
  {
    return visible_;
  }

  void Area::SetSensitive(bool sensitive)
  {
    SetInputEventSensitivity(sensitive);
  }

  void Area::SetInputEventSensitivity(bool sensitive)
  {
    if (IsLayout())
      return;

    if (sensitive_ == sensitive)
      return;

    sensitive_ = sensitive;

    OnSensitiveChanged.emit(this, sensitive_);
  }

  bool Area::IsSensitive() const
  {
    return GetInputEventSensitivity();
  }

  bool Area::GetInputEventSensitivity() const
  {
    return sensitive_;
  }

  MinorDimensionPosition Area::GetPositioning()
  {
    return minor_axis_position_;
  }

  void Area::SetPositioning(MinorDimensionPosition p)
  {
    minor_axis_position_ = p;
  }

  MinorDimensionSize Area::GetExtend()
  {
    return minor_axis_size_;
  }

  void Area::SetExtend(MinorDimensionSize ext)
  {
    minor_axis_size_ = ext;
  }

  float Area::GetPercentage()
  {
    return minor_axis_size_scale_;
  }

  void Area::SetPercentage(float p)
  {
    minor_axis_size_scale_ = p;
  }

  bool Area::IsLayoutDone()
  {
    return layout_done_;
  }

  void Area::SetLayoutDone(bool b)
  {
    layout_done_ = b;
  }

  bool Area::IsArea() const
  {
    return this->Type().IsDerivedFromType(Area::StaticObjectType);
  }

  bool Area::IsInputArea() const
  {
    return this->Type().IsDerivedFromType(InputArea::StaticObjectType);
  }

  bool Area::IsView() const
  {
    return this->Type().IsDerivedFromType(View::StaticObjectType);
  }

  bool Area::IsLayout() const
  {
    return this->Type().IsDerivedFromType(Layout::StaticObjectType);
  }

  bool Area::IsViewWindow() const
  {
    return this->Type().IsDerivedFromType(BaseWindow::StaticObjectType);
  }

  bool Area::IsSpaceLayout() const
  {
    return this->Type().IsDerivedFromType(SpaceLayout::StaticObjectType);
  }

  void Area::Set2DMatrix(const Matrix4 &mat)
  {
    _2d_xform = mat;
  }

  void Area::Set2DTranslation(float tx, float ty, float tz)
  {
    _2d_xform.Translate(tx, ty, tz);
  }

  Matrix4 Area::Get2DMatrix() const
  {
    return _2d_xform;
  }

  Matrix4 Area::Get3DMatrix() const
  {
    return _3d_xform;
  }

  bool Area::Is3DArea() const
  {
    return _3d_area;
  }

  static void MatrixXFormGeometry(const Matrix4 &matrix, Geometry &geo)
  {
    Vector4 in(geo.x, geo.y, 0, 1);
    // This is mean only for translation matrices. It will not work with matrices containing rotations or scalings.
    Vector4 out = matrix * in;
    geo.x = out.x;
    geo.y = out.y;
  }

  void Area::InnerGetAbsoluteGeometry(Geometry &geometry)
  {
    if (this->Type().IsDerivedFromType(BaseWindow::StaticObjectType) || (this == window_thread_->GetLayout()))
    {
      geometry.OffsetPosition(geometry_.x, geometry_.y);
      return;
    }

    MatrixXFormGeometry(_2d_xform, geometry);

    Area *parent = GetParentObject();
    if (parent)
      parent->InnerGetAbsoluteGeometry(geometry);
  }

  Geometry Area::GetAbsoluteGeometry() const
  {
    if (Type().IsDerivedFromType(BaseWindow::StaticObjectType) ||
#if !defined(NUX_MINIMAL)
      Type().IsDerivedFromType(MenuPage::StaticObjectType) ||
#endif
      (this == window_thread_->GetLayout()))
    {
      // Do not apply the _2D_xform matrix  to a BaseWindow or the main layout
      return geometry_;
    }
    else
    {
      nux::Geometry geo = geometry_;
      MatrixXFormGeometry(_2d_xform, geo);

      Area *parent = GetParentObject();
      if (parent)
        parent->InnerGetAbsoluteGeometry(geo);

      return geo;
    }
  }

  int Area::GetAbsoluteX() const
  {
    return GetAbsoluteGeometry().x;
  }

  int Area::GetAbsoluteY() const
  {
    return GetAbsoluteGeometry().y;
  }

  int Area::GetAbsoluteWidth() const
  {
    return GetAbsoluteGeometry().width;
  }

  int Area::GetAbsoluteHeight() const
  {
    return GetAbsoluteGeometry().height;
  }

  void Area::InnerGetRootGeometry(Geometry &geometry)
  {
    if (this->Type().IsDerivedFromType(BaseWindow::StaticObjectType) || (this == window_thread_->GetLayout()))
      return;

    MatrixXFormGeometry(_2d_xform, geometry);

    Area *parent = GetParentObject();
    if (parent)
      parent->InnerGetRootGeometry(geometry);
  }

  Geometry Area::GetRootGeometry() const
  {
    nux::Geometry geo = geometry_;
    MatrixXFormGeometry(_2d_xform, geo);

    if (Type().IsDerivedFromType(BaseWindow::StaticObjectType) || (this == window_thread_->GetLayout()))
    {
      return geo;
    }
    else
    {
      Area *parent = GetParentObject();
      if (parent)
        parent->InnerGetRootGeometry(geo);

      return geo;
    }
  }

  int Area::GetRootX() const
  {
    return GetRootGeometry().x;
  }

  int Area::GetRootY() const
  {
    return GetRootGeometry().y;
  }

  int Area::GetRootWidth() const
  {
    return GetRootGeometry().width;
  }

  int Area::GetRootHeight() const
  {
    return GetRootGeometry().height;
  }

  Area* Area::GetToplevel()
  {
    return GetRootParent();
  }

  Area* Area::GetRootParent()
  {
    if (Type().IsDerivedFromType(BaseWindow::StaticObjectType) || (this == window_thread_->GetLayout()))
    {
      return this;
    }

    Area* parent = GetParentObject();
    if (!parent) //we didn't find a way to salvation!
    {
      return 0;
    }
    return parent->GetRootParent();
  }

  Area* Area::GetTopLevelViewWindow()
  {
    Area* area = GetRootParent();

    if (area && area->IsViewWindow())
      return area;

    return NULL;
  }

  bool Area::HasTopLevelParent()
  {
    if (GetRootParent())
    {
      return true;
    }
    return false;
  }

  bool Area::IsChildOf(Area* parent)
  {
    if (this == parent)
      return true;

    if (!parent || !parent_area_)
      return false;

    return parent_area_->IsChildOf(parent);    
  }

  void Area::QueueRelayout()
  {
    window_thread_->QueueObjectLayout(this);
  }
  
  void Area::SetAcceptKeyboardEvent(bool accept_keyboard_event)
  {
    accept_keyboard_event_ = accept_keyboard_event;
  }

  bool Area::AcceptKeyboardEvent() const
  {
    return accept_keyboard_event_;
  }

  void Area::SetAcceptMouseWheelEvent(bool accept_mouse_wheel_event)
  {
    accept_mouse_wheel_event_ = accept_mouse_wheel_event;
  }

  bool Area::AcceptMouseWheelEvent() const
  {
    return accept_mouse_wheel_event_;
  }

   bool Area::TestMousePointerInclusion(const Point& mouse_position, NuxEventType event_type)
   {
     if ((IsLayout() == false) && ((visible_ == false) ||
       (sensitive_ == false) ||
       (view_enabled_ == false)))
     {
       // If this area is not a view and:
       //    - it is insensitive to input event
       //    - it is not enabled
       //    - it is not visible
       // then return false.
       return false;
     }

     bool mouse_pointer_inside_area = false;
#if !defined(NUX_MINIMAL)
     if (Type().IsDerivedFromType(MenuPage::StaticObjectType))
     {
       // A MenuPage geometry is already in absolute coordinates.
       mouse_pointer_inside_area = geometry_.IsInside(mouse_position);
     }
     else
#endif
     {
       mouse_pointer_inside_area = GetAbsoluteGeometry().IsInside(mouse_position);
     }

     if ((event_type == NUX_MOUSE_WHEEL) && mouse_pointer_inside_area)
     {
       if (accept_mouse_wheel_event_ == false)
         return false;
     }
 
     return mouse_pointer_inside_area;
   }
 
   bool Area::TestMousePointerInclusionFilterMouseWheel(const Point& mouse_position, NuxEventType /* event_type */)
   {
     if ((IsLayout() == false) && ((visible_ == false) ||
       (sensitive_ == false) ||
       (view_enabled_ == false)))
     {
       // If this area is not a view and:
       //    - it is insensitive to input event
       //    - it is not enabled
       //    - it is not visible
       // then return false.

       return false;
     }

     bool mouse_pointer_inside_area = false;
#if !defined(NUX_MINIMAL)
     if (Type().IsDerivedFromType(MenuPage::StaticObjectType))
     {
       // A MenuPage geometry is already in absolute coordinates.
       mouse_pointer_inside_area = geometry_.IsInside(mouse_position);
     }
     else
#endif
     {
       mouse_pointer_inside_area = GetAbsoluteGeometry().IsInside(mouse_position);
     }

     return mouse_pointer_inside_area;
   }

  Area* Area::FindAreaUnderMouse(const Point& /* mouse_position */, NuxEventType /* event_type */)
  {
    return NULL;
  }
  
  Area* Area::FindKeyFocusArea(unsigned int key_symbol,
   unsigned long x11_key_code,
   unsigned long special_keys_state)
  {
    if (has_key_focus_)
    {
      return this;
    }
    else if (next_object_to_key_focus_area_)
    {
      return next_object_to_key_focus_area_->FindKeyFocusArea(key_symbol, x11_key_code, special_keys_state);
    }
    return NULL;
  }

  void Area::SetNextObjectToKeyFocusArea(Area* area)
  {
    next_object_to_key_focus_area_ = area;
  }

  Area* Area::GetNextObjectToKeyFocusArea()
  {
    return next_object_to_key_focus_area_;
  }

  void Area::SetPathToKeyFocusArea()
  {
    has_key_focus_ = true;
    next_object_to_key_focus_area_ = NULL;

    Area* child = this;
    Area* parent = GetParentObject();

    while (parent)
    {
      parent->next_object_to_key_focus_area_ = child;
      parent->next_object_to_key_focus_area_->Reference();
      parent->has_key_focus_ = false;
      child = parent;
      parent = parent->GetParentObject();
    }
  }

  void Area::ResetDownwardPathToKeyFocusArea()
  {
    has_key_focus_ = false;
    if (next_object_to_key_focus_area_)
    {
      next_object_to_key_focus_area_->ResetDownwardPathToKeyFocusArea();
    }
    if (next_object_to_key_focus_area_)
      next_object_to_key_focus_area_->UnReference();

    next_object_to_key_focus_area_ = NULL;
  }

  void Area::ResetUpwardPathToKeyFocusArea()
  {
    has_key_focus_ = false;
    if (parent_area_)
    {
      parent_area_->ResetUpwardPathToKeyFocusArea();
    }
    if (next_object_to_key_focus_area_)
      next_object_to_key_focus_area_->UnReference();

    next_object_to_key_focus_area_ = NULL;
  }

  bool Area::InspectKeyEvent(unsigned int /* event_type */,
    unsigned int /* keysym */,
    const char* /* character */)
  {
    return false;
  }

  Area* Area::KeyNavIteration(KeyNavDirection /* direction */)
  {
    return NULL;
  }

  bool Area::HasKeyFocus() const
  {
    return has_key_focus_;
  }

  bool Area::IsMousePointerInside() const
  {
    Geometry geo = GetAbsoluteGeometry();
    Point position = GetWindowThread()->GetWindowCompositor().GetMousePosition();
    
    if (geo.IsInside(position))
      return true;

    return false;
  }

  /*** Support for redirected rendering ***/
  void Area::SetRedirectRenderingToTexture(bool redirect)
  {
    if (redirect_rendering_to_texture_ == redirect)
    {
      return;
    }

    if (!redirect_rendering_to_texture_ && redirect)
    {
      update_backup_texture_ = true;
    }

    redirect_rendering_to_texture_ = redirect;
    if (redirect == false)
    {
      // Free the texture of this view
      backup_fbo_.Release();
      backup_texture_.Release();
      backup_depth_texture_.Release();
      prev_fbo_.Release();
    }
  }

  bool Area::RedirectRenderingToTexture() const
  {
    return redirect_rendering_to_texture_;
  }

  void Area::SetUpdateBackupTextureForChildRendering(bool update)
  {
    update_backup_texture_ = update;
  }

  ObjectPtr<IOpenGLBaseTexture> Area::BackupTexture() const
  {
    // if RedirectRenderingToTexture() is false, then backup_texture_ is not a valid smart pointer.
    return backup_texture_;
  }

  bool Area::UpdateBackupTextureForChildRendering() const
  {
    return update_backup_texture_;
  }

  void Area::PrepareParentRedirectedView()
  {
    Area* parent = GetParentObject();

    while (parent)
    {
      if (parent->RedirectRenderingToTexture())
      {
        if (parent->UpdateBackupTextureForChildRendering())
          break;

        parent->SetUpdateBackupTextureForChildRendering(true);
        parent->PrepareParentRedirectedView();
      }
      else
      {
        parent->PrepareParentRedirectedView();
        break;
      }
    }
  }

  bool Area::HasParentRedirectedView()
  {
    Area* parent = GetParentObject();

    while (parent && !parent->Type().IsDerivedFromType(View::StaticObjectType))
    {
      parent = parent->GetParentObject();
    }

    if (parent)
    {
      View* view = static_cast<View*>(parent);
      if (view->RedirectRenderingToTexture())
      {
        return true;
      }
      else
      {
        return view->HasParentRedirectedView();
      }
    }
    return false;
  }

  Area* Area::RedirectedAncestor()
  {
    Area* parent = GetParentObject();

    while (parent)
    {
      if (parent->RedirectRenderingToTexture())
      {
        return parent;
      }
      parent = parent->GetParentObject();
    }

    return NULL;
  }

  void Area::SetCopyPreviousFboTexture(bool copy_background)
  {
    copy_previous_fbo_for_background_ = copy_background;
  }

  void Area::SetPresentRedirectedView(bool present_redirected_view)
  {
    present_redirected_view_ = present_redirected_view;
  }

  bool Area::PresentRedirectedView() const
  {
    return present_redirected_view_;
  }

#ifdef NUX_GESTURES_SUPPORT
  Area* Area::GetInputAreaHitByGesture(const GestureEvent & /* event */)
  {
    return nullptr;
  }

  bool Area::IsGestureInsideArea(const nux::GestureEvent &event) const
  {
    if (event.IsDirectTouch())
    {
      Geometry geometry = GetAbsoluteGeometry();
      Point p;
      for (const auto touch_point : event.GetTouches())
      {
        p.x = (int)touch_point.x;
        p.y = (int)touch_point.y;
        if (!geometry.IsInside(p))
          return false;
      }
      return true;
    }
    else
    {
      return GetAbsoluteGeometry().IsInside(event.GetFocus());
    }
  }

#endif // NUX_GESTURES_SUPPORT
}

