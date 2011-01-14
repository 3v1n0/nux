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
#include "Nux/TimeGraph.h"
#include "Nux/TimerProc.h"


nux::TimerFunctor *timer_functor;
nux::TimerHandle timer_handler;
float time_value = 0;

void GraphTimerInterrupt (void *data)
{
  time_value += 0.001f;
  nux::TimeGraph* timegraph = NUX_STATIC_CAST (nux::TimeGraph*, data);
  
  for (int i = 0; i < 3; i++)
  {
    if (i == 0)
      timegraph->UpdateGraph (i, nux::GetWindowThread ()->GetFrameRate() );

    if (i == 1)
      timegraph->UpdateGraph (i, nux::RandomUInt (25) + 25);

    if (i == 2)
      timegraph->UpdateGraph (i, 30 * (std::sin (time_value) + 1) + nux::RandomUInt (10) );
  }
    
  timer_handler = nux::GetTimer().AddTimerHandler (100, timer_functor, timegraph);
}

void UserInterfaceInitialization(nux::NThread* thread, void* init_data)
{
  // Create a vertical Layout
  nux::VLayout* layout = new nux::VLayout(NUX_TRACKER_LOCATION);

  nux::TimeGraph* timegraph = new nux::TimeGraph(TEXT("Graph"));
  timegraph->ShowColumnStyle ();
  timegraph->SetYAxisBounds (0.0, 200.0f);

  timegraph->AddGraph (nux::Color (0xFF9AD61F), nux::Color (0x50191919) );
  timegraph->AddGraph (nux::Color (0xFF00FF00), nux::Color (0x5000FF00) );
  timegraph->AddGraph (nux::Color (0xFFFF0022), nux::Color (0x50BB0022) );

  timer_functor = new nux::TimerFunctor ();
  timer_functor->OnTimerExpired.connect (sigc::ptr_fun (&GraphTimerInterrupt));
  timer_handler = nux::GetTimer ().AddTimerHandler (1000, timer_functor, timegraph);

  // Add the timegraph to the layout
  layout->AddView (
    timegraph,
    1,
    nux::MINOR_POSITION_CENTER,
    nux::MINOR_SIZE_FULL);

  // Control the position of elements inside the layout
  layout->SetContentDistribution (nux::MAJOR_POSITION_CENTER);

  layout->SetHorizontalExternalMargin (4);
  layout->SetVerticalExternalMargin (4);
  
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
    TEXT("Time Graph"),
    300,
    200,
    0,
    &UserInterfaceInitialization,
    0);

  // Start the main loop
  wt->Run (0);

  delete timer_functor;
  delete wt;
  return 0;
}
