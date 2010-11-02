#include "Nux/Nux.h"
#include "Nux/VLayout.h"
#include "Nux/HLayout.h"
#include "Nux/WindowThread.h"
#include "NuxGraphics/OpenGLEngine.h"

#include "Nux/TextureArea.h"
#include "NuxImage/CairoGraphics.h"


void ThreadWidgetInit(nux::NThread* thread, void* InitData)
{
    nux::CairoGraphics cairo_graphics(CAIRO_FORMAT_ARGB32, 64, 64);
    cairo_t *cr = cairo_graphics.GetContext();

    cairo_set_line_width (cr, 1);
    cairo_set_source_rgba (cr, 0.0f, 1.0f, 0.5f, 1.0f);
    cairo_rectangle (cr, 0, 0, 32, 32);
    cairo_fill (cr);
    cairo_set_source_rgba (cr, 1.0f, 0.0f, 0.5f, 1.0f);
    cairo_rectangle (cr, 32, 32, 32, 32);
    cairo_fill (cr);
    cairo_destroy (cr);

    nux::NBitmapData* bitmap =  cairo_graphics.GetBitmap();

    // NTexture2D is the high level representation of an image that is backed by an actual opengl texture.
    nux::Texture2D* texture2D = new nux::Texture2D();
    texture2D->Update(bitmap);

    nux::VLayout* MainVLayout((new nux::VLayout()));

    // TextureArea is a widget that uses the image we have created above to render on the screen.
    nux::TextureArea* texture_area = new nux::TextureArea();
    texture_area->SetTexture(texture2D);

    texture_area->SetMinMaxSize(64, 64);
    MainVLayout->AddView(texture_area, 1, nux::eCenter, nux::eFix);
    MainVLayout->SetContentDistribution(nux::eStackCenter);

    nux::GetGraphicsThread()->SetLayout(MainVLayout);
}

int main(int argc, char **argv)
{
    nux::NuxInitialize(0);
    nux::WindowThread* wt = nux::CreateGUIThread(TEXT("Cairo Graphics"), 400, 300, 0, &ThreadWidgetInit, 0);
    wt->Run(NULL);
    delete wt;
    return 0;
}
