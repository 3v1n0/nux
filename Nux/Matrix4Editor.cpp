/*
 * Copyright 2010 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "Nux.h"
#include "Matrix4Editor.h"

namespace nux
{

  static void ThreadMatrix4EditorDialog (NThread *thread, void *InitData)
  {
    VLayout *MainLayout (new VLayout (NUX_TRACKER_LOCATION) );
    Matrix4Editor *matrixeditor (new Matrix4Editor (Matrix4::IDENTITY(), NUX_TRACKER_LOCATION) );
    matrixeditor->ComputeChildLayout(); // necessary so all element of the widget get their rightful size.
    Matrix4DialogProxy *matrixeditorproxy = static_cast<Matrix4DialogProxy *> (InitData);

    if (matrixeditorproxy)
    {
      matrixeditor->SetMatrix (matrixeditorproxy->GetMatrix() );
      matrixeditor->sigMatrixChanged.connect (sigc::mem_fun (matrixeditorproxy, &Matrix4DialogProxy::RecvDialogChange) );
    }

    HLayout *ButtonLayout (new HLayout (TEXT ("Dialog Buttons") ) );

    PushButton *OkButton (new PushButton (TEXT ("OK") ) );
    OkButton->SetMinimumWidth (60);
    OkButton->SetMinimumHeight (20);

    PushButton *CancelButton (new PushButton (TEXT ("Cancel") ) );
    CancelButton->SetMinimumWidth (60);
    CancelButton->SetMinimumHeight (20);

    OkButton->sigClick.connect (sigc::mem_fun (static_cast<WindowThread *> (thread), &WindowThread::TerminateThread) );
    OkButton->sigClick.connect (sigc::bind (sigc::mem_fun (matrixeditorproxy, &Matrix4DialogProxy::RecvDialogOk), matrixeditor) );
    CancelButton->sigClick.connect (sigc::bind (sigc::mem_fun (matrixeditorproxy, &Matrix4DialogProxy::RecvDialogCancel), matrixeditor) );
    CancelButton->sigClick.connect (sigc::mem_fun (static_cast<WindowThread *> (thread), &WindowThread::TerminateThread) );

    ButtonLayout->SetHorizontalInternalMargin (6);
    ButtonLayout->SetVerticalExternalMargin (2);
    ButtonLayout->AddView (OkButton, 0);
    ButtonLayout->AddView (CancelButton, 0);

    MainLayout->AddView (matrixeditor);
    MainLayout->AddLayout (ButtonLayout, 0);
    static_cast<WindowThread *> (thread)->SetLayout (MainLayout);

    MainLayout->SetBaseWidth (1);
    MainLayout->SetBaseHeight (1);
    MainLayout->ComputeLayout2();
    static_cast<WindowThread *> (thread)->SetWindowSize (MainLayout->GetBaseWidth(), MainLayout->GetBaseHeight() );

    // Call StopThreadMonitoring in case the dialog was close by clicking the window close button.
    //matrixeditorproxy->StopThreadMonitoring();
    //delete CancelButton;
    //delete OkButton;
    //delete matrixeditor;
  }

  Matrix4DialogProxy::Matrix4DialogProxy (bool ModalWindow)
    :   m_bDialogChange (false)
    ,   m_bDialogRunning (false)
    ,   m_ModalWindow (ModalWindow)
  {

  }

  Matrix4DialogProxy::~Matrix4DialogProxy()
  {
  }

  void Matrix4DialogProxy::Start()
  {
    m_PreviousMatrix = m_Matrix;
    m_Thread = CreateModalWindowThread (WINDOWSTYLE_TOOL, TEXT ("Matrix Editor"), 290, 230, GetWindowThread (),
                                        ThreadMatrix4EditorDialog,
                                        this);

    if (m_Thread)
    {
      m_DialogThreadID = m_Thread->GetThreadId();
      m_Thread->Start (0);
    }

    m_bDialogRunning = true;
  }

  bool Matrix4DialogProxy::IsActive()
  {
    return (m_Thread && (m_Thread->GetThreadState() != THREADSTOP) && m_bDialogRunning);
  }

  void Matrix4DialogProxy::RecvDialogOk (Matrix4Editor *matrixeditor)
  {
    m_Matrix = matrixeditor->GetMatrix();
    m_bDialogChange = true;
    m_bDialogRunning = false;
  }

  void Matrix4DialogProxy::RecvDialogCancel (Matrix4Editor *matrixeditor)
  {
    m_Matrix = m_PreviousMatrix;
    m_bDialogChange = true;
    m_bDialogRunning = false;
  }

  void Matrix4DialogProxy::RecvDialogChange (Matrix4Editor *matrixeditor)
  {
    m_Matrix = matrixeditor->GetMatrix();
    m_bDialogChange = true;
  }

  void Matrix4DialogProxy::StopThreadMonitoring()
  {
    m_Matrix = m_PreviousMatrix;
    m_bDialogChange = true;
    m_bDialogRunning = false;
    m_DialogThreadID = 0;
    m_Thread = 0;
  }

  Matrix4Editor::Matrix4Editor (Matrix4 matrix, NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_Matrix (matrix)
  {
    m_vlayout           = new VLayout (NUX_TRACKER_LOCATION);
    mtx_layout          = new VLayout (NUX_TRACKER_LOCATION);
    m_MtxFunctionLayout = new HLayout (NUX_TRACKER_LOCATION);

    mtx_row_layout[0]   = new HLayout (NUX_TRACKER_LOCATION);
    mtx_row_layout[1]   = new HLayout (NUX_TRACKER_LOCATION);
    mtx_row_layout[2]   = new HLayout (NUX_TRACKER_LOCATION);
    mtx_row_layout[3]   = new HLayout (NUX_TRACKER_LOCATION);

    m_IdentityMtxBtn    = new PushButton (TEXT (""), NUX_TRACKER_LOCATION);
    m_ZeroMtxBtn        = new PushButton (TEXT (""), NUX_TRACKER_LOCATION);
    m_InverseMtxBtn     = new PushButton (TEXT (""), NUX_TRACKER_LOCATION);
    m_NegateMtxBtn      = new PushButton (TEXT (""), NUX_TRACKER_LOCATION);

/*    m_IdentityMtxBtn->SetEnableDoubleClickEnable(false);
    m_ZeroMtxBtn->SetEnableDoubleClickEnable(false);
    m_InverseMtxBtn->SetEnableDoubleClickEnable(false);
    m_NegateMtxBtn->SetEnableDoubleClickEnable(false);
*/
    m_IdentityMtxBtn->sigClick.connect (sigc::mem_fun (this, &Matrix4Editor::RecvIdentityMatrixCmd) );
    m_ZeroMtxBtn->sigClick.connect (sigc::mem_fun (this, &Matrix4Editor::RecvZeroMatrixCmd) );
    m_InverseMtxBtn->sigClick.connect (sigc::mem_fun (this, &Matrix4Editor::RecvInverseMatrixCmd) );
    m_NegateMtxBtn->sigClick.connect (sigc::mem_fun (this, &Matrix4Editor::RecvNegateMatrixCmd) );

    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        m_MtxInput[i][j] = new EditTextBox (TEXT (""), NUX_TRACKER_LOCATION);
        m_MtxInput[i][j]->SetMinimumSize (DEFAULT_WIDGET_WIDTH + 5, PRACTICAL_WIDGET_HEIGHT);
        m_MtxInput[i][j]->SetGeometry (Geometry (0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT) );
        m_MtxInput[i][j]->sigValidateKeyboardEntry.connect (
          sigc::bind (sigc::mem_fun (this, &Matrix4Editor::RecvComponentInput), i * 4 + j) );

      }
    }

    for (int i = 0; i < 4; i++)
    {
      mtx_row_layout[i]->SetHorizontalInternalMargin (4);
      //mtx_row_layout[i]->SetHorizontalExternalMargin(4);
      //mtx_row_layout[i]->SetVerticalExternalMargin(2);
      mtx_row_layout[i]->SetContentDistribution (eStackLeft);
    }

    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        mtx_row_layout[i]->AddView (m_MtxInput[i][j], 0, eCenter, eFull);
      }
    }

    for (int i = 0; i < 4; i++)
    {
      mtx_layout->AddLayout (mtx_row_layout[i], 0, eCenter);
    }

    mtx_layout->SetContentDistribution (eStackExpand);

    m_IdentityMtxBtn->SetCaption (TEXT ("Id") );
    m_ZeroMtxBtn->SetCaption (TEXT ("Zero") );
    m_InverseMtxBtn->SetCaption (TEXT ("Inv") );
    m_NegateMtxBtn->SetCaption (TEXT ("+/-") );

    m_MtxFunctionLayout->AddView (m_IdentityMtxBtn, 0);
    m_MtxFunctionLayout->AddView (m_ZeroMtxBtn, 0);
    m_MtxFunctionLayout->AddView (m_InverseMtxBtn, 0);
    m_MtxFunctionLayout->AddView (m_NegateMtxBtn, 0);
    m_MtxFunctionLayout->SetVerticalExternalMargin (4);
    m_MtxFunctionLayout->SetHorizontalExternalMargin (0);
    m_MtxFunctionLayout->SetHorizontalInternalMargin (2);
    m_MtxFunctionLayout->SetContentDistribution (eStackLeft);
    //mtx_layout->AddLayout(&m_MtxFunctionLayout, 1,  eCenter, eMatchContent);
    mtx_layout->SetContentDistribution (eStackCenter);
    mtx_layout->SetHorizontalExternalMargin (4);
    mtx_layout->SetVerticalExternalMargin (4);
    mtx_layout->SetVerticalInternalMargin (4);

    m_vlayout->AddLayout (mtx_layout, 0, eCenter, eMatchContent);
    m_vlayout->AddLayout (m_MtxFunctionLayout, 0,  eCenter, eMatchContent);
    m_vlayout->SetContentDistribution (eStackCenter);

    SetCompositionLayout (m_vlayout);
    WriteMatrix();
  }

  Matrix4Editor::~Matrix4Editor()
  {
  }

  void Matrix4Editor::SetMatrix (Matrix4 matrix)
  {
    m_Matrix = matrix;

    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        m_MtxInput[i][j]->SetText (NString::Printf (TEXT ("%.3f"), m_Matrix.m[i][j]) );
      }
    }
  }

  Matrix4 Matrix4Editor::GetMatrix() const
  {
    return m_Matrix;
  }

  void Matrix4Editor::RecvComponentInput (EditTextBox *textbox, const NString &text, int componentIndex)
  {
    int i = componentIndex / 4;
    int j = componentIndex - 4 * i;

    float f = 0;
    f = CharToDouble (text.GetTCharPtr() );
    m_MtxInput[i][j]->SetText (NString::Printf (TEXT ("%.3f"), f) );
    m_Matrix.m[i][j] = f;

    sigMatrixChanged.emit (this);
  }

  void Matrix4Editor::WriteMatrix()
  {
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        m_MtxInput[i][j]->SetText (NString::Printf (TEXT ("%.3f"), m_Matrix.m[i][j]) );
      }
    }
  }

  long Matrix4Editor::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;

    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        ret = m_MtxInput[i][j]->ProcessEvent (ievent, ret, ProcessEventInfo);
      }
    }

    ret = m_IdentityMtxBtn->BaseProcessEvent (ievent, ret, ProcessEventInfo);
    ret = m_ZeroMtxBtn->BaseProcessEvent (ievent, ret, ProcessEventInfo);
    ret = m_InverseMtxBtn->BaseProcessEvent (ievent, ret, ProcessEventInfo);
    ret = m_NegateMtxBtn->BaseProcessEvent (ievent, ret, ProcessEventInfo);

    ret = PostProcessEvent2 (ievent, ret, ProcessEventInfo);
    return ret;
  }

  void Matrix4Editor::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    //GetPainter().PushShapeBackground(GfxContext, base, eSHAPE_CORNER_ROUND4, 0xFF4D4D4D, true, false);

    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        m_MtxInput[i][j]->NeedRedraw();
      }
    }

    m_IdentityMtxBtn->NeedRedraw();
    m_ZeroMtxBtn->NeedRedraw();
    m_InverseMtxBtn->NeedRedraw();
    m_NegateMtxBtn->NeedRedraw();
    //GetPainter().PopBackground();

    //GetPainter().PopBackground();
  }

  void Matrix4Editor::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    if (force_draw)
      GetPainter().PushDrawShapeLayer (GfxContext, mtx_layout->GetGeometry(), eSHAPE_CORNER_ROUND4, Color (0xFF000000), eAllCorners);
    else
      GetPainter().PushShapeLayer (GfxContext, mtx_layout->GetGeometry(), eSHAPE_CORNER_ROUND4, Color (0xFF000000), eAllCorners);

    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        m_MtxInput[i][j]->ProcessDraw (GfxContext, force_draw);
      }
    }

    m_IdentityMtxBtn->ProcessDraw (GfxContext, force_draw);
    m_ZeroMtxBtn->ProcessDraw (GfxContext, force_draw);
    m_InverseMtxBtn->ProcessDraw (GfxContext, force_draw);
    m_NegateMtxBtn->ProcessDraw (GfxContext, force_draw);
    GetPainter().PopBackground();
  }

  void Matrix4Editor::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void Matrix4Editor::SetParameterName (const char *parameter_name)
  {
    NeedRedraw();
  }

/////////////////
//  RECEIVERS  //
/////////////////
  void Matrix4Editor::EmitIncrementComponent (int index)
  {

    //sigSetVector.emit(m_x, m_y, m_z, m_w);
  }

  void Matrix4Editor::EmitDecrementComponent (int index)
  {

    //sigSetVector.emit(m_x, m_y, m_z, m_w);
  }

  void Matrix4Editor::EmitComponentValue (float f, int index)
  {

    //sigSetVector.emit(m_x, m_y, m_z, m_w);
  }


  void Matrix4Editor::RecvIdentityMatrixCmd()
  {
    m_Matrix.Identity();
    WriteMatrix();
    sigMatrixChanged.emit (this);

    NeedRedraw();
  }

  void Matrix4Editor::RecvZeroMatrixCmd()
  {
    m_Matrix.Zero();
    WriteMatrix();
    sigMatrixChanged.emit (this);

    NeedRedraw();
  }

  void Matrix4Editor::RecvInverseMatrixCmd()
  {
    m_Matrix.Zero();
    WriteMatrix();
    sigMatrixChanged.emit (this);

    NeedRedraw();
  }

  void Matrix4Editor::RecvNegateMatrixCmd()
  {
    m_Matrix = -m_Matrix;
    WriteMatrix();
    sigMatrixChanged.emit (this);

    NeedRedraw();
  }


}
