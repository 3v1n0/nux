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
 * Authored by: Neil Jagdish Patel <neil.patel@canonical.com>
 *
 */


#ifndef GRIDHLAYOUT_H
#define GRIDHLAYOUT_H

#include "Layout.h"

namespace nux
{
  //! An layered bin layout.
  /*!
      All children are the size of the view this layout is attached to. Only one active child,
      which is painted and receives the events
  */
  class LayeredLayout: public Layout
  {
    NUX_DECLARE_OBJECT_TYPE (LayeredLayout, Layout);

  public:

    typedef enum
    {
      INPUT_MODE_ACTIVE = 0,
      INPUT_MODE_COMPOSITE

    } InputMode;

    LayeredLayout (NUX_FILE_LINE_PROTO);
    ~LayeredLayout ();

    // LayeredLayout methods
    void   SetActiveLayer (int index_);
    int    GetActiveLayer ();
    void   SetActiveArea  (Area *area);
    Area * GetActiveArea  ();

    void SetPaintAll (bool paint_all);
    bool GetPaintAll ();

    void                     SetInputMode (LayeredLayout::InputMode input_mode);
    LayeredLayout::InputMode GetInputMode ();

    // Overrides
    long ComputeLayout2 ();
    void GetCompositeList (std::list<Area *> *ViewList);
    void ProcessDraw (GraphicsEngine &GfxContext, bool force_draw);
    long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    void AddLayout (Layout                *layouy,
                    unsigned int           stretch_factor = 1,
                    MinorDimensionPosition position       = eAbove,
                    MinorDimensionSize     extend         = eFull,
                    float                  percentage     = 100.0f);
    void AddView (Area                  *view,
                  unsigned int           stretch_factor = 1,
                  MinorDimensionPosition positioning    = eAbove,
                  MinorDimensionSize     extend         = eFull,
                  float                  percentage     = 100.0f);
    void RemoveChildObject (Area *area);
    void Clear ();

  private:
    void PaintOne (Area *area, GraphicsEngine &GfxContext, bool force_draw);
  
  private:
    int                      m_active_index;
    Area                    *m_active_area;
    bool                     m_paint_all;
    LayeredLayout::InputMode m_input_mode;
  };
}

#endif // GRIDHLAYOUT_H
