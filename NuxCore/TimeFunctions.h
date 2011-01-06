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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef TIMEFUNCTIONS_H
#define TIMEFUNCTIONS_H

namespace nux
{
// Store time from [Midnight(00:00:00), January 1, 1970 UTC] up to [23:59:59, December 31, 3000 UTC]  or [03:14:07 January 19, 2038 UTC]
  class NTimeStamp
  {
  public:

    NTimeStamp();
    ~NTimeStamp();
    // Time is in UTC
    unsigned int     m_Year;           /* year                             */
    unsigned int     m_Month;          /* months since January - [0,11]    */
    unsigned int     m_Day;            /* day of the month - [1,31]        */
    unsigned int     m_Hour;           /* hours since midnight - [0,23]    */
    unsigned int     m_Minute;         /* minutes after the hour - [0,59]  */
    unsigned int     m_Second;         /* seconds after the minute - [0,59]*/
    //unsigned int     m_DayOfWeek;      /* days since Sunday - [0,6]        */
    //unsigned int     m_DayOfYear;      /* days since January 1 - [0,365]   */
    unsigned int     m_MicroSecond;

    t_s64  GetJulianDayNumber()             const;
    t_f64  GetJulianDate()                  const;
    unsigned int    GetSecondOfDay()                 const;
    bool   operator== ( NTimeStamp &Other )  const;
    bool   operator!= ( NTimeStamp &Other )  const;
    bool   operator< ( NTimeStamp &Other )  const;
    bool   operator> ( NTimeStamp &Other )  const;
    bool   operator>= ( NTimeStamp &Other )  const;
    bool   operator<= ( NTimeStamp &Other )  const;

    void GetTime();
  };

//! Returns the number of cycles that have passed. The origin is unknown.
  /*!
      Returns the number of cycles that have passed. The origin is unknown.

      @return current value of high resolution cycle counter.
  */
  DWORD inlCycles();

#if STATS
#define inl_clock(Timer)   {Timer -= inlCycles();}
#define inl_unclock(Timer) {Timer += inlCycles();}
#else
#define inl_clock(Timer)
#define inl_unclock(Timer)
#endif


//! Returns the time that has passed in seconds. The origin is unknown.
  /*!
      Returns the time that has passed in seconds. The origin is unknown.

      @return the time passed in seconds.
  */
  double Seconds();

//! Returns the time that has passed in milliseconds. The origin is unknown.
  /*!
      Returns the time that has passed in milliseconds. The origin is unknown.

      @return the time passed in milliseconds.
  */
  double MilliSeconds();

// Retrieves the current local date and time.
  void GetLocalTime (unsigned int &Year,
                     unsigned int &Month,
                     unsigned int &Day,
                     unsigned int &Hour,
                     unsigned int &Min,
                     unsigned int &Sec,
                     unsigned int &MicroSec);

// Retrieves the current system date and time. The system time is expressed in Coordinated Universal Time (UTC).
  void GetUTCTime (unsigned int &Year,
                   unsigned int &Month,
                   unsigned int &Day,
                   unsigned int &Hour,
                   unsigned int &Min,
                   unsigned int &Sec,
                   unsigned int &MicroSec);

//! Returns the time formatted in a string
  /*!
      Returns the time formatted in a string.
  */
  const TCHAR *GetFormattedLocalTime();

//! Sleep the thread for Seconds.
  /*!
      Sleep the thread for Seconds.
      A value of zero causes the thread to relinquish the remainder of its time slice to any other thread of equal priority that is ready to run.
  */
  void SleepSeconds ( float Seconds );

//! Sleep the thread for MilliSeconds.
  /*!
      Sleep the thread for MilliSeconds.
      A value of zero causes the thread to relinquish the remainder of its time slice to any other thread of equal priority that is ready to run.
  */
  void SleepMilliSeconds ( float MilliSeconds );

//! Get the time zone in hours.
  /*!
      Get the time zone in hours. Estern North American time zone is -5 hours.
  */
  t_long GetTimeZone();

  /*!
      Suspend thread execution for an interval measured in microseconds.

      @Milliseconds duration in Milliseconds.
  */
  void SleepForMilliseconds (unsigned int Milliseconds);

}

#endif // TIMEFUNCTIONS_H
