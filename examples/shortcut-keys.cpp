/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */

#include "Nux/Nux.h"
#include "Nux/VLayout.h"
#include "Nux/HLayout.h"
#include "Nux/WindowThread.h"
#include "Nux/StaticText.h"

#include "line-separator.h"

int SECTION_NAME_FONT_SIZE = 17;
int SHORTKEY_ENTRY_FONT_SIZE = 13;
int INTER_SPACE_SHORTKEY_DESCRIPTION = 0;
int SHORTKEY_COLUMN_WIDTH = 150 + 100;
int DESCRIPTION_COLUMN_WIDTH = 265 + 100;


nux::LinearLayout *CreateSectionLayout(const char *section_name)
{
  nux::VLayout *layout = new nux::VLayout(NUX_TRACKER_LOCATION);

  nux::StaticText* section_name_view = new nux::StaticText(section_name, NUX_TRACKER_LOCATION);
  section_name_view->SetTextPointSize(SECTION_NAME_FONT_SIZE);
  section_name_view->SetFontName("Ubuntu Bold");
  layout->AddView(section_name_view, 0, nux::MINOR_POSITION_START, nux::MINOR_SIZE_MATCHCONTENT);
  layout->AddView(new nux::SpaceLayout(30, 30, 30, 30), 0, nux::MINOR_POSITION_START, nux::MINOR_SIZE_MATCHCONTENT);

  return layout;
}

nux::LinearLayout *CreateShortKeyEntryLayout(const char *shortkey, const char *description)
{
  nux::HLayout *layout = new nux::HLayout("EntryLayout", NUX_TRACKER_LOCATION);
  nux::HLayout *shortkey_layout = new nux::HLayout(NUX_TRACKER_LOCATION);
  nux::HLayout *description_layout = new nux::HLayout(NUX_TRACKER_LOCATION);

  nux::StaticText* shortkey_view = new nux::StaticText(shortkey, NUX_TRACKER_LOCATION);
  shortkey_view->SetTextAlignment(nux::StaticText::ALIGN_LEFT);
  shortkey_view->SetTextPointSize(SHORTKEY_ENTRY_FONT_SIZE);
  shortkey_view->SetMinimumWidth(SHORTKEY_COLUMN_WIDTH);
  shortkey_view->SetMaximumWidth(SHORTKEY_COLUMN_WIDTH);

  nux::StaticText* description_view = new nux::StaticText(description, NUX_TRACKER_LOCATION);
  description_view->SetTextAlignment(nux::StaticText::ALIGN_LEFT);
  description_view->SetTextPointSize(SHORTKEY_ENTRY_FONT_SIZE);
  description_view->SetMinimumWidth(DESCRIPTION_COLUMN_WIDTH);
  description_view->SetMaximumWidth(DESCRIPTION_COLUMN_WIDTH);

  shortkey_layout->AddView(shortkey_view, 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
  shortkey_layout->SetContentDistribution(nux::MAJOR_POSITION_START);
  shortkey_layout->SetMinimumWidth(SHORTKEY_COLUMN_WIDTH);
  shortkey_layout->SetMaximumWidth(SHORTKEY_COLUMN_WIDTH);

  description_layout->AddView(description_view, 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
  description_layout->SetContentDistribution(nux::MAJOR_POSITION_START);
  description_layout->SetMinimumWidth(DESCRIPTION_COLUMN_WIDTH);
  description_layout->SetMaximumWidth(DESCRIPTION_COLUMN_WIDTH);

  layout->AddLayout(shortkey_layout, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
  layout->AddLayout(description_layout, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
  layout->SetSpaceBetweenChildren(INTER_SPACE_SHORTKEY_DESCRIPTION);
  description_layout->SetContentDistribution(nux::MAJOR_POSITION_START);

  return layout;
}

nux::LinearLayout *CreateIntermediateLayout()
{
  nux::VLayout *layout = new nux::VLayout(NUX_TRACKER_LOCATION);

  return layout;
}

void UserInterfaceInitialization(nux::NThread* thread, void* /* InitData */)
{
  nux::VLayout *root_layout = new nux::VLayout(NUX_TRACKER_LOCATION);

  nux::LinearLayout *section_layout = CreateSectionLayout("Launcher");
  nux::LinearLayout *intermediate_layout = CreateIntermediateLayout();
  intermediate_layout->SetContentDistribution(nux::MAJOR_POSITION_START);

  intermediate_layout->AddLayout(CreateShortKeyEntryLayout("Super (Press)",           "Open Launcher, displays shortcuts."), 0, nux::MINOR_POSITION_START, nux::MINOR_SIZE_FULL);
  intermediate_layout->AddLayout(CreateShortKeyEntryLayout("Alt + F1 (Press)",        "Open Launcher keyboard navigation mode."), 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
  intermediate_layout->AddLayout(CreateShortKeyEntryLayout("Super + Tab",             "Switch application via Launcher."), 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
  intermediate_layout->AddLayout(CreateShortKeyEntryLayout("Super + 1 to 9",          "Same as clicking on a Launcher icon."), 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
  intermediate_layout->AddLayout(CreateShortKeyEntryLayout("Super + Shift + 1 to 9",  "Open a new window of the app."), 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
  intermediate_layout->AddLayout(CreateShortKeyEntryLayout("Super + T",               "Open the Rubbish Bin."), 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);

  section_layout->AddLayout(intermediate_layout, 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);

  // Add space before the line
  section_layout->AddView(new nux::SpaceLayout(30, 30, 30, 30), 0, nux::MINOR_POSITION_START, nux::MINOR_SIZE_MATCHCONTENT);
  section_layout->AddView(new nux::HSeparator(), 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
  // Add space after the line
  section_layout->AddView(new nux::SpaceLayout(30, 30, 30, 30), 0, nux::MINOR_POSITION_START, nux::MINOR_SIZE_MATCHCONTENT);


  root_layout->AddView(section_layout, 1, nux::MINOR_POSITION_START, nux::MINOR_SIZE_FULL);

  nux::GetWindowThread ()->SetLayout(root_layout);
  nux::ColorLayer background(nux::Color(0xFF222222));
  static_cast<nux::WindowThread*>(thread)->SetWindowBackgroundPaintLayer(&background);
}

int main()
{
  // Initialize Nux.
  nux::NuxInitialize(0);

  // Create a window thread.
  nux::WindowThread* wt = nux::CreateGUIThread("Shortcut Keys", 640, 300, 0, &UserInterfaceInitialization, 0);

  // Start the main loop.
  wt->Run(NULL);

  delete wt;
  return 0;
}
