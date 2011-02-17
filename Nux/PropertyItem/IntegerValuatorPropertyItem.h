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


#ifndef INTEGERVALUATORPROPERTYITEM_H
#define INTEGERVALUATORPROPERTYITEM_H

namespace nux
{

  class ValuatorInt;

  class IntValuatorPropertyItem: public SectionProperty, public ValuatorInt
  {
    NODE_XML_CLASS_MEMBER (IntValuatorPropertyItem);
    NODE_SKIP_CHILD (false);
    NODE_SIG_RECEIVER (RecvPropertyChange, ValuatorInt *);
  public:
    IntValuatorPropertyItem (const char *name, int Value = 0, int Step = 1,  int MinValue = 0, int MaxValue = 100);
    virtual ~IntValuatorPropertyItem();

    virtual long ProcessPropertyEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void DrawProperty (GraphicsEngine &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter, RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor = Color (0x0) );
    virtual void ComputePropertyLayout (int x, int y, RowHeader *row, const std::vector<ColumnHeader>& column_vector);
    virtual int GetItemBestHeight();
  private:
  };

}

#endif // INTEGERVALUATORPROPERTYITEM_H
