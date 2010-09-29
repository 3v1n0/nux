  #include "Nux/Nux.h"
#include "Nux/VLayout.h"
#include "Nux/HLayout.h"
#include "Nux/WindowThread.h"
#include "Nux/Panel.h"
#include "Nux/MenuPage.h"
#include "Nux/MenuBar.h"
#include "Nux/HSplitter.h"
#include "Nux/VSplitter.h"
#include "Nux/BaseArea.h"
#include "NuxGraphics/OpenGLEngine.h"

#include "Nux/RGBValuator.h"
#include "Nux/ComboBoxSimple.h"
//#include "Nux/Vector3Ctrl.h"
//#include "Nux/Vector4Ctrl.h"
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

#include "NuxGraphics/IniFile.h"

//#include <math.h>
//#include "Nux/TextEditCtrl.h"


void ThreadWidgetInit(nux::NThread* thread, void* InitData)
{
     smptr(nux::VLayout) MainVLayout = smptr(nux::VLayout)(new 
nux::VLayout);
     smptr(nux::HLayout) MainHLayout = smptr(nux::VLayout)(new 
nux::HLayout(TEXT("DBG_HMAIN")));

         smptr(nux::ComboBoxComplex) combobox(new nux::ComboBoxComplex());
     smptr(nux::ComboBoxSimple) combobox_ui(new nux::ComboBoxSimple());
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
     combobox_ui->AddItem(TEXT("A"));
     combobox_ui->AddItem(TEXT("B"));
     combobox_ui->AddItem(TEXT("C"));

     combobox->SetMaximumWidth(150);
     combobox_ui->SetMaximumWidth(150);
     MainVLayout->AddActiveInterfaceObject(combobox_ui, 0);

     nux::GetGraphicsThread()->SetLayout(MainVLayout);
}

int main(int argc, char **argv)
{
     nux::NuxInitialize(0);
     nux::WindowThread* wt = nux::CreateGUIThread(TEXT("Hello World"), 
1024, 768, 0, &ThreadWidgetInit, 0);
     wt->Run(NULL);

     delete wt;
     return 0;
}

