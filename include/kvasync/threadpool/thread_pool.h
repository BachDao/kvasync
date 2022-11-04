#ifndef KVASYNC_THREAD_POOL_H
#define KVASYNC_THREAD_POOL_H
#include "worker.h"
#include <atomic>
#include <thread>
#include <vector>
namespace kvasync {
namespace impl {

class thread_pool {
  std::vector<worker> workers_;
  bool readyFlag_ = false;
  std::atomic_flag stopRequest_;
  std::atomic<int> readyCounter_ = 0;
  size_t numberOfWorker_;
  std::mutex mutex_;
  std::condition_variable cv_;
  blocking_job_queue jobQueue_;
  void worker_notify_ready();
  void waiting_for_workers_ready();
  void start_all_worker();
  void worker_wait();
  friend class worker;

public:
  explicit thread_pool(size_t numOfWorkers = 8);
  ~thread_pool();
  template <typename Fn>
    requires std::is_invocable_v<Fn>
  void submit(Fn &&fn);
  bool is_worker_thread();
  void init_workers(size_t numOfWorker);
  job get_job();
};
template <typename Fn>
  requires std::is_invocable_v<Fn>
void thread_pool::submit(Fn &&fn) {
  if (is_worker_thread()) {
    auto ptrWorker = impl::workerState.ptrCurrentWorker;
    ptrWorker->push_job(std::forward<Fn>(fn));
    return;
  }
  return jobQueue_.push(std::forward<Fn>(fn));
}
} // namespace impl
} // namespace kvasync

#endif // KVASYNC_THREAD_POOL_H
