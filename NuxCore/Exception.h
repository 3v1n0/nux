#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>

NAMESPACE_BEGIN

// *****************
// *               *
// *  Exception  *
// *               *
// *****************
//
// The exception handling classes from the STL do what we want,
// so this is nothing more than a base class for any of our 
// derived exceptions. I have changed the constructor to take a
// std::string object, but this gets converted before calling
// std::exception.


class Exception : public std::exception
{
public:
    Exception (std::string name) throw()
        : std::exception (), name_ (name) 
    {
    }

    virtual ~Exception () throw () {}

    virtual const char* what () const throw()
    { 
        return name_.c_str();
    }

protected:
    std::string name_;
};


// ************************
// *                      *
// *  Derived Exceptions  *
// *                      *
// ************************

class BoundsException : public Exception
{
public:
    BoundsException (std::string name="") 
        : Exception ("apBoundsException: " + name) 
    {
    }
};

class NotSupportedException : public Exception
{
public:
    NotSupportedException (std::string name="")
        : Exception ("NotSupportedException: " + name) 
    {
    }
};

class DivisionByZeroException : public Exception
{
public:
    DivisionByZeroException (std::string name="")
        : Exception ("DivisionByZeroException: " + name) 
    {
    }
};

class InvalidIndexException : public Exception
{
public:
    InvalidIndexException (std::string name="")
        : Exception ("InvalidIndexException: " + name) 
    {
    }
};

NAMESPACE_END

#endif // EXCEPTION_H