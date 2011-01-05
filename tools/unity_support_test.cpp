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
#include "NuxGraphics/GraphicsDisplay.h"
#include "NuxGraphics/GLWindowManager.h"
#include "NuxGraphics/GraphicsEngine.h"
#include "pcre.h"

#include <iostream>
#include <fstream>

bool frame_buffer_object = false;
bool vertex_program = false;
bool fragment_program = false;
bool non_power_of_2_textures = false;
bool rectangle_textures = false;
bool vertex_buffer_object = false;
bool pixel_buffer_object = false;

bool unity_not_supported = false;
bool unity_supported = false;

typedef struct
{
  int vendor_id;
  int device_id;
  const char* comment;
} GpuIdentification;

GpuIdentification GpuBlackList [] =
{
  {0x8086, 0x3577, "Intel: 82830M/MG"},
  {0x8086, 0x2562, "Intel: 82845G aka Poulsbo"},
  {0, 0, 0}
};

#define NuxPrintDetectionInfo(b, level, text, ...)  {if (b) {nux::LogOutputSeverityMessage (level, text, ##__VA_ARGS__);} }

void GetPciId (int &device_id, int &vendor_id, bool print_info)
{
  NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_NONE, TEXT("---------------------------"));
  NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_NONE, TEXT("Device pci id detection ..."));
  NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_NONE, TEXT("---------------------------"));

  vendor_id = 0;
  device_id = 0;

  #define BUFFER_SIZE 128
  char buffer [BUFFER_SIZE];

  FILE *fp ;
  fp = popen ("lspci | grep VGA", "r");

  if (fp == 0)
  {
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_INFO, TEXT("Cannot read device Id."));
    return;
  }

  nux::Memset (buffer, 0, BUFFER_SIZE);
  char * str = fgets (buffer, BUFFER_SIZE - 1, fp);

  pclose (fp);

  NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_INFO, TEXT("lspci: %s"), str);

  pcre *regexp;
  const char *regexp_str;

  regexp_str = "^[0-9]{1,2}:[0-9]{1,2}\\.[0-9]{1,2}";

  const char *error = 0;
  int error_offset = 0;

  regexp = pcre_compile (regexp_str, PCRE_MULTILINE, &error, &error_offset, 0);

  if (regexp == 0)
  {
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_INFO, TEXT("Invalid Regular Expression."));
    return;
  }

  int out_vector [10];
  unsigned int offset = 0;
  int len = strlen (buffer);

  nux::NString pci_string;
  int rc = pcre_exec (regexp, 0, buffer, len, offset, 0, out_vector, 10);
  if (rc <= -1)
  {
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_INFO, TEXT("No match found for reg exp."));
    return;
  }
  else
  {
    int i = 0;
    pci_string = nux::NString::Printf ("0000:%.*s", out_vector [2 * i + 1] - out_vector [2 * i], buffer + out_vector [2 * i]);
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_INFO, TEXT("PCI Domain:Bus:Slot.Function: %s"), pci_string.GetTCharPtr ());
    nux::NString device_id_file = nux::NString::Printf ("/sys/bus/pci/devices/%s/device", pci_string.GetTCharPtr ());
    nux::NString vendor_id_file = nux::NString::Printf ("/sys/bus/pci/devices/%s/vendor", pci_string.GetTCharPtr ());

    std::ifstream ifs;

    // Get the device id
    ifs.open (device_id_file.GetTCharPtr (), std::ifstream::in);
    std::string device_buffer;
    if ((ifs.eof () == false) && ifs.good ())
    {
      getline (ifs, device_buffer);

      device_id = nux::HexCharToInteger (device_buffer.c_str ());
      NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_INFO, TEXT("Device Id: 0x%x"), device_id);
    }
    ifs.close ();

    // Get the vendor id
    ifs.open (vendor_id_file.GetTCharPtr (), std::ifstream::in);
    std::string vendor_buffer;
    if ((ifs.eof () == false) && ifs.good ())
    {
      getline (ifs, vendor_buffer);

      vendor_id = nux::HexCharToInteger (vendor_buffer.c_str ());
      NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_INFO, TEXT("Vendor Id: 0x%x"), vendor_id);
    }
    ifs.close ();
  }

}

bool UnitySupportTest (bool print_info)
{
  NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_NONE, TEXT("------------------------"));
  NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_NONE, TEXT("Graphics device info ..."));
  NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_NONE, TEXT("------------------------"));

  nux::GraphicsDisplay* graphics_display = gGLWindowManager.CreateGLWindow("Window", 100, 100, nux::WINDOWSTYLE_NORMAL, 0, false);
  nux::GpuDevice* gpu_device = graphics_display->GetGpuDevice();

  
  int opengl_major = gpu_device->GetOpenGLMajorVersion ();
  int opengl_minor = gpu_device->GetOpenGLMinorVersion ();

  NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_NONE, TEXT("------------------------"));
  NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_NONE, TEXT("OpenGL version check ..."));
  NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_NONE, TEXT("------------------------"));
  
  if ((opengl_major == 1) && (opengl_minor < 4))
  {
    unity_not_supported = true;
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_ALERT, TEXT("System OpenGL: %d.%d [Minimum Required 1.4]"), opengl_major, opengl_minor);
  }
  else
  {
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_INFO, TEXT("System OpenGL: %d.%d [Minimum Required 1.4]"), opengl_major, opengl_minor);
  }

  NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_NONE, TEXT("-------------------_------"));
  NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_NONE, TEXT("OpenGL extension check ..."));
  NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_NONE, TEXT("--------------------------"));
  if (gpu_device->GetGpuInfo ().Support_EXT_Framebuffer_Object ())
  {
    frame_buffer_object = true;
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_INFO, "Frame Buffer Object support: YES");
  }
  else
  {
    unity_not_supported = true;
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_ALERT, "Frame Buffer Object support: NO [Required]");
  }
  
  if (gpu_device->GetGpuInfo ().Support_ARB_Vertex_Program ())
  {
    vertex_program = true;
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_INFO, "ARB Vertex Program: YES");
  }
  else
  {
    unity_not_supported = true;
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_ALERT, "ARB Vertex Program: NO [Required]");
  }
  
  if (gpu_device->GetGpuInfo ().Support_ARB_Fragment_Program ())
  {
    fragment_program = true;
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_INFO, "ARB Fragment Program: YES");
  }
  else
  {
    unity_not_supported = true;
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_ALERT, "ARB Fragment Program: NO [Required]");
  }
  
  if (gpu_device->GetGpuInfo ().Support_ARB_Texture_Non_Power_Of_Two ())
  {
    non_power_of_2_textures = true;
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_INFO, "Non Power Of Two Textures: YES");
  }
  else
  {
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_ALERT, "Non Power Of Two Textures: NO [Optional]");
  }
  
  if (gpu_device->GetGpuInfo ().Support_EXT_Texture_Rectangle () || gpu_device->GetGpuInfo ().Support_ARB_Texture_Rectangle())
  {
    rectangle_textures = true;
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_INFO, "Rectangle Texture: YES");
  }
  else
  {
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_ALERT, "Rectangle Texture: NO [Required]");
  }
  
  if (gpu_device->GetGpuInfo ().Support_ARB_Vertex_Buffer_Object ())
  {
    vertex_buffer_object = true;
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_INFO, "Vertex Buffer Object: YES");
  }
  else
  {
    unity_not_supported = true;
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_ALERT, "Vertex Buffer Object: NO [Required]");
  }

  if (gpu_device->GetGpuInfo ().Support_ARB_Pixel_Buffer_Object ())
  {
    pixel_buffer_object = true;
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_INFO, "Pixel Buffer Object: YES");
  }
  else
  {
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_ALERT, "Pixel Buffer Object: NO [Optional]");
  }

  // Evaluation
  NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_NONE, TEXT("---------------------------"));
  NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_NONE, TEXT("Unity support assesment ..."));
  NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_NONE, TEXT("---------------------------"));
  if (unity_not_supported)
  {
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_CRITICAL, "The system graphics capabilities are insufficient to run Unity.");
  }
  else
  {
    NuxPrintDetectionInfo (print_info, nux::NUX_MSG_SEVERITY_INFO, "The system can run Unity.");
  }

  delete graphics_display;
  
  if (unity_not_supported)
    return false;

  return true;
}


int main(int argc, char **argv)
{
  bool print_result = false;

  for(int i = 1; i < argc; ++i)
  {
    if (strcmp (argv[i], "--print") == 0 || strcmp (argv[i], "-p") == 0)
    {
      print_result = true;
    }
    else if (strcmp (argv[i], "--help") == 0 || strcmp (argv[i], "-h") == 0)
    {
      printf ("Usage: unity_support_test [options]           \n");
      printf ("Options:                                      \n");
      printf ("         -h, --help: print help.              \n");
      printf ("         -p, --print: print detection results.\n");

      exit (0);
    }
  }

  nux::NuxCoreInitialize (0);
  nux::NuxGraphicsInitialize ();

  int vendor_id = 0;;
  int device_id = 0;
  GetPciId (device_id, vendor_id, print_result);
  
  bool black_listed = false;
  int i = 0;
  while (GpuBlackList [i].vendor_id != 0)
  {
    if ((GpuBlackList [i].vendor_id == vendor_id) && (GpuBlackList [i].device_id == device_id))
    {
      black_listed = true;
      NuxPrintDetectionInfo (print_result, nux::NUX_MSG_SEVERITY_ALERT, TEXT("GPU Device id is not support ..."));
      break;
    }
    i++;
  }
  
  if (black_listed)
  {
    return 1;
  }

  bool supported = UnitySupportTest (print_result);

  if (supported)
    return 0;

  return 1;
}

