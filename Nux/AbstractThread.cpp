/*
 * Copyright 2010 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */

#include "Nux.h"
#include "AbstractThread.h"

namespace nux
{
  class WindowThread;
  class SystemThread;

  NUX_IMPLEMENT_OBJECT_TYPE(AbstractThread);

  AbstractThread::AbstractThread(AbstractThread *Parent)
    :   parent_(Parent)
    ,   user_init_func_(NULL)
    ,   user_exit_func_(NULL)
  {

  }

  AbstractThread::~AbstractThread()
  {

  }

  void AbstractThread::TerminateChildWindows()
  {
    std::list<AbstractThread*>::iterator it;

    for (it = children_thread_list_.begin(); it != children_thread_list_.end(); it++)
    {
      if ((*it)->Type().IsObjectType(WindowThread::StaticObjectType))
      {
        // Exit the main loop of a window.
        static_cast<WindowThread*>(*it)->ExitMainLoop();
      }
    }
  }

  void AbstractThread::JoinChildThreads()
  {
#if defined(NUX_OS_WINDOWS)
    size_t sz = children_thread_list_.size();

    if (sz == 0)
    {
      return;
    }

    HANDLE *hdl = new HANDLE[sz];

    int i = 0;
    std::list<AbstractThread*>::iterator it;
    for (it = children_thread_list_.begin(); it != children_thread_list_.end(); ++it, ++i)
    {
      hdl[i] = (*it)->GetThreadHandle();
    }

    unsigned int result = WaitForMultipleObjects(sz, hdl, FALSE, INFINITE);
#else
  //#error WindowThread::JoinChildThreads has not been implemented for this platform.
  // Todo
#endif

    return;
  }

}
