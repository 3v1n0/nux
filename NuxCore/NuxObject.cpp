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


#include "NKernel.h"
#include "NuxObject.h"
#include "SmartPtr/NSmartPtr.h"

namespace nux { //NUX_NAMESPACE_BEGIN

NUX_IMPLEMENT_ROOT_OBJECT_TYPE(NuxTrackable);
NUX_IMPLEMENT_OBJECT_TYPE(NuxObject);

std::new_handler NuxTrackable::m_new_current_handler = 0;
NuxTrackable::AllocationList NuxTrackable::m_allocation_list;

NuxTrackable::AllocationList::AllocationList()
{

}

NuxTrackable::AllocationList::~AllocationList()
{

}

int NuxTrackable::m_total_allocated_size = 0;
int NuxTrackable::m_number_of_objects = 0;

NuxTrackable::NuxTrackable()
{
    m_owns_the_reference = false;
    m_total_allocated_size = 0;
    m_number_of_objects = 0;
}

NuxTrackable::~NuxTrackable()
{

}

void NuxTrackable::Reference()
{

}

bool NuxTrackable::UnReference()
{
    return false;
}

bool NuxTrackable::SinkReference()
{
    return false;
}

bool NuxTrackable::Dispose()
{
    return false;
}

bool NuxTrackable::OwnsTheReference()
{
    return m_owns_the_reference;
}

void NuxTrackable::SetOwnedReference(bool b)
{
    if(m_owns_the_reference == true)
    {
        nuxDebugMsg(TEXT("[NuxTrackable::SetOwnedReference] Do not change the ownership if is already set to true!"));
        return;
    }
    m_owns_the_reference = b;
}

std::new_handler
NuxTrackable::set_new_handler(std::new_handler handler)
{
    std::new_handler old_handler = m_new_current_handler;
    m_new_current_handler = handler;
    return old_handler;
}

void*
NuxTrackable::operator new(size_t size)
{
    // Set the new_handler for this call
    std::new_handler global_handler  = std::set_new_handler(m_new_current_handler);

    // If allocation fails m_new_current_handler is called, if specified, otherwise the global new_handler is called.
    void *ptr;

    try
    {
        ptr = ::operator new(size);
        m_allocation_list.push_front(ptr);
        NUX_STATIC_CAST(NuxTrackable*, ptr)->m_size_of_this_object = size;
        m_total_allocated_size += size;
        ++m_number_of_objects;
    }
    catch(std::bad_alloc&)
    {
        std::set_new_handler(global_handler);
        throw;
    }

    //  Reset gloabal new_handler
    std::set_new_handler(global_handler);
    return ptr;
}

#if (__GNUC__ < 4 && __GNUC_MINOR__ < 4)

void* NuxTrackable::operator new(size_t size, void *ptr)
{
    return ::operator new(size, ptr);
}

#endif

void NuxTrackable::operator delete(void *ptr)
{
    AllocationList::iterator i = std::find(m_allocation_list.begin(), m_allocation_list.end(), ptr);
    if (i != m_allocation_list.end())
    {
        m_total_allocated_size -= NUX_STATIC_CAST(NuxTrackable*, ptr)->m_size_of_this_object;
        --m_number_of_objects;
        m_allocation_list.erase(i);
        ::operator delete(ptr);
    }
}

bool NuxTrackable::IsHeapAllocated() const
{
    return IsDynamic();
}

bool NuxTrackable::IsDynamic() const
{
    // Get pointer to beginning of the memory occupied by this.
    const void* ptr = dynamic_cast<const void*>(this);

    // Search for ptr in allocation_list
    AllocationList::iterator i = std::find(m_allocation_list.begin(), m_allocation_list.end(), ptr);
    return i != m_allocation_list.end();
}

//////////////////////////////////////////////////////////////////////

NuxObject::NuxObject(bool OwnTheReference, NUX_FILE_LINE_DECL)
{
#if defined(NUX_DEBUG)
    m_allocation_file_name      = __Nux_FileName__;
    m_allocation_line_number    = __Nux_LineNumber__;
#endif

    m_reference_count = new NThreadSafeCounter();
    m_weak_reference_count = new NThreadSafeCounter();
    
    m_reference_count->Set(1);
    m_weak_reference_count->Set(1);

    SetOwnedReference(OwnTheReference);
}

NuxObject::~NuxObject()
{
    //nuxAssertMsg(m_reference_count.GetValue() == 0, TEXT("[NuxObject::~NuxObject] Invalid object destruction."));
}

void NuxObject::Reference()
{
    if(!OwnsTheReference())
    {
        SetOwnedReference(true);
        // The ref count remains at 1. Exit the method.
        return;
    }
    m_reference_count->Increment();
    m_weak_reference_count->Increment();
}

bool NuxObject::UnReference()
{
    if(!OwnsTheReference())
    {
        nuxAssertMsg(0, TEXT("[NuxObject::Unref] Never call Unref on an object with a floating reference. Call Dispose() instead."));
        return false;
    }

    m_reference_count->Decrement();
    m_weak_reference_count->Decrement();
    if((m_reference_count->GetValue() == 0) && IsDynamic())
    {
        Destroy();
        return true;
    }
    return false;
}

bool NuxObject::SinkReference()
{
    if(!OwnsTheReference())
    {
        SetOwnedReference(true);
        // The ref count remains at 1. Exit the method.
        return true;
    }
    return false;
}

bool NuxObject::Dispose()
{
    if(OwnsTheReference() && (m_reference_count->GetValue() == 1))
    {
        Destroy();
        return true;
    }
    nuxAssertMsg(0, TEXT("[NuxObject::Dispose] Trying to destroy and object taht is still referenced"));
    return false;
}

void NuxObject::Destroy()
{
    nuxAssert(m_reference_count->GetValue() == 0);
    if((m_reference_count->GetValue() == 0) && (m_weak_reference_count->GetValue() == 0))
    {
        delete m_reference_count;
        delete m_weak_reference_count;
    }
    delete this;
}

void NuxObject::IncrementWeakCounter()
{
    m_weak_reference_count->Increment();
}

void NuxObject::DecrementWeakCounter()
{
    m_weak_reference_count->Decrement();
}

} //NUX_NAMESPACE_END

