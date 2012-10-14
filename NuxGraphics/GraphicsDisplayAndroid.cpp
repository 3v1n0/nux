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


#include "NuxCore/NuxCore.h"
#include "GraphicsDisplay.h"
#include "GraphicsEngine.h"

#include "SystemEventSource.h"

namespace nux
{

static const std::size_t max_event_queue_size = 1;

void GraphicsDisplay::OnNewEvent(const nux::Event& event)
{
    NScopeLock lock(&m_event_guard);
    m_current_event = event;
}

/* note: these functions have been stubbed out and may not be complete yet */
Event GraphicsDisplay::GetCurrentEvent() 
{
    //NScopeLock lock(&m_event_guard);
    //return m_current_event;
}

bool GraphicsDisplay::HasFrameBufferSupport() 
{
    return false;
}

bool GraphicsDisplay::GetSystemEvent(Event *evt) 
{
    {
        NScopeLock lock(&m_event_guard);        
        *evt = m_current_event;
    }
    return true;
}

void * GraphicsDisplay::PointerGrabData() 
{
    return NULL;
}

bool GraphicsDisplay::GrabPointer(
    GrabReleaseCallback callback, 
    void *data, 
    bool replace_existing) 
{
    return false;
}

bool GraphicsDisplay::UngrabPointer(void *data) 
{
    return false;
}

bool GraphicsDisplay::GrabKeyboard(GrabReleaseCallback callback, void *data, bool replace_existing) 
{
    return false;
}

bool GraphicsDisplay::UngrabKeyboard(void *data) 
{
    return false;
}

void * GraphicsDisplay::KeyboardGrabData() 
{
    return NULL;
}

bool GraphicsDisplay::IsPauseThreadGraphicsRendering() const 
{
    return false;
}

Rect GraphicsDisplay::GetWindowGeometry()
{
  //return Rect(0, 0, 0, 0);
};

void GraphicsDisplay::ResetWindowSize() 
{
}

void GraphicsDisplay::SetWindowSize(int width, int height) 
{
}

void GraphicsDisplay::SetViewPort(int x, int y, int width, int height) 
{
}

/* note: end stub functions from this point down  */

GraphicsDisplay::GraphicsDisplay() 
{
    inlSetThreadLocalStorage(_TLS_GraphicsDisplay, this);
    SystemEventSource::Instance()->AddListener(this);
}

GraphicsDisplay::~GraphicsDisplay() 
{
    // TODO: Cleanup surfaces and client connection here
}

GpuDevice* GraphicsDisplay::GetGpuDevice() const
{
    return m_DeviceFactory;
}

GraphicsEngine* GraphicsDisplay::GetGraphicsEngine() const
{
    return m_GraphicsContext;
}

int GraphicsDisplay::GetWindowWidth()
{
    return m_ViewportSize.width;
}

int GraphicsDisplay::GetWindowHeight()
{
    return m_ViewportSize.height;
}

void GraphicsDisplay::GetWindowSize(int &w, int &h)
{
    w = m_ViewportSize.width;
    h = m_ViewportSize.height;
}

void GraphicsDisplay::ShowWindow()
{
    return;
}


void GraphicsDisplay::SwapBuffer(bool glswap)
{
    eglSwapBuffers(egl_display, egl_surface);
};


static void *createEvThread(void *arg)
{
    SystemEventSource::Instance()->Start();
}

void GraphicsDisplay::SurfaceflingerClientInit(int x, int y, float alpha)
{
    compositor_client = sf_client_create();
    
    if (!compositor_client)
    {
        printf("error creating client\n");
        return;
    }

    int w = sf_get_display_width(SURFACE_FLINGER_DEFAULT_DISPLAY_ID);
    int h = sf_get_display_height(SURFACE_FLINGER_DEFAULT_DISPLAY_ID);

    SfSurfaceCreationParameters params = {
        x,
        y,
        w,
        h,
        -1, /*rgba8888*/
        std::numeric_limits<int>::max() - 1, /* set at the middle level (ranges from -INT_MAX to INT MAX */ 
        alpha,
        "nuxsurface"
    };

    compositor_surface = sf_surface_create(compositor_client, &params);
    
    sf_surface_make_current(compositor_surface);
    egl_display = sf_client_get_egl_display(compositor_client);
    egl_config = sf_client_get_egl_config(compositor_client);
    egl_surface = sf_surface_get_egl_surface(compositor_surface);
}

bool GraphicsDisplay::CreateOpenGLWindow(
    const char *WindowTitle,
    unsigned int WindowWidth,
    unsigned int WindowHeight,
    WindowStyle Style,
    const GraphicsDisplay *Parent,
    bool FullscreenFlag,
    bool create_rendering_data)
{
    SurfaceflingerClientInit(0, 0, 1.0f);
    m_ViewportSize = Size(sf_get_display_width(SURFACE_FLINGER_DEFAULT_DISPLAY_ID), sf_get_display_height(SURFACE_FLINGER_DEFAULT_DISPLAY_ID));

    m_DeviceFactory = new GpuDevice(
        m_ViewportSize.width, 
        m_ViewportSize.height, 
        BITFMT_R8G8B8A8,
        egl_display,
        egl_config,
        egl_context,
        2,
        0);

    m_GraphicsContext = new GraphicsEngine(*this);
    return true;
 }
}
