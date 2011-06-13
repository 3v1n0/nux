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
 * Authored by: Neil Jagdish Patel <neil.patel@canonical.com>
 *
 */

#include "Nux/Nux.h"
#include "Nux/WindowThread.h"
#include "NuxGraphics/GraphicsEngine.h"

#include "Nux/TextureArea.h"
#include "Nux/HLayout.h"
#include "Nux/VLayout.h"
#include "Nux/LayeredLayout.h"
#include "Nux/ToggleButton.h"
#include "Nux/ComboBoxSimple.h"

class Foo
{
public:
  Foo ()
  {
    nux::VLayout *main_layout((new nux::VLayout(NUX_TRACKER_LOCATION)));
    nux::ComboBoxSimple *combo = new nux::ComboBoxSimple (NUX_TRACKER_LOCATION);

    combo->SetMinimumWidth (150);
    combo->sigTriggered.connect (sigc::mem_fun (this, &Foo::OnComboChangedFoRealz));
    main_layout->AddView (combo, 0, nux::eCenter, nux::eFix);

    layered_layout = new nux::LayeredLayout (NUX_TRACKER_LOCATION);
    for (int i = 0; i < 10; i++)
    {
      gchar *text = g_strdup_printf ("Button %d", i);
      nux::LayeredLayout *layered = new nux::LayeredLayout (NUX_TRACKER_LOCATION);

      nux::ColorLayer color (nux::color::RandomColor ());
      nux::TextureArea* texture_area = new nux::TextureArea ();
      texture_area->SetPaintLayer (&color);
      layered->AddLayer (texture_area);

      nux::HLayout *hori = new nux::HLayout (NUX_TRACKER_LOCATION);
      nux::Button* button = new nux::Button ("Big Button", NUX_TRACKER_LOCATION);
      button->SetMinMaxSize (200, 100);
      hori->AddView (button, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
      hori->SetContentDistribution (nux::MAJOR_POSITION_CENTER);
      layered->AddLayer (hori);

      hori = new nux::HLayout (NUX_TRACKER_LOCATION);
      button = new nux::ToggleButton (text, NUX_TRACKER_LOCATION);
      button->SetMinMaxSize (100, 50);
      hori->AddView (button, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
      hori->SetContentDistribution (nux::MAJOR_POSITION_CENTER);
      layered->AddLayout (hori);

      button = new nux::ToggleButton ("This button is insensitive", NUX_TRACKER_LOCATION);
      button->SetSensitive (false);
      layered->AddLayer (button, false, 10, 10, 180, 40);

      button = new nux::ToggleButton ("This button has x, y, w, h set", NUX_TRACKER_LOCATION);
      layered->AddLayer (button, false, 400, 10, 180, 40);

      nux::ROPConfig rop;
      rop.Blend = true;
      rop.SrcBlend = GL_ONE;
      rop.DstBlend = GL_ONE_MINUS_SRC_ALPHA;
      nux::Color col (0x55005500);
      nux::ColorLayer c (col, true, rop);
      texture_area = new nux::TextureArea ();
      texture_area->SetPaintLayer (&c);
      layered->AddLayer (texture_area, false, 0, 100, 600, 200);

      button = new nux::ToggleButton ("YOU CANT SEE ME!!!!!", NUX_TRACKER_LOCATION);
      layered->AddLayer (button, true);
      button->SetVisible (false);

      layered->SetPaintAll (true);
      layered->SetInputMode (nux::LayeredLayout::INPUT_MODE_COMPOSITE);

      layered->Raise (hori, texture_area);

      button = new nux::ToggleButton ("YOU CANT SEE ME!!!!!", NUX_TRACKER_LOCATION);
      layered->AddLayer (button, true);
      layered->RemoveLayer (button);

      layered_layout->AddLayout (layered);
      combo->AddItem (text);

      g_free (text);
    }

    main_layout->AddLayout (layered_layout, 1);

    nux::GetWindowThread ()->SetLayout(main_layout);
  }

  ~Foo ()
  {

  }

  void OnComboChangedFoRealz (nux::ComboBoxSimple *simple)
  {
    g_debug ("Active: %d", simple->GetSelectionIndex ());
    layered_layout->SetActiveLayerN (simple->GetSelectionIndex ());
  }

  nux::LayeredLayout *layered_layout;
};

void LayeredLayoutInit(nux::NThread* thread, void* InitData)
{
}

int main(int argc, char **argv)
{
  Foo *foo;

  nux::NuxInitialize(0);

  nux::WindowThread* wt = nux::CreateGUIThread(TEXT("Layered Layout"), 600, 400, 0, &LayeredLayoutInit, 0);
  foo = new Foo ();

  wt->Run(NULL);

  delete wt;
  delete foo;

  return 0;
}
