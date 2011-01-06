#include "Nux/Nux.h"
#include "Nux/TimelineEasings.h"
#include "Nux/VLayout.h"
#include "Nux/HLayout.h"
#include "Nux/WindowThread.h"
#include "Nux/Button.h"
#include "Nux/ColorPreview.h"
#include "Nux/TextureArea.h"
#include "Nux/PaintLayer.h"

class TimelineTestClass
{
public:
  nux::Timeline *timeline_1;
  nux::Timeline *timeline_2;
  nux::TextureArea *texture_area;
  nux::TextureArea *texture_area_2;

  void OnNewFrame (unsigned long msecs)
  {
    nux::Color color = nux::Color ((float)timeline_1->GetProgress (), 0.5, 0.6, 1.0);
    nux::ColorLayer *colorlayer = new nux::ColorLayer(color);
    texture_area->SetPaintLayer (colorlayer);
  }

  void OnNewFrame2 (unsigned long msecs)
  {
    nux::Color color = nux::Color (0.6, (float)timeline_2->GetProgress (), 0.5, 1.0);
    nux::ColorLayer *colorlayer = new nux::ColorLayer(color);
    texture_area_2->SetPaintLayer (colorlayer);
  }

  void Init (nux::Layout *layout)
  {
    texture_area = new nux::TextureArea ();

    layout->AddView(texture_area, 1, nux::eCenter, nux::eFull);

    texture_area_2 = new nux::TextureArea ();
    layout->AddView (texture_area_2, 1, nux::eCenter, nux::eFull);

    timeline_1 = new nux::TimelineEaseInOutQuad (1000, "Timeline_1", NUX_TRACKER_LOCATION);
    timeline_1->Looping = true;
    timeline_1->NewFrame.connect (sigc::mem_fun (this, &TimelineTestClass::OnNewFrame));

    timeline_2 = new nux::TimelineEaseOutQuad (2000, "Timeline_2", NUX_TRACKER_LOCATION);
    //timeline_2->Looping = true;
    timeline_2->NewFrame.connect (sigc::mem_fun (this, &TimelineTestClass::OnNewFrame2));
  }

};

void ThreadWidgetInit(nux::NThread* thread, void* InitData)
{
  TimelineTestClass *self = (TimelineTestClass*) InitData;
  nux::VLayout* MainVLayout = new nux::VLayout("", NUX_TRACKER_LOCATION);

  self->Init (MainVLayout);
  MainVLayout->SetContentDistribution(nux::eStackCenter);

  nux::GetWindowThread ()->SetLayout(MainVLayout);
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(thread)->SetWindowBackgroundPaintLayer(&background);
}


int main(int argc, char **argv)
{
    TimelineTestClass *test_class = new TimelineTestClass ();
    nux::NuxInitialize(0);
    nux::WindowThread* wt = nux::CreateGUIThread(TEXT("Timeline Test"), 400, 300, 0, ThreadWidgetInit, test_class);

    wt->Run(NULL);

    delete wt;
    return 0;
}
