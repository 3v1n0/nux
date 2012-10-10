// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
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
 * Authored by: Jason Smith <jason.smith@canonical.com>
 * Authored by: Jay Taoko <jay.taoko@canonical.com>
 */

const char* movie_list[] = {
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
"nux.png",
0
};

#include "Nux/Nux.h"
#include "Nux/HLayout.h"

#include "NuxGraphics/GraphicsDisplay.h"
#include "NuxGraphics/GLShader.h"
#include "NuxGraphics/GpuDevice.h"
#include "NuxGraphics/GLDeviceObjects.h"
#include "NuxGraphics/GLShader.h"
#include "NuxGraphics/GraphicsEngine.h"

#include "Nux/Coverflow.h"
#include "Nux/CoverflowItem.h"

namespace nux
{
//class BaseTexture;

class BasicCoverflowItem : public CoverflowItem
{
public:
  BasicCoverflowItem(std::string const& name, std::string const& filename);
  ObjectPtr<BaseTexture> GetTexture() const;

private:
  ObjectPtr<BaseTexture> texture_;
};

BasicCoverflowItem::BasicCoverflowItem(std::string const& name,
                                       std::string const& filename)
: CoverflowItem(name)
{
  texture_.Adopt(LoadTextureFromFile(filename));
}

ObjectPtr<BaseTexture> BasicCoverflowItem::GetTexture() const
{
  return texture_;
}

}

void CoverflowThread(nux::NThread* thread, void* /* InitData */)
{
  nux::Coverflow* coverflow = new nux::Coverflow();
  coverflow->fov = 45;
  coverflow->true_perspective = false;
  coverflow->folding_angle = 45;
  coverflow->reflection_size = 0.5f;
  coverflow->show_reflection = true;
  nux::HLayout* main_layout(new nux::HLayout(NUX_TRACKER_LOCATION));
  main_layout->AddView(coverflow, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);

  static_cast<nux::WindowThread*>(thread)->SetLayout(main_layout);

  int i = 0;
  nux::CoverflowModel::Ptr model = coverflow->model();
  std::string base_path = PKGDATADIR"/UITextures/";
  while(movie_list[i] != NULL)
  {
    std::string name = "Nux The Movie";
    std::string movie_path = base_path + movie_list[i];
    nux::CoverflowItem::Ptr item(new nux::BasicCoverflowItem(name, movie_path));
    model->AddItem(item);
    i++;
  }
}

int main()
{
  nux::NuxInitialize(0);

  std::unique_ptr<nux::WindowThread> wt(nux::CreateGUIThread("CoverFlow", 1100, 480, 0, &CoverflowThread, 0));
  wt->Run(0);

  return 0;
}
