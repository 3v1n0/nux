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


#include "../NuxCore.h"
#include "Constants.h"

namespace nux
{
  const float Const::pi       = 3.1415926535f;
  const float Const::Pi       = 3.1415926535f;
  const float Const::e        = 2.7182817f;
  const float Const::sqrt2    = 1.414214f;
  const float Const::sqrt3    = 1.732051f;
  const float Const::golden   = 1.618034f;

  const double Const::EpsilonMilli  =   0.001;
  const double Const::EpsilonMicro  =   0.000001;
  const double Const::EpsilonNano   =   0.000000001;
  const double Const::EpsilonPico   =   0.000000000001;
  const double Const::EpsilonFemto  =   0.000000000000001;

#if defined(NUX_OS_WINDOWS)
  const float Const::flt_epsilon    = FLT_EPSILON;
  const float Const::dbl_epsilon    = DBL_EPSILON;
#elif defined(NUX_OS_LINUX)
  const float Const::flt_epsilon    = __FLT_EPSILON__;
  const float Const::dbl_epsilon    = __DBL_EPSILON__;
#elif defined(NUX_OS_MACOSX)
  const float Const::dbl_epsilon    = _FLT_EPSILON_;
  const float Const::dbl_epsilon    = _DBL_EPSILON_;
#else
  #error Undefined OS.
#endif

}
