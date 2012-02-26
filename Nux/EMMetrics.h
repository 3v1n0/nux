/*
 * Copyright (C) 2012 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Mirco Müller <mirco.mueller@canonical.com
 */

#ifndef NUX_EMMETRICS_H
#define NUX_EMMETRICS_H

#include <X11/Xlib.h>

namespace nux
{
  class EMMetrics
  {
    public:
      EMMetrics(Display* dpy, int scr, double points);

      double Pixel2EM(int value);
      int EM2Pixel(double value);

    private:
      double pixels_per_em_;
  };
}

#endif // NUX_EMMETRICS_H
