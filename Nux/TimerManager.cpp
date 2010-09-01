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
#include "TimerManager.h"

NAMESPACE_BEGIN_GUI

static const t_u32 sBase_ID = 0x1234;
TimerManagerImpl* TimerManagerImpl::m_pInstance = 0;
t_u32 TimerManagerImpl::ID = 0;

TimerManagerImpl::TimerManagerImpl()
{
	ID = 0;
}

TimerManagerImpl& TimerManagerImpl::Instance()
{
    if(m_pInstance == 0)
    {
        m_pInstance = new TimerManagerImpl();
    }
    return *m_pInstance;
}

t_u32 TimerManagerImpl::CreateTimer()
{
    TimeDelay* td = new TimeDelay;
    //td->SetID(ID++);
    //m_TimeDelayList.push_back(td);

	t_u32 vector_size = (t_u32)m_TimeDelayVector.size();
	t_u32 new_index = -1;
	for(t_u32 i = 0; i < vector_size; i++)
	{
		if(m_TimeDelayVector[i] == 0)
		{
			new_index = i;
		}
	}
	if(new_index != -1)
	{
		m_TimeDelayVector[new_index] = td;
		td->SetID(new_index);
		return new_index + sBase_ID;
	}
	else
	{
		m_TimeDelayVector.push_back(td);
		new_index = vector_size;
		td->SetID(new_index);
		return new_index + sBase_ID;
	}

    //return td->GetID() + Base_ID;
}

void TimerManagerImpl::DestroyTimer(t_u32 id)
{
//    std::list<TimeDelay*>::iterator it;
//    for(it = m_TimeDelayList.begin(); it != m_TimeDelayList.end(); it++)
//    {
//        if((*it)->GetID() == id)
//        {
//            delete (*it);
//            m_TimeDelayList.erase(it);
//            return;
//        }
//    }

	t_u32 vector_size = (t_u32)m_TimeDelayVector.size();
	if(id - sBase_ID >= vector_size)
	{
		return;
	}
	TimeDelay *td = m_TimeDelayVector[id - sBase_ID];
	m_TimeDelayVector[id - sBase_ID] = 0;
	delete td;
}

void TimerManagerImpl::Update(float ms_dt)
{
//    std::list<TimeDelay*>::iterator it;
//    for(it = m_TimeDelayList.begin(); it != m_TimeDelayList.end(); it++)
//    {
//        (*it)->Update(ms_dt);
//    }

	t_u32 vector_size = (t_u32)m_TimeDelayVector.size();
	for(t_u32 i = 0; i < vector_size; i++)
	{
        if(m_TimeDelayVector[i])
		    m_TimeDelayVector[i]->Update(ms_dt);
	}
}

void TimerManagerImpl::Start(t_u32 id)
{
//    std::list<TimeDelay*>::iterator it;
//    for(it = m_TimeDelayList.begin(); it != m_TimeDelayList.end(); it++)
//    {
//        if((*it)->GetID() == id)
//            (*it)->Start();
//    }

	t_u32 vector_size = (t_u32)m_TimeDelayVector.size();
	if(id - sBase_ID >= vector_size)
	{
		return;
	}
	m_TimeDelayVector[id - sBase_ID]->Start();
}
void TimerManagerImpl::Stop(t_u32 id)
{    
//    std::list<TimeDelay*>::iterator it;
//    for(it = m_TimeDelayList.begin(); it != m_TimeDelayList.end(); it++)
//    {
//        if((*it)->GetID() == id)
//            (*it)->Stop();
//    }

	t_u32 vector_size = (t_u32)m_TimeDelayVector.size();
	if(id - sBase_ID >= vector_size)
	{
		return;
	}
	m_TimeDelayVector[id - sBase_ID]->Stop();
}
void TimerManagerImpl::ResetTimer(t_u32 id)
{
//    std::list<TimeDelay*>::iterator it;
//    for(it = m_TimeDelayList.begin(); it != m_TimeDelayList.end(); it++)
//    {
//        if((*it)->GetID() == id)
//            (*it)->Reset();
//    }

	t_u32 vector_size = (t_u32)m_TimeDelayVector.size();
	if(id - sBase_ID >= vector_size)
	{
		return;
	}
	m_TimeDelayVector[id - sBase_ID]->Reset();
}

bool TimerManagerImpl::IsExpired(t_u32 id)
{
//    std::list<TimeDelay*>::iterator it;
//	int s = m_TimeDelayList.size();
//    for(it = m_TimeDelayList.begin(); it != m_TimeDelayList.end(); it++)
//    {
//        if((*it)->GetID() == id)
//            return (*it)->IsExpired();
//    }
//    return false;


	t_u32 vector_size = (t_u32)m_TimeDelayVector.size();
	if(id - sBase_ID >= vector_size)
	{
		return false;
	}
	return m_TimeDelayVector[id - sBase_ID]->IsExpired();
}

bool TimerManagerImpl::IsRunning(t_u32 id)
{
//    std::list<TimeDelay*>::iterator it;
//    for(it = m_TimeDelayList.begin(); it != m_TimeDelayList.end(); it++)
//    {
//        if((*it)->GetID() == id)
//            return (*it)->IsRunning();
//    }
//    return false;

	t_u32 vector_size = (t_u32)m_TimeDelayVector.size();
	if(id - sBase_ID >= vector_size)
	{
		return false;
	}
	return m_TimeDelayVector[id - sBase_ID]->IsRunning();
}

bool TimerManagerImpl::IsStop(t_u32 id)
{
//    std::list<TimeDelay*>::iterator it;
//    for(it = m_TimeDelayList.begin(); it != m_TimeDelayList.end(); it++)
//    {
//        if((*it)->GetID() == id)
//            return (*it)->IsStop();
//    }
//    return false;

	t_u32 vector_size = (t_u32)m_TimeDelayVector.size();
	if(id - sBase_ID >= vector_size)
	{
		return false;
	}
	return m_TimeDelayVector[id - sBase_ID]->IsStop();
}

void TimerManagerImpl::SetDelay(t_u32 id, float ms_dt)
{
//    std::list<TimeDelay*>::iterator it;
//    for(it = m_TimeDelayList.begin(); it != m_TimeDelayList.end(); it++)
//    {
//        if((*it)->GetID() == id)
//            return (*it)->SetDelay(ms_dt);
//    }

	t_u32 vector_size = (t_u32)m_TimeDelayVector.size();
	if(id - sBase_ID >= vector_size)
	{
		return;
	}
	m_TimeDelayVector[id - sBase_ID]->SetDelay(ms_dt);
}


TimeDelay::TimeDelay()
:m_delay(0),
m_state(eSTOP),
m_id(-1)
{
}

TimeDelay::~TimeDelay()
{
}

void TimeDelay::Start()
{
    if(m_delay > 0)
    {
        if(m_state == eSTOP)
        {
            m_state = eRUNNING;
            m_countdown = m_delay;
        }
    }
}

void TimeDelay::SetDelay(float ms_dt)
{
    m_delay = ms_dt;
}

void TimeDelay::Stop()
{
    m_state = eSTOP;
    m_countdown = 0;
}

void TimeDelay::Reset()
{
    Stop();
}

void TimeDelay::Update(float ms_dt)
{
    if(m_state == eRUNNING)
    {
        m_countdown -= ms_dt;
        if(m_countdown < 0)
        {
            m_state = eEXPIRED;
        }
    }
}

bool TimeDelay::IsExpired()
{
    if(m_state == eEXPIRED)
        return true;
    return false;
}

bool TimeDelay::IsRunning()
{
    if(m_state == eRUNNING)
        return true;
    return false;
}

bool TimeDelay::IsStop()
{
    if(m_state == eSTOP)
        return true;
    return false;
}

void TimeDelay::SetID(t_u32 id)
{
    m_id = id;
}

t_u32 TimeDelay::GetID()
{
    return m_id;
}


// TimeCounter::TimeCounter()
// {
//     LARGE_INTEGER freq;
//     QueryPerformanceFrequency(&freq);
//     QueryPerformanceCounter(&now);
//     res = (float) (1.0f / (double) freq.QuadPart);  
// }
// TimeCounter::~TimeCounter()
// {
// }
// 
// void TimeCounter::Update(void)
// {
//     QueryPerformanceCounter(&now);
// }
// 
// float TimeCounter::Passed(void)
// {
//     LARGE_INTEGER temp;
//     QueryPerformanceCounter(&temp);
//     return (temp.QuadPart - now.QuadPart) * res * 1000.0f;
// }
// 
// float TimeCounter::PassedFrac(void)
// {
//     LARGE_INTEGER temp;
//     QueryPerformanceCounter(&temp);
//     return (temp.QuadPart - now.QuadPart) * res;
// }

NAMESPACE_END_GUI
