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

bool frame_buffer_object = false;
bool vertex_program = false;
bool fragment_program = false;
bool non_power_of_2_textures = false;
bool rectangle_textures = false;
bool vertex_buffer_object = false;
bool pixel_buffer_object = false;

bool unity_not_supported = false;
bool unity_supported = false;

void UnitySupportTest ()
{
  nux::GraphicsDisplay* graphics_display = gGLWindowManager.CreateGLWindow("Window", 100, 100, nux::WINDOWSTYLE_NORMAL, 0, false);
  nux::GraphicsEngine* graphics_engine = graphics_display->GetGraphicsEngine();
  nux::GpuDevice* gpu_device = graphics_display->GetGpuDevice();

  
  int opengl_major = gpu_device->GetOpenGLMajorVersion ();
  int opengl_minor = gpu_device->GetOpenGLMinorVersion ();
  
  if ((opengl_major == 1) && (opengl_minor < 4))
  {
    unity_not_supported = true;
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_ALERT, "System OpenGL: %d.%d [Minimum Required %d.%d]");
  }
  else
  {
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_INFO, "System OpenGL: %d.%d [Minimum Required %d.%d]");
  }

#if defined (NUX_OS_LINUX)

#endif

  if (gpu_device->GetGpuInfo ().Support_EXT_Framebuffer_Object ())
  {
    frame_buffer_object = true;
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_INFO, "Frame Buffer Object support: YES");
  }
  else
  {
    unity_not_supported = true;
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_ALERT, "Frame Buffer Object support: NO [Required]");
  }
  
  if (gpu_device->GetGpuInfo ().Support_ARB_Vertex_Program ())
  {
    vertex_program = true;
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_INFO, "ARB Vertex Program: YES");
  }
  else
  {
    unity_not_supported = true;
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_ALERT, "ARB Vertex Program: NO [Required]");
  }
  
  if (gpu_device->GetGpuInfo ().Support_ARB_Fragment_Program ())
  {
    fragment_program = true;
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_INFO, "ARB Fragment Program: YES");
  }
  else
  {
    unity_not_supported = true;
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_ALERT, "ARB Fragment Program: NO [Required]");
  }
  
  if (gpu_device->GetGpuInfo ().Support_ARB_Texture_Non_Power_Of_Two ())
  {
    non_power_of_2_textures = true;
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_INFO, "Non Power Of Two Textures: YES");
  }
  else
  {
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_ALERT, "Non Power Of Two Textures: NO [Optional]");
  }
  
  if (gpu_device->GetGpuInfo ().Support_EXT_Texture_Rectangle () || gpu_device->GetGpuInfo ().Support_ARB_Texture_Rectangle())
  {
    rectangle_textures = true;
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_INFO, "Rectangle Texture: YES");
  }
  else
  {
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_ALERT, "Rectangle Texture: NO [Required]");
  }
  
  if (gpu_device->GetGpuInfo ().Support_ARB_Vertex_Buffer_Object ())
  {
    vertex_buffer_object = true;
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_INFO, "Vertex Buffer Object: YES");
  }
  else
  {
    unity_not_supported = true;
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_ALERT, "Vertex Buffer Object: NO [Required]");
  }

  if (gpu_device->GetGpuInfo ().Support_ARB_Pixel_Buffer_Object ())
  {
    pixel_buffer_object = true;
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_INFO, "Pixel Buffer Object: YES");
  }
  else
  {
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_ALERT, "Pixel Buffer Object: NO [Optional]");
  }

  // Evaluation

  if (unity_not_supported)
  {
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_CRITICAL, "The system graphics capabilities are insufficient to run Unity.");
  }
  else
  {
    nux::LogOutputSeverityMessage (nux::NUX_MSG_SEVERITY_CRITICAL, "The system can run Unity.");
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
