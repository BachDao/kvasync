#ifndef KVASYNC_WORKER_H
#define KVASYNC_WORKER_H
#include "blocking_job_queue.h"
#include "kvasync/forward_declarations.h"
#include "thread_util.h"
#include <thread>
namespace kvasync {
namespace impl {

class worker {
  thread_pool &threadPool_;
  blocking_job_queue jobQueue_;
  std::thread worker_;
  void init_worker_state();
  void do_work();
  void waiting_for_start_signal();
  void drain_local_job_queue();
  job steal_global_job();
  job steal_another_worker_job();
  void sleep_if_necessary();
  bool stop_requested();
  void notify_ready();
  friend class thread_pool;

public:
  explicit worker(thread_pool &tp);
  template <typename Fn>
    requires std::is_invocable_v<Fn>
  void push_job(Fn &&fn);
};
template <typename Fn>
  requires std::is_invocable_v<Fn>
void worker::push_job(Fn &&fn) {
  jobQueue_.push(std::forward<Fn>(fn));
}
} // namespace impl
} // namespace kvasync

#endif // KVASYNC_WORKER_H
