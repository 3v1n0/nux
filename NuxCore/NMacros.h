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


#ifndef NMACROS_H
#define NMACROS_H


// Disable object copy constructor and assignment operator
#define NUX_DISABLE_OBJECT_COPY(Class)         \
    private:                                    \
    Class(const Class &);                       \
    const Class& operator = (const Class &);

/// GLOBAL OBJECT ///////////////////////////////////////////////////////////////////////
#define NUX_GLOBAL_OBJECT_VARIABLE(Class) Class m_##Class;
#define NUX_GLOBAL_OBJECT_INSTANCE(Class) Class::Instance()
#define NUX_GLOBAL_OBJECT_REFERENCE(Class) Class::Instance()

#define NUX_IMPLEMENT_GLOBAL_OBJECT(Class) Class* Class::pInstance =  0;


// Note: Methods that are implicitly generated by the compiler if they are not explicitly defined are:
//     * Default constructor (C::C())
//     * Copy constructor (C::C (const C& rhs))
//     * Destructor (C::~C())
//     * Assignment operator (C& C::operator= (const C& rhs))
//     * Address-of operator (C* C::operator&())
//     * Address-of operator (const C* C::operator&() const;) 

/*! A Global Object with a behavior similar to a singleton. Only one instance of this object can exist.
    The unique instance is created by a friend class: NGlobalInitializer. Only NGlobalInitializer can create this instance.
    example: 
        class GlobalObject
        {
            NUX_DECLARE_GLOBAL_OBJECT(GlobalObject, , NGlobalSingletonInitializer)

            public:
                void foo(void);
        };

    The macro also define a Constructor and Destructor method to be implemented:
        - void Constructor()
        - void Destructor()
    They are replacement for the real Constructor and Destructor.
*/

#define NUX_DECLARE_GLOBAL_OBJECT(Class, GlobalInitializer)                     \
    NUX_DISABLE_OBJECT_COPY(Class);                                             \
    /* Disable address-of operator */                                           \
    /*Class* operator & ();             */                                      \
    /*const Class* operator & () const; */                                      \
    friend class GlobalInitializer;                                             \
        private:                                                                \
        static Class* pInstance;                                                \
        Class()                                                                 \
        {                                                                       \
            pInstance = this;                                                   \
            Constructor();                                                      \
        }                                                                       \
        virtual ~Class()                                                        \
        {                                                                       \
            Destructor();                                                       \
        }                                                                       \
                                                                                \
        void Constructor();                                                     \
        void Destructor();                                                      \
                                                                                \
        public:                                                                 \
        static bool Ready()                                                     \
        {                                                                       \
            return pInstance != 0;                                              \
        }                                                                       \
        static Class& Instance()                                                \
        {                                                                       \
            if(pInstance == 0)                                                      \
            {                                                                       \
                PrintOutputDebugString(ANSI_TO_TCHAR(__FILE__), __LINE__,                      \
                    TEXT("Global object %s has not been initialized"), ANSI_TO_TCHAR(TEXT(#Class)));      \
                inlDebugBreak();                                                    \
            }                                                                       \
            return *pInstance;                                                  \
        }                                                                       \


/// Loki Singleton ////////////////////////////////////////////////////////////////////////////////////

#define NUX_SINGLETON_CLASS_INTERNAL(ClassImpl)             \
    private:                                                \
    ClassImpl();                                            \
    ~ClassImpl();                                           \
    ClassImpl(const ClassImpl &);                           \
    ClassImpl& operator=(const ClassImpl &);                \
    ClassImpl* operator &();                                \
    friend struct Loki::CreateUsingNew<ClassImpl>;          \
    friend struct Loki::CreateUsingMalloc<ClassImpl>;       \
    friend struct Loki::CreateStatic<ClassImpl>;

#define NUX_SINGLETON_CLASS_DECLARE(ClassImpl, SingletonClass) typedef ::Loki::SingletonHolder<ClassImpl, ::Loki::CreateUsingNew, ::Loki::DefaultLifetime, ::Loki::ClassLevelLockable> SingletonClass;
#define NUX_SINGLETON_INSTANCE(SingletonClass) SingletonClass::Instance()
// Silence unused parameters: no warning
#define NUX_UNUSED(parameter) (void)parameter;

#endif // NMACROS_H