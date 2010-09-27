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


#include "Nux.h"

#include "FolderTreeItem.h"

namespace nux { //NUX_NAMESPACE_BEGIN

FolderTreeItem::FolderTreeItem(const TCHAR* name, const TCHAR* Path, bool Searchable, NodeParameterType type)
:   TreeItem(name, type)
{
    m_Synchronize   = false;
    m_Path          = Path;
    m_Searchable    = Searchable;

    TableItem::SetAlwaysShowOpeningButton(true);

    NString IconPath = NUX_FINDRESOURCELOCATION(TEXT("Media/Icons/FolderOpen-16x16.png"));
    FolderOpenIcon.Update(IconPath.GetTCharPtr());
    IconPath = NUX_FINDRESOURCELOCATION(TEXT("Media/Icons/FolderClose-16x16.png"));
    FolderClosedIcon.Update(IconPath.GetTCharPtr());
    //FolderOpenIcon.Update(TEXT("../Media/Icons/FolderOpenGrey-16x16.png"));
    //FolderClosedIcon.Update(TEXT("../Media/Icons/FolderClosedGrey-16x16.png"));
}

FolderTreeItem::~FolderTreeItem()
{

}

void FolderTreeItem::DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, RowHeader* row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor)
{
    Geometry FirstColumnGeometry = m_ItemGeometryVector[0];
    if(isDirtyItem())
    {
        UINT nBackground;
        if(m_bIsMouseInside)
        {
            nBackground = table->PushItemBackground(GfxContext, this, true);
        }
        else
        {
            nBackground = table->PushItemBackground(GfxContext, this, false);
        }
        GfxContext.PushClippingRectangle(geo);
        {
            Geometry IconGeometry(geo.x, geo.y, 16, 16);
            Geometry TextGeometry(geo);
            TextGeometry.OffsetPosition(18, 0);
            {
                GetThreadGraphicsContext()->GetRenderStates().SetBlend(TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                GetThreadGraphicsContext()->GetRenderStates().SetColorMask(TRUE, TRUE, TRUE, FALSE);
                nux::TexCoordXForm texxform;
                if(isOpen())
                {
                    GfxContext.QRP_GLSL_1Tex(IconGeometry.x, IconGeometry.y, IconGeometry.width, IconGeometry.height, (&FolderOpenIcon)->GetDeviceTexture(), texxform, nux::Color::White);
                }
                else
                {
                    GfxContext.QRP_GLSL_1Tex(IconGeometry.x, IconGeometry.y, IconGeometry.width, IconGeometry.height, (&FolderClosedIcon)->GetDeviceTexture(), texxform, nux::Color::White);
                }
                GetThreadGraphicsContext()->GetRenderStates().SetColorMask(TRUE, TRUE, TRUE, TRUE);
                GetThreadGraphicsContext()->GetRenderStates().SetBlend(FALSE);
            }
            Painter.PaintTextLineStatic(GfxContext, GFont, TextGeometry, row->m_item->GetName(), GetItemTextColor());
        }
        GfxContext.PopClippingRectangle();
        table->PopItemBackground(GfxContext, nBackground);
    }
}


} //NUX_NAMESPACE_END
