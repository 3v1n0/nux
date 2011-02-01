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
 * Authored by: Neil Jagdish Patel <neil.patel@canonical.com>
 *
 */

#include "Nux/Nux.h"
#include "Nux/WindowThread.h"
#include "NuxGraphics/GraphicsEngine.h"

#include "Nux/TextureArea.h"
#include "Nux/HLayout.h"
#include "Nux/VLayout.h"
#include "Nux/LayeredLayout.h"


void LayeredLayoutInit(nux::NThread* thread, void* InitData)
{
  nux::HLayout *main_layout((new nux::HLayout(NUX_TRACKER_LOCATION)));

  nux::LayeredLayout *layered_layout ((new nux::LayeredLayout (NUX_TRACKER_LOCATION)));
  for (int i = 0; i < 60; i++)
  {
    nux::ColorLayer color (nux::Color::RandomColor ());
    nux::TextureArea* texture_area = new nux::TextureArea ();
    texture_area->SetPaintLayer (&color);

    layered_layout->AddView (texture_area, 1, nux::eLeft, nux::eFull);
  }

  layered_layout->ForceChildrenSize (true);
  layered_layout->SetChildrenSize (64, 42);
  layered_layout->EnablePartialVisibility (false);

  layered_layout->SetVerticalExternalMargin (4);
  layered_layout->SetHorizontalExternalMargin (4);
  layered_layout->SetVerticalInternalMargin (4);
  layered_layout->SetHorizontalInternalMargin (4);


  main_layout->AddLayout (layered_layout, 1);
  
  nux::GetWindowThread ()->SetLayout(main_layout);
}

int main(int argc, char **argv)
{
  nux::NuxInitialize(0);

  nux::WindowThread* wt = nux::CreateGUIThread(TEXT("Layered Layout"), 400, 200, 0, &LayeredLayoutInit, 0);
  wt->Run(NULL);

  delete wt;

  return 0;
}
