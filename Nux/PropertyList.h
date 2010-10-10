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


#ifndef PROPERTYLIST_H
#define PROPERTYLIST_H

#include "View.h"
#include "ActionItem.h"
#include "RangeValue.h"
#include "Button.h"
#include "ComboBoxComplex.h"
#include "SpinBox.h"
#include "DoubleValidator.h"
#include "SpinBoxDouble.h"
#include "CheckBox.h"
#include "TableItem.h"
#include "NodeItem.h"
#include "TableCtrl.h"
#include "EditTextBox.h"
#include "SplineCurveEditor.h"
#include "SplineCurvePreview.h"
#include "ComboBoxSimple.h"
#include "ValuatorDouble.h"
#include "ValuatorInt.h"
#include "Vector3ValuatorDouble.h"
#include "ColorPreview.h"
#include "Matrix4Preview.h"
#include "Matrix3Preview.h"
#include "RangeValueInteger.h"
#include "ColorGradient.h"

namespace nux
{

  extern Color GPropertyItemTextColor0;
  extern Color GPropertyItemTextColor1;

  extern const int PROPERTY_BORDER_X;
  extern const int PROPERTY_BORDER_Y;

#if NODE_XML_NET_PROTOCOL
#define NODE_XML_CLASS_MEMBER(cls)   public:                                                                    \
        static cls* CreateFromXML(const TiXmlElement* elementxml, NodeNetCom* parent, const TCHAR* Name, int id); \
        virtual TiXmlElement* ToXML() const;                                                                        \
        virtual bool FromXML(const TiXmlElement* elementxml);

#define NODE_SKIP_CHILD(b)  public:         \
        virtual bool SkipChild() const {return b;}
#else
#define NODE_XML_CLASS_MEMBER(cls)

#define NODE_SKIP_CHILD(b)
#endif

#define NODE_SIG_RECEIVER(MemFunc, Cls)                     \
    void MemFunc(Cls)                                       \
    {                                                       \
        TableItem* parent = (TableItem*)Parent();           \
        bool ParentCanHandleChild = false;                  \
        while(parent)                                       \
        {                                                   \
            if(parent->SkipChild() == true)                 \
            {                                               \
                ParentCanHandleChild = true;                \
                break;                                      \
            }                                               \
            parent = (TableItem*)parent->Parent();          \
        }                                                   \
        if(ParentCanHandleChild)                            \
        {                                                   \
            TableItem* root = (TableItem*) RootNode();      \
            root->GetTable()->sigItemChange.emit(*parent);  \
        }                                                   \
        else                                                \
        {                                                   \
            TableItem* root = (TableItem*) RootNode();      \
            root->GetTable()->sigItemChange.emit(*this);    \
        }                                                   \
    }

#define NODE_SIG_RECEIVER2(MemFunc, Cls)                    \
    void MemFunc(Cls)                                       \
    {                                                       \
        TableItem* parent = (TableItem*)Parent();           \
        bool ParentCanHandleChild = false;                  \
        while(parent)                                       \
        {                                                   \
            if(parent->SkipChild() == true)                 \
            {                                               \
                ParentCanHandleChild = true;                \
                break;                                      \
            }                                               \
            parent = (TableItem*)parent->Parent();          \
        }                                                   \
        if(ParentCanHandleChild)                            \
        {                                                   \
            TableItem* root = (TableItem*) RootNode();      \
            root->GetTable()->sigItemChange.emit(*parent);  \
        }                                                   \
        else                                                \
        {                                                   \
            TableItem* root = (TableItem*) RootNode();      \
            root->GetTable()->sigItemChange.emit(*this);    \
        }                                                   \
    }

#define NODE_SIG_CONNECT(signal, Cls, MemFun) signal.connect(sigc::mem_fun(this, &Cls::MemFun));

  class SectionProperty : public TableItem//, public View
  {
    NODE_XML_CLASS_MEMBER (SectionProperty);
  public:
    SectionProperty (const char *name, NodeParameterType type = NODE_TYPE_STATICTEXT);
    virtual ~SectionProperty();

//    virtual long ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void DrawProperty (GraphicsContext &GfxContext, TableCtrl *table, bool force_draw, Geometry geo, const BasePainter &Painter, RowHeader *row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor = Color (0x0) );
    virtual void ComputePropertyLayout (int x, int y, RowHeader *row, const std::vector<ColumnHeader>& column_vector);
    virtual int GetItemBestHeight();

    void SetString (NString str)
    {
      m_String = str;
    }
    NString GetString (NString str) const
    {
      return m_String;
    }

  private:
    NString m_String;
  };

// class ComboBoxPropertyItem: public SectionProperty, public ComboBoxComplex
// {
//     //NODE_XML_CLASS_MEMBER(ComboBoxPropertyItem);
//     NODE_SKIP_CHILD(true);
// public:
//     ComboBoxPropertyItem(const TCHAR* name);
//     virtual ~ComboBoxPropertyItem();
//
//     virtual long ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
//     virtual void DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, RowHeader* row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor = Color(0x0));
//     virtual void ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<ColumnHeader>& column_vector);
//     virtual int GetItemBestHeight();
// private:
//     //RangeValue m_range_value;
// };

// class ComboBoxSimplePropertyItem: public SectionProperty, public ComboBoxSimple
// {
//     NODE_XML_CLASS_MEMBER(ComboBoxSimplePropertyItem);
//     NODE_SKIP_CHILD(true);
//     NODE_SIG_RECEIVER(RecvPropertyChange, ComboBoxSimple*);
// public:
//     ComboBoxSimplePropertyItem(const TCHAR* name);
//     virtual ~ComboBoxSimplePropertyItem();
//
//     virtual long ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
//     virtual void DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, RowHeader* row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor = Color(0x0));
//     virtual void ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<ColumnHeader>& column_vector);
//     virtual int GetItemBestHeight();
// private:
//     //RangeValue m_range_value;
// };

// class SpinBoxPropertyItem: public SectionProperty, public SpinBox
// {
//     NODE_XML_CLASS_MEMBER(SpinBoxPropertyItem);
//     NODE_SIG_RECEIVER(RecvPropertyChange, SpinBox*);
// public:
//     SpinBoxPropertyItem(const TCHAR* name, int Value = 0, int Step = 1, int MinValue = 0, int MaxValue = 100);
//     virtual ~SpinBoxPropertyItem();
//
//     virtual long ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
//     virtual void DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, RowHeader* row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor = Color(0x0));
//     virtual void ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<ColumnHeader>& column_vector);
//     virtual int GetItemBestHeight();
//
// private:
//     //RangeValue m_range_value;
// };

// class SpinBoxDoublePropertyItem: public SectionProperty, public SpinBoxDouble
// {
//     NODE_XML_CLASS_MEMBER(DoubleSpinBoxPropertyItem);
//     NODE_SIG_RECEIVER(RecvPropertyChange, SpinBoxDouble*);
// public:
//     DoubleSpinBoxPropertyItem(const TCHAR* name, double Value = 0.0, double Step = 1.0, double MinValue = 0.0, double MaxValue = 100.0);
//     virtual ~DoubleSpinBoxPropertyItem();
//
//     virtual long ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
//     virtual void DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, RowHeader* row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor = Color(0x0));
//     virtual void ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<ColumnHeader>& column_vector);
//     virtual int GetItemBestHeight();
//
// private:
//     //RangeValue m_range_value;
// };

// class Vector3ValuatorPropertyItem: public SectionProperty, public Vector3DoubleValuator
// {
//     NODE_XML_CLASS_MEMBER(Vector3ValuatorPropertyItem);
//     NODE_SIG_RECEIVER(RecvPropertyChange, Vector3DoubleValuator*);
// public:
//     Vector3ValuatorPropertyItem(const TCHAR* name, double X = 0, double Y = 0, double Z = 0,
//         double Step = 1.0, double MinValue = -1.7E308, double MaxValue = 1.7E308);
//     virtual ~Vector3ValuatorPropertyItem();
//
//     virtual long ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
//     virtual void DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, RowHeader* row, const std::vector<ColumnHeader>& column_vector, Color ItemBackgroundColor = Color(0x0));
//     virtual void ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<ColumnHeader>& column_vector);
//     virtual int GetItemBestHeight();
//
// private:
//     //RangeValue m_range_value;
// };







}

#endif //PROPERTYLIST_H
