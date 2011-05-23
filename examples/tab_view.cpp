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
#include "Nux/VLayout.h"
#include "Nux/WindowThread.h"
#include "Nux/TabView.h"
#include "Nux/TextureArea.h"
#include "Nux/PaintLayer.h"

void UserInterfaceInitialization(nux::NThread* thread, void* init_data)
{
  // Create a vertical Layout
  nux::VLayout* layout = new nux::VLayout(NUX_TRACKER_LOCATION);
  
  nux::TabView* tabview(new nux::TabView());
  
  nux::ColorLayer color_layer (nux::color::Black, true);
  nux::VLayout *tab_view_layout = 0;
  nux::TextureArea *texture_area = 0;

  for (int i = 0; i < 50; i++)
  {
    tab_view_layout = new nux::VLayout ();
    texture_area = new nux::TextureArea ();
    color_layer.SetColor (nux::color::RandomColor ());
    texture_area->SetPaintLayer (&color_layer);

    tab_view_layout->AddView (texture_area, 1);

    nux::NString str = nux::NString::Printf("View %d", i);

    tabview->AddTab (str.GetTCharPtr (), tab_view_layout);
  }

  // Add the button to the layout
  layout->AddView (
    tabview,
    1,
    nux::MINOR_POSITION_CENTER,
    nux::MINOR_SIZE_FULL);

  // Set the layout as the container of the window thread
  nux::GetWindowThread ()->SetLayout (layout);

  // Set the background color of the window
  nux::ColorLayer background (nux::Color (0xFF2D2D2D));
  static_cast<nux::WindowThread*> (thread)->SetWindowBackgroundPaintLayer(&background);
}

int main(int argc, char **argv)
{
  // Initialize Nux subsystem
  nux::NuxInitialize (0);

  // Create a Window thread
  nux::WindowThread* wt = nux::CreateGUIThread(
    TEXT("Tab View"),
    400,
    300,
    0,
    &UserInterfaceInitialization,
    0);

  // Start the main loop
  wt->Run (0);

  delete wt;
  return 0;
}
