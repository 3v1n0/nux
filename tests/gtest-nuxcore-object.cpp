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

#include <gmock/gmock.h>

#include "NuxCore/NuxCore.h"
#include "NuxCore/Object.h"
#include "NuxCore/ObjectPtr.h"

using namespace testing;

namespace {

const int ARRAY_SIZE = 1000;

class OwnedObject: public nux::Object
{
public:
  OwnedObject(NUX_FILE_LINE_PROTO)
  : nux::Object(true, NUX_FILE_LINE_PARAM)
  {
  }

  ~OwnedObject() {}

  int array[ARRAY_SIZE];
};

class ChildOwnedObject: public OwnedObject
{
public:
  ChildOwnedObject(NUX_FILE_LINE_PROTO)
  : OwnedObject(NUX_FILE_LINE_PARAM)
  {
  }

  ~ChildOwnedObject() {}

  int array [ARRAY_SIZE];
};


class UnOwnedObject: public nux::Object
{
public:
  UnOwnedObject(NUX_FILE_LINE_PROTO)
  : nux::Object(false, NUX_FILE_LINE_PARAM)
  {
  }

  ~UnOwnedObject() {}

  int array [ARRAY_SIZE];
};

class ChildUnOwnedObject: public UnOwnedObject
{
public:
  ChildUnOwnedObject(NUX_FILE_LINE_PROTO)
    : UnOwnedObject (NUX_FILE_LINE_PARAM)
  {
  }

  ~ChildUnOwnedObject () {}

  int array [ARRAY_SIZE];
};

TEST(TestObject, TestObject) {

  OwnedObject* a = new OwnedObject(NUX_TRACKER_LOCATION);
  OwnedObject b(NUX_TRACKER_LOCATION);

  EXPECT_THAT(a, NotNull());
  EXPECT_TRUE(a->IsHeapAllocated());

  EXPECT_FALSE(b.IsHeapAllocated());

  EXPECT_THAT(a->GetObjectSize(), Ge(ARRAY_SIZE));

  a->UnReference();
}

TEST(TestObject, TestObjectReference) {

  OwnedObject *a = new OwnedObject (NUX_TRACKER_LOCATION);  // ref count = 1, owned
  UnOwnedObject *b = new UnOwnedObject (NUX_TRACKER_LOCATION); // ref count = 1, unowned

  EXPECT_THAT(a->GetReferenceCount(), Eq(1));
  EXPECT_THAT(b->GetReferenceCount(), Eq(1));
  EXPECT_FALSE(b->OwnsTheReference());

  a->Reference (); // ref count = 2
  a->Reference (); // ref count = 3
  b->Reference (); // ref count = 1, owned

  EXPECT_THAT(a->GetReferenceCount(), Eq(3));
  EXPECT_THAT(b->GetReferenceCount(), Eq(1));
  EXPECT_TRUE(b->OwnsTheReference());

  EXPECT_FALSE(a->UnReference());
  EXPECT_THAT(a->GetReferenceCount(), Eq(2));
  EXPECT_FALSE(a->UnReference());
  EXPECT_THAT(a->GetReferenceCount(), Eq(1));
  EXPECT_TRUE(a->UnReference()); // object destroyed

  EXPECT_TRUE(b->UnReference()); // object destroyed
}

TEST(TestObject, TestObjectPtr) {

  OwnedObject* a = new OwnedObject(NUX_TRACKER_LOCATION);  // ref count = 1, owned

  nux::ObjectPtr<OwnedObject> object_ptr(a); // ref count = 2

  EXPECT_THAT(a->GetReferenceCount(), Eq(2));
  EXPECT_FALSE(a->UnReference()); // ref count = 1
  EXPECT_THAT(a->GetReferenceCount(), Eq(1));

  object_ptr.Release();
}

TEST(TestObject, TestObjectPtrAdopt) {

  OwnedObject* a = new OwnedObject(NUX_TRACKER_LOCATION);  // ref count = 1, owned

  nux::ObjectPtr<OwnedObject> object_ptr;

  object_ptr.Adopt(a);

  EXPECT_THAT(a->GetReferenceCount(), Eq(1));
}

TEST(TestObject, TestObjectPtrGetPointer) {

  OwnedObject* a = new OwnedObject(NUX_TRACKER_LOCATION);  // ref count = 1, owned
  nux::ObjectPtr<OwnedObject> object_ptr;
  object_ptr.Adopt(a);

  EXPECT_THAT(object_ptr.GetPointer(), Eq(a));
  // Const too
  nux::ObjectPtr<OwnedObject> const& object_ptr_ref = object_ptr;
  EXPECT_THAT(object_ptr_ref.GetPointer(), Eq(a));
}

TEST(TestObject, TestObjectPtr1) {

  ChildOwnedObject* c = new ChildOwnedObject(NUX_TRACKER_LOCATION);  // ref count = 1, owned

  nux::ObjectPtr<OwnedObject> object_ptr0(c); // ref count = 2

  EXPECT_THAT(c->GetReferenceCount(), Eq(2));

  nux::ObjectPtr<OwnedObject> object_ptr1(object_ptr0); // ref count = 3

  EXPECT_THAT(c->GetReferenceCount(), Eq(3));

  EXPECT_FALSE(c->UnReference()); // ref count = 2

  object_ptr1.Release();

  EXPECT_THAT(c->GetReferenceCount(), Eq(1));

  object_ptr0.Release ();
}

TEST(TestObject, TestObjectPtr2) {

  ChildOwnedObject *c = new ChildOwnedObject(NUX_TRACKER_LOCATION);

  nux::ObjectPtr<OwnedObject> obj_ptr(c);
  nux::ObjectWeakPtr<OwnedObject> weak_ptr(obj_ptr);

  EXPECT_THAT(c->GetReferenceCount(), Eq(2));
  EXPECT_FALSE(c->UnReference());

  EXPECT_THAT(c->GetReferenceCount(), Eq(1));

  // Clearing the smart pointer deletes the object.
  EXPECT_TRUE(obj_ptr.Release());

  EXPECT_FALSE(weak_ptr.IsValid());
  EXPECT_TRUE(weak_ptr.IsNull());
  EXPECT_FALSE(weak_ptr());
}

bool g_signal_called = false;

void on_destroyed_cb (nux::Object * /* obj */)
{
  g_signal_called = true;
}

TEST(TestObject, TestObjectSignal) {

  nux::Object *obj = new nux::Object ();
  obj->OnDestroyed.connect(sigc::ptr_fun(on_destroyed_cb));
  EXPECT_FALSE(g_signal_called);
  obj->UnReference ();
  EXPECT_TRUE(g_signal_called);
}

TEST(TestObject, TestObjectAdoptUnOwned) {

  UnOwnedObject* obj = new UnOwnedObject(NUX_TRACKER_LOCATION);
  EXPECT_THAT(obj->OwnsTheReference(), Eq(false));
  nux::ObjectPtr<UnOwnedObject> obj_smart_ptr;
  obj_smart_ptr.Adopt(obj);
  EXPECT_THAT(obj->GetReferenceCount(), Eq(1));
  EXPECT_THAT(obj->ObjectPtrCount(), Eq(1));
  EXPECT_THAT(obj->OwnsTheReference(), Eq(true));
}

TEST(TestObject, TestObjectAdoptOwned) {

  OwnedObject* obj = new OwnedObject(NUX_TRACKER_LOCATION);
  EXPECT_THAT(obj->OwnsTheReference(), Eq(true));
  nux::ObjectPtr<OwnedObject> obj_smart_ptr;
  obj_smart_ptr.Adopt(obj);
  EXPECT_THAT(obj->GetReferenceCount(), Eq(1));
  EXPECT_THAT(obj->ObjectPtrCount(), Eq(1));
}
}
