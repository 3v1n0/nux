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


#ifndef MATRIX3PREVIEW_H
#define MATRIX3PREVIEW_H

#include "NuxGraphics/GLDeviceFactory.h"
#include "NuxGraphics/GLDeviceObjects.h"
#include "Matrix3Editor.h"

namespace nux   //NUX_NAMESPACE_BEGIN
{

  class HLayout;
  class StaticTextBox;

  class Matrix3Preview : public View
  {
  public:
    Matrix3Preview (Matrix3 matrix = Matrix3::IDENTITY(), NUX_FILE_LINE_PROTO);
    ~Matrix3Preview();
    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsContext &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsContext &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsContext &GfxContext, bool force_draw);

    sigc::signal<void, Matrix3Editor * > sigMatrixChanged;
    void RecvDialogChange (Matrix3Editor *matrixeditor);

    void SetMatrix (Matrix3 matrix);
    Matrix3 GetMatrix() const;

  private:
    void RecvTimer (void *v);
    void RecvClick (int x, int y, unsigned long button_flags, unsigned long key_flags);
    TimerFunctor *m_ChangeDetectionTimer;
    TimerHandle m_ChangeTimerHandler;

    Matrix3 m_Matrix;
    Matrix3DialogProxy *m_DialogThreadProxy;
  };


} //NUX_NAMESPACE_END

#endif // MATRIX3PREVIEW_H

