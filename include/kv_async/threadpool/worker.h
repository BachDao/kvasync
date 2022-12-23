//
// Created by Bach Dao.
//
#ifndef KV_ASYNC_WORKER_H
#define KV_ASYNC_WORKER_H
#include "job_queue.h"
#include "kv_async/forward_declarations.h"
#include "kv_async/threadpool/thread_util.h"
#include <queue>
#include <type_traits>
namespace kv_async {
namespace detail {
class worker {
  friend class ::kv_async::thread_pool;
  std::queue<job> privateQueue_;
  std::queue<job> publicQueue_;
  job_queue &globalJobQueue_;
  thread_pool& threadPool_;
  std::thread workerThread_;
  size_t lastDequeueIdx_ = 0;

  std::condition_variable cv_;
  std::mutex mutex_;
  bool jobAvailable_ = true;
  std::atomic<bool> stopRequested_ = false;
  std::atomic<bool> pendingStopRequest_ = false;
  void do_work();
  void wait_for_job();
  template <typename Fn>
    requires std::is_invocable_v<Fn>
  void private_enqueue(Fn &&fn);
  void drain_private_queue(job &curJob);
  bool steal_job_from_global_queue(job &curJob);

  // depend on value of "pending", worker will try to drain job queue
  // before quit
  void stop(bool pending);

public:
  explicit worker(job_queue &globalJobQueue, thread_pool& tp);
  ~worker();
  void wake_up();
};
template <typename Fn>
  requires std::is_invocable_v<Fn>
void worker::private_enqueue(Fn &&fn) {}
} // namespace detail
} // namespace kv_async
#endif // KV_ASYNC_WORKER_H
