#include "Nux.h"
#include "AbstractPaintLayer.h"


namespace nux
{
  AbstractPaintLayer::AbstractPaintLayer()
  {

  }

  AbstractPaintLayer::~AbstractPaintLayer()
  {

  }

  void AbstractPaintLayer::SetGeometry(const Geometry &geo)
  {
    geometry_ = geo;
  }

  Geometry const& AbstractPaintLayer::GetGeometry() const
  {
    return geometry_;
  }

  void AbstractPaintLayer::SetModelViewMatrix(const Matrix4 &mat)
  {
    model_view_matrix_ = mat;
  }

  Matrix4 AbstractPaintLayer::GetModelViewMatrix()
  {
    return model_view_matrix_;
  }
}
