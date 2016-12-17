#ifndef TYPETAG_H
#define TYPETAG_H

template<class Tag, class impl, impl t_default_value>
class ID
{
public:
        static ID invalid() { return ID(); }

        // Defaults to ID::invalid()
        ID() : m_val(t_default_value) { }

        // Explicit constructor:
        explicit ID(impl val) : m_val(val) { }

        // Explicit conversion to get back the impl:
        explicit operator impl() const { return m_val; }

        friend bool operator==(ID a, ID b) { return a.m_val == b.m_val; }
        friend bool operator!=(ID a, ID b) { return a.m_val != b.m_val; }
        friend bool operator <(ID a, ID b) { return a.m_val < b.m_val; }
        friend bool operator >(ID a, ID b) { return a.m_val > b.m_val; }

        static constexpr impl default_value = t_default_value;

private:
        impl m_val;
};

/*template<class T, T val>

template<class T, T val>
class typetag {
    struct typetag_tag{};
    typedef ID<primitive_type_tag, PRIMITIVE_GL_CODE, GL_TRIANGLES> gl_primitive_type;
};*/


#endif // TYPETAG_H
