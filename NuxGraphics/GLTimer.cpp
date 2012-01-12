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


#include "NuxCore/NuxCore.h"
#include "GLResource.h"
#include "GLTimer.h"

namespace nux
{
  GLTimer::GLTimer()
  {
#if defined(NUX_OS_WINDOWS)
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&now);
    res = (float) (1.0f / (double) freq.QuadPart);
#elif defined(NUX_OS_LINUX)
    gettimeofday(&m_last_time, NULL);
#endif
  }

  GLTimer::~GLTimer()
  {
  }

  void GLTimer::Reset(void)
  {
#if defined(NUX_OS_WINDOWS)
    QueryPerformanceCounter(&now);
#elif defined(NUX_OS_LINUX)
    gettimeofday(&m_last_time, NULL);
#endif
  }

  float GLTimer::PassedMilliseconds(void)
  {
#if defined(NUX_OS_WINDOWS)
    LARGE_INTEGER temp;
    QueryPerformanceCounter(&temp);
    double elapsedTime = (temp.QuadPart - now.QuadPart) * res * 1000.0f;
    //now.QuadPart = temp.QuadPart;
    return elapsedTime;

#elif defined(NUX_OS_LINUX)
    timeval current_time;
    float elapsedTime;

    gettimeofday(&current_time, NULL);

    elapsedTime = (current_time.tv_sec - m_last_time.tv_sec) * 1000.0f;      // seconds to millisecond
    elapsedTime += (current_time.tv_usec - m_last_time.tv_usec) / 1000.0f;   // micro seconds to millisecond
    return elapsedTime;
#endif
  }

// float GLTimer::PassedFrac(void)
// {
//     LARGE_INTEGER temp;
//     QueryPerformanceCounter(&temp);
//     return(temp.QuadPart - now.QuadPart) * res;
// }
}
