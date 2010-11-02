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


#ifndef FOLDERTREEITEM_H
#define FOLDERTREEITEM_H

#include "TreeControl.h"

namespace nux
{

  class FolderTreeItem: public TreeItem
  {
    //NODE_XML_CLASS_MEMBER(FolderTreeItem);
  public:
    FolderTreeItem (const TCHAR *name, const TCHAR *Path, bool Searchable = true, NodeParameterType type = NODE_TYPE_STATICTEXT);
    virtual ~FolderTreeItem();

    virtual void DrawProperty (GraphicsContext &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter, RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor = Color (0x0) );

    const TCHAR *GetPathName()
    {
      return m_Path.GetTCharPtr();
    }
    const TCHAR *GetCleanFilename()
    {
      return m_Path.GetCleanFilename().GetTCharPtr();
    }

    void SetSynchronize (bool b)
    {
      m_Synchronize = b;
    }
    bool IsSynchronized()
    {
      return m_Synchronize;
    }

    void SetOpenIcon (const BaseTexture* Texture)
    {
      FolderOpenIcon = Texture->Clone ();
    }
    void SetCloseIcon (const BaseTexture* Texture)
    {
      FolderClosedIcon = Texture->Clone ();
    }

    bool IsSearchable()
    {
      return m_Searchable;
    }
  private:
    BaseTexture* FolderOpenIcon;
    BaseTexture* FolderClosedIcon;

    NFileName m_Path;
    NString m_FolderName;
    bool m_Synchronize; // synchronize with windows explorer
    // True if this is a real directory that can be explorer. False if this is just a decoration item.
    // Non searchable folders must always be at the top of the tree;
    bool m_Searchable;

    //virtual long ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    //virtual void DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, RowHeader* row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor = 0x0);
    //virtual void ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<ColumnHeader>& column_vector);
    //virtual int GetItemBestHeight();
  };

}

#endif // FOLDERTREEITEM_H
