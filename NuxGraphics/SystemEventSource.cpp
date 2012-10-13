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
 * Authored by: Thomas Voß <thomas.voss@canonical.com>
 */

#include "SystemEventSource.h"

#include <set>

#include <stdio.h>

nux::SystemEventSource::~SystemEventSource()
{
}

bool nux::SystemEventSource::HasListener(nux::SystemEventSource::Listener* listener) const
{
    printf("%s \n", __PRETTY_FUNCTION__);
    // TODO: make this thread-safe
    return listeners.find(listener) != listeners.end();
}

void nux::SystemEventSource::AddListener(nux::SystemEventSource::Listener* listener)
{
    printf("%s\n", __PRETTY_FUNCTION__);
    // TODO: make this thread-safe
    listeners.insert(listener);
}

void nux::SystemEventSource::RemoveListener(nux::SystemEventSource::Listener* listener)
{
    printf("%s \n", __PRETTY_FUNCTION__);
    // TODO: make this thread-safe
    listeners.erase(listener);
}

void nux::SystemEventSource::NotifyListeners(const nux::Event& event)
{
    std::set<nux::SystemEventSource::Listener*>::iterator it;
    for(it = listeners.begin(); it != listeners.end(); ++it)
        (*it)->OnNewEvent(event);
}
