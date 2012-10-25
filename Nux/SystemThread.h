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


#ifndef SYSTEMTHREAD_H
#define SYSTEMTHREAD_H

namespace nux
{

  class WindowCompositor;
  class WindowThread;

  class SystemThread: public AbstractThread
  {
    NUX_DECLARE_OBJECT_TYPE(SystemThread, AbstractThread);
  public:
    SystemThread(AbstractThread *Parent = 0);
    ~SystemThread();

    virtual ThreadState Start(void *arg);

  protected:
    virtual int Run(void *arg);

    virtual ThreadState StartChildThread(AbstractThread *thread, bool Modal);
    virtual void AddChildThread(AbstractThread *);
    virtual void RemoveChildThread(AbstractThread *);
    virtual void ChildHasFinished(AbstractThread *app);
    virtual void TerminateChildThreads();

//    std::list<NThread*> m_ChildThread;
//    SystemThread* m_Parent;
//    ThreadUserInitFunc m_UserInitFunc;
//    ThreadUserExitFunc m_UserExitFunc;

    /*!
        Info: Constructor-like function.

        Will be called by EntryPoint before executing the thread body.
        Override this function to provide your extra initialization.

        NOTE: do not confuse it with the classes constructor
    */
    virtual bool ThreadCtor();

    /*!
        Info: Destructor-like function.

        Will be called by EntryPoint after executing the thread body.
        Override this function to provide your extra destruction.

        NOTE: do not confuse it with the classes constructor
    */
    virtual bool ThreadDtor();

    /*!
        This pointer maybe set by the user in ThreadInitFunc and reused in ThreadExitFunc
    */
  private:
    SystemThread(const SystemThread &);
    // Does not make sense for a singleton. This is a self assignment.
    SystemThread &operator= (const SystemThread &);
    // Declare operator adress-of as private
    SystemThread *operator &();

    std::string m_ThreadName;

    friend SystemThread *CreateSystemThread(AbstractThread *Parent, ThreadUserInitFunc UserInitFunc, void *InitData);
    
    friend class WindowThread;
  };

}

#endif // SYSTEMTHREAD_H
