#include "Nux/Nux.h"
#include "Nux/WindowThread.h"
#include "NuxGraphics/GraphicsEngine.h"

#include "Nux/TextureArea.h"
#include "Nux/HLayout.h"
#include "Nux/VLayout.h"
#include "Nux/GridHLayout.h"
#include "Nux/GridVLayout.h"


void GridLayoutInit(nux::NThread* thread, void* InitData)
{
  nux::HLayout *main_layout((new nux::HLayout(NUX_TRACKER_LOCATION)));

  nux::GridHLayout *grid_h_layout ((new nux::GridHLayout (NUX_TRACKER_LOCATION)));
  for (int i = 0; i < 60; i++)
  {
    nux::ColorLayer color (nux::Color::RandomColor ());
    nux::TextureArea* texture_area = new nux::TextureArea ();
    texture_area->SetPaintLayer (&color);

    grid_h_layout->AddView (texture_area, 1, nux::eLeft, nux::eFull);
  }

  grid_h_layout->ForceChildrenSize (true);
  grid_h_layout->SetChildrenSize (64, 42);
  grid_h_layout->EnablePartialVisibility (false);

  grid_h_layout->SetVerticalExternalMargin (4);
  grid_h_layout->SetHorizontalExternalMargin (4);
  grid_h_layout->SetVerticalInternalMargin (4);
  grid_h_layout->SetHorizontalInternalMargin (4);


  nux::GridVLayout *grid_v_layout ((new nux::GridVLayout (NUX_TRACKER_LOCATION)));
  for (int i = 0; i < 60; i++)
  {
    nux::ColorLayer color (nux::Color::RandomColor());
    nux::TextureArea* texture_area = new nux::TextureArea();
    texture_area->SetPaintLayer (&color);

    grid_v_layout->AddView(texture_area, 1, nux::eLeft, nux::eFull);
  }

  grid_v_layout->ForceChildrenSize (true);
  grid_v_layout->SetChildrenSize (64, 64);
  grid_v_layout->EnablePartialVisibility (false);

  grid_v_layout->SetVerticalExternalMargin (6);
  grid_v_layout->SetHorizontalExternalMargin (6);
  grid_v_layout->SetVerticalInternalMargin (6);
  grid_v_layout->SetHorizontalInternalMargin (6);


  main_layout->AddLayout (grid_h_layout, 1);
  main_layout->AddLayout (grid_v_layout, 1);
  
  nux::GetWindowThread ()->SetLayout(main_layout);
}

int main(int argc, char **argv)
{
  nux::NuxInitialize(0);

  nux::WindowThread* wt = nux::CreateGUIThread(TEXT("Grid Layout"), 400, 200, 0, &GridLayoutInit, 0);
  wt->Run(NULL);

  delete wt;

  return 0;
}
