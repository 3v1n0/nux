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


#ifndef VECTOR4PROPERTY_H
#define VECTOR4PROPERTY_H

NAMESPACE_BEGIN_GUI

class EditTextLinePropertyItem;

class Vector4PropertyItem: public SectionProperty
{
    NODE_XML_CLASS_MEMBER(Vector4PropertyItem);
    NODE_SKIP_CHILD(true);
    NODE_SIG_RECEIVER(RecvPropertyChange, const weaksmptr(EditTextBox));
public:
    Vector4PropertyItem(const TCHAR* name,
        double X = 0.0,
        double Y = 0.0,
        double Z = 0.0,
        double W = 0.0,
        const TCHAR* XName = TEXT("X"),
        const TCHAR* YName = TEXT("Y"),
        const TCHAR* ZName = TEXT("Z"),
        const TCHAR* WName = TEXT("W"));
    virtual ~Vector4PropertyItem();

    virtual long ProcessPropertyEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void DrawProperty(GraphicsContext& GfxContext, TableCtrl* table, bool force_draw, Geometry geo, const BasePainter& Painter, RowHeader* row, const std::vector<header2>& column_vector, Color ItemBackgroundColor = Color(0x0));
    virtual void ComputePropertyLayout(int x, int y, RowHeader* row, const std::vector<header2>& column_vector);

    double GetX() const;
    double GetY() const;
    double GetZ() const;
    double GetW() const;
    void SetX(double x) {m_X->SetText(inlPrintf(TEXT("%f"), x));};
    void SetY(double y) {m_Y->SetText(inlPrintf(TEXT("%f"), y));};
    void SetZ(double z) {m_Z->SetText(inlPrintf(TEXT("%f"), z));};
    void SetW(double w) {m_W->SetText(inlPrintf(TEXT("%f"), w));};

    const TCHAR* GetXLabel() const {return m_X->GetName();}
    const TCHAR* GetYLabel() const {return m_Y->GetName();}
    const TCHAR* GetZLabel() const {return m_Z->GetName();}
    const TCHAR* GetWLabel() const {return m_W->GetName();}
private:
    EditTextLinePropertyItem* m_X;
    EditTextLinePropertyItem* m_Y;
    EditTextLinePropertyItem* m_Z;
    EditTextLinePropertyItem* m_W;
};

NAMESPACE_END_GUI

#endif // VECTOR4PROPERTY_H

