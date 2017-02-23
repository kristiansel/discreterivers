#ifndef INITPTR_H
#define INITPTR_H

namespace stdext {

template<typename T>
class InitPtr
{
    T *p_;

    InitPtr() = delete;
public:
    InitPtr(T *ptr) : p_(ptr) {}

    //T&                                 operator*(){return *m_ptr;}
    //const T&                           operator*()const{return *m_ptr;}
    // etc...
    T*           operator->()       { return p_; }
    const T*     operator->() const { return p_; }

    bool operator==(const InitPtr &o) const { return p_ == o.p_; }
    bool operator!=(const InitPtr &o) const { return p_ != o.p_; }

    operator     bool()       const { return p_ != nullptr; }
};

template<typename T>
class OwningInitPtr
{
    T *p_;

    OwningInitPtr() = delete;                       // cannot be uninitialized
    OwningInitPtr(const OwningInitPtr&) = delete;   // owning initptr can only have one owner
    OwningInitPtr& operator=(OwningInitPtr const& other) = delete;

public:
    OwningInitPtr(T *ptr) : p_(ptr) {}
    OwningInitPtr(OwningInitPtr&& o) : p_(o.p_) { o.p_ = nullptr; }
    ~OwningInitPtr() { delete p_;}

    OwningInitPtr& operator=(OwningInitPtr&& o) { delete p_; p_=o.p_; o.p_=nullptr; }

    // move assign

    //T&                                 operator*(){return *m_ptr;}
    //const T&                           operator*()const{return *m_ptr;}
    // etc...
    T*           operator->()       { return p_; }
    const T*     operator->() const { return p_; }

    bool operator==(const OwningInitPtr &o) const { return p_ == o.p_; }
    bool operator!=(const OwningInitPtr &o) const { return p_ != o.p_; }

    operator     bool()       const { return p_ != nullptr; }
};

}

#endif // INITPTR_H
