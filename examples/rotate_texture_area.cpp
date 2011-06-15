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
#include "Nux/TextureArea.h"
#include "Nux/TimerProc.h"

nux::TimerFunctor *timer_functor = 0;
nux::TimerHandle timer_handle;
float angle = 0.0f;

void UpdateAngle (void *v)
{
  angle += 0.1f;
  if (angle >= nux::Const::pi)
  {
    angle = 0.0f;
  }

  nux::TextureArea* texture_area = NUX_STATIC_CAST (nux::TextureArea*, v);
  if (texture_area)
  {
    texture_area->Set2DRotation (angle);
    timer_handle = nux::GetTimer().AddTimerHandler (10, timer_functor, texture_area);
    if (angle == 0)
    {
      nux::ColorLayer color_layer (nux::color::RandomColor ());
      texture_area->SetPaintLayer (&color_layer);
    }
  }
}

void UserInterfaceInitialization(nux::NThread* thread, void* init_data)
{
  // Create a vertical Layout
  nux::VLayout* layout = new nux::VLayout(NUX_TRACKER_LOCATION);
  
  //Create a button of type PushButton
  nux::TextureArea* texture_area = new nux::TextureArea(NUX_TRACKER_LOCATION);

  // Set the button maximum width/height
  texture_area->SetMaximumWidth (160);
  texture_area->SetMaximumHeight (80);
  texture_area->SetTextColor (nux::color::Pink);
  texture_area->Set2DRotation (0.0);

  // Add the button to the layout
  layout->AddView (
    texture_area,
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

  timer_functor = new nux::TimerFunctor;
  timer_functor->OnTimerExpired.connect (sigc::ptr_fun (&UpdateAngle));
  timer_handle = nux::GetTimer().AddTimerHandler (5, timer_functor, texture_area);

}

int main(int argc, char **argv)
{
  // Initialize Nux subsystem
  nux::NuxInitialize (0);

  // Create a Window thread
  nux::WindowThread* wt = nux::CreateGUIThread(
    TEXT("Rotate Texture Area"),
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
