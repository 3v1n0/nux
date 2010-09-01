#ifndef NOBJECTTYPE_H
#define NOBJECTTYPE_H

struct NObjectType
{
    const TCHAR* m_Name;
    NObjectType* m_Super;
    static const NObjectType  Null_Type;

    NObjectType()
        :   m_Name(TEXT("Null_Type"))
        ,   m_Super(0)
    {
    }

    NObjectType(const TCHAR* Name, NObjectType* Super)
        :   m_Name(Name)
        ,   m_Super(Super)
    {
    }

    //! Return true is this has the same type as the argument type.
    inline bool operator == (const NObjectType& Type) const
    {
        return IsObjectType(Type);
    }

    //! Return true is this has is of a different type than the argument type.
    inline bool operator != (const NObjectType& Type) const
    {
        return !IsObjectType(Type);
    }

    //! Return the name of the type.
    inline const TCHAR*              GetName() const
    {
        return m_Name;
    }

    //! Return true is this has the same type as the argument type.
    inline bool IsObjectType(const NObjectType& Type) const
    {
        if(this == &Type)
        {
            return true;
        }
        return false;
    }

    //! Return true if this has the same type as the argument type or is derived from it.
    inline bool IsDerivedFromType(const NObjectType& Type) const
    {
        const NObjectType* current_type = this;
        while(current_type)
        {
            if(current_type == &Type)
                return true;
            current_type = current_type->m_Super;
        }
        return false;
    }

    inline unsigned int SubClassDepth() const
    {
        const NObjectType* current_type = this;
        unsigned int depth = 0;
        while(current_type)
        {
            depth++;
            current_type = current_type->m_Super;
        }
        return depth;
    }
};

#define DECLARE_OBJECT_TYPE(TypeName, SuperType)                            \
    public:                                                                 \
    typedef SuperType SuperObject;                                          \
    static NObjectType StaticObjectType;                                    \
    public:                                                                 \
    virtual NObjectType& Type() const { return StaticObjectType; }          \
    NObjectType& GetTypeInfo() const { return StaticObjectType; }


#define IMPLEMENT_OBJECT_TYPE(TypeName)                                     \
    NObjectType TypeName::StaticObjectType(TEXT(#TypeName), &TypeName::SuperObject::StaticObjectType);

// #define DECLARE_ROOT_OBJECT_TYPE(TypeName)      DECLARE_OBJECT_TYPE(TypeName, TypeName)
// #define IMPLEMENT_ROOT_OBJECT_TYPE(TypeName)    IMPLEMENT_OBJECT_TYPE(TypeName)

#define DECLARE_ROOT_OBJECT_TYPE(TypeName)                                  \
    public:                                                                 \
    typedef NObjectType SuperObject;                                        \
    static NObjectType StaticObjectType;                                    \
    public:                                                                 \
    virtual NObjectType& Type() const { return StaticObjectType; }          \
    NObjectType& GetTypeInfo() const { return StaticObjectType; }

#define IMPLEMENT_ROOT_OBJECT_TYPE(TypeName)                                \
    NObjectType TypeName::StaticObjectType(TEXT(#TypeName), 0);

#endif // NOBJECTTYPE_H

