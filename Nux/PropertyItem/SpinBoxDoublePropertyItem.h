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


#ifndef SPINBOXDOUBLEPROPERTYITEM_H
#define SPINBOXDOUBLEPROPERTYITEM_H

namespace nux
{

  class SpinBoxDouble;

  class SpinBoxDoublePropertyItem: public SectionProperty, public SpinBoxDouble
  {
    NODE_XML_CLASS_MEMBER (SpinBoxDoublePropertyItem);
    NODE_SIG_RECEIVER2 (RecvPropertyChange, SpinBoxDouble *);
  public:
    SpinBoxDoublePropertyItem (const TCHAR *name, double Value = 0.0, double Step = 1.0, double MinValue = 0.0, double MaxValue = 100.0);
    virtual ~SpinBoxDoublePropertyItem();

    virtual long ProcessPropertyEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void DrawProperty (GraphicsEngine &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter, RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor = Color (0x0) );
    virtual void ComputePropertyLayout (int x, int y, RowHeader *row, const std::vector<ColumnHeader>& column_vector);
    virtual int GetItemBestHeight();

  private:
    //RangeValue m_range_value;
  };

}

#endif // SPINBOXDOUBLEPROPERTYITEM_H
