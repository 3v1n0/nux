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
#include "Nux/PushButton.h"


void UserInterfaceInitialization(nux::NThread* thread, void* init_data)
{
  // Create a vertical Layout
  nux::VLayout* layout = new nux::VLayout(NUX_TRACKER_LOCATION);
  
  //Create a button of type PushButton
  nux::PushButton* button = new nux::PushButton(
    TEXT ("Hello World!"),
    NUX_TRACKER_LOCATION);

  // Set the button maximum width/height
  button->SetMaximumWidth (80);
  button->SetMaximumHeight (40);
  button->SetTextColor (nux::color::Black);

  // Add the button to the layout
  layout->AddView (
    button,
    1,
    nux::MINOR_POSITION_CENTER,
    nux::MINOR_SIZE_FULL);

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
    TEXT("Push Button"),
    200,
    150,
    0,
    &UserInterfaceInitialization,
    0);

  // Start the main loop
  wt->Run (0);

  delete wt;
  return 0;
}
