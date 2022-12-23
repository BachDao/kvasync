//
// Created by Bach Dao.
//

#include "kv_async/threadpool/job_queue.h"
#include <cassert>
namespace kv_async {
namespace detail {
job_queue::job_queue(size_t innerQueueCount) : sizeMask_(innerQueueCount - 1) {
  assert((innerQueueCount & sizeMask_) == 0);
  jobQueues_.reserve(innerQueueCount);
  for (int i = 0; i < innerQueueCount; ++i) {
    jobQueues_.emplace_back(std::make_unique<local_job_queue>(128));
  }
}
local_job_queue &job_queue::at(size_t idx) {
  idx &= sizeMask_;
  return *jobQueues_[idx];
}
size_t job_queue::size() const { return sizeMask_ + 1; }

bool local_job_queue::dequeue(job &outVal) {
  std::lock_guard lck{mutex_};
  if (jobQueue_.size() == 0)
    return false;
  outVal = std::move(jobQueue_.front());
  jobQueue_.pop_front();
  return true;
}
local_job_queue::local_job_queue(size_t defaultSize) : mutex_() {
  jobQueue_.resize(defaultSize);
}
} // namespace detail
} // namespace kv_async
