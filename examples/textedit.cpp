#include "Nux/Nux.h"
#include "Nux/VLayout.h"
#include "Nux/HLayout.h"
#include "Nux/WindowThread.h"
#include "Nux/EditTextBox.h"

void ThreadWidgetInit(nux::NThread* thread, void* InitData)
{
    nux::VLayout* MainVLayout = new nux::VLayout(TEXT(""), NUX_TRACKER_LOCATION);

    nux::EditTextBox* text_box_edit = new nux::EditTextBox(TEXT("Hello World!"), NUX_TRACKER_LOCATION);

    text_box_edit->SetMaximumWidth(300);

    
    MainVLayout->AddActiveInterfaceObject(text_box_edit, 0, nux::eCenter, nux::eFull);
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
