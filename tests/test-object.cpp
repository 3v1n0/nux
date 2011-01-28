/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include <glib.h>
#include "Nux/Nux.h"

static void TestObject (void);
static void TestObjectReference (void);
static void TestObjectPtr (void);
static void TestObjectPtr1 (void);
static void TestObjectPtr2 (void);
static void TestObjectSignal (void);

void
TestObjectSuite (void)
{
#define TESTDOMAIN "/Object/"

  g_test_add_func (TESTDOMAIN"/TestObject",           TestObject);
  g_test_add_func (TESTDOMAIN"/TestObjectReference",  TestObjectReference);
  g_test_add_func (TESTDOMAIN"/TestObjectPtr",        TestObjectPtr);
  g_test_add_func (TESTDOMAIN"/TestObjectPtr1",       TestObjectPtr1);
  g_test_add_func (TESTDOMAIN"/TestObjectPtr2",       TestObjectPtr2);
  g_test_add_func (TESTDOMAIN"/TestObjectSignal",     TestObjectSignal);
}

static const int ARRAY_SIZE = 1000;

class OwnedObject: public nux::Object
{
public:
  OwnedObject (NUX_FILE_LINE_PROTO) : nux::Object (true, NUX_FILE_LINE_PARAM)
  {

  }

  ~OwnedObject () {}

  int array [ARRAY_SIZE];
};

class ChildOwnedObject: public OwnedObject
{
public:
  ChildOwnedObject (NUX_FILE_LINE_PROTO) : OwnedObject (NUX_FILE_LINE_PARAM)
  {

  }

  ~ChildOwnedObject () {}

  int array [ARRAY_SIZE];
};


class UnOwnedObject: public nux::Object
{
public:
  UnOwnedObject (NUX_FILE_LINE_PROTO) : nux::Object (false, NUX_FILE_LINE_PARAM)
  {

  }

  ~UnOwnedObject () {}

  int array [ARRAY_SIZE];
};

class ChildUnOwnedObject: public UnOwnedObject
{
public:
  ChildUnOwnedObject (NUX_FILE_LINE_PROTO) : UnOwnedObject (NUX_FILE_LINE_PARAM)
  {

  }

  ~ChildUnOwnedObject () {}

  int array [ARRAY_SIZE];
};

static void
TestObject (void)
{
  OwnedObject *a = new OwnedObject (NUX_TRACKER_LOCATION);
  OwnedObject b(NUX_TRACKER_LOCATION);

  g_assert (a != NULL);
  g_assert (a->IsHeapAllocated ());

  g_assert (b.IsHeapAllocated () == false);

  int sz = a->GetObjectSize ();
  g_assert (sz >= ARRAY_SIZE);

  a->UnReference ();
}

static void
TestObjectReference (void)
{
  OwnedObject *a = new OwnedObject (NUX_TRACKER_LOCATION);  // ref count = 1, owned
  UnOwnedObject *b = new UnOwnedObject (NUX_TRACKER_LOCATION); // ref count = 1, unowned

  g_assert (a->GetReferenceCount () == 1);
  g_assert (b->GetReferenceCount () == 1);
  g_assert (b->OwnsTheReference () == false);

  a->Reference (); // ref count = 2
  a->Reference (); // ref count = 3
  b->Reference (); // ref count = 1, owned

  g_assert (a->GetReferenceCount () == 3);
  g_assert (b->GetReferenceCount () == 1);
  g_assert (b->OwnsTheReference () == true);

  g_assert (a->UnReference () == false); // ref count = 2
  g_assert (a->GetReferenceCount () == 2);
  g_assert (a->UnReference () == false); // ref count = 1
  g_assert (a->GetReferenceCount () == 1);
  g_assert (a->UnReference () == true); // object destroyed

  g_assert (b->UnReference () == true); // object destroyed
}

static void
TestObjectPtr (void)
{
  OwnedObject *a = new OwnedObject (NUX_TRACKER_LOCATION);  // ref count = 1, owned
  UnOwnedObject *b = new UnOwnedObject (NUX_TRACKER_LOCATION); // ref count = 1, unowned

  nux::ObjectPtr<OwnedObject> object_ptr (a); // ref count = 2

  g_assert (a->GetReferenceCount () == 2);

  g_assert (a->UnReference () == false); // ref count = 1

  g_assert (object_ptr->GetReferenceCount () == 1);

  g_assert (a->UnReference() == false); // Calling UnReference repeatedly should not destroy the object when there are ObjectPtr's hosting it.

  g_assert (object_ptr->GetReferenceCount () == 1);

  object_ptr.Release ();

}

static void
TestObjectPtr1 (void)
{
  ChildOwnedObject *c = new ChildOwnedObject (NUX_TRACKER_LOCATION);  // ref count = 1, owned
  ChildUnOwnedObject *d = new ChildUnOwnedObject (NUX_TRACKER_LOCATION); // ref count = 1, unowned

  nux::ObjectPtr<OwnedObject> object_ptr0 (c); // ref count = 2

  g_assert (c->GetReferenceCount () == 2);

  nux::ObjectPtr<OwnedObject> object_ptr1 (object_ptr0); // ref count = 3

  g_assert (c->GetReferenceCount () == 3);

  g_assert (c->UnReference () == false); // ref count = 2
  g_assert (c->UnReference () == false); // ref count = 2
  g_assert (c->UnReference () == false); // ref count = 2
  g_assert (c->UnReference () == false); // ref count = 2
  g_assert (c->UnReference () == false); // ref count = 2

  g_assert (object_ptr1->GetReferenceCount () == 2);

  object_ptr1.Release ();

  g_assert (object_ptr0->GetReferenceCount () == 1);

  object_ptr0.Release ();
}

static void
TestObjectPtr2 (void)
{
  ChildOwnedObject *c = new ChildOwnedObject (NUX_TRACKER_LOCATION);  // ref count = 1, owned
  ChildUnOwnedObject *d = new ChildUnOwnedObject (NUX_TRACKER_LOCATION); // ref count = 1, unowned

  nux::ObjectPtr<OwnedObject> object_ptr0 (c); // ref count = 2

  nux::ObjectWeakPtr<OwnedObject> object_ptr1 = object_ptr0; // ref count = 2, weak ref count = 3

  nux::ObjectWeakPtr<OwnedObject> object_ptr2 = object_ptr1; // ref count = 2, weak ref count = 4

  g_assert (c->GetReferenceCount () == 2);

  g_assert (c->GetWeakReferenceCount() == 4);

  g_assert (c->UnReference () == false); // ref count = 2

  g_assert (object_ptr0->GetReferenceCount () == 1);
  g_assert (object_ptr0->GetWeakReferenceCount () == 3);

  g_assert (object_ptr0.Release () == true);

  g_assert (object_ptr1.IsValid () == false);
  g_assert (object_ptr1.IsNull () == true);
  g_assert (object_ptr1 () == false);
  g_assert (object_ptr1.GetWeakReferenceCount () == 2);

  g_assert (object_ptr1.Release () == false);


  g_assert (object_ptr2.GetWeakReferenceCount () == 1);
  g_assert (object_ptr2.Release () == true);
}

static bool g_signal_called = false;

static void on_destroyed_cb () {
  g_signal_called = true;
}

static void
TestObjectSignal (void)
{
  nux::Object *obj = new nux::Object ();
  obj->OnDestroyed.connect (sigc::ptr_fun (on_destroyed_cb));
  g_assert (g_signal_called == false);
  obj->UnReference ();
  g_assert (g_signal_called == true);
}
