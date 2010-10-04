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


#ifndef MATRIX3EDITOR_H
#define MATRIX3EDITOR_H

#include "EditTextBox.h"
#include "StaticTextBox.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "Button.h"
#include "TimerProc.h"

namespace nux { //NUX_NAMESPACE_BEGIN

class Matrix3Editor;
class Matrix3Preview;

class Matrix3DialogProxy
{
public:
    Matrix3DialogProxy(bool ModalWindow);
    ~Matrix3DialogProxy();

    void RecvDialogOk(Matrix3Editor* matrixeditor);
    void RecvDialogCancel(Matrix3Editor* matrixeditor);
    void RecvDialogChange(Matrix3Editor* matrixeditor);

    void Start();
    bool IsActive();
    void StopThreadMonitoring();

    Matrix3 GetMatrix() {return m_Matrix;}
    void SetMatrix(Matrix3 matrix) {m_Matrix = matrix;}

private:
    bool m_bDialogChange;
    bool m_bDialogRunning;

    Matrix3 m_PreviousMatrix;
    Matrix3 m_Matrix;

    bool m_ModalWindow;
    unsigned int m_DialogThreadID;
    NThread* m_Thread;

    friend class Matrix3Preview;
};



//! A User Interface component hosting a 4x4 Matrix. 
/*!
    Matrix3Editor is a UI component representing a 4x4 matrix.
    All elements of the matrix are writeable through the UI. The Matrix3Editor API provides access to the embeded matrix 
    through the member functions getMatrix() and setMatrix().
*/
class Matrix3Editor : public ActiveInterfaceObject
{
public:
    Matrix3Editor(Matrix3 matrix = Matrix3::IDENTITY(), NUX_FILE_LINE_PROTO);
    ~Matrix3Editor();
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    void SetParameterName(const char* parameter_name);

    void EmitButtonPress() {};

    ////////////////
    //  EMITTERS  //
    ////////////////
    void EmitIncrementComponent(int index);
    void EmitDecrementComponent(int index);
    void EmitComponentValue(float f, int index);

    /////////////////
    //  RECEIVERS  //
    /////////////////

    //! Set the UI matrix.
    /*!
        Set each component of the 3x3 matrix UI to the ones provided by the input matrix.
        \param matrix a 3x3 matrix of type Matrix3.
        \return NULL.
        \sa getMatrix().
    */
    void SetMatrix(Matrix3 matrix);

    //! Get the UI matrix.
    /*!
        Get the matrix hosted by the UI.
        \param NULL.
        \return a Matrix3 object.
        \sa setMatrix().
    */
    Matrix3 GetMatrix() const;



    /////////////////
    //  SIGNALS    //
    /////////////////
    //sigc::signal<void> sigClick;
//    sigc::signal<void, int> sigIncrementComponent;
//    sigc::signal<void, int> sigDecrementComponent;
    sigc::signal<void, int, char*> sigValidateKeyboarEntry;

    sigc::signal<void, Matrix3Editor* > sigMatrixChanged;

private:


    void RecvIdentityMatrixCmd();
    void RecvZeroMatrixCmd();
    void RecvInverseMatrixCmd();
    void RecvNegateMatrixCmd();

    void WriteMatrix();

    void RecvComponentInput(EditTextBox* textbox, const NString& text, int componentIndex);

private:
    VLayout*    m_vlayout;
    VLayout*    mtx_layout;
    HLayout*    mtx_row_layout[3];

    EditTextBox* m_MtxInput[3][3];

    Button*     m_IdentityMtxBtn;
    Button*     m_ZeroMtxBtn;
    Button*     m_InverseMtxBtn;
    Button*     m_NegateMtxBtn;
    HLayout*    m_MtxFunctionLayout;

    Matrix3x3<float> m_Matrix;
};

} //NUX_NAMESPACE_END

#endif // MATRIX3EDITOR_H
