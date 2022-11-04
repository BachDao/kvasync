#include "kvasync/threadpool/thread_pool.h"
#include <iostream>
using namespace std::chrono_literals;
namespace kvasync {
namespace impl {
thread_pool::thread_pool(size_t numOfWorkers) : numberOfWorker_(numOfWorkers) {
  init_workers(numOfWorkers);
  waiting_for_workers_ready();
}
void thread_pool::init_workers(size_t numOfWorker) {
  while (numOfWorker--) {
    workers_.emplace_back(*this);
  }
}

void thread_pool::worker_notify_ready() {
  auto newCounterVal = readyCounter_.fetch_add(1);
  if (newCounterVal == numberOfWorker_ - 1) {
    start_all_worker();
  }
}
void thread_pool::waiting_for_workers_ready() {
  std::unique_lock lck{mutex_};
  cv_.wait(lck, [this] { return readyFlag_; });
}
void thread_pool::start_all_worker() {
  {
    std::lock_guard lck{mutex_};
    readyFlag_ = true;
  }
  cv_.notify_all();
}
void thread_pool::worker_wait() {
  std::unique_lock lck{mutex_};
  cv_.wait(lck, [this] { return readyFlag_; });
}
bool thread_pool::is_worker_thread() { return impl::workerState.valid(); }
thread_pool::~thread_pool() {
  std::cout << "thread pool dtor" << std::endl;
  stopRequest_.test_and_set();
  for (auto &v : workers_) {
    if (v.worker_.joinable()) {
      v.worker_.join();
    }
  }
}
job thread_pool::get_job() {
  return jobQueue_.pop();
}
} // namespace impl
} // namespace kvasync