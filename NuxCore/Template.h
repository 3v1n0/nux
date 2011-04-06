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


#ifndef NTEMPLATE_H
#define NTEMPLATE_H

namespace nux
{

  class NString;

// Number of elements in an array.
#define NUX_ARRAY_COUNT( array ) \
    ( sizeof(array) / sizeof((array)[0]) )


// -----------------------------------------------------------------------------
// Type information.
// -----------------------------------------------------------------------------

// Is void
  template <typename U> struct VoidTraits
  {
    enum { result = 0 };
  };
// Full specialization of template <typename T> NTraitIsVoid
  template <> struct VoidTraits<void>
  {
    enum { result = 1 };
  };

// Is pointer
  template <typename U> struct PointerTraits
  {
    enum { result = 0 };
  };
// Partial specialization
  template <typename U> struct PointerTraits<U *>
  {
    enum { result = 1 };
  };

  template <typename U> struct UnicodeCharTraits
  {
    enum { result = 0 };
  };

  template <> struct UnicodeCharTraits<wchar_t>
  {
    enum { result = 1 };
  };

  template <typename U> struct AnsiCharTraits
  {
    enum { result = 0 };
  };

  template <> struct AnsiCharTraits<char>
  {
    enum { result = 1 };
  };

// Base type information for atomic types which pass by value.
  template<typename T>
  class TypeTraitsNoConstructor
  {
  public:
    typedef T ConstInitType;
    enum { HasConstructor = 0	};
    enum { HasDestructor = 0	};
  };

// Base type information for constructed types which pass by reference.
  template<typename T>
  class TypeTraitsConstructor
  {
  public:
    typedef const T &ConstInitType;
    enum { HasConstructor = 1	};
    enum { HasDestructor = 1	};
  };


// The default behaviour is for types to behave as constructed types.
  template<typename T>
  class ConstructorTraits : public TypeTraitsConstructor<T> {};

// Pointers don't have a constructor.
  template<typename T>
  class ConstructorTraits<T *>: public TypeTraitsNoConstructor<T *> {};

  template <> class ConstructorTraits<bool>               : public TypeTraitsNoConstructor<bool>     {};
  template <> class ConstructorTraits<char>               : public TypeTraitsNoConstructor<char>     {};
  template <> class ConstructorTraits<unsigned char>      : public TypeTraitsNoConstructor<unsigned char>     {};
  template <> class ConstructorTraits<short>              : public TypeTraitsNoConstructor<short>     {};
  template <> class ConstructorTraits<unsigned short>     : public TypeTraitsNoConstructor<unsigned short>    {};
  template <> class ConstructorTraits<int>                : public TypeTraitsNoConstructor<int>      {};
  template <> class ConstructorTraits<unsigned int>       : public TypeTraitsNoConstructor<unsigned int>      {};
  template <> class ConstructorTraits<long>               : public TypeTraitsNoConstructor<long>    {};
  template <> class ConstructorTraits<unsigned long>      : public TypeTraitsNoConstructor<unsigned long>    {};
  template <> class ConstructorTraits<long long>          : public TypeTraitsNoConstructor<long long>    {};
  template <> class ConstructorTraits<unsigned long long> : public TypeTraitsNoConstructor<unsigned long long>    {};
  template <> class ConstructorTraits<float>              : public TypeTraitsNoConstructor<float>    {};
  template <> class ConstructorTraits<double>             : public TypeTraitsNoConstructor<double>   {};

  template<typename T>
  class NTypeTraits
  {
  private:

  public:
    enum { IsVoid = VoidTraits<T>::result };
    enum { IsPointer = PointerTraits<T>::result };
    enum { NeedsConstructor = ConstructorTraits<T>::HasConstructor };
    enum { NeedsDestructor = ConstructorTraits<T>::HasDestructor };
    enum { IsAnsiChar = AnsiCharTraits<T>::result };
    enum { IsUnicodeChar = UnicodeCharTraits<T>::result };
  };

  template <typename T>
  struct RemovePointerFromType
  {
    typedef T type;
  };

  template <typename T>
  struct RemovePointerFromType<T *>
  {
    typedef T type;
  };
  template <typename T>
  struct RemovePointerFromType<const T *>
  {
    typedef T type;
  };
  template <typename T>
  struct RemovePointerFromType<volatile T *>
  {
    typedef T type;
  };
  template <typename T>
  struct RemovePointerFromType<const volatile T *>
  {
    typedef T type;
  };


}

#endif // NTEMPLATE_H

