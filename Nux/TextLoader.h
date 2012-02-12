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

#ifndef NUX_TEXTLOADER_H
#define NUX_TEXTLOADER_H

#include <Nux/Nux.h> // FIXME

namespace nux {

class TextLoader
{
public:
  enum TextAlignment
  {
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT,
  };

  TextLoader();
  ~TextLoader();

  Property<TextAlignment> alignment;

  Property<Color> color;

  Property<std::string> font_name;

  Property<std::string> text;
  
  Property<int> font_size;

  Property<int> width;

  Property<int> minimum_width;

  Property<int> lines;

  ObjectPtr<BaseTexture> CreateTexture();

private:
  struct Impl;
  Impl* pimpl;
};

}

#endif