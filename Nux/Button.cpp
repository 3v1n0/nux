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
#include "Button.h"
#include "StaticText.h"
#include "HLayout.h"
#include "VLayout.h"
#include "TextureArea.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(Button);

  Button::Button(TextureArea *image, NUX_FILE_LINE_DECL)
  : AbstractButton(NUX_FILE_LINE_PARAM)
  {
    image_ = NULL;
    Initialize(std::string(), image);
  }

  Button::Button(const std::string& button_label, NUX_FILE_LINE_DECL)
  : AbstractButton(NUX_FILE_LINE_PARAM)
  {
    image_ = NULL;
    Initialize(button_label, NULL);
  }

  Button::Button(const std::string& button_label, TextureArea *image, NUX_FILE_LINE_DECL)
  : AbstractButton(NUX_FILE_LINE_PARAM)
  {
    image_ = NULL;
    Initialize(button_label, image);
  }

  Button::Button(NUX_FILE_LINE_DECL)
  : AbstractButton(NUX_FILE_LINE_PARAM)
  {
    image_ = NULL;
    Initialize(std::string(), NULL);
  }

  Button::~Button()
  {
    if (image_)
      image_->UnReference();

    if (static_text_)
      static_text_->UnReference();
  }

  void Button::Initialize(const std::string &str, TextureArea *image)
  {
    same_size_as_content_ = true;
    persistent_active_state_ = false;
    layout_type_ = HORIZONTAL;
    item_order_ = IMAGE_FIRST;
    distribution_ = CENTER_OF_LAYOUT;
    space_between_items_ = 0;
    
    layout_top_padding_ = 2;
    layout_right_padding_ = 2;
    layout_bottom_padding_ = 2;
    layout_left_padding_ = 2;

    int clip = 2;
    left_clip_ = clip;
    right_clip_ = clip;
    top_clip_ = clip;
    bottom_clip_ = clip;


    // Set Geometry
    SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    image_minimum_size_ = Size(16, 16);
    image_maximum_size_ = Size(AREA_MAX_WIDTH, AREA_MAX_HEIGHT);
    BuildLayout(str, image);
  }

  void Button::SetImage(TextureArea *image)
  {
    if (image == NULL)
      return;
    BuildLayout(label_, image);
  }

  void Button::SetLabel(const std::string &button_label)
  {
    BuildLayout(button_label, image_);
  }

  TextureArea* Button::GetImage()
  {
    if (image_ == NULL)
      return NULL;

    TextureArea *texture_area = new TextureArea();
    texture_area->SetPaintLayer(image_->GetPaintLayer());

    return texture_area;
  }

  std::string Button::GetLabel() const
  {
    return label_;
  }

  void Button::SetImageMinimumSize(int width, int height)
  {
    image_minimum_size_ = Size(width, height);
    if (image_)
      image_->SetMinimumSize(width, height);
  }

  void Button::SetImageMaximumSize(int width, int height)
  {
    image_maximum_size_ = Size(width, height);
    if (image_)
      image_->SetMaximumSize(width, height);
  }

  void Button::SetLayoutPadding(int top, int right, int bottom, int left)
  {
    layout_top_padding_ = top >= 0 ? top : 0;
    layout_right_padding_ = right >= 0 ? right : 0;
    layout_bottom_padding_ = bottom >= 0 ? bottom : 0;
    layout_left_padding_ = left >= 0 ? left : 0;

    if (view_layout_)
    {
      static_cast<LinearLayout*>(view_layout_)->SetPadding(layout_top_padding_,
        layout_right_padding_,
        layout_bottom_padding_,
        layout_left_padding_);
    }
    ComputeContentSize();
    QueueDraw();
  }

  void Button::SetButtonClipRegion(int top_clip, int right_clip, int bottom_clip, int left_clip)
  {
    top_clip_ = top_clip >= 0 ? top_clip : 0;
    right_clip_ = right_clip >= 0 ? right_clip : 0;
    bottom_clip_ = bottom_clip >= 0 ? bottom_clip : 0;
    left_clip_ = left_clip >= 0 ? left_clip : 0;
  }

  void Button::BuildLayout(const std::string &str, TextureArea* image)
  {
    if (image_ != image)
    {
      if (image_)
      {
        image_->UnReference();
        nuxAssert(image_->GetReferenceCount() == 1);
        image_ = NULL;
      }

      if (image)
      {
        image_ = new TextureArea();
        image_->Reference();
        // WARNING: GetPaintLayer returns a clone and SetPaintLayer makes a copy of the clone
        // UnReference temp otherwise it will be lost and cause a mem leak.
        AbstractPaintLayer *temp = image->GetPaintLayer();
        image_->SetPaintLayer(temp);
        delete temp;

        SetImageMinimumSize(image->GetMinimumWidth(), image->GetMinimumHeight());
        SetImageMaximumSize(image->GetMaximumWidth(), image->GetMaximumHeight());
      }
    }

    bool create_new_text = false;
    if (static_text_ && (str != static_text_->GetText()))
    {
      create_new_text = true;
    }

    if ((static_text_ == NULL) && (str != ""))
    {
      create_new_text = true;
    }

    if (create_new_text)
    {
      if (static_text_)
      {
        static_text_->UnReference();
        nuxAssert(static_text_->GetReferenceCount() == 1);
        static_text_ = NULL;
      }

      if (str != "")
      {
        label_ = str;
        static_text_ = new StaticText(str, NUX_TRACKER_LOCATION);
        static_text_->Reference();
        static_text_->SetTextColor(label_color_);
      }
    }

    RemoveLayout();

    LinearLayout *layout = NULL;
    if (static_text_ && image_)
    {
      if ((layout_type_ == HORIZONTAL) && (item_order_ == IMAGE_FIRST))
      {
        layout = new HLayout(NUX_TRACKER_LOCATION);
        layout->AddView(image_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
        layout->AddView(static_text_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
        layout->SetHorizontalInternalMargin(space_between_items_);
      }
      else if ((layout_type_ == HORIZONTAL) && (item_order_ == LABEL_FIRST))
      {
        layout = new HLayout(NUX_TRACKER_LOCATION);
        layout->AddView(static_text_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
        layout->AddView(image_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
        layout->SetHorizontalInternalMargin(space_between_items_);
      }
      else if ((layout_type_ == VERTICAL) && (item_order_ == IMAGE_FIRST))
      {
        layout = new VLayout(NUX_TRACKER_LOCATION);
        layout->AddView(image_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
        layout->AddView(static_text_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
        layout->SetVerticalInternalMargin(space_between_items_);
      }
      else if ((layout_type_ == VERTICAL) && (item_order_ == LABEL_FIRST))
      {
        layout = new VLayout(NUX_TRACKER_LOCATION);
        layout->AddView(static_text_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
        layout->AddView(image_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
        layout->SetVerticalInternalMargin(space_between_items_);
      }

      // Both image_ and static_text_ have a reference count of 2.
      nuxAssert(image_->GetReferenceCount() == 2);
      nuxAssert(static_text_->GetReferenceCount() == 2);
    }
    else if (static_text_)
    {
      if (layout_type_ == HORIZONTAL)
      {
        layout = new HLayout(NUX_TRACKER_LOCATION);
        layout->AddView(static_text_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
      }
      else if (layout_type_ == VERTICAL)
      {
        layout = new VLayout(NUX_TRACKER_LOCATION);
        layout->AddView(static_text_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
      }
      nuxAssert(static_text_->GetReferenceCount() == 2);
    }
    else if (image_)
    {
      if (layout_type_ == HORIZONTAL)
      {
        layout = new HLayout(NUX_TRACKER_LOCATION);
        layout->AddView(image_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
      }
      else if (layout_type_ == VERTICAL)
      {
        layout = new VLayout(NUX_TRACKER_LOCATION);
        layout->AddView(image_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
      }
      nuxAssert(image_->GetReferenceCount() == 2);
    }

    if (static_text_)
    {
      static_text_->SetInputEventSensitivity(false);
      static_text_->SetTextPointSize(label_font_size_);
    }

    if (image_)
    {
      image_->SetInputEventSensitivity(false);
    }


    if (layout)
    {
      switch(distribution_)
      {
      case START_OF_LAYOUT:
        layout->SetContentDistribution(MAJOR_POSITION_LEFT);
        break;

      case END_OF_LAYOUT:
        layout->SetContentDistribution(MAJOR_POSITION_RIGHT);
        break;

      case CENTER_OF_LAYOUT:
        layout->SetContentDistribution(MAJOR_POSITION_CENTER);
        break;

      case SPREAD_OVER_LAYOUT:
        layout->SetContentDistribution(MAJOR_POSITION_SPREAD);
        break;
      }
    }

    if (layout)
    {
      layout->SetHorizontalInternalMargin(space_between_items_);
      layout->SetPadding(layout_top_padding_,
        layout_right_padding_,
        layout_bottom_padding_,
        layout_left_padding_);
    }

    if (layout)
    {
      SetLayout(layout);
    }

    ComputeContentSize();

    QueueDraw();
  }

  void Button::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    Geometry base = GetGeometry();

    graphics_engine.PushClippingRectangle(base);

    UXStyleImageRef ref_style = eIMAGE_STYLE_NONE;

    if (visual_state_ == VISUAL_STATE_PRESSED)
    {
      ref_style = eBUTTON_FOCUS;
    }
    else if (visual_state_ == VISUAL_STATE_PRELIGHT)
    {
      ref_style = eBUTTON_PRELIGHT;
    }
    else
    {
      ref_style = eBUTTON_NORMAL;
    }

    TexCoordXForm texxform;
    ROPConfig rop;
    rop.Blend = true;
    rop.SrcBlend = GL_ONE;
    rop.DstBlend = GL_ONE_MINUS_SRC_ALPHA;

    GetPainter().PushDrawSliceScaledTextureLayer(graphics_engine, base, ref_style, color::White, eAllCorners, true, rop);

    if (GetCompositionLayout())
    {
      GetPainter().PushPaintLayerStack();
      {
        Geometry clip_geo = base;
        clip_geo.OffsetPosition(left_clip_, top_clip_);
        clip_geo.OffsetSize(-left_clip_ - right_clip_, -top_clip_ - bottom_clip_);

        graphics_engine.PushClippingRectangle(clip_geo);

        GetCompositionLayout()->ProcessDraw(graphics_engine, true);

        graphics_engine.PopClippingRectangle();
      }
      GetPainter().PopPaintLayerStack();
    }
    GetPainter().PopPaintLayer();
    graphics_engine.PopClippingRectangle();
  }

  long Button::ComputeContentSize()
  {
    return View::ComputeContentSize();
  }

  void Button::Activate()
  {
    if (persistent_active_state_ == false)
    {
      return;
    }

    if (active_ == true)
    {
      // already active
      return;
    }

    active_ = true;
    
    state_change.emit(this);
    QueueDraw();
  }

  void Button::Deactivate()
  {
    if (persistent_active_state_ == false)
    {
      return;
    }

    if (active_ == false)
    {
      // already deactivated
      return;
    }

    active_ = false;

    state_change.emit(this);
    QueueDraw();
  }

  void Button::RecvClick(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (persistent_active_state_)
    {
      active_ = !active_;
    }

    click.emit(this);
    state_change.emit(this);
    QueueDraw();
  }

  void Button::SetDistribution(Distribution distribution)
  {
    distribution_ = distribution;
    BuildLayout(label_, image_);
  }

  void Button::SetItemOrder(ItemOrder item_order)
  {
    item_order_ = item_order;
    BuildLayout(label_, image_);
  }

  void Button::SetLayoutType(LayoutType layout_type)
  {
    layout_type_ = layout_type;
    BuildLayout(label_, image_);
  }

  void Button::SetSpaceBetweenItems(int space_between_items)
  {
    space_between_items_ = (space_between_items >= 0) ? space_between_items : 0;
    if (view_layout_)
    {
      static_cast<LinearLayout*>(view_layout_)->SetSpaceBetweenChildren(space_between_items_);
    }

    ComputeContentSize();
    QueueDraw();

  }

  void Button::SetLabelFontSize(int point)
  {
    AbstractButton::SetLabelFontSize(point);

    if (static_text_ == NULL)
      return;

    ComputeContentSize();
    QueueDraw();
  }
}
