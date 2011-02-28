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

#include "Nux/VLayout.h"
#include "Nux/HLayout.h"
#include "Nux/ScrollView.h"
#include "Nux/PushButton.h"
#include "Nux/TextureArea.h"

static void OnFocusChildChanged (nux::Layout *layout, nux::Area *view)
{
  g_debug ("focus has changed, woo");
}

static void OnFocusChanged (nux::Area *view)
{
  g_debug ("focus on a specific child has changed");
  nux::TextureArea *texarea = (nux::TextureArea *)view;

  if (view->GetFocused ())
  {
    nux::ColorLayer color (nux::Color (0.2, 1.0, 0.2, 1.0));
    texarea->SetPaintLayer (&color);
  }
  else
  {
    nux::ColorLayer color (nux::Color (1.0, 0.2, 0.2, 1.0));
    texarea->SetPaintLayer (&color);
  }


}

void UserInterfaceInitialization(nux::NThread* thread, void* init_data)
{
  // Create a vertical Layout
  nux::VLayout* layout = new nux::VLayout(NUX_TRACKER_LOCATION);
  nux::HLayout* layout_top = new nux::HLayout(NUX_TRACKER_LOCATION);

  nux::ScrollView *layout_scroll = new nux::ScrollView (NUX_TRACKER_LOCATION);
  nux::HLayout* layout_bottom = new nux::HLayout(NUX_TRACKER_LOCATION);
  layout->AddLayout (layout_top, 0, nux::MINOR_POSITION_TOP);

  layout_scroll->SetLayout (layout_bottom);
  layout->AddView (layout_scroll, 0, nux::MINOR_POSITION_TOP);
  layout->ChildFocusChanged.connect (sigc::ptr_fun(&OnFocusChildChanged));

  //~ //Create a button of type PushButton
  //~ nux::PushButton* button = new nux::PushButton (TEXT ("Hello World!"), NUX_TRACKER_LOCATION);
//~
  //~ // Set the button maximum width/height
  //~ button->SetMaximumWidth (80);
  //~ button->SetMaximumHeight (40);
//~
  //~ // Add the button to the layout
  //~ layout->AddView (button, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
//~
  //~ //Create a button of type PushButton
  //~ button = new nux::PushButton (TEXT ("Button2"), NUX_TRACKER_LOCATION);
//~
  //~ // Set the button maximum width/height
  //~ button->SetMaximumWidth (80);
  //~ button->SetMaximumHeight (40);
//~
  //~ // Add the button to the layout
  //~ layout->AddView (button, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
//~
  //~ //Create a button of type PushButton
  //~ button = new nux::PushButton( TEXT ("Button 3"), NUX_TRACKER_LOCATION);
//~
  //~ // Set the button maximum width/height
  //~ button->SetMaximumWidth (80);
  //~ button->SetMaximumHeight (40);
//~
  //~ // Add the button to the layout
  //~ layout->AddView (button, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);

  for (int i = 0; i < 6; i++)
  {
    nux::ColorLayer color (nux::Color (0.2, 0.2, 0.2, 1.0));
    nux::TextureArea* texture_area = new nux::TextureArea ();
    texture_area->SetPaintLayer (&color);
    texture_area->FocusChanged.connect (sigc::ptr_fun (&OnFocusChanged));

    layout_top->AddView (texture_area, 1, nux::eLeft, nux::eFull);
  }

  for (int i = 0; i < 3; i++)
  {
    nux::ColorLayer color (nux::Color (0.2, 0.2, 0.4, 1.0));
    nux::TextureArea* texture_area = new nux::TextureArea ();
    texture_area->SetPaintLayer (&color);
    texture_area->FocusChanged.connect (sigc::ptr_fun (&OnFocusChanged));
  //~ //~
    layout_bottom->AddView (texture_area, 1, nux::eLeft, nux::eFull);
  }


  // Control the position of elements inside the layout
  layout->SetContentDistribution (nux::MAJOR_POSITION_CENTER);

  // Set the layout as the container of the window thread
  nux::GetWindowThread ()->SetLayout (layout);

  layout->SetFocused (true);

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
    TEXT("Push Button"),
    500,
    500,
    0,
    &UserInterfaceInitialization,
    0);

  // Start the main loop
  wt->Run (0);

  delete wt;
  return 0;
}
