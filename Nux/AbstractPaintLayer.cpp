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

  Geometry const& AbstractPaintLayer::GetGeometry () const
  {
    return _geometry;
  }

  void AbstractPaintLayer::SetModelViewMatrix (const Matrix4 &mat)
  {
    _model_view_matrix = mat;
  }

  Matrix4 AbstractPaintLayer::GetModelViewMatrix ()
  {
    return _model_view_matrix;
  }
}
