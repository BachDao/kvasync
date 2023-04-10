//
// Created by Bach Dao.
//

#ifndef KV_ASYNC_CORO_EXECUTOR_H
#define KV_ASYNC_CORO_EXECUTOR_H
#include "kv_async/threadpool/thread_pool.h"

namespace kv_async {
enum class executor_type { thread_pool, single_thread };
static constexpr size_t THREAD_POOL_COUNT = 4;
namespace detail {
class coro_executor {

public:
  virtual ~coro_executor();
  virtual void submit(work w) = 0;
};

class thread_pool_executor final : public coro_executor {
  static kv_async::thread_pool threadPool_;

public:
  thread_pool_executor() = default;
  ~thread_pool_executor() override;
  void submit(work w) override;
};

class single_thread_executor final : public coro_executor {
  std::thread thread_;
  std::mutex mutex_;
  std::condition_variable cv_;
  void do_work();
  std::deque<work> jobQueue_;
  std::atomic<bool> stopRequest_ = false;

public:
  single_thread_executor();
  ~single_thread_executor() override;
  void submit(work w) override;
};

} // namespace detail
} // namespace kv_async

#endif // KV_ASYNC_CORO_EXECUTOR_H
