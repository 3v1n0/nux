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


#ifndef MATRIX4PREVIEW_H
#define MATRIX4PREVIEW_H

#include "NuxGraphics/GLDeviceFactory.h"
#include "NuxGraphics/GLDeviceObjects.h"
#include "Matrix4Editor.h"

NAMESPACE_BEGIN_GUI

class HLayout;
class StaticTextBox;

class Matrix4Preview : public ActiveInterfaceObject
{
public:
    Matrix4Preview(Matrix4 matrix = Matrix4::IDENTITY());
    ~Matrix4Preview();
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    sigc::signal<void, const weaksmptr(Matrix4Editor) > sigMatrixChanged;
    void RecvDialogChange(const weaksmptr(Matrix4Editor) matrixeditor);

    void SetMatrix(Matrix4 matrix);
    Matrix4 GetMatrix() const;

private:
    void RecvTimer(void* v);
    void RecvClick(int x, int y, unsigned long button_flags, unsigned long key_flags);
    TimerFunctor* m_ChangeDetectionTimer;
    TimerHandle* m_ChangeTimerHandler;

    Matrix4 m_Matrix;
    Matrix4DialogProxy* m_DialogThreadProxy;
};


NAMESPACE_END_GUI

#endif // MATRIX4PREVIEW_H

