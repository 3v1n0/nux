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
#include "Nux/PushButton.h"
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
      nux::PushButton* button = new nux::PushButton (text, NUX_TRACKER_LOCATION);

      //nux::Color c = nux::Color::RandomColor ();
      //c.SetAlpha (0.5f);
      //nux::ColorLayer color (c);
      //nux::TextureArea* texture_area = new nux::TextureArea ();
      //texture_area->SetPaintLayer (&color);

      layered_layout->AddView (button, 1, nux::eLeft, nux::eFull);
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
    layered_layout->SetActiveLayer (simple->GetSelectionIndex ());
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

  nux::WindowThread* wt = nux::CreateGUIThread(TEXT("Layered Layout"), 400, 200, 0, &LayeredLayoutInit, 0);
  foo = new Foo ();

  wt->Run(NULL);

  delete wt;
  delete foo;

  return 0;
}
