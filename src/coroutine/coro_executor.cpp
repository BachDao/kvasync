//
// Created by Bach Dao.
//

#include "kv_async/coroutine/coro_executor.h"
namespace kv_async {
namespace detail {

single_thread_executor::single_thread_executor() {
  thread_ = std::thread{[&] { do_work(); }};
}
void single_thread_executor::do_work() {
  work curJob;
  while (!stopRequest_.load(std::memory_order_relaxed)) {
    {
      std::unique_lock lck{mutex_};
      if (jobQueue_.size() > 0) {
        curJob = std::move(jobQueue_.back());
        jobQueue_.pop_back();
      } else {
        cv_.wait(lck, [&] { return jobQueue_.size() > 0; });
      }
    }
    if (curJob) {
      curJob();
    }
  }
}
single_thread_executor::~single_thread_executor() {
  stopRequest_.store(true, std::memory_order_seq_cst);
  if (thread_.joinable()) {
    thread_.join();
  }
}
void single_thread_executor::submit(work w) {
  {
    std::lock_guard lck{mutex_};
    jobQueue_.emplace_back(std::move(w));
  }
  cv_.notify_one();
}
void thread_pool_executor::submit(work w) { threadPool_.submit(std::move(w)); }
thread_pool_executor::~thread_pool_executor() {}
thread_pool thread_pool_executor::threadPool_{THREAD_POOL_COUNT};
coro_executor::~coro_executor() {}
} // namespace detail
} // namespace kv_async