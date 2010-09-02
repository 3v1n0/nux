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


#ifndef SYSTEMTHREAD_H
#define SYSTEMTHREAD_H

NAMESPACE_BEGIN_GUI

class WindowCompositor;
class WindowThread;

class SystemThread: public AbstractThread
{
    DECLARE_OBJECT_TYPE(SystemThread, AbstractThread);
public:
    SystemThread(AbstractThread* Parent = 0);
    ~SystemThread();

    virtual ThreadState Start(void* arg);

protected:
    virtual unsigned int Run(void* arg);

    virtual ThreadState StartChildThread(NThread* thread, bool Modal);
    virtual void AddChildThread(NThread*);
    virtual void RemoveChildThread(NThread*);
    virtual void ChildHasFinished(NThread* app);
    virtual void TerminateAllChildThread();

//    std::list<NThread*> m_ChildThread;
//    SystemThread* m_Parent;
//    ThreadUserInitFunc m_UserInitFunc;
//    ThreadUserExitFunc m_UserExitFunc;

    /*!
    This pointer maybe set by the user in ThreadInitFunc and reused in ThreadExitFunc
    */
    void* m_InitData;
    void* m_ExitData;

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
    SystemThread(const SystemThread&);
    // Does not make sense for a singleton. This is a self assignment.
    SystemThread& operator=(const SystemThread&);
    // Declare operator adress-of as private 
    SystemThread* operator &();

private:
    NString m_ThreadName;

    friend class GfxServerImpl;
    friend class WindowThread;
};

NAMESPACE_END_GUI

#endif // SYSTEMTHREAD_H