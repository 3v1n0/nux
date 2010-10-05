#include "Nux/Nux.h"
#include "Nux/VLayout.h"
#include "Nux/HLayout.h"
#include "Nux/WindowThread.h"
#include "Nux/CheckBox.h"


void ThreadWidgetInit(nux::NThread* thread, void* InitData)
{
    nux::VLayout* MainVLayout = new nux::VLayout(TEXT(""), NUX_TRACKER_LOCATION);

    nux::CheckBox* checkbox = new nux::CheckBox(TEXT("Hello World!"), false, NUX_TRACKER_LOCATION);

    checkbox->SetMaximumWidth(80);
    checkbox->SetMaximumHeight(60);

    
    MainVLayout->AddActiveInterfaceObject(checkbox, 1, nux::eCenter, nux::eFull);
    MainVLayout->SetContentDistribution(nux::eStackCenter);
    
    nux::GetGraphicsThread()->SetLayout(MainVLayout);
    nux::ColorLayer background(nux::Color(0xFF4D4D4D));
    static_cast<nux::WindowThread*>(thread)->SetWindowBackgroundPaintLayer(&background);
}

int main(int argc, char **argv)
{
    nux::NuxInitialize(0);
    nux::WindowThread* wt = nux::CreateGUIThread(TEXT("Edit Text Box"), 400, 300, 0, &ThreadWidgetInit, 0);
    wt->Run(NULL);

    delete wt;
    return 0;
}
