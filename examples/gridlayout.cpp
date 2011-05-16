/*
 * Copyright 2010 Inalogic Inc.
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

#include "Nux/Nux.h"
#include "Nux/WindowThread.h"
#include "NuxGraphics/GraphicsEngine.h"

#include "Nux/TextureArea.h"
#include "Nux/HLayout.h"
#include "Nux/VLayout.h"
#include "Nux/GridHLayout.h"
#include "Nux/GridVLayout.h"


void GridLayoutInit(nux::NThread* thread, void* InitData)
{
  int a = 0;

  nux::HLayout *main_layout((new nux::HLayout(NUX_TRACKER_LOCATION)));

  nux::GridHLayout *grid_h_layout ((new nux::GridHLayout (NUX_TRACKER_LOCATION)));
  for (int i = 0; i < 30; i++)
  {
    nux::ColorLayer color (nux::color::RandomColor ());
    nux::TextureArea* texture_area = new nux::TextureArea ();
    texture_area->SetPaintLayer (&color);
    texture_area->SetVisible (a % 2);
    grid_h_layout->AddView (texture_area, 1, nux::eLeft, nux::eFull);

    a++;
  }

  grid_h_layout->ForceChildrenSize (true);
  grid_h_layout->SetChildrenSize (64, 42);
  grid_h_layout->EnablePartialVisibility (false);

  grid_h_layout->SetVerticalExternalMargin (4);
  grid_h_layout->SetHorizontalExternalMargin (4);
  grid_h_layout->SetVerticalInternalMargin (4);
  grid_h_layout->SetHorizontalInternalMargin (4);


  nux::GridVLayout *grid_v_layout ((new nux::GridVLayout (NUX_TRACKER_LOCATION)));
  for (int i = 0; i < 30; i++)
  {
    nux::ColorLayer color (nux::color::RandomColor());
    nux::TextureArea* texture_area = new nux::TextureArea();
    texture_area->SetPaintLayer (&color);
    texture_area->SetVisible (a % 2);
    grid_v_layout->AddView(texture_area, 1, nux::eLeft, nux::eFull);

    a++;
  }

  grid_v_layout->ForceChildrenSize (true);
  grid_v_layout->SetChildrenSize (64, 64);
  grid_v_layout->EnablePartialVisibility (false);

  grid_v_layout->SetVerticalExternalMargin (6);
  grid_v_layout->SetHorizontalExternalMargin (6);
  grid_v_layout->SetVerticalInternalMargin (6);
  grid_v_layout->SetHorizontalInternalMargin (6);


  main_layout->AddLayout (grid_h_layout, 1);
  main_layout->AddLayout (grid_v_layout, 1);
  
  nux::GetWindowThread ()->SetLayout(main_layout);
}

int main(int argc, char **argv)
{
  nux::NuxInitialize(0);

  nux::WindowThread* wt = nux::CreateGUIThread(TEXT("Grid Layout"), 1024, 600, 0, &GridLayoutInit, 0);
  wt->Run(NULL);

  delete wt;

  return 0;
}
