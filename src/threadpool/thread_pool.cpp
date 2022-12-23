//
// Created by Bach Dao.
//

#include "kv_async/threadpool/thread_pool.h"
#include <iostream>
#include <string>
namespace kv_async {

thread_pool::thread_pool(size_t workerCount)
    : globalJobQueue_(workerCount), workers_(), workerCount_(workerCount) {
  workers_.reserve(workerCount);
  std::atomic_thread_fence(std::memory_order_seq_cst);
  for (int i = 0; i < workerCount; ++i) {
    workers_.emplace_back(
        std::make_unique<detail::worker>(globalJobQueue_, *this));
  }
}
void thread_pool::drain_work_queue_on_exit(bool flag) {
  drainQueueOnExit_ = flag;
}

thread_pool::~thread_pool() {
  for (auto &w : workers_) {
    w->stop(drainQueueOnExit_);
  }
  waiting_for_worker_exit();
  return;
}
void thread_pool::waiting_for_worker_exit() {
  std::unique_lock lck{mutex_};
  cv_.wait(lck, [&] { return workerDoneCounter_.load() == workerCount_; });
}
void thread_pool::worker_report_done() {
  int prevCount = 0;
  {
    std::lock_guard lck{mutex_};
    prevCount = workerDoneCounter_.fetch_add(1);
  }
  if (prevCount == workerCount_ - 1) {
    cv_.notify_one();
  }
}
} // namespace kv_async
