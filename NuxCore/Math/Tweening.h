#ifndef TWEENING_H
#define TWEENING_H


NAMESPACE_BEGIN

// Back
double BackEaseIn (double t, double b, double c, double d, double s = 1.70158);
double BackEaseOut (double t, double b, double c, double d, double s = 1.70158);
double BackEaseInOut (double t, double b, double c, double d, double s = 1.70158);
// Bounce
double BounceEaseOut (double t, double b, double c, double d);
double BounceEaseIn (double t, double b, double c, double d);
double BounceEaseInOut (double t, double b, double c, double d);
// Circ
double CircEaseIn (double t, double b, double c, double d);
double CircEaseOut (double t, double b, double c, double d);
double CircEaseInOut (double t, double b, double c, double d);
// Cubic
double CubicEaseIn (double t, double b, double c, double d);
double CubicEaseOut (double t, double b, double c, double d);
double CubicEaseInOut (double t, double b, double c, double d);
// Elastic
double ElasticEaseIn (double t, double b, double c, double d, double a, double p);
double ElasticEaseOut (double t, double b, double c, double d, double a, double p);
double ElasticEaseInOut (double t, double b, double c, double d, double a, double p);
// Expo
double ExpoEaseIn (double t, double b, double c, double d);
double ExpoEaseOut (double t, double b, double c, double d);
double ExpoEaseInOut (double t, double b, double c, double d);
// Linear
double LinearEaseNone (double t, double b, double c, double d);
double LinearEaseIn (double t, double b, double c, double d);
double LinearEaseOut (double t, double b, double c, double d);
double LinearEaseInOut (double t, double b, double c, double d);
// Quad
double QuadEaseIn (double t, double b, double c, double d);
double QuadEaseOut (double t, double b, double c, double d);
double QuadEaseInOut (double t, double b, double c, double d);
// Quart
double QuartEaseIn (double t, double b, double c, double d);
double QuartEaseOut (double t, double b, double c, double d);
double QuartEaseInOut (double t, double b, double c, double d);
// Quint
double QuintEaseIn (double t, double b, double c, double d);
double QuintEaseOut (double t, double b, double c, double d);
double QuintEaseInOut (double t, double b, double c, double d);
// Sine
double SineEaseIn (double t, double b, double c, double d);
double SineEaseOut (double t, double b, double c, double d);
double SineEaseInOut (double t, double b, double c, double d);

NAMESPACE_END

#endif // TWEENING_H

