#include "Nux.h"
#include "Timeline.h"
#include "TimelineEasings.h"

namespace nux
{
  double TimelineEaseInOutQuad::GetEasing ()
  {
    double prog = GetProgress ();
    prog *= 2.0;
    if (prog < 1)
    {
      return (prog * prog) * 0.5;
    }

    prog -= 1.0;

    return -0.5 * (prog * (prog - 2) - 1);
  }

  double TimelineEaseInQuad::GetEasing ()
  {
    double prog = GetProgress ();
    return prog * prog;
  }

  double TimelineEaseOutQuad::GetEasing ()
  {
    double prog = GetProgress ();
    return -1.0 * prog * (prog - 2);
  }

  double TimelineEaseInOutCubic::GetEasing ()
  {
    double prog = GetProgress ();
    prog *= 2.0;

    if (prog < 1)
      {
        return (prog * prog * prog) * 0.5;
      }

    prog -= 2.0;
    return 0.5 * (prog * prog * prog + 2.0);
  }

  double TimelineEaseInCubic::GetEasing ()
  {
    double prog = GetProgress ();
    return prog * prog * prog;
  }

  double TimelineEaseOutCubic::GetEasing ()
  {
    double prog = GetProgress ();
    prog -= 1.0;
    return prog * prog * prog + 1.0;
  }
}
