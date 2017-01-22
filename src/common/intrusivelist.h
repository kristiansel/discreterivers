#ifndef INTRUSIVELIST_H
#define INTRUSIVELIST_H

namespace stdext {

/**
 * Idea:
 *      anything with intrusive list trait can
 */

class intrusive_list
{
public:
    class node
    {
    public:
        node() : next_(nullptr), prev_(nullptr) {}
        node(intrusive_list &list) { list.insert_front(this); }
        ~node() { remove(); }

    private:
        node* next_;
        node* prev_;

        inline void remove()
        {
            // checks should not be necessary. Links should never be nullptr
            if (next_) next_->prev_ = prev_;
            if (prev_) prev_->next_ = next_;

            next_ = nullptr;
            prev_ = nullptr;
        }

        friend class intrusive_list;
    };

    intrusive_list()
    {
        root_.next_ = &root_;
        root_.prev_ = &root_;
    }

    inline void insert(node* nd_ptr, node* prev, node* next) {

      nd_ptr->next_ = next;

      nd_ptr->prev_ = prev;

      next->prev_ = nd_ptr;

      prev->next_ = nd_ptr;

    }

    inline void insert_front(node * nd_ptr)
    {
        insert(nd_ptr, &root_, root_.next_);
    }

private:
    node root_;
};

}

#endif // INTRUSIVELIST_H
