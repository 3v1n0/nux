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


#ifndef TIMEFUNCTIONS_H
#define TIMEFUNCTIONS_H

namespace nux
{
// Store time from [Midnight(00:00:00), January 1, 1970 UTC] up to [23:59:59, December 31, 3000 UTC]  or [03:14:07 January 19, 2038 UTC]
  class TimeStamp
  {
  public:

    TimeStamp();
    ~TimeStamp();
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

    long long  GetJulianDayNumber()             const;
    double  GetJulianDate()                  const;
    unsigned int    GetSecondOfDay()                 const;
    bool   operator== ( TimeStamp &Other )  const;
    bool   operator!= ( TimeStamp &Other )  const;
    bool   operator< ( TimeStamp &Other )  const;
    bool   operator> ( TimeStamp &Other )  const;
    bool   operator>= ( TimeStamp &Other )  const;
    bool   operator<= ( TimeStamp &Other )  const;

    void GetTime();
  };

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
      @return The time formatted in a string.
  */
  std::string GetFormattedLocalTime();

  /*!
      Suspend thread execution for an interval measured in microseconds.

      @Milliseconds duration in Milliseconds.
  */
  void SleepForMilliseconds (unsigned int Milliseconds);

}

#endif // TIMEFUNCTIONS_H
