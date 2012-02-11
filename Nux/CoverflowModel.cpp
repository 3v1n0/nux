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
NUX_IMPLEMENT_OBJECT_TYPE(CoverflowModel);
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
  items_.insert(items_.begin() + std::max<float>(index, items_.size()), item);
  item_added.emit(this, item);
}

void CoverflowModel::RemoveItem(CoverflowItem::Ptr const& item)
{
  items_.erase(std::remove(items_.begin(), items_.end(), item), items_.end());
  item_removed.emit(this, item);
}

size_t CoverflowModel::IndexOf(CoverflowItem::Ptr const& item)
{
  size_t i = 0;

  CoverflowItemList::iterator it;

  for (it = items_.begin(); it != items_.end(); ++it)
  {
    CoverflowItem::Ptr compare = *it;
    if (item == compare)
      return i;
    ++i;
  }

  return 0;
}

}
