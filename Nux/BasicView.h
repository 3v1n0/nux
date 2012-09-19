/*
 * Copyright 2012 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */

#ifndef BASIC_VIEW_H
#define BASIC_VIEW_H


namespace nux {

//! A very basic View object with no rendering.
class BasicView: public nux::View
{
  NUX_DECLARE_OBJECT_TYPE(BasicView, View);
public:
  BasicView(NUX_FILE_LINE_PROTO);
  ~BasicView();

  
protected:
  void Draw(nux::GraphicsEngine& graphics_engine, bool force_draw);
};

}
#endif // BASIC_VIEW_H

