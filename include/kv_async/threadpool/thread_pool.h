//
// Created by Bach Dao.
//

#ifndef KV_ASYNC_THREAD_POOL_H
#define KV_ASYNC_THREAD_POOL_H
#include "job_queue.h"
#include "kv_async/threadpool/worker.h"
#include <vector>
namespace kv_async {
class thread_pool {
  friend class detail::worker;
  std::vector<std::unique_ptr<detail::worker>> workers_;
  detail::job_queue globalJobQueue_;
  bool drainQueueOnExit_ = false;
  std::atomic<int> workerDoneCounter_ = 0;
  std::mutex mutex_;
  std::condition_variable cv_;
  size_t workerCount_;

  void worker_report_done();
  void waiting_for_worker_exit();

public:
  explicit thread_pool(size_t workerCount);
  ~thread_pool();
  template <typename Fn> void submit(Fn &&fn);
  void drain_work_queue_on_exit(bool flag);
};
template <typename Fn> void thread_pool::submit(Fn &&fn) {
  // call from worker thread
  if (detail::workerState.valid()) {
    return detail::workerState.ptrCurrentWorker->private_enqueue(
        std::forward<Fn>(fn));
  }

  // call from outside thread
  globalJobQueue_.enqueue(std::forward<Fn>(fn));
}
} // namespace kv_async

#endif // KV_ASYNC_THREAD_POOL_H
