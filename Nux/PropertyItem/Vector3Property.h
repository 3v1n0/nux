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


#ifndef VECTOR3PROPERTY_H
#define VECTOR3PROPERTY_H

namespace nux
{

  class EditTextLinePropertyItem;

  class Vector3PropertyItem: public SectionProperty
  {
    NODE_XML_CLASS_MEMBER (Vector3PropertyItem);
    NODE_SKIP_CHILD (true);
    NODE_SIG_RECEIVER2 (RecvPropertyChange, EditTextBox *);
  public:
    Vector3PropertyItem (const TCHAR *name,
                         double X = 0.0,
                         double Y = 0.0,
                         double Z = 0.0,
                         const TCHAR *XName = TEXT ("X"),
                         const TCHAR *YName = TEXT ("Y"),
                         const TCHAR *ZName = TEXT ("Z") );
    virtual ~Vector3PropertyItem();

    virtual long ProcessPropertyEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void DrawProperty (GraphicsEngine &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter, RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor = Color (0x0) );
    virtual void ComputePropertyLayout (int x, int y, RowHeader *row, const std::vector<ColumnHeader>& column_vector);

    double GetX() const;
    double GetY() const;
    double GetZ() const;
    void SetX (double x)
    {
      m_X->SetText (NString::Printf (TEXT ("%f"), x) );
    };
    void SetY (double y)
    {
      m_Y->SetText (NString::Printf (TEXT ("%f"), y) );
    };
    void SetZ (double z)
    {
      m_Z->SetText (NString::Printf (TEXT ("%f"), z) );
    };

    const TCHAR *GetXLabel() const
    {
      return m_X->GetName();
    }
    const TCHAR *GetYLabel() const
    {
      return m_Y->GetName();
    }
    const TCHAR *GetZLabel() const
    {
      return m_Z->GetName();
    }
  private:
    EditTextLinePropertyItem *m_X;
    EditTextLinePropertyItem *m_Y;
    EditTextLinePropertyItem *m_Z;
  };

}

#endif // VECTOR3PROPERTY_H
