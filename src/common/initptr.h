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

    operator     bool()       const { return p_ ? true : false; }
};

}

#endif // INITPTR_H
