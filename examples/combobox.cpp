#include "Nux/Nux.h"
#include "Nux/VLayout.h"
#include "Nux/HLayout.h"
#include "Nux/WindowThread.h"
#include "Nux/ComboBoxComplex.h"
#include "Nux/TableCtrl.h"

void ThreadWidgetInit(nux::NThread* thread, void* InitData)
{
    nux::VLayout* MainVLayout = new nux::VLayout(TEXT(""), NUX_TRACKER_LOCATION);

    nux::ComboBoxComplex* combobox = new nux::ComboBoxComplex(NUX_TRACKER_LOCATION);
    combobox->AddItem((new nux::TableItem(TEXT("A"))));
    combobox->AddItem((new nux::TableItem(TEXT("B"))));
    combobox->AddItem((new nux::TableItem(TEXT("C"))));
    combobox->AddItem((new nux::TableItem(TEXT("D"))));
    combobox->AddItem((new nux::TableItem(TEXT("E"))));

    combobox->SetPopupWindowSize(120, 150);
    combobox->SetMaximumWidth(150);

    
    MainVLayout->AddView(combobox, 0, nux::eCenter, nux::eFull);
    MainVLayout->SetContentDistribution(nux::eStackCenter);
    
    nux::GetGraphicsThread()->SetLayout(MainVLayout);
    nux::ColorLayer background(nux::Color(0xFF4D4D4D));
    static_cast<nux::WindowThread*>(thread)->SetWindowBackgroundPaintLayer(&background);
}

int main(int argc, char **argv)
{
    nux::NuxInitialize(0);
    nux::WindowThread* wt = nux::CreateGUIThread(TEXT("ComboBox"), 400, 300, 0, &ThreadWidgetInit, 0);
    wt->Run(NULL);

    delete wt;
    return 0;
}
