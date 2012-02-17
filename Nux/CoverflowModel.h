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

#ifndef NUX_COVERFLOWMODEL_H
#define NUX_COVERFLOWMODEL_H

#include <memory>
#include <vector>

#include <sigc++/sigc++.h>
#include "CoverflowItem.h"

namespace nux
{

class CoverflowModel : public InitiallyUnownedObject
{
  NUX_DECLARE_OBJECT_TYPE(CoverflowModel, InitiallyUnownedObject);
public:
  typedef ObjectPtr<CoverflowModel> Ptr;
  typedef std::vector<CoverflowItem::Ptr> CoverflowItemList;

  CoverflowModel();
  ~CoverflowModel();

  CoverflowItemList const& Items() const;

  void AddItem(CoverflowItem::Ptr const& item);
  void InsertItem(CoverflowItem::Ptr const& item, size_t index);
  void RemoveItem(CoverflowItem::Ptr const& item);

  size_t IndexOf(CoverflowItem::Ptr const& item);

  CoverflowItem::Ptr Selection();
  size_t SelectionIndex();

  void SetSelection(size_t index);
  void SetSelection(CoverflowItem::Ptr item);

  void SelectNext();
  void SelectPrev();

  sigc::signal<void, CoverflowModel*, CoverflowItem::Ptr> item_added;
  sigc::signal<void, CoverflowModel*, CoverflowItem::Ptr> item_removed;
  sigc::signal<void, CoverflowModel*, CoverflowItem::Ptr> selection_changed;

private:
  struct Impl;
  Impl* pimpl;
};

}

#endif
