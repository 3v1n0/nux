#ifndef TIMELINEEASINGS_H
#define TIMELINEEASINGS_H

namespace nux
{
  class TimelineEaseInOutQuad : public Timeline
  {
  public:
    TimelineEaseInOutQuad(unsigned int msecs, const char *Caption, NUX_FILE_LINE_DECL);
    double GetEasing();
  };

  class TimelineEaseInQuad : public Timeline
  {
  public:
    TimelineEaseInQuad(unsigned int msecs, const char *Caption, NUX_FILE_LINE_DECL);
    double GetEasing();
  };

  class TimelineEaseOutQuad : public Timeline
  {
  public:
    TimelineEaseOutQuad(unsigned int msecs, const char *Caption, NUX_FILE_LINE_DECL);
    double GetEasing();
  };

  class TimelineEaseInOutCubic : public Timeline
  {
  public:
    TimelineEaseInOutCubic(unsigned int msecs, const char *Caption, NUX_FILE_LINE_DECL);
    double GetEasing();
  };

  class TimelineEaseInCubic : public Timeline
  {
  public:
    TimelineEaseInCubic(unsigned int msecs, const char *Caption, NUX_FILE_LINE_DECL);
    double GetEasing();
  };

  class TimelineEaseOutCubic : public Timeline
  {
  public:
    TimelineEaseOutCubic(unsigned int msecs, const char *Caption, NUX_FILE_LINE_DECL);
    double GetEasing();
  };
};


#endif // TIMELINEEASINGS_H
