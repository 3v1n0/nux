#include "Nux.h"
#include "AbstractPaintLayer.h"


namespace nux
{
  AbstractPaintLayer::AbstractPaintLayer ()
  {

  }

  AbstractPaintLayer::~AbstractPaintLayer ()
  {

  }

  void AbstractPaintLayer::SetGeometry (const Geometry &geo)
  {
    _geometry = geo;
  }

  Geometry AbstractPaintLayer::GetGeometry () const
  {
    return _geometry;
  }
}