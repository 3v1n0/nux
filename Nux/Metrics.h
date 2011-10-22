/*
 * Copyright (C) 2011 Canonical Ltd
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

#ifndef METRICS_H
#define METRICS_H

#include <X11/Xlib.h>

class Metrics
{
  public:
    Metrics (Display* dpy, int src, double points);
    ~Metrics ();

    double Pixel2EM (int value);
    int EM2Pixel (double value);

  private:
    double pixel_per_em_;
};

#endif // METRICS_H
