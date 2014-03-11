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
#include "LinearLayout.h"
#include "View.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(LinearLayout);

  LinearLayout::LinearLayout(NUX_FILE_LINE_DECL)
  : Layout(NUX_FILE_LINE_PARAM)
  {
  }

  LinearLayout::~LinearLayout()
  {
  }

// If(stretchfactor == 0): the WidgetLayout geometry will be set to SetGeometry(0,0,1,1);
// and the children will take their natural size by expending WidgetLayout.
// If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.
  void LinearLayout::AddLayout(Layout *layout, unsigned int stretchFactor, MinorDimensionPosition minor_position, MinorDimensionSize minor_size, float percentage, LayoutPosition index)
  {
    nuxAssertMsg(layout != 0, "[Layout::AddView] Invalid parameter.");
    NUX_RETURN_IF_TRUE(layout == 0);
    //  Should never happen
    nuxAssertMsg(layout != this, "[Layout::AddLayout] Error: Trying to add a layout to itself.");
    NUX_RETURN_IF_FALSE(layout != 0);

    Area *parent = layout->GetParentObject();
    nuxAssertMsg(parent == 0, "[Layout::AddLayout] Trying to add an object that already has a parent.");
    NUX_RETURN_IF_TRUE(parent != 0);

    nuxAssertMsg(index >= 0, "[Layout::AddLayout] Invalid index position. Adding at the beginning of the list..");

    layout->SetScaleFactor(stretchFactor);
    layout->SetPositioning(minor_position);
    layout->SetExtend(minor_size);

    if (percentage < 1.0f)
    {
      layout->SetPercentage(1.0f);
    }
    else if (percentage > 100.0f)
    {
      layout->SetPercentage(100.0f);
    }
    else
    {
      layout->SetPercentage(percentage);
    }

    layout->SetParentObject(this);

    layout->child_queue_draw.connect(sigc::mem_fun(this, &Layout::ChildQueueDraw));
    layout->queue_draw.connect(sigc::mem_fun(this, &Layout::ChildQueueDraw));

    if (index < 0)
      index = NUX_LAYOUT_BEGIN;

    if (index == NUX_LAYOUT_END || index >= _layout_element_list.size())
    {
      _layout_element_list.push_back(layout);
    }
    else
    {
      std::list<Area *>::iterator pos = _layout_element_list.begin();
      int idx = index;
      while (pos != _layout_element_list.end() && idx > 0)
      {
        idx--;
        pos++;
      }
      _layout_element_list.insert(pos, layout);
    }

    ViewAdded.emit(this, layout);
  }

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

      The minor_position parameter controls how the layout will place the object within itself. A vertical layout object controls the horizontal positioning
      of its children baseobjects, While an horizontal layout object controls the vertical positioning of its children baseobjects.

      The minor_size parameter controls how much size the baseobject minor dimension gets from the layout minor dimension. See MinorDimensionSize.

      /param baseobject The object that is being added.
      /param stretchFactor This value controls how the layout object share space between its children baseobject.
      /param minor_position Controls how the layout position the object.
      /param minor_size Controls the object minor dimension size.
      /param percentage Controls the object minor dimension size in percentage of the layout minor dimension size.
      /param index Controls the object position in the layout children.
  */

  void LinearLayout::AddView(Area *bo, unsigned int stretchFactor, MinorDimensionPosition minor_position, MinorDimensionSize minor_size, float percentage, LayoutPosition index)
  {
    nuxAssertMsg(bo != 0, "[Layout::AddView] Invalid parameter.");
    NUX_RETURN_IF_TRUE(bo == 0);

    Area *parent = bo->GetParentObject();
    nuxAssertMsg(parent == 0, "[Layout::AddView] Trying to add an object that already has a parent.");
    NUX_RETURN_IF_TRUE(parent != 0);

    nuxAssertMsg(index >= 0, "[Layout::AddView] Invalid index position. Adding at the beginning of the list..");

    bo->SetScaleFactor(stretchFactor);
    bo->SetPositioning(minor_position);
    bo->SetExtend(minor_size);

    if (percentage < 1.0f)
    {
      bo->SetPercentage(1.0f);
    }
    else if (percentage > 100.0f)
    {
      bo->SetPercentage(100.0f);
    }
    else
    {
      bo->SetPercentage(percentage);
    }

    bo->SetParentObject(this);

    if (bo->IsView())
      (static_cast<View *> (bo))->queue_draw.connect(sigc::mem_fun(this, &Layout::ChildQueueDraw));

    //if(HasFocusControl() && HasFocusableEntries() == false)
    //{
      //bo->SetFocused(true);
      //ChildFocusChanged(this, bo);
    //}

    if (index < 0)
      index = NUX_LAYOUT_BEGIN;

    if (index == NUX_LAYOUT_END || index >= _layout_element_list.size())
    {
      _layout_element_list.push_back(bo);
    }
    else
    {
#if defined(NUX_OS_WINDOWS) && !defined(NUX_VISUAL_STUDIO_2010)
      std::list<Area *>::iterator pos = _layout_element_list.begin();
#else
      auto pos = _layout_element_list.begin();
#endif
      int idx = index;
      while (pos != _layout_element_list.end() && idx > 0)
      {
        idx--;
        pos++;
      }
      _layout_element_list.insert(pos, bo);
    }

    ViewAdded.emit(this, bo);
    //--->> Removed because it cause problem with The splitter widget: ComputeContentSize();
  }

  void LinearLayout::AddSpace(unsigned int /* width */, unsigned int stretchFactor, LayoutPosition /* index */)
  {
    AddLayout(new SpaceLayout(), stretchFactor);
  }

  void LinearLayout::SetHorizontalInternalMargin(int space)
  {
    SetSpaceBetweenChildren(space);
  }

  void LinearLayout::SetVerticalInternalMargin(int space)
  {
    SetSpaceBetweenChildren(space);
  }

  void LinearLayout::SetSpaceBetweenChildren(int space)
  {
#if DEBUG_LAYOUT
    return;
#endif
    space_between_children_ = space >= 0 ? space : 0;
  }

}
