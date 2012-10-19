/*
 * Copyright 2010 Inalogic® Inc.
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
 *              Marco Trevisan <marco.trevisan@canonical.com>
 */

#include <gmock/gmock.h>
#include <boost/shared_ptr.hpp>
#include "Nux/Nux.h"
#include "Nux/HLayout.h"
#include "Nux/StaticText.h"
#include "Nux/ProgramFramework/TestView.h"


using namespace testing;

namespace {

struct MockTestView : nux::TestView
{
  MockTestView()
    : nux::TestView("MockTestView")
  {}

  void SetNextObjectToKeyFocusArea(nux::Area* area)
  {
    TestView::SetNextObjectToKeyFocusArea(area);
  }

  nux::Area* GetNextObjectToKeyFocusArea()
  {
    return TestView::GetNextObjectToKeyFocusArea();
  }
};

struct TestArea : public testing::Test
{
  void SetUp()
  {
    nux::NuxInitialize(0);
    wnd_thread.reset(nux::CreateNuxWindow("Area Test", 300, 200, nux::WINDOWSTYLE_NORMAL,
                                          NULL, false, NULL, NULL));
    test_view = new MockTestView();
  }

  boost::shared_ptr<nux::WindowThread> wnd_thread;
  nux::ObjectPtr<MockTestView> test_view;
};

TEST_F(TestArea, TestAreaSize)
{
  EXPECT_EQ(test_view->GetMinimumWidth(), nux::AREA_MIN_WIDTH);
  EXPECT_EQ(test_view->GetMaximumWidth(), nux::AREA_MAX_WIDTH);
  EXPECT_EQ(test_view->GetMinimumHeight(), nux::AREA_MIN_HEIGHT);
  EXPECT_EQ(test_view->GetMaximumHeight(), nux::AREA_MAX_HEIGHT);

  test_view->SetMinimumWidth(100);
  EXPECT_EQ(test_view->GetMinimumWidth(), 100);

  test_view->SetMaximumWidth(1111);
  EXPECT_EQ(test_view->GetMaximumWidth(), 1111);

  test_view->SetMinimumHeight(123);
  EXPECT_EQ(test_view->GetMinimumHeight(), 123);

  test_view->SetMaximumHeight(1234);
  EXPECT_EQ(test_view->GetMaximumHeight(), 1234);

  test_view->SetMaximumSize(4321, 1234);
  test_view->SetMinimumSize(123, 432);

  EXPECT_EQ(test_view->GetMinimumWidth(), 123);
  EXPECT_EQ(test_view->GetMaximumWidth(), 4321);
  EXPECT_EQ(test_view->GetMinimumHeight(), 432);
  EXPECT_EQ(test_view->GetMaximumHeight(), 1234);
}

TEST_F(TestArea, TestAreaGeometry)
{
  static_cast<nux::Area*>(test_view.GetPointer())->SetGeometry(0, 0, 100, 100);

  EXPECT_EQ(test_view->GetBaseX(), 0);
  EXPECT_EQ(test_view->GetBaseX(), 0);
  EXPECT_EQ(test_view->GetBaseWidth(), 100);
  EXPECT_EQ(test_view->GetBaseHeight(), 100);
}

static bool object_destroyed = false;
void OnObjectDestroyed(nux::Object* /* object */)
{
  object_destroyed = true;
}

TEST_F(TestArea, TestUnParentKeyFocus)
{
  nux::TestView* test_view1 = new nux::TestView("");
  nux::HLayout* layout = new nux::HLayout();
  test_view1->object_destroyed.connect(sigc::ptr_fun(&OnObjectDestroyed));

  test_view1->Reference();
  layout->AddView(test_view1, 1);

  wnd_thread->SetLayout(layout);

  EXPECT_EQ(test_view1->HasKeyFocus(), false);
  nux::GetWindowThread()->GetWindowCompositor().SetKeyFocusArea(test_view1);
  EXPECT_EQ(test_view1->HasKeyFocus(), true);
  layout->RemoveChildObject(test_view1);
  EXPECT_EQ(test_view1->HasKeyFocus(), false);
  test_view1->UnReference();
  EXPECT_EQ(object_destroyed, true);
}

TEST_F(TestArea, NextObjectToKeyFocusSetReset)
{
  MockTestView* parent = new MockTestView();
  MockTestView* brother = new MockTestView();
  MockTestView* child1 = new MockTestView();
  MockTestView* child2 = new MockTestView();
  MockTestView* child3 = new MockTestView();

  parent->SetNextObjectToKeyFocusArea(brother);
  ASSERT_EQ(parent->GetNextObjectToKeyFocusArea(), brother);

  child1->SetParentObject(parent);
  child1->SetNextObjectToKeyFocusArea(child2);
  ASSERT_EQ(child1->GetNextObjectToKeyFocusArea(), child2);

  child2->SetParentObject(parent);
  child2->SetNextObjectToKeyFocusArea(child3);
  ASSERT_EQ(child2->GetNextObjectToKeyFocusArea(), child3);

  child3->SetParentObject(parent);
  ASSERT_EQ(child3->GetNextObjectToKeyFocusArea(), nullptr);

  child1->UnReference();
  EXPECT_EQ(child2->GetNextObjectToKeyFocusArea(), nullptr);
  EXPECT_EQ(parent->GetNextObjectToKeyFocusArea(), nullptr);

  parent->UnReference();
}

}
