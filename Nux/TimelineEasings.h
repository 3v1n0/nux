#ifndef TIMELINEEASINGS_H
#define TIMELINEEASINGS_H

namespace nux
{
  class TimelineEaseInOutQuad : public Timeline
  {
  public:
    double GetEasing ();
  };

  class TimelineEaseInQuad : public Timeline
  {
  public:
    double GetEasing ();
  };

  class TimelineEaseOutQuad : public Timeline
  {
  public:
    double GetEasing ();
  };

  class TimelineEaseInOutCubic : public Timeline
  {
  public:
    double GetEasing ();
  };

  class TimelineEaseInCubic : public Timeline
  {
  public:
    double GetEasing ();
  };

  class TimelineEaseOutCubic : public Timeline
  {
  public:
    double GetEasing ();
  };
};


#endif // TIMELINEEASINGS_H
