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


#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H

NAMESPACE_BEGIN_GUI

typedef t_u32 HTimer;
class TimeDelay
{
public:
    TimeDelay();
    ~TimeDelay();

    void Update(float ms_dt);
    bool IsExpired();
    bool IsRunning();
    bool IsStop();
    void Start();
    void Stop();
    void SetDelay(float ms_delay);
    void Reset();

    float m_delay;
    float m_countdown;

    t_u32 GetID();
private:
    void SetID(t_u32 id);
    typedef enum 
    {       
        eSTOP,
        eRUNNING,
        eEXPIRED
    } TimeDelayState;
    TimeDelayState m_state;
    t_u32 m_id;
    friend class TimerManagerImpl;
};


class TimerManagerImpl
{
public:
    t_u32 CreateTimer();
    void DestroyTimer(t_u32 id);
    void Update(float ms_dt);
    void Start(t_u32 id);
    void Stop(t_u32 id);
    void ResetTimer(t_u32 id);
    bool IsExpired(t_u32 id);
    bool IsRunning(t_u32 id);
    bool IsStop(t_u32 id);
    void SetDelay(t_u32 id, float ms_dt);

    static TimerManagerImpl& Instance();
private:
    static TimerManagerImpl* m_pInstance;
    static t_u32 ID;
	//std::list<TimeDelay*> m_TimeDelayList;
	std::vector<TimeDelay*> m_TimeDelayVector;

    TimerManagerImpl();
    TimerManagerImpl(const TimerManagerImpl&);
    // Does not make sense for a singleton. This is a self assignment.
    TimerManagerImpl& operator=(const TimerManagerImpl&);
    // Declare operator address-of as private 
    TimerManagerImpl* operator &();
    ~TimerManagerImpl(){}
};

#define gTimerManager TimerManagerImpl::Instance()


// class TimeCounter
// {
// public:
// 
//     TimeCounter();				// Initialisiert Timer
//     ~TimeCounter();
// 
//     void  Update();      // Setzt die aktuelle Zeit
//     float Passed();      // Zeit in Millisekunden
//     float PassedFrac();  // Zeit in Sekunden
// 
// protected:
//     LARGE_INTEGER now;				//	Zeit des High Performace Counters (HPC)
//     float res;						// Auflösung des Counters
// };

NAMESPACE_END_GUI

#endif // TIMERMANAGER_H
