#ifndef KVASYNC_BLOCKING_JOB_QUEUE_H
#define KVASYNC_BLOCKING_JOB_QUEUE_H
#include "job.h"
#include <deque>
#include <mutex>
namespace kvasync {
namespace impl {
class blocking_job_queue {
  mutable std::mutex mutex_;
  std::deque<job> jobQueue_;

public:
  blocking_job_queue() = default;
  blocking_job_queue(const blocking_job_queue& rhs);
  template <typename Fn>
    requires std::is_invocable_v<Fn>
  void push(Fn &&fn);
  job pop();
  job steal();
  size_t size();
};
template <typename Fn>
  requires std::is_invocable_v<Fn>
void blocking_job_queue::push(Fn &&fn) {
  std::lock_guard lck{mutex_};
  jobQueue_.template emplace_back(std::forward<Fn>(fn));
}
} // namespace impl
} // namespace kvasync

#endif // KVASYNC_BLOCKING_JOB_QUEUE_H
