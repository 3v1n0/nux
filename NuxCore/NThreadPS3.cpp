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


#include "NKernel.h"
#include "NThread.h"

#include <cell/atomic.h>

NAMESPACE_BEGIN

#ifdef _WIN32
    #define INL_ATOMOP_ITERLOCKED_INCREMENT      InterlockedIncrement
    #define INL_ATOMOP_ITERLOCKED_DECREMENT      InterlockedDecrement
    #define INL_ATOMOP_ITERLOCKED_EXCHANGED      InterlockedExchange
    #define INL_ATOMOP_ITERLOCKED_VALUE
#elif _WIN64
    #define INL_ATOMOP_ITERLOCKED_INCREMENT      InterlockedIncrement64
    #define INL_ATOMOP_ITERLOCKED_DECREMENT      InterlockedDecrement64
    #define INL_ATOMOP_ITERLOCKED_EXCHANGED      InterlockedExchange64
    #define INL_ATOMOP_ITERLOCKED_VALUE
#endif

t_u32 NThreadSafeCounter::Increment()
{
    return cellAtomicIncr32(&m_Counter);
}
t_u32 NThreadSafeCounter::Decrement()
{
    return cellAtomicDecr32(&m_Counter);
}
t_u32 NThreadSafeCounter::Set(t_u32 i)
{
    return cellAtomicStore32(&m_Counter, i);
}

t_u32 NThreadSafeCounter::GetValue() const
{
    return m_Counter;
}

t_u32 NThreadSafeCounter::operator ++ ()
{
    return Increment();
}

t_u32 NThreadSafeCounter::operator -- ()
{
    return Decrement();
}

t_bool NThreadSafeCounter::operator == (t_u32 i)
{
    return (m_Counter == i);
}

__thread void* NThreadLocalStorage::m_TLSIndex[NThreadLocalStorage::NbTLS];    
BOOL NThreadLocalStorage::m_TLSUsed[NThreadLocalStorage::NbTLS];
NThreadLocalStorage::TLS_ShutdownCallback  NThreadLocalStorage::m_TLSCallbacks[NThreadLocalStorage::NbTLS];

BOOL NThreadLocalStorage::RegisterTLS(t_u32 index, NThreadLocalStorage::TLS_ShutdownCallback shutdownCallback)
{
    nuxAssert(!m_TLSUsed[index]);

    if (!m_TLSUsed[index])
    {
//         m_TLSIndex[index] = TlsAlloc();
//         if(m_TLSIndex[index] == TLS_OUT_OF_INDEXES)
//         {
//             nuxAssertMsg(0, TEXT("[NThreadLocalStorage::RegisterTLS] Out of TLS index."));
//         }
        m_TLSUsed[index]  = TRUE;	
        m_TLSCallbacks[index] =  shutdownCallback;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void NThreadLocalStorage::Initialize()
{
    Memset(m_TLSUsed, 0, sizeof(m_TLSUsed));

    for (t_u32 i = 0; i < NThreadLocalStorage::NbTLS; i++)
    {
        // Fill the array with invalid values
        m_TLSIndex[i] = 0;
    }
}

void NThreadLocalStorage::Shutdown()
{
    ThreadShutdown();
}

void NThreadLocalStorage::ThreadInit()
{
}

void NThreadLocalStorage::ThreadShutdown()
{
    TLS_ShutdownCallback *callback = m_TLSCallbacks;
    for (t_u32 i = 0; i < NThreadLocalStorage::NbTLS; ++i, ++callback)
    {
        if (*callback)
        {
            (**callback)();
        }
    }
}

IMPLEMENT_ROOT_OBJECT_TYPE(NThread);

NThread::NThread()
:   m_ThreadState(THREADINIT)
{ 
    m_pThreadFunc = NThread::EntryPoint; // Can call Detach() also.
}

NThread::NThread(ThreadRoutineFunc lpExternalRoutine)
{
    Attach(lpExternalRoutine);
}

NThread::~NThread()
{
    sys_ppu_thread_detach(m_ThreadCtx.m_dwTID);
}

ThreadState NThread::Start( void* arg )
{
    m_ThreadCtx.m_pUserData = arg;
    if(sys_ppu_thread_create(&m_ThreadCtx.m_dwTID,
        m_pThreadFunc,
        (t_u64)0,
        0,
        4096,
        0,
        "inalogic_thread") == CELL_OK)
    {
        nuxDebugMsg(TEXT("[NThread::Start] Cannot start thread."));
        m_ThreadState = THREAD_START_ERROR;
        return m_ThreadState;
    }
    return m_ThreadState;
}

ThreadState NThread::Stop( bool bForceKill )
{
    sys_ppu_thread_detach(m_ThreadCtx.m_dwTID);

    m_ThreadState = THREADSTOP;
    return m_ThreadState;
}

ThreadState NThread::Suspend()
{
    m_ThreadState = THREADSUSPENDED;
    return m_ThreadState;
}

ThreadState NThread::Resume()
{
    return m_ThreadState;
}

// go from suspended to thread start
ThreadState NThread::ResumeStart()
{
    m_ThreadState = THREADINIT;
    return m_ThreadState;
}

// go from suspended to thread exit
ThreadState NThread::ResumeExit()
{
    m_ThreadState = THREADSTOP;
    return m_ThreadState;
}

void NThread::EntryPoint(t_u64 pArg)
{
    NThread *pParent = reinterpret_cast<NThread*>(pArg);
    if(!pParent->ThreadCtor())
    {
        // return another message saying the thread could not execute due to error in ThreadCtor;
    }
    pParent->Run( pParent->m_ThreadCtx.m_pUserData );
    pParent->ThreadDtor();
}

t_u32 NThread::GetExitCode() const 
{ 
    return m_ThreadCtx.m_dwExitCode;
}

t_u32 NThread::GetThreadId()
{
    return (t_u32)m_ThreadCtx.m_dwTID;
}

ThreadState NThread::GetThreadState() const
{
    return m_ThreadState;
}

void NThread::SetThreadState(ThreadState state)
{
    m_ThreadState = state;
}

NAMESPACE_END
