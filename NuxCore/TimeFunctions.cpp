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


#include "NuxCore.h"

namespace nux
{

  TimeStamp::TimeStamp()
  {
    GetTime();
  }

  TimeStamp::~TimeStamp()
  {
    m_Year          = 1970;
    m_Month         = 1;
    m_Day           = 1;
    m_Hour          = 0;
    m_Minute        = 0;
    m_Second        = 0;
    m_MicroSecond   = 0;
  }


  long long TimeStamp::GetJulianDayNumber (void) const
  {
    long long JDN =  m_Day - 32075L +
                 1461L * (m_Year  + 4800L + (m_Month  - 14L) / 12L) / 4L +
                 367L * (m_Month - 2L - ( (m_Month - 14L) / 12L) * 12L) / 12L -
                 3L * ( (m_Year + 4900L - (m_Month  - 14L) / 12L) / 100L) / 4L;
    return JDN;
  }

  double TimeStamp::GetJulianDate() const
  {
    double JD = GetJulianDayNumber() + (m_Hour - 12) / 1440.0f + m_Minute / 1440.0f + m_Second / 86400.0f;
    return JD;
  }

  unsigned int TimeStamp::GetSecondOfDay (void) const
  {
    return m_Hour * 60 * 60 + m_Minute * 60 + m_Second;
  }

  bool TimeStamp::operator == (TimeStamp &Other) const
  {
    bool b = (m_Year  ==  Other.m_Year)       &&
             (m_Day        ==  Other.m_Day)      &&
             (m_Month      ==  Other.m_Month)    &&
             (m_Hour       ==  Other.m_Hour)     &&
             (m_Minute     ==  Other.m_Minute)   &&
             (m_Second     ==  Other.m_Second);
    return b;
  }

  bool TimeStamp::operator != (TimeStamp &Other) const
  {
    if (*this == Other)
      return false;

    return true;
  }

  bool TimeStamp::operator < (TimeStamp &Other) const
  {
    double JD = GetJulianDate();

    if (JD < Other.GetJulianDate() )
      return true;

    return false;
  }

  bool TimeStamp::operator >  (TimeStamp &Other) const
  {
    double JD = GetJulianDate();

    if (JD > Other.GetJulianDate() )
      return true;

    return false;
  }

  bool TimeStamp::operator >= (TimeStamp &Other) const
  {
    double JD = GetJulianDate();

    if (JD >= Other.GetJulianDate() )
      return true;

    return false;
  }

  bool TimeStamp::operator <= (TimeStamp &Other) const
  {
    double JD = GetJulianDate();

    if (JD <= Other.GetJulianDate() )
      return true;

    return false;
  }

  void TimeStamp::GetTime()
  {
    GetLocalTime (m_Year, m_Month, m_Day, m_Hour, m_Minute, m_Second, m_MicroSecond);
  }


  std::string GetFormattedLocalTime()
  {
    TCHAR buffer[1024];
    Memset(buffer, 0, 1024);

    unsigned int Year;
    unsigned int Month;
    unsigned int Day;
    unsigned int Hour;
    unsigned int Minute;
    unsigned int Second;
    unsigned int MicroSec;

    GetLocalTime (Year, Month, Day, Hour, Minute, Second, MicroSec);
#ifdef _WIN32
    _stprintf_s (buffer, 1024 - 1, TEXT ("%d:%d:%d: %d/%d/%d"), Hour, Minute, Second, Day, Month, Year);
    std::string result = buffer;
#else
    _stprintf (buffer, TEXT ("%d:%d:%d: %d/%d/%d"), Hour, Minute, Second, Day, Month, Year);
    std::string result = buffer;
#endif

    return result;
  }

  void GetLocalTime (unsigned int &Year,
                     unsigned int &Month,
                     unsigned int &Day,
                     unsigned int &Hour,
                     unsigned int &Min,
                     unsigned int &Sec,
                     unsigned int &MicroSec)
  {
#ifdef NUX_OS_WINDOWS
    SYSTEMTIME st;
    ::GetLocalTime (&st);

    Year		= st.wYear;
    Month		= st.wMonth;
    Day			= st.wDay;
    Hour		= st.wHour;
    Min			= st.wMinute;
    Sec			= st.wSecond;
    MicroSec	= st.wMilliseconds * 1000;

#elif (defined NUX_OS_LINUX) || (defined NUX_OS_MACOSX)
    time_t dt;
    struct tm dc;
    time (&dt);
    localtime_r (&dt, &dc);

    Year		= dc.tm_year - 100 + 2000;
    Month		= dc.tm_mon + 1;
    Day			= dc.tm_mday;
    Hour		= dc.tm_hour;
    Min			= dc.tm_min;
    Sec			= dc.tm_sec;
    MicroSec	= 0;
#else
    nuxAssert (0);
#endif
  }

  void GetUTCTime (unsigned int &Year,
                   unsigned int &Month,
                   unsigned int &Day,
                   unsigned int &Hour,
                   unsigned int &Min,
                   unsigned int &Sec,
                   unsigned int &MicroSec)
  {
#if (defined _WIN32)
    SYSTEMTIME st;
    ::GetSystemTime (&st);

    Year		= st.wYear;
    Month		= st.wMonth;
    Day			= st.wDay;
    Hour		= st.wHour;
    Min			= st.wMinute;
    Sec			= st.wSecond;
    MicroSec	= st.wMilliseconds * 1000;

#elif (defined NUX_OS_LINUX) || (defined NUX_OS_MACOSX)
    time_t dt;
    struct tm dc;
    time (&dt);
    gmtime_r (&dt, &dc);

    Year		= dc.tm_year - 100 + 2000;
    Month		= dc.tm_mon + 1;
    Day			= dc.tm_mday;
    Hour		= dc.tm_hour;
    Min			= dc.tm_min;
    Sec			= dc.tm_sec;
    MicroSec	= 0;
#else
    nuxAssert (0);
#endif
  }

  long GetTimeZone()
  {
#if (defined _WIN32)
    long seconds = 0;
    _get_timezone (&seconds);
    long hour = seconds / 3600;
    return hour;
#else
    return 0;
#endif
  }

  void SleepForMilliseconds (unsigned int Milliseconds)
  {
#if defined(NUX_OS_WINDOWS)
    Sleep (Milliseconds);

#elif defined(NUX_OS_LINUX)
    int ret = usleep (Milliseconds * 1000);

    if (ret != 0)
    {
      nuxDebugMsg (TEXT ("[SleepForMilliseconds] usleep has failed.") );
    }

#else
#error Sleep(milliseconds) is not implemented for this platform.
#endif
  }

}
