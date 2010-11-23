/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef VSEPARATOR_H
#define VSEPARATOR_H

namespace nux
{

  class VSeparator: public AbstractSeparator
  {
  public:
    VSeparator();
    VSeparator (const Color &color, float Alpha0, float Alpha1, int Border);
    ~VSeparator();

    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw) {};
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw) {};

  protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

  };

}

#endif // VSEPARATOR_H
