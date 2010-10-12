/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#include "Nux.h"

#include "Layout.h"
#if defined(NUX_OS_WINDOWS)
#include "NuxGraphics/Gfx_OpenGL.h"
#elif defined(NUX_OS_LINUX)
#include "NuxGraphics/GfxSetupX11.h"
#endif
#include "NuxGraphics/OpenGLEngine.h"
#include "ClientArea.h"
#include "WindowCompositor.h"
#include "TimerProc.h"
#include "WindowCompositor.h"
#include "WindowThread.h"
#include "SystemThread.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (SystemThread);


  SystemThread::SystemThread (AbstractThread *Parent/* = 0*/)
    :   AbstractThread (Parent)
  {

  }

  SystemThread::~SystemThread()
  {
  }

  ThreadState SystemThread::Start ( void *arg )
  {
    if (!m_Parent)
    {
      return NThread::Start();
    }
    else
    {
      if (m_Parent->Type().IsObjectType (SystemThread::StaticObjectType) )
        return static_cast<SystemThread *> (m_Parent)->StartChildThread (this, true);

      if (m_Parent->Type().IsObjectType (WindowThread::StaticObjectType) )
        return static_cast<WindowThread *> (m_Parent)->StartChildThread (this, true);

      nuxAssertMsg (0, TEXT ("[WindowThread::Start] This should not happen.") );
      return THREAD_START_ERROR;
    }
  }

  UINT SystemThread::Run (void *arg)
  {
    if (m_UserInitFunc)
    {
      (*m_UserInitFunc) (this, m_InitData);
    }

    if (m_Parent)
    {
      if (m_Parent->Type().IsObjectType (SystemThread::StaticObjectType) )
        static_cast<SystemThread *> (m_Parent)->ChildHasFinished (this);

      if (m_Parent->Type().IsObjectType (WindowThread::StaticObjectType) )
        static_cast<WindowThread *> (m_Parent)->ChildHasFinished (this);
    }

    SetThreadState (THREADSTOP);
    TerminateAllChildThread();
    return 0;
  }

  ThreadState SystemThread::StartChildThread (NThread *thread, bool Modal)
  {
    ThreadState state = thread->NThread::Start();
    //if(state == THREADRUNNING)
    AddChildThread (thread);
    return state;
  }

  void SystemThread::AddChildThread (NThread *thread)
  {
    nuxAssert (thread);
    std::list<NThread *>::iterator it;
    it = find (m_ChildThread.begin(), m_ChildThread.end(), thread);

    if (it == m_ChildThread.end() )
    {
      m_ChildThread.push_back (thread);
    }
  }

  void SystemThread::RemoveChildThread (NThread *window)
  {
    nuxAssert (window);
    std::list<NThread *>::iterator it;
    it = find (m_ChildThread.begin(), m_ChildThread.end(), window);

    if (it != m_ChildThread.end() )
    {
      m_ChildThread.erase (it);
    }
  }

  void SystemThread::ChildHasFinished (NThread *thread)
  {
    RemoveChildThread (thread);

    if (thread->Type().IsObjectType (WindowThread::StaticObjectType) )
    {
      //SuspendChildGraphics(static_cast<WindowThread*>(thread));
    }

    thread->SetThreadState (THREADSTOP);
  }

  void SystemThread::TerminateAllChildThread()
  {
    std::list<NThread *>::iterator it;

    for (it = m_ChildThread.begin(); it != m_ChildThread.end(); it++)
    {
      (*it)->SetThreadState (THREADSTOP);
    }

    m_ChildThread.clear();
  }

  bool SystemThread::ThreadCtor()
  {
    SetThreadState (THREADRUNNING);

    return true;
  }

  bool SystemThread::ThreadDtor()
  {
    return true;
  }

}
