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

nux::TimeOutSignal *timeout_signal = NULL;

void TimeOutCallback(void *data)
{
  nux::WindowThread *thread = static_cast<nux::WindowThread*>(data);
  thread->ExitMainLoop();
}

void UserInterfaceInitialization(nux::NThread* thread, void* /* init_data */)
{

  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(thread)->SetWindowBackgroundPaintLayer(&background);

  timeout_signal = new nux::TimeOutSignal();
  timeout_signal->tick.connect(sigc::ptr_fun(TimeOutCallback));
  static_cast<nux::WindowThread*>(thread)->GetTimerHandler().AddOneShotTimer(1000, timeout_signal, thread, NULL);
}

int main()
{
  // Initialize Nux subsystem
  nux::NuxInitialize (0);

  // Create a Window thread
  nux::WindowThread* wt = nux::CreateGUIThread(
    TEXT("Empty Window Test"),
    300,
    200,
    0,
    &UserInterfaceInitialization,
    0);

  // // Start the main loop
  wt->Run (0);

  delete wt;
  return 0;
}
