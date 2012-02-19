// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
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

#ifndef NOBJECTTYPE_H
#define NOBJECTTYPE_H

#include <string>

namespace nux
{
// TODO: write a nice is_instance (and is_derived_instance)

//template <typename T, typename I>
//bool is_instance(T const&

struct NObjectType
{
  const char* name;
  NObjectType* super;

  static const NObjectType  Null_Type;

  NObjectType()
    : name("Null_Type")
    , super(NULL)
  {
  }

  NObjectType(const char* type_name, NObjectType* super_type)
    : name(type_name)
    , super(super_type)
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

  //! Return true is this has the same type as the argument type.
  inline bool IsObjectType (const NObjectType &Type) const
  {
    return this == &Type;
  }

  //! Return true if this has the same type as the argument type or is derived from it.
  inline bool IsDerivedFromType (const NObjectType &Type) const
  {
    const NObjectType *current_type = this;

    while (current_type)
    {
      if (current_type == &Type)
        return true;

      current_type = current_type->super;
    }

    return false;
  }

  inline unsigned int SubClassDepth() const
  {
    const NObjectType* current_type = this;
    unsigned int depth = 0;

    while (current_type)
    {
      depth++;
      current_type = current_type->super;
    }

    return depth;
  }
};

#define NUX_DECLARE_OBJECT_TYPE(TypeName, SuperType)                            \
    public:                                                                 \
    typedef SuperType SuperObject;                                          \
    static ::nux::NObjectType StaticObjectType;                         \
    public:                                                                 \
    virtual ::nux::NObjectType& Type() const { return StaticObjectType; }          \
    ::nux::NObjectType& GetTypeInfo() const { return StaticObjectType; }


#define NUX_IMPLEMENT_OBJECT_TYPE(TypeName)                                     \
    ::nux::NObjectType TypeName::StaticObjectType(#TypeName, &TypeName::SuperObject::StaticObjectType);

#define NUX_DECLARE_ROOT_OBJECT_TYPE(TypeName)                                  \
    public:                                                                 \
    typedef ::nux::NObjectType SuperObject;                                        \
    static ::nux::NObjectType StaticObjectType;                                    \
    public:                                                                 \
    virtual ::nux::NObjectType& Type() const { return StaticObjectType; }          \
    ::nux::NObjectType& GetTypeInfo() const { return StaticObjectType; }

#define NUX_IMPLEMENT_ROOT_OBJECT_TYPE(TypeName)                                \
    ::nux::NObjectType TypeName::StaticObjectType(#TypeName, 0);

} // namespace nux

#endif // NOBJECTTYPE_H

