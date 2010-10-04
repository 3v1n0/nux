/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef CURVECONTROL_H
#define CURVECONTROL_H

namespace nux   //NUX_NAMESPACE_BEGIN
{

  typedef float (*FunctionCallback) (float);

  class CurveControl : public View
  {
  public:
    CurveControl (NUX_FILE_LINE_PROTO);
    ~CurveControl();
    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsContext &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsContext &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsContext &GfxContext, bool force_draw);


  private:
    void SetXAxisBounds (float minX, float maxX);
    void SetYAxisBounds (float minY, float maxY);
    void SetFunctionCallback (FunctionCallback f);
    float EvalFunction (float x);
    void UpdateGraph();

    float m_minX, m_minY, m_maxX, m_maxY;
    FunctionCallback m_FunctionCallback;

  };


} //NUX_NAMESPACE_END

#endif // CURVECONTROL_H
