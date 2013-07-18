// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright 2011 Inalogic® Inc.
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
#ifndef NUX_TESTS_FILE_HELPERS_H
#define NUX_TESTS_FILE_HELPERS_H

#include <string>
#include <sstream>
#include <vector>
#include <glib.h>
#include <sigc++/sigc++.h>
#include <functional>


namespace nux
{
namespace testing
{

std::string ReadFile(std::string const& filename);
void PumpGObjectMainLoop();

class TestCallback
{
public:
  TestCallback() : happened(false) {}

  sigc::slot<void> sigc_callback() {
    return sigc::mem_fun(this, &TestCallback::callback);
  }

  static gboolean glib_callback(gpointer data) {
    TestCallback* test = reinterpret_cast<TestCallback*>(data);
    test->callback();
    return FALSE;
  }
  void callback() {
    happened = true;
  }

  bool happened;
};


class CaptureLogOutput
{
public:
  CaptureLogOutput();
  ~CaptureLogOutput();
  std::string GetOutput();
private:
  std::ostringstream sout_;
};


template <typename T>
struct ChangeRecorder : sigc::trackable
{
  typedef std::function<void(T const&)> Listener;

  Listener listener()
  {
    return sigc::mem_fun(this, &ChangeRecorder<T>::value_changed);
  }

  void value_changed(T const& value)
    {
      changed_values.push_back(value);
    }
  typedef std::vector<T> ChangedValues;
  ChangedValues changed_values;

  int size() const { return changed_values.size(); }
  T last() const { return *changed_values.rbegin(); }
};

}
}

#endif
