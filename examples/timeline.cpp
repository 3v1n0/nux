#include "Nux/Nux.h"
#include "Nux/VLayout.h"
#include "Nux/HLayout.h"
#include "Nux/WindowThread.h"
#include "Nux/Button.h"

void timeline_callback (unsigned long msecs)
{
  // got a new frame!
  printf ("New Frame!\n");
}

void timeline_completed ()
{
  printf ("Completed!\n");
}
class TimelineTestClass
{
public:
  nux::Button *button;
  nux::Timeline *timeline_1;

  void OnNewFrame (unsigned long msecs)
  {
    printf ("InClass New Frame!\n");
  }

  void Init (nux::Layout *layout)
  {
    button = new nux::Button("Timeline Test!", NUX_TRACKER_LOCATION);

    button->SetMaximumWidth(80);
    button->SetMaximumHeight(60);

    layout->AddView(button, 1, nux::eCenter, nux::eFull);

    nux::Timeline *timeline = new nux::Timeline (1000, "Timeline_1", NUX_TRACKER_LOCATION);
    timeline->Looping = true;
    timeline->NewFrame.connect (sigc::mem_fun (this, &TimelineTestClass::OnNewFrame));
    button->GetApplication ()->AddTimeline (timeline);
  }

};

void ThreadWidgetInit(nux::NThread* thread, void* InitData)
{
  TimelineTestClass *self = (TimelineTestClass*) InitData;
  nux::VLayout* MainVLayout = new nux::VLayout("", NUX_TRACKER_LOCATION);

  self->Init (MainVLayout);
  MainVLayout->SetContentDistribution(nux::eStackCenter);

  nux::GetGraphicsThread()->SetLayout(MainVLayout);
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(thread)->SetWindowBackgroundPaintLayer(&background);
}


int main(int argc, char **argv)
{
    TimelineTestClass *test_class = new TimelineTestClass ();
    nux::NuxInitialize(0);
    nux::WindowThread* wt = nux::CreateGUIThread(TEXT("Timeline Test"), 400, 300, 0, ThreadWidgetInit, test_class);

    nux::Timeline *timeline = new nux::Timeline (1000, "My Timeline", NUX_TRACKER_LOCATION);
    timeline->Looping = true;
    timeline->NewFrame.connect (sigc::ptr_fun (timeline_callback));
    timeline->Completed.connect (sigc::ptr_fun (timeline_completed));

    wt->AddTimeline (timeline);
    wt->Run(NULL);

    delete wt;
    return 0;
}
