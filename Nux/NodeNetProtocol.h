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


#ifndef NODENETPROTOCOL_H
#define NODENETPROTOCOL_H

#include "NodeItem.h"
#include "NuxCore/TinyXML/tinyxml.h"

//#pragma pack(1)
//
//enum TypeHeader
//{
//    TRANSFER_ACK,
//    BYTE_HEADER,
//    INT_HEADER,
//    STRING_HEADER,
//};
//
//struct INL_HEADER_PACKET
//{
//    NSockAddrIn     address;
//    unsigned long   Code;       // => 4 bytes (the header type or protocol)
//    unsigned long   Type;       // => 4 bytes (the service requested/packet type)
//    unsigned long   Handle;     // => 4 bytes (the client/connection handle) 
//    unsigned long   Len;        // => 4 bytes (the data len)
//};
//
//
//struct INL_DATA_PACKET
//{
//    //NSockAddrIn address;
//    BYTE data[1024];
//};
//
//struct INL_DATA_HEADER
//{
//    BYTE data0[1024];
//    BYTE data1[1024];
//};
//
//#pragma pack()

#define NODE_XML_NET_PROTOCOL 1

NAMESPACE_BEGIN_GUI

enum NodeParameterType
{
    NODE_TYPE_UNDEFINED = 1,
    NODE_TYPE_FOLDER,
    NODE_TYPE_BOOL,
    NODE_TYPE_CHECKBOX,
    NODE_TYPE_SPINBOX,
    NODE_TYPE_DOUBLESPINBOX,
    NODE_TYPE_COMBOSIMPLE,
    NODE_TYPE_COMBOCOMPLEX,
    NODE_TYPE_VECTOR3,
    NODE_TYPE_VECTOR4,
    NODE_TYPE_RGB,
    NODE_TYPE_RGBA,
    NODE_TYPE_STATICTEXT,
    NODE_TYPE_SPLINE,
    NODE_TYPE_EDITTEXT,
    NODE_TYPE_RANGE,
    NODE_TYPE_RANGEINTEGER,
    NODE_TYPE_INTVALUATOR,
    NODE_TYPE_DOUBLEVALUATOR,
    NODE_TYPE_VECTORVALUATOR,
    NODE_TYPE_COLORPREVIEW,
    NODE_TYPE_MATRIX4PREVIEW,
    NODE_TYPE_MATRIX3PREVIEW,

    NODE_TYPE_FORCE_DWORD = 0x7fffffff,
};

#if NODE_XML_NET_PROTOCOL
    bool QueryNodeXMLIntAttribute(const TiXmlElement* elementxml, const TCHAR* attribute, int* Value, int searchid);
    bool QueryNodeXMLDoubleAttribute(const TiXmlElement* elementxml, const TCHAR* attribute, double* Value, int searchid);
    bool QueryNodeXMLAttribute(const TiXmlElement* elementxml, const TCHAR* attribute, std::string& Value, int searchid);
    bool QueryNodeXMLAttribute(const TiXmlElement* elementxml, const TCHAR* attribute, std::string& Value, int searchid);
    bool QueryNodeXMLStringAttribute(const TiXmlElement* elementxml, const TCHAR* attribute, tstring& Value, int searchid);
#endif

class NodeNetCom: public NodeItem
{
    DECLARE_OBJECT_TYPE(NodeNetCom, NodeItem);
public:
    //! Search the ID in the whole tree below this node. Search also this node.
    NodeNetCom* FindNodeID(int id);
    //! Search the ID in the immediate children of this node.
    NodeNetCom* FindChildNodeID(int id);

#if NODE_XML_NET_PROTOCOL
    virtual TiXmlElement* ToXML() const;
    virtual bool FromXML(const TiXmlElement* elementxml);
    //! Used by client to create node from XML after initialization.
    virtual bool CreateFromXML(const TiXmlElement* elementxml);

    //! By default, when generating xml (ToXML) or creating from xml (FromXML), it may be necessary to skip the child of some elements because they
    // automatically take care of parsing in and out their own children. For instance, Vector3PropertyItem.
    virtual bool SkipChild() const {return false;}

//    virtual bool QueryNodeXMLIntAttribute(const TiXmlElement* elementxml, const char* attribute, int* Value, int id);
//    virtual bool QueryNodeXMLDoubleAttribute(const TiXmlElement* elementxml, const char* attribute, double* Value, int id);
//    virtual bool QueryNodeXMLAttribute(const TiXmlElement* elementxml, const char* attribute, std::string& Value, int id);
protected:
    void SetNodeXMLAttributes(TiXmlElement* elementxml) const;
    void GetNodeXMLAttributes(const TiXmlElement* elementxml);

#endif

public:
    void DeleteChildren();

    const TCHAR* GetName() const {return m_Name.GetTCharPtr();}
    void SetName(const TCHAR* str) {m_Name = str;}

protected:
    NodeNetCom(const TCHAR* Name, NodeParameterType type = NODE_TYPE_UNDEFINED);
    virtual ~NodeNetCom(){};

    void SetParameterType(NodeParameterType t) {m_Type = t;}
    NodeParameterType GetParameterType() const {return m_Type;}
    int GetID() const {return m_ID;}
    void SetID(int id) {m_ID = id;}
    void SetEnable(bool b) {m_Enable = b? 1 : 0;}
    int IsEnable() const {return m_Enable;}
    void SetVisible(bool b) {m_Visible = b? 1 : 0;}
    int IsVisible() const {return m_Visible;}
private:

    static int m_IDGenerator;
    NString m_Name;
    NodeParameterType m_Type;
    int m_Enable;  // true is client can change the value.
    int m_Visible; // true, if visible to the client.
    int m_ID;       // ID for network communications

    friend class ParameterManagerImpl;
};


NAMESPACE_END_GUI

#endif // NODENETPROTOCOL_H

