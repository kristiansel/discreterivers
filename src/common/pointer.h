#ifndef WritePtr_H
#define WritePtr_H

#include "macro/macrodebugassert.h"

namespace Ptr {

template<typename T>
class WritePtr
{
    T *p_;

    WritePtr() = delete;
public:
    WritePtr(T *ptr) : p_(ptr) {}

    //T&                                 operator*(){return *m_ptr;}
    //const T&                           operator*()const{return *m_ptr;}
    // etc...
    T*           operator->()       { return p_; }
    const T*     operator->() const { return p_; }

    bool operator==(const WritePtr &o) const { return p_ == o.p_; }
    bool operator!=(const WritePtr &o) const { return p_ != o.p_; }

    operator     bool()       const { return p_ != nullptr; }
};

template<typename T>
class ReadPtr
{
    const T * const p_;

    ReadPtr() = delete;
public:
    ReadPtr(const T *ptr) : p_(ptr) { DEBUG_ASSERT(ptr!=nullptr); }

    const T*     operator->() const { return p_; }

    bool operator==(const ReadPtr &o) const { return p_ == o.p_; }
    bool operator!=(const ReadPtr &o) const { return p_ != o.p_; }
};


template<typename T>
class OwningPtr
{
    T *p_;

    OwningPtr() = delete;                       // cannot be uninitialized
    OwningPtr(const OwningPtr&) = delete;       // owning ptr can only have one owner
    OwningPtr& operator=(OwningPtr const& other) = delete;

public:
    OwningPtr(T *ptr) : p_(ptr) {}
    OwningPtr(OwningPtr&& o) : p_(o.p_) { o.p_ = nullptr; }
    ~OwningPtr() { delete p_;}

    OwningPtr& operator=(OwningPtr&& o) { delete p_; p_=o.p_; o.p_=nullptr; }

    ReadPtr<T> getReadPtr() { return ReadPtr<T>(p_); }

    // move assign

    //T&                                 operator*(){return *m_ptr;}
    //const T&                           operator*()const{return *m_ptr;}
    // etc...
    T*           operator->()       { return p_; }
    const T*     operator->() const { return p_; }

    bool operator==(const OwningPtr &o) const { return p_ == o.p_; }
    bool operator!=(const OwningPtr &o) const { return p_ != o.p_; }

    operator     bool()       const { return p_ != nullptr; }
};



}

#endif // WritePtr_H