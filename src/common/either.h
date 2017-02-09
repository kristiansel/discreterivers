#ifndef EITHER_H
#define EITHER_H

#include <type_traits>

namespace stdext {

static constexpr std::size_t invalid_value = std::size_t(-1);

// Static max
template <std::size_t arg1, std::size_t... others>
struct static_max;

template <std::size_t arg>
struct static_max<arg>
{
    static const std::size_t value = arg;
};

template <std::size_t arg1, std::size_t arg2, std::size_t... others>
struct static_max<arg1, arg2, others...>
{
    static const std::size_t value = arg1 >= arg2 ? static_max<arg1, others...>::value : static_max<arg2, others...>::value;
};

// Type index helper
template <typename T, typename... Types>
struct get_type_index;

template <typename T>
struct get_type_index<T>
{
    static const std::size_t value = invalid_value;
};

template <typename T, typename... Tail>
struct get_type_index<T, T, Tail...>
{
    static const std::size_t value = sizeof...(Tail);
};

template <typename T, typename H, typename... Tail>
struct get_type_index<T, H, Tail...>
{
    static const std::size_t value = get_type_index<T, Tail...>::value;
};

// Lifetime functions for either
template <typename... Types>
struct either_lifetime_helper;

template <typename T, typename... Types>
struct either_lifetime_helper<T, Types...>
{
    inline static void destroy(const std::size_t type_index, void* data)
    {
        if (type_index == sizeof...(Types))
        {
            reinterpret_cast<T*>(data)->~T();
        }
        else
        {
            either_lifetime_helper<Types...>::destroy(type_index, data);
        }
    }

    inline static void move(const std::size_t old_type_index, void* old_value, void* new_value)
    {
        if (old_type_index == sizeof...(Types))
        {
            new (new_value) T(std::move(*reinterpret_cast<T*>(old_value)));
        }
        else
        {
            either_lifetime_helper<Types...>::move(old_type_index, old_value, new_value);
        }
    }

    inline static void copy(const std::size_t old_type_index, const void* old_value, void* new_value)
    {
        if (old_type_index == sizeof...(Types))
        {
            new (new_value) T(*reinterpret_cast<const T*>(old_value));
        }
        else
        {
            either_lifetime_helper<Types...>::copy(old_type_index, old_value, new_value);
        }
    }
};

template <>
struct either_lifetime_helper<>
{
    inline static void destroy(const std::size_t, void*) {}
    inline static void move(const std::size_t, void*, void*) {}
    inline static void copy(const std::size_t, const void*, void*) {}
};


template<typename... Types>
class either // need to make sure none of the types are the same
{
private:
    static const std::size_t data_size = static_max<sizeof(Types)...>::value;
    static const std::size_t data_align = static_max<alignof(Types)...>::value;
    using data_type = typename std::aligned_storage<data_size, data_align>::type;
public:
    template<typename T>
    inline either(const T &other) : type_index_(get_type_index<T, Types...>::value)
    {
        static_assert(get_type_index<T, Types...>::value != invalid_value, "could not get type index, check that the type of the constructor argument is a part of the `either`");

        new(&data_) T(other);
    }

    inline ~either()
    {
        either_lifetime_helper<Types...>::destroy(type_index_, &data_);
    }

    // move constructor is also fishy
    inline either(either<Types...> &&other) : type_index_(other.type_index_)
    {
        either_lifetime_helper<Types...>::move(other.type_index_, &other.data_, &data_);
    }

    inline either(const either<Types...> &other) : type_index_(other.type_index_)
    {
        either_lifetime_helper<Types...>::copy(other.type_index_, &other.data_, &data_);
    }

private:
    inline void copy_assign(either<Types...> const& rhs)
    {
        either_lifetime_helper<Types...>::destroy(type_index_, &data_);
        type_index_ = invalid_value;
        either_lifetime_helper<Types...>::copy(rhs.type_index_, &rhs.data_, &data_);
        type_index_ = rhs.type_index;
    }

    inline void move_assign(either<Types...>&& rhs)
    {
        either_lifetime_helper<Types...>::destroy(type_index_, &data_);
        type_index_ = invalid_value;
        either_lifetime_helper<Types...>::move(rhs.type_index_, &rhs.data_, &data_);
        type_index_ = rhs.type_index;
    }

public:
    inline either<Types...>& operator=(either<Types...>&& other)
    {
        move_assign(std::move(other));
        return *this;
    }

    inline either<Types...>& operator=(either<Types...> const& other)
    {
        copy_assign(other);
        return *this;
    }

    inline std::size_t get_type() const { return type_index_; }

    template<typename T>
    struct is_a
    {
        static constexpr std::size_t value = get_type_index<T, Types...>::value;
    };

    //======================================================================================
    // Get as type needs some work... could constrain valid arguments to Types...
    // Could also add a debug_assert that the requested type and current type index matches.
    //======================================================================================

    // should add safety to this
    template<typename T>
    T &get() { return reinterpret_cast<T&>(data_); }

    // should add safety to this
    template<typename T>
    const T &get_const() const { return reinterpret_cast<const T&>(data_); }


private:
    std::size_t type_index_;
    data_type data_;
};

} // stdext

#endif // EITHER_H
