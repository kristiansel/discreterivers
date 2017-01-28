#ifndef INDIRECTIONSET_H
#define INDIRECTIONSET_H

#include <iostream>
#include <array>
#include <algorithm>
#include <memory>

#include "macro/macrodebugassert.h"

namespace stdext {

template<typename T, std::size_t Capacity>
class indirection_set
{
public:
    using SizeT = std::size_t;
    using IndexT = std::size_t;
    static const IndexT invalid_indirection_index = -1;

    class node
    {
    private:
        friend class indirection_set;
        static const std::size_t data_size = sizeof(T);
        static const std::size_t data_align = alignof(T);
        using data_type = typename std::aligned_storage<data_size, data_align>::type;
    public:
        node() : version_(0), indirection_index_(invalid_indirection_index) {}
        node(node &&n) { move(std::move(n)); }
        node(const node &n) { copy(n); }

        template<typename ...Args>
        inline void create(IndexT indirection_index, Args... args)
        {
            indirection_index_ = indirection_index;
            version_++; // just to be sure
            new(&data_) T(std::forward<Args...>(args)...);
        }

        inline void destruct()
        {
            version_++;
            indirection_index_ = invalid_indirection_index;
            destroy(&data_);
        }

        inline T &get() { return reinterpret_cast<T&>(data_); }

        inline node &operator=(node&& other)
        {
            move_assign(std::move(other));
            return *this;
        }

        inline node &operator=(node const& other)
        {
            copy_assign(other);
            return *this;
        }

    private:
        void move(node &&n)
        {
            //std::cout << "move called" << std::endl;
            version_ = n.version_;
            indirection_index_ = n.indirection_index_;
            memcpy(&data_, &n.data_, data_size);

            // invalidate other
            n.indirection_index_ = invalid_indirection_index;
        }

        void copy(const node &n)
        {
            //std::cout << "copy called" << std::endl;
            //std::cout << "other data: " << reinterpret_cast<const float*>(&(n.data_))[0] << std::endl;
            version_ = n.version_;
            indirection_index_ = n.indirection_index_;
            memcpy(&data_, &n.data_, data_size);
        }

        inline static void destroy(void* data)
        {
            reinterpret_cast<T*>(data)->~T();
        }

        inline void copy_assign(node const& rhs)
        {
            copy(rhs);
            version_++;
        }

        inline void move_assign(node&& rhs)
        {
            move(std::move(rhs));
            version_++;
        }

    private:
        data_type data_;
        IndexT version_;
        IndexT indirection_index_;
    };



private:
    std::array<node,   Capacity>                    store_;
    std::array<IndexT, Capacity>                    indirection_;
    std::array<IndexT, Capacity>                    freelist_;
    SizeT                                           count_;

    inline IndexT get_freelist_top() const { return Capacity-count_-1; }
    inline void freelist_push(IndexT index) { freelist_[get_freelist_top()] = index; }

public:
    indirection_set() : count_(0)
    {
        for (int i = 0; i<Capacity; i++)
        {
            freelist_[i] = Capacity-i-1;
            //std::cout << "freelist_[" << i << "] = " << freelist_[i] << std::endl;


            //std::cout << "store_[" << i << "].indirection_index_ = " << store_[i].indirection_index_ << std::endl;
        }
    }

    ~indirection_set()
    {
        for (int i = 0; i<count_; i++)
        {
            node &n = store_[i];

            //std::cout << "store_[" << i << "].indirection_index_ = " << store_[i].indirection_index_ << std::endl;
            if (is_valid(n))
            {
                //std::cout << "destroying location " << i << std::endl;
                n.destruct();
            }
        }
    }

    template<typename ...Args>
    inline IndexT create(Args... args)
    {
        DEBUG_ASSERT(count_ < Capacity);
        //std::cout << "CREATE" << std::endl;
        IndexT freelist_top = get_freelist_top();
        IndexT stable_index = freelist_[freelist_top];

        //std::cout << "freelist top index: " << freelist_top << std::endl;
        //std::cout << "freelist top value: " << stable_index << std::endl;

        IndexT store_index = count_;
        indirection_[stable_index] = store_index;

        store_[store_index].create(stable_index, std::forward<Args...>(args)...);

        count_++;

        return stable_index;
    }

    inline T& get(IndexT stable_index)
    {
        //std::cout << "GET" << std::endl;
        //std::cout << "stable_index: " << stable_index << std::endl;
        IndexT store_index = indirection_[stable_index];
        //std::cout << "store_index: " << store_index << std::endl;
        return store_[store_index].get();
    }

    inline bool is_valid(const node &n) { return n.indirection_index_ != invalid_indirection_index; }

    template<typename F>
    void for_all(const F &f)
    {
        //for (std::size_t i=0; i<count_; i++)
        for (std::size_t i=0; i<count_; i++)
        {
            node &n = store_[i];
            //std::cout << "store_[" << i << "].indirection_index_ = " << store_[i].indirection_index_ << std::endl;
            if (is_valid(n))
            {
                f(n.get());
            }
        }
    }

    inline void destroy(IndexT stable_index)
    {
        /*for (int i = 0; i<Capacity; i++)
        {
            std::cout << "store_[" << i << "].indirection_index_ = " << store_[i].indirection_index_ << std::endl;
        }*/

        //std::cout << "DESTROY" << std::endl;
        //std::cout << "stable_index = " << stable_index << std::endl;
        // call destructor on destroyed object (also increments version)
        IndexT store_index = indirection_[stable_index];
        //std::cout << "store_index = " << store_index << std::endl;
        store_[store_index].destruct();

        /*for (int i = 0; i<Capacity; i++)
        {
            std::cout << "store_[" << i << "].indirection_index_ = " << store_[i].indirection_index_ << std::endl;
        }*/


        // swap the position of the destroyed object with the last;
        auto end = count_-1;
        if (end != store_index)
        {
            //std::cout << "swapping contents of " << end << " and " << store_index << std::endl;
            std::swap(store_[end], store_[store_index]);
            // update the link between indirection table and moved object
            indirection_[store_[store_index].indirection_index_] = store_index;
        }

        // add the newly freed indirection slot to the free list
        freelist_push(stable_index);

        // decrement the count
        count_--;

        //std::cout << "freelist top index: " << get_freelist_top() << std::endl;
        //std::cout << "freelist top value: " << stable_index << std::endl;
    }
};

}

#endif // INDIRECTIONSET_H
