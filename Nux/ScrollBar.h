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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef SCROLLBAR_H
#define SCROLLBAR_H

namespace nux
{

  class ScrollBar : public View //public ValuatorAbstraction
  {
  public:
    ScrollBar (NUX_FILE_LINE_PROTO);
    ~ScrollBar();

  private:
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw) = 0;
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw) = 0;
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw) = 0;

  public:
    int visible() const
    {
      return m_visible;
    }
    void clear_visible()
    {
      m_visible = 0;
    }
    void set_visible()
    {
      m_visible = 1;
    }
    void resize (int x, int y, int w, int h)
    {
      SetGeometry (Geometry (x, y, w, h) );
    }

    void SetScrollUnit (int unit);
    int GetScrollUnit() const;

  protected:
    int m_ScrollUnit;
    int m_visible;
  };


}

#endif // SCROLLBAR_H



