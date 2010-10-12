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


#ifndef NOBJECTTYPE_H
#define NOBJECTTYPE_H

struct NObjectType
{
  const TCHAR *m_Name;
  NObjectType *m_Super;
  static const NObjectType  Null_Type;

  NObjectType()
    :   m_Name (TEXT ("Null_Type") )
    ,   m_Super (0)
  {
  }

  NObjectType (const TCHAR *Name, NObjectType *Super)
    :   m_Name (Name)
    ,   m_Super (Super)
  {
  }

  //! Return true is this has the same type as the argument type.
  inline bool operator == (const NObjectType &Type) const
  {
    return IsObjectType (Type);
  }

  //! Return true is this has is of a different type than the argument type.
  inline bool operator != (const NObjectType &Type) const
  {
    return !IsObjectType (Type);
  }

  //! Return the name of the type.
  inline const TCHAR              *GetName() const
  {
    return m_Name;
  }

  //! Return true is this has the same type as the argument type.
  inline bool IsObjectType (const NObjectType &Type) const
  {
    if (this == &Type)
    {
      return true;
    }

    return false;
  }

  //! Return true if this has the same type as the argument type or is derived from it.
  inline bool IsDerivedFromType (const NObjectType &Type) const
  {
    const NObjectType *current_type = this;

    while (current_type)
    {
      if (current_type == &Type)
        return true;

      current_type = current_type->m_Super;
    }

    return false;
  }

  inline unsigned int SubClassDepth() const
  {
    const NObjectType *current_type = this;
    unsigned int depth = 0;

    while (current_type)
    {
      depth++;
      current_type = current_type->m_Super;
    }

    return depth;
  }
};

#define NUX_DECLARE_OBJECT_TYPE(TypeName, SuperType)                            \
    public:                                                                 \
    typedef SuperType SuperObject;                                          \
    static NObjectType StaticObjectType;                                    \
    public:                                                                 \
    virtual NObjectType& Type() const { return StaticObjectType; }          \
    NObjectType& GetTypeInfo() const { return StaticObjectType; }


#define NUX_IMPLEMENT_OBJECT_TYPE(TypeName)                                     \
    NObjectType TypeName::StaticObjectType(TEXT(#TypeName), &TypeName::SuperObject::StaticObjectType);

// #define NUX_DECLARE_ROOT_OBJECT_TYPE(TypeName)      NUX_DECLARE_OBJECT_TYPE(TypeName, TypeName)
// #define NUX_IMPLEMENT_ROOT_OBJECT_TYPE(TypeName)    NUX_IMPLEMENT_OBJECT_TYPE(TypeName)

#define NUX_DECLARE_ROOT_OBJECT_TYPE(TypeName)                                  \
    public:                                                                 \
    typedef NObjectType SuperObject;                                        \
    static NObjectType StaticObjectType;                                    \
    public:                                                                 \
    virtual NObjectType& Type() const { return StaticObjectType; }          \
    NObjectType& GetTypeInfo() const { return StaticObjectType; }

#define NUX_IMPLEMENT_ROOT_OBJECT_TYPE(TypeName)                                \
    NObjectType TypeName::StaticObjectType(TEXT(#TypeName), 0);

#endif // NOBJECTTYPE_H

