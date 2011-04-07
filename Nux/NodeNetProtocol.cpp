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
#include "NodeNetProtocol.h"
#include "PropertyList.h"

#include "PropertyItem/ColorGradientPropertyItem.h"
#include "PropertyItem/RGBAProperty.h"
#include "PropertyItem/RGBProperty.h"
#include "PropertyItem/EditTextLinePropertyItem.h"
#include "PropertyItem/Vector3Property.h"
#include "PropertyItem/Vector4Property.h"
#include "PropertyItem/CheckBoxProperty.h"
#include "PropertyItem/RangeValuePropertyItem.h"
#include "PropertyItem/RangeValueIntegerPropertyItem.h"
#include "PropertyItem/DoubleValuatorPropertyItem.h"
#include "PropertyItem/IntegerValuatorPropertyItem.h"
#include "PropertyItem/SplineCurvePropertyItem.h"
#include "PropertyItem/ColorPreviewPropertyItem.h"
#include "PropertyItem/Matrix3PreviewPropertyItem.h"
#include "PropertyItem/Matrix4PreviewPropertyItem.h"
#include "PropertyItem/ComboBoxPropertyItem.h"
#include "PropertyItem/ComboBoxListPropertyItem.h"
#include "PropertyItem/SpinBoxPropertyItem.h"
#include "PropertyItem/SpinBoxDoublePropertyItem.h"
#include "PropertyItem/Vector3ValuatorPropertyItem.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (NodeNetCom);

  struct TypeString
  {
    int Type;
    const char *String;
  };


  TypeString TypeToString[] =
  {
    {NODE_TYPE_UNDEFINED,       "ROOT_NODE"},
    {NODE_TYPE_FOLDER,          "Folder"},
    {NODE_TYPE_BOOL,            "Bool"},
    {NODE_TYPE_CHECKBOX,        "Check"},
    {NODE_TYPE_SPINBOX,         "SpinBox"},
    {NODE_TYPE_DOUBLESPINBOX,   "DoubleSpinBox"},
    {NODE_TYPE_COMBOSIMPLE,     "ComboBoxSimple"},
    {NODE_TYPE_COMBOCOMPLEX,    "ComboBoxComplex"},
    {NODE_TYPE_VECTOR3,         "Vector3"},
    {NODE_TYPE_VECTOR4,         "Vector4"},
    {NODE_TYPE_RGB,             "RGB"},
    {NODE_TYPE_RGBA,            "RGBA"},
    {NODE_TYPE_STATICTEXT,      "StaticText"},
    {NODE_TYPE_SPLINE,          "Spline"},
    {NODE_TYPE_EDITTEXT,        "EditText"},
    {NODE_TYPE_RANGE,           "RangeValue"},
    {NODE_TYPE_RANGEINTEGER,    "RangeValueInteger"},
    {NODE_TYPE_INTVALUATOR,     "IntValuator"},
    {NODE_TYPE_DOUBLEVALUATOR,  "DoubleValuator"},
    {NODE_TYPE_VECTORVALUATOR,  "Vector3Valuator"},
    {NODE_TYPE_COLORPREVIEW,    "ColorPreview"},
    {NODE_TYPE_MATRIX4PREVIEW,  "Matrix4Preview"},
    {NODE_TYPE_MATRIX3PREVIEW,  "Matrix3Preview"},

    {0,                 0},
  };

  const char *ConvertTypeToString (NodeParameterType type)
  {
    int i = 0;

    while (TypeToString[i].Type)
    {
      if (type == TypeToString[i].Type)
        return TypeToString[i].String;

      ++i;
    }

    nuxAssertMsg (0, TEXT ("[ConvertTypeToString]: Cannot convert Type to String") );
    return 0;
  }

  int NodeNetCom::m_IDGenerator = 0x12345678;

  NodeNetCom::NodeNetCom (const TCHAR *Name, NodeParameterType type)
    :   m_Name (Name)
    ,   m_Type (type)
    ,   m_ID (-1)
  {
    m_ID = m_IDGenerator++;

    if (m_Name.Length() == 0)
      m_Name = "Undefined";
  }

#if NODE_XML_NET_PROTOCOL
  TiXmlElement *NodeNetCom::ToXML() const
  {
    TiXmlElement *elementxml = new TiXmlElement ("Parameter");
    SetNodeXMLAttributes (elementxml);

    if (!SkipChild() )
    {
      NodeNetCom *first = (NodeNetCom *) FirstChildNode();

      while (first)
      {
        TiXmlElement *childxml = first->ToXML();

        if (childxml)
          elementxml->LinkEndChild (childxml);

        first = (NodeNetCom *) first->Next();
      }
    }

    return elementxml;
  }

  bool NodeNetCom::FromXML (const TiXmlElement *elementxml)
  {
    GetNodeXMLAttributes (elementxml);
    nuxDebugMsg (TEXT ("Updated Node %s - ID: %d - Type: %s"), m_Name.GetTCharPtr(), m_ID, ConvertTypeToString (m_Type) );

    const TiXmlElement *childelement = elementxml->FirstChildElement();

    if (!SkipChild() )
    {
      while (childelement)
      {
        int ID;

        if (childelement->QueryIntAttribute ("ID", &ID) != TIXML_SUCCESS)
        {
          nuxAssertMsg (0, TEXT ("[NodeNetCom::FromXML]: 'ID' Attribute not found") );
          return false;
        }

        NodeNetCom *childnode = FindChildNodeID (ID);

        if (childnode)
          childnode->FromXML (childelement);
        else
        {
          nuxAssertMsg (0, TEXT ("[NodeNetCom::FromXML]: Can't find child node with id %d"), ID);
          return false;
        }

        childelement = childelement->NextSiblingElement();
      }
    }

    return true;
  }

  bool NodeNetCom::CreateFromXML (const TiXmlElement *elementxml)
  {
    if (elementxml == 0)
      return false;

    //GetNodeXMLAttributes(elementxml);
    //nuxDebugMsg(TEXT("Created Node %s - ID: %d - Type: %s"), m_Name.c_str(), m_ID, ConvertTypeToString(m_Type));

    const TiXmlElement *childelement = elementxml->FirstChildElement();

    if (!SkipChild() )
    {
      while (childelement)
      {
        int Type;
        int Enable;
        int Visible;
        int ID;
        const char *Name;

        if (childelement->QueryIntAttribute ("Type", &Type) != TIXML_SUCCESS)
        {
          nuxAssertMsg (0, TEXT ("[NodeNetCom::CreateFromXML]: 'Type' Attribute not found") );
          return false;
        }

        if (childelement->QueryIntAttribute ("Enable", &Enable) != TIXML_SUCCESS)
        {
          nuxAssertMsg (0, TEXT ("[NodeNetCom::CreateFromXML]: 'Enable' Attribute not found") );
          return false;
        }

        if (childelement->QueryIntAttribute ("Visible", &Visible) != TIXML_SUCCESS)
        {
          nuxAssertMsg (0, TEXT ("[NodeNetCom::CreateFromXML]: 'Visible' Attribute not found") );
          return false;
        }

        if (childelement->QueryIntAttribute ("ID", &ID) != TIXML_SUCCESS)
        {
          nuxAssertMsg (0, TEXT ("[NodeNetCom::CreateFromXML]: 'ID' Attribute not found") );
          return false;
        }

        Name = childelement->Attribute ("Name");

        if (Name == 0)
        {
          nuxAssertMsg (0, TEXT ("[NodeNetCom::CreateFromXML]: 'Name' Attribute not found") );
          return false;
        }


        //         m_Name = xmlelt->Attribute("Name");
        //         nuxVerifyExpr( xmlelt->QueryIntAttribute("Enable", &m_Enable) == TIXML_SUCCESS);
        //         nuxVerifyExpr( xmlelt->QueryIntAttribute("Visible", &m_Visible) == TIXML_SUCCESS);
        //         nuxVerifyExpr( xmlelt->QueryIntAttribute("Type", (int*)&m_Type) == TIXML_SUCCESS);
        //         nuxVerifyExpr( xmlelt->QueryIntAttribute("ID", &m_ID) == TIXML_SUCCESS);



        NodeNetCom *child = 0;

        //        if(Type == TYPE_FOLDER)
        //        {
        //            child = inlFolder::CreateFromXML(childelement, this, Name, ID);
        //        }
        //        else if(Type == TYPE_BOOL)
        //        {
        //            child = inlBool::CreateFromXML(childelement, this, Name, ID);
        //        }
        //        else
        if (Type == NODE_TYPE_CHECKBOX)
        {
          child = (NodeNetCom *) CheckBoxPropertyItem::CreateFromXML (childelement, this, Name, ID);
        }
        else if (Type == NODE_TYPE_SPINBOX)
        {
          child = (NodeNetCom *) SpinBoxPropertyItem::CreateFromXML (childelement, this, Name, ID);
        }
        else if (Type == NODE_TYPE_DOUBLESPINBOX)
        {
          child = (NodeNetCom *) SpinBoxDoublePropertyItem::CreateFromXML (childelement, this, Name, ID);
        }

        else if (Type == NODE_TYPE_COMBOSIMPLE)
        {
          child = (NodeNetCom *) ComboBoxPropertyItem::CreateFromXML (childelement, this, Name, ID);
        }
        else if (Type == NODE_TYPE_VECTOR4)
        {
          child = (NodeNetCom *) Vector4PropertyItem::CreateFromXML (childelement, this, Name, ID);
        }
        else if (Type == NODE_TYPE_VECTOR3)
        {
          child = (NodeNetCom *) Vector3PropertyItem::CreateFromXML (childelement, this, Name, ID);
        }
        else if (Type == NODE_TYPE_RGBA)
        {
          child = (NodeNetCom *) RGBAPropertyItem::CreateFromXML (childelement, this, Name, ID);
        }
        else if (Type == NODE_TYPE_RGB)
        {
          child = (NodeNetCom *) RGBPropertyItem::CreateFromXML (childelement, this, Name, ID);
        }
        else if (Type == NODE_TYPE_STATICTEXT)
        {
          child = (NodeNetCom *) SectionProperty::CreateFromXML (childelement, this, Name, ID);
        }
        else if (Type == NODE_TYPE_SPLINE)
        {
          child = (NodeNetCom *) SplineCurvePropertyItem::CreateFromXML (childelement, this, Name, ID);
        }
        else if (Type == NODE_TYPE_EDITTEXT)
        {
          child = (NodeNetCom *) EditTextLinePropertyItem::CreateFromXML (childelement, this, Name, ID);
        }
        else if (Type == NODE_TYPE_INTVALUATOR)
        {
          child = (NodeNetCom *) IntValuatorPropertyItem::CreateFromXML (childelement, this, Name, ID);
        }
        else if (Type == NODE_TYPE_DOUBLEVALUATOR)
        {
          child = (NodeNetCom *) DoubleValuatorPropertyItem::CreateFromXML (childelement, this, Name, ID);
        }
        else if (Type == NODE_TYPE_VECTORVALUATOR)
        {
          child = (NodeNetCom *) Vector3ValuatorPropertyItem::CreateFromXML (childelement, this, Name, ID);
        }
        else if (Type == NODE_TYPE_COLORPREVIEW)
        {
          child = (NodeNetCom *) ColorPreviewPropertyItem::CreateFromXML (childelement, this, Name, ID);
        }
        else if (Type == NODE_TYPE_RANGE)
        {
          child = (NodeNetCom *) RangeValuePropertyItem::CreateFromXML (childelement, this, Name, ID);
        }
        else if (Type == NODE_TYPE_RANGEINTEGER)
        {
          child = (NodeNetCom *) RangeValueIntegerPropertyItem::CreateFromXML (childelement, this, Name, ID);
        }
        else
        {
          nuxAssertMsg (0, TEXT ("Unknown Type %s"), Type);
        }

        if (child)
          PushChildBack (child);

        child->CreateFromXML (childelement);
        childelement = childelement->NextSiblingElement();
      }
    }

    return true;
  }

  void NodeNetCom::SetNodeXMLAttributes (TiXmlElement *xmlelt) const
  {
    xmlelt->SetAttribute ("Name", m_Name.GetTCharPtr() );
    xmlelt->SetAttribute ("Enable", (m_Enable ? 1 : 0) );
    xmlelt->SetAttribute ("Visible", (m_Visible ? 1 : 0) );
    xmlelt->SetAttribute ("Type", m_Type);
    xmlelt->SetAttribute ("TypeString", ConvertTypeToString (m_Type) );
    xmlelt->SetAttribute ("ID", m_ID);
  }

  void NodeNetCom::GetNodeXMLAttributes (const TiXmlElement *xmlelt)
  {
    m_Name = xmlelt->Attribute ("Name");
    nuxVerifyExpr ( xmlelt->QueryIntAttribute ("Enable", &m_Enable) == TIXML_SUCCESS);
    nuxVerifyExpr ( xmlelt->QueryIntAttribute ("Visible", &m_Visible) == TIXML_SUCCESS);
    nuxVerifyExpr ( xmlelt->QueryIntAttribute ("Type", (int *) &m_Type) == TIXML_SUCCESS);
    nuxVerifyExpr ( xmlelt->QueryIntAttribute ("ID", &m_ID) == TIXML_SUCCESS);
  }

  bool QueryNodeXMLIntAttribute (const TiXmlElement *elementxml, const TCHAR *attribute, int *Value, int searchid)
  {
    int ID;

    if ( (searchid >= 0) && elementxml->QueryIntAttribute ("ID", &ID) != TIXML_SUCCESS)
    {
      nuxAssertMsg (0, TEXT ("[NodeNetCom::QueryNodeXMLIntAttribute]: 'ID' Attribute not found") );
      return false;
    }
    else
    {
      if ( (searchid >= 0) && (ID != searchid) )
      {
        nuxAssertMsg (0, TEXT ("[NodeNetCom::QueryNodeXMLIntAttribute]: Incorrect 'ID' number") );
        return false;
      }
      else
      {
        if (elementxml->QueryIntAttribute (attribute, Value) != TIXML_SUCCESS)
        {
          nuxAssertMsg (0, TEXT ("[NodeNetCom::QueryNodeXMLIntAttribute]: Attribute not found") );
          return false;
        }
      }

      return true;
    }

    return false;
  }

  bool QueryNodeXMLDoubleAttribute (const TiXmlElement *elementxml, const TCHAR *attribute, double *Value, int searchid)
  {
    int ID;

    if ( (searchid >= 0) && elementxml->QueryIntAttribute ("ID", &ID) != TIXML_SUCCESS)
    {
      nuxAssertMsg (0, TEXT ("[NodeNetCom::QueryNodeXMLDoubleAttribute]: 'ID' Attribute not found") );
      return false;
    }
    else
    {
      if ( (searchid >= 0) && (ID != searchid) )
      {
        nuxAssertMsg (0, TEXT ("[NodeNetCom::QueryNodeXMLDoubleAttribute]: Incorrect 'ID' number") );
        return false;
      }
      else
      {
        if (elementxml->QueryDoubleAttribute (attribute, Value) != TIXML_SUCCESS)
        {
          nuxAssertMsg (0, TEXT ("[NodeNetCom::QueryNodeXMLDoubleAttribute]: Attribute not found") );
          return false;
        }
      }

      return true;
    }

    return false;
  }

  bool QueryNodeXMLStringAttribute (const TiXmlElement *elementxml, const TCHAR *attribute, tstring &Value, int searchid)
  {
    int ID;

    if ( (searchid >= 0) && elementxml->QueryIntAttribute ("ID", &ID) != TIXML_SUCCESS)
    {
      nuxAssertMsg (0, TEXT ("[NodeNetCom::QueryNodeXMLStringAttribute]: 'ID' Attribute not found") );
      return false;
    }
    else
    {
      if ( (searchid >= 0) && (ID != searchid) )
      {
        nuxAssertMsg (0, TEXT ("[NodeNetCom::QueryNodeXMLStringAttribute]: Incorrect 'ID' number") );
        return false;
      }
      else
      {
        Value = elementxml->Attribute (attribute);

        if (Value.size() == 0)
        {
          // Do not break... this could be a null string which is allowed.
          // nuxAssertMsg(0, TEXT("[NodeNetCom::QueryNodeXMLStringAttribute]: Attribute not found"));
          return false;
        }
      }

      return true;
    }

    return false;
  }

  bool QueryNodeXMLAttribute (const TiXmlElement *elementxml, const TCHAR *attribute, std::string &Value, int searchid)
  {
    int ID;

    if ( (searchid >= 0) && elementxml->QueryIntAttribute ("ID", &ID) != TIXML_SUCCESS)
    {
      nuxAssertMsg (0, TEXT ("[NodeNetCom::QueryNodeXMLAttribute]: 'ID' Attribute not found") );
      return false;
    }
    else
    {
      if ( (searchid >= 0) && (ID != searchid) )
      {
        nuxAssertMsg (0, TEXT ("[NodeNetCom::QueryNodeXMLAttribute]: Incorrect 'ID' number") );
        return false;
      }
      else
      {
        const char *s = elementxml->Attribute (attribute);

        if (s == 0)
        {
          nuxAssertMsg (0, TEXT ("[NodeNetCom::QueryNodeXMLAttribute]: 'Check' Attribute not found") );
          return false;
        }

        Value = s;
      }

      return true;
    }

    return false;
  }

#endif // NODE_XML_NET_PROTOCOL

  NodeNetCom *NodeNetCom::FindNodeID (int id)
  {
    if (this->m_ID != id)
    {
      NodeNetCom *child = (NodeNetCom *) FirstChildNode();

      if (child)
      {
        // we have a child node; recurse through it
        NodeNetCom *findnode =  child->FindNodeID (id);

        if (findnode)
          return findnode;
      }
    }
    else
    {
      return this;
    }

    // Not found in child node; check the siblings
    NodeNetCom *sibling = (NodeNetCom *) Next();

    if (sibling)
    {
      NodeNetCom *findnode = sibling->FindNodeID (id);

      if (findnode)
        return findnode;
    }

    return 0;
  }

  NodeNetCom *NodeNetCom::FindChildNodeID (int id)
  {
    //     if(id < 0)
    //         return 0;

    if (GetID() == id)
      return this;

    NodeNetCom *child = (NodeNetCom *) FirstChildNode();

    while (child)
    {
      // we have a child node; recurse through it
      NodeNetCom *findnode = child->FindNodeID (id);

      if (findnode)
        return findnode;

      child = (NodeNetCom *) child->Next();
    }

    return 0;
  }

  void NodeNetCom::DeleteChildren()
  {
    NodeNetCom *last = (NodeNetCom *) LastChildNode();

    while (last)
    {
      last->DeleteChildren();
      last->Unlink();
      delete last;
      last = (NodeNetCom *) LastChildNode();
    }
  }

#if NODE_XML_NET_PROTOCOL

  SectionProperty *SectionProperty::CreateFromXML (const TiXmlElement *elementxml, NodeNetCom *parent, const char *Name, int id)
  {
    tstring text;
    QueryNodeXMLStringAttribute (elementxml, TEXT ("Text"),       text,     id);
    SectionProperty *node = new SectionProperty (Name);
    //node->setCaption(text.c_str());
    node->SetID (id);
    return node;
  }

  TiXmlElement *SectionProperty::ToXML() const
  {
    TiXmlElement *elementxml = NodeNetCom::ToXML();
    elementxml->SetAttribute (TEXT ("Text"), GetName() );
    return elementxml;
  }

  bool SectionProperty::FromXML (const TiXmlElement *elementxml)
  {
    tstring text;
    QueryNodeXMLStringAttribute (elementxml, TEXT ("Text"), text, GetID() );
    return NodeNetCom::FromXML (elementxml);
  }

#endif


}
