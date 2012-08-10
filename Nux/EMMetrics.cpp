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

#include "Nux/EMMetrics.h"

namespace nux
{
  EMMetrics::EMMetrics(Display* dpy, int scr, double points)
  {
    double dpi  = 96.0; // default DPI if no Display provided (e.g. unit-test)

    if (dpy)
    {
      // determine current system DPI, remember that: 1 inch == 2.54 cm == 25.4 mm
      double xres = ((static_cast<double> DisplayWidth(dpy,scr) * 25.4) /
                     (static_cast<double> DisplayWidthMM(dpy,scr)));
      double yres = ((static_cast<double> DisplayHeight(dpy,scr) * 25.4) /
                     (static_cast<double> DisplayHeightMM(dpy,scr)));
      dpi = (xres + yres) / 2.0; 
    }

    // update stored ppe
    pixels_per_em_ = points * dpi / 72.0;

    // sanity-check
    if (pixels_per_em_ == 0.0)
      pixels_per_em_ = 10.0; // assume points == 10.0, dpi == 96.0
  }

  double EMMetrics::Pixel2EM(int value)
  {
    return static_cast<double> (value) / pixels_per_em_;
  }

  int EMMetrics::EM2Pixel(double value)
  {
    return static_cast<int> (value * pixels_per_em_);
  }
} // nux namespace

