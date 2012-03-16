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

struct CoverflowModel::Impl
{
  Impl(CoverflowModel *parent);
  ~Impl();

  CoverflowModel* parent_;
  size_t selection_index_;
  CoverflowItemList items_;
};

CoverflowModel::Impl::Impl(CoverflowModel* parent)
  : parent_(parent)
  , selection_index_(0)
{
}

CoverflowModel::Impl::~Impl()
{
}

NUX_IMPLEMENT_OBJECT_TYPE(CoverflowModel);

CoverflowModel::CoverflowModel()
  : pimpl(new CoverflowModel::Impl(this))
{
}

CoverflowModel::~CoverflowModel()
{
  delete pimpl;
}

CoverflowModel::CoverflowItemList const& CoverflowModel::Items() const
{
  return pimpl->items_;
}

void CoverflowModel::AddItem(CoverflowItem::Ptr const& item)
{
  pimpl->items_.push_back(item);
  item_added.emit(this, item);
  SetSelection(pimpl->selection_index_); // perform bounds check
}

void CoverflowModel::InsertItem(CoverflowItem::Ptr const& item, size_t index)
{
  pimpl->items_.insert(pimpl->items_.begin() + std::max<float>(index, pimpl->items_.size()), item);
  item_added.emit(this, item);
  SetSelection(pimpl->selection_index_); // perform bounds check
}

void CoverflowModel::RemoveItem(CoverflowItem::Ptr const& item)
{
  pimpl->items_.erase(std::remove(pimpl->items_.begin(), pimpl->items_.end(), item), pimpl->items_.end());
  item_removed.emit(this, item);
  SetSelection(pimpl->selection_index_); // perform bounds check
}

size_t CoverflowModel::IndexOf(CoverflowItem::Ptr const& item)
{
  size_t i = 0;

  CoverflowItemList::iterator it;

  for (it = pimpl->items_.begin(); it != pimpl->items_.end(); ++it)
  {
    CoverflowItem::Ptr compare = *it;
    if (item == compare)
      return i;
    ++i;
  }

  return 0;
}

CoverflowItem::Ptr CoverflowModel::Selection()
{
  if (pimpl->selection_index_ >= pimpl->items_.size())
    return CoverflowItem::Ptr();
  return pimpl->items_[pimpl->selection_index_];
}

size_t CoverflowModel::SelectionIndex()
{
  return pimpl->selection_index_;
}

void CoverflowModel::SetSelection(size_t index)
{
  index = std::min<size_t>(index, pimpl->items_.size() - 1);
  if (index != pimpl->selection_index_)
  {
    pimpl->selection_index_ = index;
    selection_changed.emit(this, Selection());
  }
}

void CoverflowModel::SetSelection(CoverflowItem::Ptr item)
{
  SetSelection(IndexOf(item));
}

void CoverflowModel::SelectNext()
{
  SetSelection(pimpl->selection_index_ + 1);
}

void CoverflowModel::SelectPrev()
{
  if (pimpl->selection_index_ == 0)
    return;

  SetSelection(pimpl->selection_index_ - 1);
}


}
