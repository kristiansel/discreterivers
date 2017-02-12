#ifndef ASYNC_H
#define ASYNC_H

#include <list>
#include <iostream>
#include <functional>
#include <stdlib.h>

//#include "../common/macro/macrodebugassert.h"
#include "../common/threads/threadpool.h"

namespace sys {

class Async {

    struct Job
    {
        std::future<void> future;
        std::function<void(void)> on_return;
    };

    template<typename T>
    using storage_type = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

    std::list<Job> mJobs;
    Threads::ThreadPool mThreadPool;

    Async() : mThreadPool(5) {}

    static Async &get()
    {
        static Async a;
        return a;
    }

public:
    template<typename F, typename R>
    static void addJob(F && async_job, R && return_job);


    inline static void processReturnedJobs();
};

template<typename R>
bool is_ready(std::future<R> const& f)
{ return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }

inline void Async::processReturnedJobs()
{
    Async &async = get();

    auto erase_it = async.mJobs.end();
    for (auto job_it=async.mJobs.begin(); job_it!=async.mJobs.end(); job_it++)
    {
        if (is_ready(job_it->future))
        {
            std::cout << "processing return job" << std::endl;
            job_it->on_return();
            erase_it = job_it;
            break;
        }
    }
    if (erase_it!=async.mJobs.end()) async.mJobs.erase(erase_it);
}

template<typename F, typename R>
void Async::addJob(F && async_job, R && return_job)
{
    using RetT = decltype(async_job());

    Async &async = get();

    RetT *ret = reinterpret_cast<RetT*>(new storage_type<RetT>);

    async.mJobs.push_back(Job());
    Job &job = async.mJobs.back();

    job.future = async.mThreadPool.push([ret, async_job](int job_id){
        new(ret) RetT(async_job());
    });

    job.on_return = [ret, return_job](){
        return_job(*ret);
        delete ret;
    };
}

} // namespace sys

#endif // ASYNC_H
