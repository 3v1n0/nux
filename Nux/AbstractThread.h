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


#ifndef ABSTRACTTHREAD_H
#define ABSTRACTTHREAD_H

namespace nux
{

  typedef void(*ThreadUserInitFunc) (NThread *, void *InitData);
  typedef void(*ThreadUserExitFunc) (NThread *, void *ExitData);
  typedef void(*ThreadMainFunc) (NThread *, void *MainData);

  class AbstractThread: public NThread, public sigc::trackable
  {
    NUX_DECLARE_OBJECT_TYPE(AbstractThread, NThread);

  public:
    AbstractThread(AbstractThread *Parent);
    virtual ~AbstractThread();

  protected:
    virtual int Run(void *) = 0;

    virtual ThreadState StartChildThread(AbstractThread *thread, bool Modal) = 0;
    virtual void AddChildThread(AbstractThread *) = 0;
    virtual void RemoveChildThread(AbstractThread *) = 0;
    virtual void ChildHasFinished(AbstractThread *app) = 0;
    virtual void TerminateChildThreads() = 0;
    void TerminateChildWindows();
    void JoinChildThreads();

    std::list<AbstractThread*> children_thread_list_;
    AbstractThread *parent_;
    ThreadUserInitFunc user_init_func_;
    ThreadUserExitFunc user_exit_func_;

    /*!
        This pointer maybe set by the user in ThreadInitFunc and reused in ThreadExitFunc
    */
    void *initialization_data_;

    std::list<NThread *> m_ThreadList;

  private:
    AbstractThread(const AbstractThread &);
    // Does not make sense for a singleton. This is a self assignment.
    AbstractThread &operator= (const AbstractThread &);
    // Declare operator adress-of as private
    AbstractThread *operator &();
  };

}
#endif // ABSTRACTTHREAD_H

