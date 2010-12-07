/*
 * Copyright (C) 2010 Canonical Ltd
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
 * Authored by: Jay Taoko <jaytaoko@canonical.com>
 *
 */

/*
 * Tests: 
 *  - Test the GPU ability to run Unity
 */

#include "NuxCore/NuxCore.h"
#include "NuxImage/BitmapFormats.h"

#include "NuxGraphics/GraphicsDisplayX11.h"

#include "NuxGraphics/GLWindowManager.h"
#include "NuxGraphics/GraphicsEngine.h"


void UnitySupportTest ()
{
  nux::GraphicsDisplay* graphics_display = gGLWindowManager.CreateGLWindow("Window", 100, 100, nux::WINDOWSTYLE_NORMAL, 0, false);
  nux::GraphicsEngine* graphics_engine = graphics_display->GetGraphicsEngine();
  nux::GpuDevice* gpu_device = graphics_display->GetGpuDevice();
  
  if (gpu_device->GetGpuInfo ().Support_EXT_Framebuffer_Object ())
  {
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_INFO, "Frame Buffer Object support: YES");
  }
  else
  {
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_ALERT, "Frame Buffer Object support: NO");
  }
  
  if (gpu_device->GetGpuInfo ().Support_ARB_Vertex_Program ())
  {
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_INFO, "ARB Vertex Program: YES");
  }
  else
  {
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_ALERT, "ARB Vertex Program: NO");
  }
  
  if (gpu_device->GetGpuInfo ().Support_ARB_Fragment_Program ())
  {
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_INFO, "ARB Fragment Program: YES");
  }
  else
  {
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_ALERT, "ARB Fragment Program: NO");
  }
  
  if (gpu_device->GetGpuInfo ().Support_ARB_Texture_Non_Power_Of_Two ())
  {
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_INFO, "Non Power Of Two Textures: YES");
  }
  else
  {
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_ALERT, "Non Power Of Two Textures: NO");
  }
  
  if (gpu_device->GetGpuInfo ().Support_EXT_Texture_Rectangle () || gpu_device->GetGpuInfo ().Support_ARB_Texture_Rectangle())
  {
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_INFO, "Rectangle Texture: YES");
  }
  else
  {
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_ALERT, "Rectangle Texture: NO");
  }
  
  delete graphics_display;
}


int main(int argc, char **argv)

{
  nux::NuxCoreInitialize(0);
  nux::NuxGraphicsInitialize();

  UnitySupportTest ();

  return 0;
}
