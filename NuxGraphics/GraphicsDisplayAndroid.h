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
 * Authored by: Kevin DuBois <kevin.dubois@canonical.com>
 */

#ifndef GRAPHICSDISPLAYNUX_OS_ANDROID_H
#define GRAPHICSDISPLAYNUX_OS_ANDROID_H

#include "Events.h"
#include "Gfx_Interface.h"
#include "GLTimer.h"
#include "GLDeviceObjects.h"
#include "GLRenderStates.h"
#include "SystemEventSource.h"
#include "NuxCore/ThreadGNU.h"

#include <hybris/surface_flinger/surface_flinger_compatibility_layer.h>

#include <queue>

namespace nux
{
  class GpuDevice;
  class GraphicsEngine;
  
  enum WindowStyle
  {
    WINDOWSTYLE_NORMAL,
    WINDOWSTYLE_TOOL
  };
  
  class GraphicsDisplay : 
          public GraphicSystem,
          public SystemEventSource::Listener
  {
    public:
      GraphicsDisplay();
      ~GraphicsDisplay();

      // From SystemEventSource::Listener
      void OnNewEvent(const Event& event);

      typedef void(*GrabReleaseCallback) (bool replaced, void *user_data);

      GpuDevice* GetGpuDevice() const;
      GraphicsEngine* GetGraphicsEngine() const;
      Event GetCurrentEvent();

      bool HasFrameBufferSupport();
      int GetWindowWidth();
      int GetWindowHeight();
      bool GetSystemEvent(Event *evt);
      bool isWindowMinimized() const
      {
        return false;
      }
      
      void * PointerGrabData();

      bool GrabPointer(
          GrabReleaseCallback callback, 
          void *data, 
          bool replace_existing);

      bool UngrabPointer(void *data);

      bool GrabKeyboard(
          GrabReleaseCallback callback, 
          void *data, 
          bool replace_existing);
      
      bool UngrabKeyboard(void *data);
      void * KeyboardGrabData();
      void ShowWindow();
      bool IsPauseThreadGraphicsRendering() const;
      Rect GetWindowGeometry();
      void GetWindowSize(int &w, int &h);
      void SetViewPort(int x, int y, int width, int height);
      void SwapBuffer(bool glswap = true);
      void ResetWindowSize();
      void SetWindowSize(int width, int height);

      int m_num_gfx_device_modes;
      std::vector<DeviceModes> m_gfx_device_modes;

    bool CreateOpenGLWindow(
        const char *WindowTitle,
        unsigned int WindowWidth,
        unsigned int WindowHeight,
        WindowStyle Style,
        const GraphicsDisplay *Parent,
        bool FullscreenFlag = false,
        bool create_rendering_data = true);

      friend class DisplayAccessController;
      friend class GraphicsEngine;
    
    private:

      GraphicsDisplay(const GraphicsDisplay&);
      GraphicsDisplay& operator=(const GraphicsDisplay&);
      bool operator==(const GraphicsDisplay&) const;

      // Input related
      NCriticalSection m_event_guard;
      Event m_current_event;
      
      // Android related
      void SurfaceflingerClientInit(int x, int y, float alpha);

      GraphicsEngine *m_GraphicsContext;

      GpuDevice *m_DeviceFactory;
      Size m_ViewportSize;
      
      // Compositor integration
      SfClient* compositor_client;
      SfSurface* compositor_surface;
      // EGL/GL related 
      EGLDisplay egl_display;
      EGLConfig  egl_config;
      EGLContext egl_context;
      EGLSurface egl_surface;
      
  };
}
#endif
