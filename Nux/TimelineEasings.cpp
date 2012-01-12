#include "Nux.h"
#include "Timeline.h"
#include "TimelineEasings.h"

namespace nux
{
  TimelineEaseInOutQuad::TimelineEaseInOutQuad(unsigned int msecs, const char *Caption, NUX_FILE_LINE_DECL)
    :   Timeline(msecs, Caption, NUX_FILE_LINE_PARAM)
  {
  }

  double TimelineEaseInOutQuad::GetEasing()
  {
    double prog = GetProgress();
    prog *= 2.0;
    if (prog < 1)
    {
      return (prog * prog) * 0.5;
    }

    prog -= 1.0;

    return -0.5 * (prog * (prog - 2) - 1);
  }


  TimelineEaseInQuad::TimelineEaseInQuad(unsigned int msecs, const char *Caption, NUX_FILE_LINE_DECL)
    :   Timeline(msecs, Caption, NUX_FILE_LINE_PARAM)
  {
  }

  double TimelineEaseInQuad::GetEasing()
  {
    double prog = GetProgress();
    return prog * prog;
  }


  TimelineEaseOutQuad::TimelineEaseOutQuad(unsigned int msecs, const char *Caption, NUX_FILE_LINE_DECL)
    :   Timeline(msecs, Caption, NUX_FILE_LINE_PARAM)
  {
  }

  double TimelineEaseOutQuad::GetEasing()
  {
    double prog = GetProgress();
    return -1.0 * prog * (prog - 2);
  }

  TimelineEaseInOutCubic::TimelineEaseInOutCubic(unsigned int msecs, const char *Caption, NUX_FILE_LINE_DECL)
    :   Timeline(msecs, Caption, NUX_FILE_LINE_PARAM)
  {
  }


  double TimelineEaseInOutCubic::GetEasing()
  {
    double prog = GetProgress();
    prog *= 2.0;

    if (prog < 1)
      {
        return (prog * prog * prog) * 0.5;
      }

    prog -= 2.0;
    return 0.5 * (prog * prog * prog + 2.0);
  }

  TimelineEaseInCubic::TimelineEaseInCubic(unsigned int msecs, const char *Caption, NUX_FILE_LINE_DECL)
    :   Timeline(msecs, Caption, NUX_FILE_LINE_PARAM)
  {
  }


  double TimelineEaseInCubic::GetEasing()
  {
    double prog = GetProgress();
    return prog * prog * prog;
  }

  TimelineEaseOutCubic::TimelineEaseOutCubic(unsigned int msecs, const char *Caption, NUX_FILE_LINE_DECL)
    :   Timeline(msecs, Caption, NUX_FILE_LINE_PARAM)
  {
  }


  double TimelineEaseOutCubic::GetEasing()
  {
    double prog = GetProgress();
    prog -= 1.0;
    return prog * prog * prog + 1.0;
  }
}
