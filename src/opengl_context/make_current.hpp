#pragma once

#include "make_current.h"

// other projects
#include "error/ft_assert.h"

namespace ft {
namespace rf {
namespace context {

// Make a object the current active object
template<class T>
make_current<T>::make_current(const T& p_object)
{
    const auto was_added = push_object(p_object);

    if (was_added == true)
    {
        m_object = { &p_object, [](const T*& p_ptr) {
            if (p_ptr != nullptr)
            {
                pop_object(*p_ptr);
            }
        } };
    }
}


// Make a object active
template<class T>
void make_current<T>::activate(const T& p_object)
{
    p_object.activate();
}


// Make no object active
template<class T>
void make_current<T>::deactivate(const T& p_object)
{
    p_object.deactivate();
}


// Replace the current active object with this given object
//  and stash the previously active object in the object stack
// Returns true if the active object changed
//  and the object was put in the stack
// Returns false if nothing was done
// Put this object in m_object is this function returns true
template<class T>
bool make_current<T>::push_object(const T& p_object)
{
    auto& stack = get_object_stack();

    // No need to stack
    if (stack.empty() == true || stack.back() != &p_object)
    {
        // Make the previous instance inactive


        // Add this object to the stack
        stack.emplace_back(&p_object);

        // Make the object current
        activate(p_object);

        return true;
    }
    else
    {
        return false;
    }
}

// Make the current active object inactive
//  and replace it with the previously active one
// Makes sure the passed object is the currently active one
template<class T>
void make_current<T>::pop_object(const T& p_object)
{
    auto& stack = get_object_stack();

    FT_ASSERT(stack.empty() == false);
    FT_ASSERT(stack.back() == &p_object);

    // Make the previously active object inactive
    deactivate(p_object);

    // Remove this object from the stack
    stack.pop_back();

    // Make the previously active object active again
    if (stack.empty() == false)
    {
        activate(*stack.back());
    }
}


// Get the active object stack for this thread
template<class T>
std::vector<const T*>&
make_current<T>::get_object_stack()
{
    thread_local static std::vector<const T*> thread_stack;
    return thread_stack;
}

}   // namespace context
}   // namespace rf
}   // namespace ft
