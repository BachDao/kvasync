//
// Created by Bach Dao.
//

#include "kv_async/threadpool/worker.h"
#include "kv_async/threadpool/thread_pool.h"
namespace kv_async {
namespace detail {

void worker::do_work() {
  work curJob;
  while (true) {
    auto stopRequested = stopRequested_.load(std::memory_order_relaxed);
    if (stopRequested) {
      break;
    }
    drain_private_queue(curJob);
    if (!steal_job_from_global_queue(curJob)) {
      auto pendingReq = pendingStopRequest_.load(std::memory_order_relaxed);
      if (pendingReq) {
        break;
      }
      wait_for_job();
    }
  }
  threadPool_.worker_report_done();
}
void worker::drain_private_queue(work &curJob) {
  while (privateQueue_.size() > 0) {
    curJob = std::move(privateQueue_.front());
    privateQueue_.pop();
    if (curJob)
      curJob();
  }
}
bool worker::steal_job_from_global_queue(work &curJob) {
  auto idx = lastDequeueIdx_;
  auto retryCount = 0;
  while (true) {
    auto &innerQueue = globalJobQueue_.at(idx);
    if (innerQueue.dequeue(curJob)) {
      if (curJob)
        curJob();
      return true;
    }
    retryCount++;
    idx++;
    if (retryCount == globalJobQueue_.size()) {
      return false;
    }
  }
}
worker::worker(job_queue &globalJobQueue, thread_pool &tp)
    : globalJobQueue_(globalJobQueue), threadPool_(tp) {
  workerState.ptrCurrentWorker = this;
  workerThread_ = std::thread{[&] { do_work(); }};
}
worker::~worker() {
  if (workerThread_.joinable()) {
    workerThread_.join();
  }
}
void worker::wait_for_job() {
  std::unique_lock lck{mutex_};
  jobAvailable_ = false;
  cv_.wait(lck, [&] {
    return jobAvailable_ || stopRequested_.load(std::memory_order_acquire) ||
           pendingStopRequest_.load(std::memory_order_relaxed);
  });
}
void worker::wake_up() {
  {
    std::lock_guard lck{mutex_};
    jobAvailable_ = true;
  }
  cv_.notify_one();
}
void worker::stop(bool pending) {
  if (pending) {
    pendingStopRequest_.store(true, std::memory_order_release);
  } else {
    stopRequested_ = true;
  }
  cv_.notify_all();
}
} // namespace detail
} // namespace kv_async