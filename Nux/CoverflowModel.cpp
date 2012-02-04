// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2012 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Jason Smith <jason.smith@canonical.com>
 */

#include "CoverflowModel.h"

namespace nux
{ 


CoverflowModel::CoverflowModel()
: selection_changed_time_(0)
, selection_index_(0)
{
  
}

CoverflowItem::Ptr CoverflowModel::Selection() const
{
  if (selection_index_ >= items_.size())
    return CoverflowItem::Ptr();
  
  return items_[selection_index_];
}

size_t CoverflowModel::SelectionIndex() const
{
  return selection_index_;
}

gint64 CoverflowModel::SelectionChangedTime() const
{
  return selection_changed_time_;
}

CoverflowModel::CoverflowItemList const& CoverflowModel::Items() const
{
  return items_;
}

void CoverflowModel::AddItem(CoverflowItem::Ptr const& item)
{
  items_.push_back(item);
  item_added.emit(this, item);
}

void CoverflowModel::InsertItem(CoverflowItem::Ptr const& item, size_t index)
{
  items_.insert(items_.begin() + std::max(index, items_.size()), item);
  item_added.emit(this, item);
}

void CoverflowModel::RemoveItem(CoverflowItem::Ptr const& item)
{
  items_.erase(std::remove(items_.begin(), items_.end(), item), items_.end());
  
  if (selection_index_ >= items_.size())
    selection_index_ = items_.size() - 1;
  item_removed.emit(this, item);
  selection_changed.emit(this, Selection(), selection_index_);
}

void CoverflowModel::SelectNext()
{
  if (selection_index_ >= items_.size() - 1)
    return;

  SetSelection(selection_index_ + 1);
}

void CoverflowModel::SelectPrev()
{
  if (selection_index_ == 0)
    return;

  SetSelection(selection_index_ - 1);
}

void CoverflowModel::SelectIndex(size_t index)
{
  if (index < items_.size() && index != selection_index_)
    SetSelection(index);
}

void CoverflowModel::SetSelection(size_t selection)
{
  if (selection == selection_index_)
    return;
  
  selection_index_ = selection;
  selection_changed_time_ = g_get_monotonic_time();
  selection_changed.emit(this, Selection(), selection_index_);
}

}
