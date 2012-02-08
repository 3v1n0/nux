/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * version 3.0 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3.0 for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Authored by Gordon Allott <gord.allott@canonical.com>
 */

#ifndef TIMELINE_H
#define TIMELINE_H

namespace nux
{
  class Timeline : public Object
  {
  public:

    Timeline(unsigned int msecs, const char *Caption, NUX_FILE_LINE_DECL);
    ~Timeline();

    void Stop();
    void Start();
    void Pause();
    void Rewind();

    double GetProgress(); //ranges from 0.0 -> 1.0
    virtual double GetEasing(); // override to provide easing values

    void DoTick(unsigned long msecs); // If you use this, you make baby kittens cry

    bool Looping;
    unsigned long Duration;
    bool IsPlaying;

    sigc::signal <void, unsigned long> NewFrame;
    sigc::signal <void> Completed;
    sigc::signal <void> Started;
    sigc::signal <void> Paused;

  protected:
    unsigned long _ElapsedTime;
  };


  // template <typename T>
  // class PropertyAnimationContainer
  // {

  // };

  // class AnimatedPropertyBase
  // {
  // public:
  //   AnimatedPropertyBase(){};

  //   virtual void Animate(float t){};

  // };

  // // Explicit specialization. The explicit type could be float, int, Color....
  // template <>
  // class PropertyAnimationContainer<float> : public AnimatedPropertyBase
  // {
  // public:
  //   PropertyAnimationContainer(Property<float>& prop, float start_value, float end_value)
  //     : prop_(prop)
  //   {
  //     start_value_ = start_value;
  //     end_value_ = end_value;
  //   }

  //   virtual void Animate(float t)
  //   {
  //     float value = (1.0f - t) * start_value_ + t * end_value_;
  //     prop_.set(value);
  //   }

  //   Property<float>& prop_;
  //   float start_value_;
  //   float end_value_;

  // };

  // template <>
  // class PropertyAnimationContainer<Color> : public AnimatedPropertyBase
  // {
  // public:
  //   PropertyAnimationContainer(Property<Color>& prop, Color start_value, Color end_value)
  //     : prop_(prop)
  //   {
  //     start_value_ = start_value;
  //     end_value_ = end_value;
  //   }

  //   virtual void Animate(float t)
  //   {
  //     Color value = (1.0f - t) * start_value_ + t * end_value_;
  //     prop_.set(value);
  //   }

  //   Property<Color>& prop_;
  //   Color start_value_;
  //   Color end_value_;

  // };

  // class AnimationTimeline : public Object
  // {
  // public:
  //   AnimationTimeline() {};

  //   template <typename T>
  //   void AddProperty(Property<T>& prop, T start_value, T end_value)
  //   {
  //     PropertyAnimationContainer<T>* a = new PropertyAnimationContainer<T> (prop, start_value, end_value);

  //     animated_properties_.push_back(a);
  //   }

  //   void Animate(float t)
  //   {
  //     std::list<AnimatedPropertyBase*>::iterator it;

  //     // Go through all the properties and update them.
  //     for (it = animated_properties_.begin(); it != animated_properties_.end(); ++it)
  //     {
  //       (*it)->Animate(t);
  //     }
  //   }

  //   std::list<AnimatedPropertyBase*> animated_properties_;
  // };
}

#endif // TIMELINE_H
