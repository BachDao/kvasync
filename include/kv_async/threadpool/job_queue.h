//
// Created by Bach Dao.
//

#ifndef KV_ASYNC_JOB_QUEUE_H
#define KV_ASYNC_JOB_QUEUE_H
#include "kv_async/threadpool/work.h"
#include <deque>
#include <mutex>
#include <thread>
#include <vector>
namespace kv_async {
namespace detail {
class local_job_queue {
  std::deque<work> jobQueue_;
  std::mutex mutex_;

public:
  explicit local_job_queue(size_t defaultSize);
  template <typename Fn> void enqueue(Fn &&fn);
  bool dequeue(work &outVal);
};
template <typename Fn> void local_job_queue::enqueue(Fn &&fn) {
  std::lock_guard lck{mutex_};
  if constexpr (std::is_same_v<std::remove_cvref_t<Fn>, work>) {
    jobQueue_.push_back(std::forward<Fn>(fn));
    return;
  }
  jobQueue_.emplace_back(std::forward<Fn>(fn));
  return;
}
class job_queue {
  std::vector<std::unique_ptr<local_job_queue>> jobQueues_;
  size_t sizeMask_;

public:
  explicit job_queue(size_t innerQueueCount);
  template <typename Fn> void enqueue(Fn &&fn);
  local_job_queue &at(size_t idx);
  size_t size() const;
};
template <typename Fn> void job_queue::enqueue(Fn &&fn) {
  auto threadId = std::this_thread::get_id();
  auto hashVal = std::hash<std::thread::id>{}(threadId);
  auto idx = hashVal & sizeMask_;
  return jobQueues_[idx]->enqueue(std::forward<Fn>(fn));
}
} // namespace detail
} // namespace kv_async

#endif // KV_ASYNC_JOB_QUEUE_H
