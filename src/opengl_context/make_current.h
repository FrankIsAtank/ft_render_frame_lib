#pragma once

// Make an activatable object the currently active one
// When this object is destroyed the previously active object is
//  restored as the currently active object
// Acts as a stack

// other projects
#include "handle/ressource_handle.hpp"

// standard headers
#include <vector>

namespace ft {
namespace rf {
namespace context  {

template<class T>
class make_current
{
public:
    // Default constructor
    // Equivalent to a moved-from instance
    make_current() = default;

    // Make this object the current active object
    explicit make_current(const T& p_object);


    // Make a object active
    // Don't call manually
    static void activate(const T& p_object);

    // Make no object active
    // Don't call manually
    static void deactivate(const T& p_object);

private:
    // Replace the current active object with this given object
    //  and stash the previously active object in the object stack
    // Returns true if the active object changed
    //  and the object was put in the stack
    // Returns false if nothing was done
    // Put this object in m_object is this function returns true
    static bool push_object(const T& p_object);

    // Make the current active object inactive
    //  and replace it with the previously active one
    // Makes sure the passed object is the currently active one
    static void pop_object(const T& p_object);

    // Get the active object stack for this thread
    static std::vector<const T*>& get_object_stack();

private:
    // If this instance is responsible for an active object
    //  then this pointer points to that object
    ft::base::handle::ressource_handle<const T*, nullptr> m_object;
};

}   // namespace context
}   // namespace rf
}   // namespace ft

#include "make_current.hpp"
