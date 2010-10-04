#include "Nux/Nux.h"
#include "Nux/VLayout.h"
#include "Nux/HLayout.h"
#include "Nux/WindowThread.h"
#include "Nux/Panel.h"
#include "Nux/MenuPage.h"
#include "Nux/MenuBar.h"
#include "Nux/HSplitter.h"
#include "Nux/VSplitter.h"
#include "Nux/Area.h"
#include "NuxGraphics/OpenGLEngine.h"
#include "Nux/RGBValuator.h"
#include "Nux/ComboBoxSimple.h"
#include "Nux/NumericValuator.h"
#include "Nux/Matrix4Editor.h"
#include "Nux/Matrix3Editor.h"
#include "Nux/CheckBox.h"
#include "Nux/RadioButton.h"
#include "Nux/RadioButtonGroup.h"
#include "Nux/Button.h"
#include "Nux/TableCtrl.h"
#include "Nux/PropertyList.h"
#include "Nux/GroupBox.h"
#include "Nux/TabView.h"

void ThreadWidgetInit(nux::NThread* thread, void* InitData)
{
    nux::VLayout* MainVLayout = new nux::VLayout(TEXT(""), NUX_TRACKER_LOCATION);

    nux::ComboBoxComplex* combobox = new nux::ComboBoxComplex(NUX_TRACKER_LOCATION);
    combobox->AddItem((new nux::TableItem(TEXT("TRUETRUETRUE"))));
    combobox->AddItem((new nux::TableItem(TEXT("FALSE"))));
    combobox->AddItem((new nux::TableItem(TEXT("Maybe"))));
    combobox->AddItem((new nux::TableItem(TEXT("A"))));
    combobox->AddItem((new nux::TableItem(TEXT("B"))));
    combobox->AddItem((new nux::TableItem(TEXT("C"))));
    combobox->AddItem((new nux::TableItem(TEXT("D"))));
    combobox->AddItem((new nux::TableItem(TEXT("E"))));
    combobox->AddItem((new nux::TableItem(TEXT("F"))));
    combobox->AddItem((new nux::TableItem(TEXT("G"))));
    combobox->AddItem((new nux::TableItem(TEXT("H"))));
    combobox->AddItem((new nux::TableItem(TEXT("I"))));
    combobox->AddItem((new nux::TableItem(TEXT("J"))));
    combobox->SetPopupWindowSize(120, 150);
    combobox->SetMaximumWidth(150);

    
//     nux::ComboBoxSimple* combobox_ui(new nux::ComboBoxSimple(NUX_TRACKER_LOCATION));
//     combobox_ui->AddItem(TEXT("A"));
//     combobox_ui->AddItem(TEXT("B"));
//     combobox_ui->AddItem(TEXT("C"));
//     combobox_ui->SetMaximumWidth(150);
    
    MainVLayout->AddView(combobox, 0);

    nux::GetGraphicsThread()->SetLayout(MainVLayout);
    nux::ColorLayer background(nux::Color(0xFF4D4D4D));
    static_cast<nux::WindowThread*>(thread)->SetWindowBackgroundPaintLayer(&background);
}

int main(int argc, char **argv)
{
    nux::NuxInitialize(0);
    nux::WindowThread* wt = nux::CreateGUIThread(TEXT("Hello World"), 400, 300, 0, &ThreadWidgetInit, 0);
    wt->Run(NULL);

    delete wt;
    return 0;
}
