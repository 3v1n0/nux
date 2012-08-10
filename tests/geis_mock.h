/*
 * Copyright 2012 Canonical Ltd.
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
 * Authored by: Daniel d'Andrada <daniel.dandrada@canonical.com>
 *
 */

#ifndef GEIS_MOCK_H
#define GEIS_MOCK_H

#include <memory>
#include <set>
#include <string>
#include <vector>
#include <list>

/* helper class */
class GeisAttributes
{
 public:
  GeisAttr GetByName(GeisString name);
  GeisAttr GetByIndex(GeisSize index);
  GeisSize Count();
  void AddBoolean(const char *name, GeisBoolean value);
  void AddFloat(const char *name, GeisFloat value);
  void AddInteger(const char *name, GeisInteger value);
  void AddPointer(const char *name, void *value);
  std::vector< std::unique_ptr<struct _GeisAttr> > vector;
};

struct _GeisAttr
{
  GeisAttrType type;
  std::string name;

  GeisBoolean  b;
  GeisFloat    f;
  GeisInteger  i;
  std::string  s;
  void*        p;
};

struct GeisStructWithAttributes
{
  GeisAttributes attributes;

  bool GetBoolean(const char *name)
  {
    return attributes.GetByName(name)->b;
  }

  float GetFloat(const char *name)
  {
    return attributes.GetByName(name)->f;
  }

  int GetInteger(const char *name)
  {
    return attributes.GetByName(name)->i;
  }

  std::string GetString(const char *name)
  {
    return attributes.GetByName(name)->s;
  }

  void* GetPointer(const char *name)
  {
    return attributes.GetByName(name)->p;
  }
};

struct _Geis
{
  int fd;
  std::list<GeisEvent> pending_events;
};

struct _GeisGestureClass : GeisStructWithAttributes
{
  int id;
  std::string name;
};

struct _GeisEvent : GeisStructWithAttributes
{
  GeisEventType type;
};

struct _GeisDevice : GeisStructWithAttributes
{
  int id;
};

struct _GeisGroup
{
  int id;
  std::vector<GeisFrame> vector;
};

struct _GeisGroupSet
{
  std::vector<GeisGroup> vector;
};

struct _GeisTouch : GeisStructWithAttributes
{
  int id;
};

struct _GeisTouchSet
{
  std::vector<GeisTouch> vector;
};

struct _GeisFrame : GeisStructWithAttributes
{
  int id;
  std::set<int> class_ids;
};

struct _GeisSubscription
{
  int id;
  std::string name;
  std::vector<GeisFilter> filters;
};

struct _GeisFilter
{
  int foo;
};

#endif
