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

NAMESPACE_BEGIN

NuxTrackable::NuxTrackable()
{
    m_owns_the_reference = false;
}

NuxTrackable::~NuxTrackable()
{

}

void NuxTrackable::Ref()
{

}

void NuxTrackable::Unref()
{

}

bool NuxTrackable::OwnsTheReference()
{
    return m_owns_the_reference;
}

void NuxTrackable::SetOwnedReference(bool b)
{
    m_owns_the_reference = b;
}

std::new_handler
NuxTrackable::set_new_handler(std::new_handler handler)
{
    std::new_handler old_handler = current_handler_;
    current_handler_ = handler;
    return old_handler;
}

void*
NuxTrackable::operator new(size_t size)
{
    // Set the new_handler for this call
    std::new_handler global_handler  = std::set_new_handler(current_handler_);

    // If allocation fails current_handler_ is called, if specified, otherwise the global new_handler is called.
    void *ptr;

    try
    {
        ptr = ::operator new(size);
        allocation_list_.push_front(ptr);
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
    AllocationList::iterator i = std::find(allocation_list_.begin(), allocation_list_.end(), ptr);
    if (i != allocation_list_.end())
    {
        allocation_list_.erase(i);
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
    AllocationList::iterator i = std::find(allocation_list_.begin(), allocation_list_.end(), ptr);
    return i != allocation_list_.end();
}

//////////////////////////////////////////////////////////////////////

NuxObject::NuxObject()
{
    m_reference_count.Set(1);
    SetOwnedReference(true);
}


void NuxObject::Ref()
{
    if(!OwnsTheReference())
    {
        SetOwnedReference(true);
        // The ref count remains at 1. Exit the method.
        return;
    }
    m_reference_count.Increment();
}

void NuxObject::Unref()
{
    if(!OwnsTheReference())
    {
        nuxAssertMsg(0, TEXT("[NuxObject::Unref] Never call Unref on an object with a floating reference."));
        return;
    }

    m_reference_count.Decrement();
    if((m_reference_count == 0) && IsDynamic())
    {
        Destroy();
    }
}

void NuxObject::Destroy()
{
    delete this;
}

NAMESPACE_END
