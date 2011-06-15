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

#include "FolderTreeItem.h"

namespace nux
{

  FolderTreeItem::FolderTreeItem (const TCHAR *name, const TCHAR *Path, bool Searchable, NodeParameterType type)
    :   TreeItem (name, type)
  {
    m_Synchronize   = false;
    m_Path          = Path;
    m_Searchable    = Searchable;

    TableItem::SetAlwaysShowOpeningButton (true);

    NString IconPath = NUX_FINDRESOURCELOCATION (TEXT ("Icons/FolderOpen-16x16.png") );
    FolderOpenIcon = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture ();
    FolderOpenIcon->Update (IconPath.GetTCharPtr() );
    IconPath = NUX_FINDRESOURCELOCATION (TEXT ("Icons/FolderClose-16x16.png") );
    FolderClosedIcon = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture ();
    FolderClosedIcon->Update (IconPath.GetTCharPtr() );
    //FolderOpenIcon.Update(TEXT("../Media/Icons/FolderOpenGrey-16x16.png"));
    //FolderClosedIcon.Update(TEXT("../Media/Icons/FolderClosedGrey-16x16.png"));
  }

  FolderTreeItem::~FolderTreeItem()
  {
    if (FolderOpenIcon->OwnsTheReference())
      FolderOpenIcon->UnReference ();
    else
      FolderOpenIcon->Dispose ();

    if (FolderClosedIcon->OwnsTheReference())
      FolderClosedIcon->UnReference ();
    else
      FolderClosedIcon->Dispose ();
  }

  void FolderTreeItem::DrawProperty (GraphicsEngine &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter, RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor)
  {
    Geometry FirstColumnGeometry = m_ItemGeometryVector[0];

    if (isDirtyItem() )
    {
      t_u32 nBackground;

      if (m_bIsMouseInside)
      {
        nBackground = table->PushItemBackground (GfxContext, this, true);
      }
      else
      {
        nBackground = table->PushItemBackground (GfxContext, this, false);
      }

      GfxContext.PushClippingRectangle (geo);
      {
        Geometry IconGeometry (geo.x, geo.y, 16, 16);
        Geometry TextGeometry (geo);
        TextGeometry.OffsetPosition (18, 0);
        {
          GetGraphicsDisplay()->GetGraphicsEngine()->GetRenderStates().SetBlend (TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          GetGraphicsDisplay()->GetGraphicsEngine()->GetRenderStates().SetColorMask (TRUE, TRUE, TRUE, FALSE);
          nux::TexCoordXForm texxform;

          if (isOpen() )
          {
            GfxContext.QRP_1Tex (IconGeometry.x, IconGeometry.y, IconGeometry.width, IconGeometry.height, FolderOpenIcon->GetDeviceTexture(), texxform, nux::color::White);
          }
          else
          {
            GfxContext.QRP_1Tex (IconGeometry.x, IconGeometry.y, IconGeometry.width, IconGeometry.height, FolderClosedIcon->GetDeviceTexture(), texxform, nux::color::White);
          }

          GetGraphicsDisplay()->GetGraphicsEngine()->GetRenderStates().SetColorMask (TRUE, TRUE, TRUE, TRUE);
          GetGraphicsDisplay()->GetGraphicsEngine()->GetRenderStates().SetBlend (FALSE);
        }
        Painter.PaintTextLineStatic (GfxContext, GetSysFont(), TextGeometry, row->_table_item->GetName(), GetItemTextColor() );
      }
      GfxContext.PopClippingRectangle();
      table->PopItemBackground (GfxContext, nBackground);
    }
  }


}
