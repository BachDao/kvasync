#include "kvasync/threadpool/blocking_job_queue.h"

namespace kvasync::impl {
job blocking_job_queue::pop() {
  std::lock_guard lck{mutex_};
  if (jobQueue_.size() > 0) {
    auto retVal = jobQueue_.back();
    jobQueue_.pop_back();
    return retVal;
  }
  return {};
}
job blocking_job_queue::steal() {
  std::lock_guard lck{mutex_};
  if (jobQueue_.size() > 0) {
    auto retVal = jobQueue_.front();
    jobQueue_.pop_front();
    return retVal;
  }
  return {};
}
size_t blocking_job_queue::size() {
  std::lock_guard lck{mutex_};
  return jobQueue_.size();
}
blocking_job_queue::blocking_job_queue(const blocking_job_queue &rhs) {
  std::lock_guard lck{rhs.mutex_};
  jobQueue_ = std::deque<job>(rhs.jobQueue_);
}
} // namespace kvasync::impl