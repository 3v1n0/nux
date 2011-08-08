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
#include "Nux/CheckBox.h"
#include "Nux/ToggleButton.h"
#include "Nux/Button.h"
#include "Nux/TextureArea.h"

void UserInterfaceInitialization(nux::NThread* thread, void* init_data)
{
  // Create a vertical Layout
  nux::VLayout* layout = new nux::VLayout(NUX_TRACKER_LOCATION);

  //Create a button of type Button
  nux::Button* button = new nux::Button ("Party on Garth", NUX_TRACKER_LOCATION);

  // Add the button to the layout
  layout->AddView (
    button,
    0,
    nux::MINOR_POSITION_CENTER,
    nux::MINOR_SIZE_MATCHCONTENT);

  // Create a button with an image
  nux::ColorLayer color (nux::Color (0.6, 0.4, 0.7, 1.0));
  nux::TextureArea* texture_area = new nux::TextureArea ();
  texture_area->SetPaintLayer (&color);

  nux::Button* button_with_image = new nux::Button("Party on Wayne", texture_area, NUX_TRACKER_LOCATION);
  //button_with_image->image_position = nux::NUX_POSITION_BOTTOM;

  // Add the button to the layout
  layout->AddView (
    button_with_image,
    0,
    nux::MINOR_POSITION_CENTER,
    nux::MINOR_SIZE_MATCHCONTENT);

  color = nux::Color (0.6, 0.4, 0.7, 1.0);
  texture_area = new nux::TextureArea ();
  texture_area->SetPaintLayer (&color);

  nux::Button* button_without_image = new nux::Button(texture_area, NUX_TRACKER_LOCATION);

  // Add the button to the layout
  layout->AddView (
    button_without_image,
    0,
    nux::MINOR_POSITION_CENTER,
    nux::MINOR_SIZE_MATCHCONTENT);

  nux::ToggleButton *toggle_button = new nux::ToggleButton ("This is a Toggle button, nux just doesn't have a theme for that", NUX_TRACKER_LOCATION);
  layout->AddView (
    toggle_button,
    0,
    nux::MINOR_POSITION_CENTER,
    nux::MINOR_SIZE_MATCHCONTENT);

  nux::CheckBox *check_button = new nux::CheckBox ("Check box widget? Check!", NUX_TRACKER_LOCATION);
  layout->AddView (
    check_button,
    0,
    nux::MINOR_POSITION_CENTER,
    nux::MINOR_SIZE_MATCHCONTENT);

  // Control the position of elements inside the layout
  layout->SetContentDistribution (nux::MAJOR_POSITION_CENTER);

  // Set the layout as the container of the window thread
  nux::GetWindowThread ()->SetLayout (layout);

  // Set the background color of the window
  nux::ColorLayer background (nux::Color (0xFF222222));
  static_cast<nux::WindowThread*> (thread)->SetWindowBackgroundPaintLayer(&background);
}

int main(int argc, char **argv)
{
  // Initialize Nux subsystem
  nux::NuxInitialize (0);

  // Create a Window thread
  nux::WindowThread* wt = nux::CreateGUIThread(
    TEXT("Button"),
    800,
    600,
    0,
    &UserInterfaceInitialization,
    0);

  // Start the main loop
  wt->Run (0);

  delete wt;
  return 0;
}
