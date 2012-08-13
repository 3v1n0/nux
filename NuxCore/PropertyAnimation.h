// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright 2012 Inalogic® Inc.
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
 * Authored by: Tim Penhey <tim.penhey@canonical.com>
 *
 */
#ifndef NUX_CORE_PROPERTY_ANIMATION_H
#define NUX_CORE_PROPERTY_ANIMATION_H

#include "Animation.h"
#include "Property.h"

#include <boost/shared_ptr.hpp>

namespace nux
{
namespace animation
{

template <typename T>
boost::shared_ptr<AnimateValue<T>> animate_property(Property<T>& prop,
                                                    T const& start,
                                                    T const& finish,
                                                    int ms_duration)
{
  boost::shared_ptr<AnimateValue<T> > anim(new AnimateValue<T>(start, finish, ms_duration));
  anim->updated.connect([&prop](T const& v) { prop = v; });
  return anim;
}

}}

#endif
