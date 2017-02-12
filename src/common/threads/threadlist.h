#ifndef THREADLIST_H
#define THREADLIST_H

#include <list>
#include <mutex>
/*
namespace Threads {

template <typename T>
class ThreadList {
public:
    inline bool emplace_back(T const & value) {
        std::unique_lock<std::mutex> lock(this->mutex);
        this->q.push(value);
        return true;
    }
    // deletes the retrieved element, do not use for non integral types
    inline bool erase(T & v) {
        std::unique_lock<std::mutex> lock(this->mutex);
        if (this->q.empty())
            return false;
        v = this->q.front();
        this->q.pop();
        return true;
    }
    inline bool empty() {
        std::unique_lock<std::mutex> lock(this->mutex);
        return this->l.empty();
    }
    inline std::size_t size() {
        std::unique_lock<std::mutex> lock(this->mutex);
        return l.size();
    }
private:
    std::list<T> l;
    std::mutex mutex;
};


} // namespace Threads
*/

#endif // THREADLIST_H
