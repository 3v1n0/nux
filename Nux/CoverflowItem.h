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

#ifndef NUX_COVERFLOWITEM_H
#define NUX_COVERFLOWITEM_H

#include <memory>
#include <string>

#include "Nux.h"
#include <sigc++/sigc++.h>

namespace nux
{

class CoverflowItem : public InitiallyUnownedObject
{
  NUX_DECLARE_OBJECT_TYPE(CoverflowItem, InitiallyUnownedObject);
public:
  typedef ObjectPtr<CoverflowItem> Ptr;

  CoverflowItem(std::string const& name);

  Property<std::string> name;
  Property<ObjectPtr<BaseTexture> > text_texture;

  virtual ObjectPtr<BaseTexture> GetTexture() const = 0;
  virtual void Activate(int /* button */) {}
};

}

#endif
