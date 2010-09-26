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


#ifndef ABSTRACTTHREAD_H
#define ABSTRACTTHREAD_H

NAMESPACE_BEGIN_GUI

typedef void (*ThreadUserInitFunc)(NThread*, void* InitData);
typedef void (*ThreadUserExitFunc)(NThread*, void* ExitData);
typedef void (*ThreadMainFunc)(NThread*, void* MainData);

class AbstractThread: public NThread, public sigc::trackable
{
    NUX_DECLARE_OBJECT_TYPE(AbstractThread, NThread);

public:
    AbstractThread(AbstractThread* Parent);
    virtual ~AbstractThread();

protected:
    virtual unsigned int Run(void*) = 0;

    virtual ThreadState StartChildThread(NThread* thread, bool Modal) = 0;
    virtual void AddChildThread(NThread*) = 0;
    virtual void RemoveChildThread(NThread*) = 0;
    virtual void ChildHasFinished(NThread* app) = 0;
    virtual void TerminateAllChildThread() = 0;

    std::list<NThread*> m_ChildThread;
    AbstractThread* m_Parent;
    ThreadUserInitFunc m_UserInitFunc;
    ThreadUserExitFunc m_UserExitFunc;

    /*!
        This pointer maybe set by the user in ThreadInitFunc and reused in ThreadExitFunc
    */
    void* m_InitData;
    void* m_ExitData;

    std::list<NThread*> m_ThreadList;

private:
    AbstractThread(const AbstractThread&);
    // Does not make sense for a singleton. This is a self assignment.
    AbstractThread& operator=(const AbstractThread&);
    // Declare operator adress-of as private 
    AbstractThread* operator &();
};

NAMESPACE_END_GUI
#endif // ABSTRACTTHREAD_H

