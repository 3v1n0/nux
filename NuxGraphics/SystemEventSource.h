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

#ifndef SYSTEM_EVENT_SOURCE_H_
#define SYSTEM_EVENT_SOURCE_H_

#include <set>

namespace nux
{

class Event;

class SystemEventSource
{
  public:

    static SystemEventSource* Instance();

    class Listener
    {
      public:
        virtual ~Listener() {}

        virtual void OnNewEvent(const Event& event) = 0;

      protected:
        Listener() {}
        Listener(const Listener&);
        Listener& operator=(const Listener&);
        bool operator==(const Listener&) const;
    };

    virtual ~SystemEventSource();

    bool HasListener(Listener* listener) const;
    void AddListener(Listener* listener);
    void RemoveListener(Listener* listener);

    virtual void PollOnce() = 0;
    virtual void Start() = 0;
    virtual void Stop() = 0;
    
  protected:
    SystemEventSource()
    {
    }

    void NotifyListeners(const Event& event);

  private:
    SystemEventSource(const SystemEventSource&);
    SystemEventSource& operator=(const SystemEventSource&);
    bool operator==(const SystemEventSource&) const;

    std::set<Listener*> listeners;
};

}

#endif // SYSTEM_EVENT_SOURCE_H_
