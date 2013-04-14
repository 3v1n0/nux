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
#include "FileSelector.h"

#include "EditTextBox.h"
#include "HLayout.h"
#include "Button.h"

namespace nux
{

  Color FILESELECTOR_BUTTON_COLOR = Color(0xFF4D4D4D);
  Color FILESELECTOR_BUTTON_MOUSEOVER_COLOR = Color(0xFF222222);

  FileSelector::FileSelector(NUX_FILE_LINE_DECL)
    :   View(NUX_FILE_LINE_PARAM)
  {
    m_hlayout           = new HLayout(NUX_TRACKER_LOCATION);
    m_OpenButton        = new Button("", NUX_TRACKER_LOCATION);
    m_FileEditTextBox   = new EditTextBox("", NUX_TRACKER_LOCATION);

    m_hlayout->AddView(m_FileEditTextBox, 1, eCenter);
    m_hlayout->AddView(m_OpenButton, 0, eCenter);

    //m_OpenButton->setCaption("...");
    m_OpenButton->SetMinimumWidth(20);

    m_OpenButton->mouse_enter.connect(sigc::mem_fun(this, &FileSelector::RecvMouseEnter));
    m_OpenButton->mouse_leave.connect(sigc::mem_fun(this, &FileSelector::RecvMouseLeave));
    m_OpenButton->mouse_click.connect(sigc::mem_fun(this, &FileSelector::RecvOpenButtonClick));

    SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    SetLayout(m_hlayout);

    std::string Path = NUX_FINDRESOURCELOCATION("Icons/Folder-16x16.png");
    m_Texture = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture();
    m_Texture->Update(Path.c_str());
  }

  FileSelector::~FileSelector()
  {
    m_Texture->UnReference();
  }

  void FileSelector::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    Geometry base = GetGeometry();

    GetPainter().PaintBackground(graphics_engine, base);

    if (m_OpenButton->IsMouseInside())
    {

      GetPainter().PaintShapeCorner(graphics_engine, m_OpenButton->GetGeometry(), FILESELECTOR_BUTTON_MOUSEOVER_COLOR, eSHAPE_CORNER_ROUND4,
                                 eCornerTopRight | eCornerBottomRight, false);
    }
    else
    {
      GetPainter().PaintShapeCorner(graphics_engine, m_OpenButton->GetGeometry(), FILESELECTOR_BUTTON_COLOR, eSHAPE_CORNER_ROUND4,
                                 eCornerTopRight | eCornerBottomRight, false);
    }

    GeometryPositioning gp(eHACenter, eVACenter);
    Geometry TextureGeo = Geometry(0, 0, m_Texture->GetWidth(), m_Texture->GetHeight());
    Geometry GeoPo = ComputeGeometryPositioning(m_OpenButton->GetGeometry(), TextureGeo, gp);

    GetGraphicsDisplay()->GetGraphicsEngine()->GetRenderStates().SetBlend(TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GetGraphicsDisplay()->GetGraphicsEngine()->GetRenderStates().SetColorMask(TRUE, TRUE, TRUE, FALSE);

    nux::TexCoordXForm texxform;
    graphics_engine.QRP_1Tex(GeoPo.x, GeoPo.y, GeoPo.width, GeoPo.height, m_Texture->GetDeviceTexture(), texxform, nux::color::White);

    GetGraphicsDisplay()->GetGraphicsEngine()->GetRenderStates().SetColorMask(TRUE, TRUE, TRUE, TRUE);
    GetGraphicsDisplay()->GetGraphicsEngine()->GetRenderStates().SetBlend(FALSE);

    m_FileEditTextBox->QueueDraw();
  }

  void FileSelector::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    m_FileEditTextBox->ProcessDraw(graphics_engine, force_draw);
  }

  void FileSelector::RecvMouseEnter(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }

  void FileSelector::RecvMouseLeave(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {

    QueueDraw();
  }

  void FileSelector::RecvOpenButtonClick(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
    sigClick.emit();
  }


}
