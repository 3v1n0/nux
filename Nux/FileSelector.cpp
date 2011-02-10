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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "Nux.h"
#include "FileSelector.h"

#include "EditTextBox.h"
#include "HLayout.h"
#include "Button.h"

namespace nux
{

  Color FILESELECTOR_BUTTON_COLOR = Color (0xFF4D4D4D);
  Color FILESELECTOR_BUTTON_MOUSEOVER_COLOR = Color (0xFF222222);

  FileSelector::FileSelector (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
  {
    m_hlayout           = new HLayout (NUX_TRACKER_LOCATION);
    m_OpenButton        = new Button (TEXT (""), NUX_TRACKER_LOCATION);
    m_FileEditTextBox   = new EditTextBox (TEXT (""), NUX_TRACKER_LOCATION);

    m_hlayout->AddView (m_FileEditTextBox, 1, eCenter);
    m_hlayout->AddView (m_OpenButton, 0, eCenter);

    //m_OpenButton->setCaption(TEXT("..."));
    m_OpenButton->SetMinimumWidth (20);

    m_OpenButton->OnMouseEnter.connect (sigc::mem_fun (this, &FileSelector::RecvMouseEnter) );
    m_OpenButton->OnMouseLeave.connect (sigc::mem_fun (this, &FileSelector::RecvMouseLeave) );
    m_OpenButton->OnMouseClick.connect (sigc::mem_fun (this, &FileSelector::RecvOpenButtonClick) );

    SetMinimumSize (DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    SetLayout(m_hlayout);

    NString Path = NUX_FINDRESOURCELOCATION (TEXT ("Icons/Folder-16x16.png") );
    m_Texture = GetGpuDevice()->CreateSystemCapableTexture ();
    m_Texture->Update (Path.GetTCharPtr() );
  }

  FileSelector::~FileSelector()
  {
    m_Texture->UnReference ();
  }

  long FileSelector::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    ret = m_FileEditTextBox->OnEvent (ievent, ret, ProcessEventInfo);
    ret = m_OpenButton->OnEvent (ievent, ret, ProcessEventInfo);
    ret = PostProcessEvent2 (ievent, ret, ProcessEventInfo);
    return ret;
  }

  void FileSelector::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    GetPainter().PaintBackground (GfxContext, base);

    if (m_OpenButton->IsMouseInside() )
    {

      GetPainter().PaintShapeCorner (GfxContext, m_OpenButton->GetGeometry(), FILESELECTOR_BUTTON_MOUSEOVER_COLOR, eSHAPE_CORNER_ROUND4,
                                 eCornerTopRight | eCornerBottomRight, false);
    }
    else
    {
      GetPainter().PaintShapeCorner (GfxContext, m_OpenButton->GetGeometry(), FILESELECTOR_BUTTON_COLOR, eSHAPE_CORNER_ROUND4,
                                 eCornerTopRight | eCornerBottomRight, false);
    }

    GeometryPositioning gp (eHACenter, eVACenter);
    Geometry TextureGeo = Geometry (0, 0, m_Texture->GetWidth(), m_Texture->GetHeight() );
    Geometry GeoPo = ComputeGeometryPositioning (m_OpenButton->GetGeometry(), TextureGeo, gp);

    GetThreadGraphicsContext()->GetRenderStates().SetBlend (TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GetThreadGraphicsContext()->GetRenderStates().SetColorMask (TRUE, TRUE, TRUE, FALSE);

    nux::TexCoordXForm texxform;
    GfxContext.QRP_1Tex (GeoPo.x, GeoPo.y, GeoPo.width, GeoPo.height, m_Texture->GetDeviceTexture(), texxform, nux::Color::White);

    GetThreadGraphicsContext()->GetRenderStates().SetColorMask (TRUE, TRUE, TRUE, TRUE);
    GetThreadGraphicsContext()->GetRenderStates().SetBlend (FALSE);

    m_FileEditTextBox->NeedRedraw();
  }

  void FileSelector::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    m_FileEditTextBox->ProcessDraw (GfxContext, force_draw);
  }

  void FileSelector::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void FileSelector::RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void FileSelector::RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {

    NeedRedraw();
  }

  void FileSelector::RecvOpenButtonClick (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
    sigClick.emit();
  }


}
