//
// Created by Bach Dao.
//

#ifndef KV_ASYNC_CORO_SCOPE_H
#define KV_ASYNC_CORO_SCOPE_H
#include "kv_async/coroutine/coro_executor.h"
#include "kv_async/coroutine/coro_promise.h"
#include <functional>

namespace kv_async {
namespace detail {

template <typename T> struct is_task_t : public std::false_type {};
template <typename T> struct is_task_t<task<T>> : public std::true_type {};
template <typename T> static constexpr bool is_task_v = is_task_t<T>::value;
struct launch_task {
  using handle_t = coroutine::coroutine_handle<launch_promise>;
  coroutine::coroutine_handle<launch_promise> handle_;
  launch_task(handle_t handle) : handle_(handle) {}
  launch_task(const launch_task &) = default;
};

struct launch_promise {
  executor_type exeType_;
  ::kv_async::coro_scope *ptrScope_ = nullptr;

  bool completed_ = false;
  std::mutex mutex_;
  std::condition_variable cv_;

  template <typename Fn>
  launch_promise(coro_scope &ptrScope, executor_type exeType, Fn &&);
  ~launch_promise();
  coroutine::suspend_never initial_suspend() noexcept;
  coroutine::suspend_always final_suspend() noexcept;
  void unhandled_exception();
  void return_void();
  launch_task get_return_object();
  void wait();
  template <typename T> task<T> &&await_transform(task<T> &&task);
  void resume();
};
template <typename Fn>
launch_promise::launch_promise(coro_scope &ptrScope, executor_type exeType,
                               Fn &&)
    : exeType_(exeType), ptrScope_(&ptrScope) {}
template <typename T>
task<T> &&launch_promise::await_transform(task<T> &&task) {
  auto &calleePromise = task.handle_.promise();

  assert(calleePromise.ptrExecutor_ == nullptr);
  assert(calleePromise.ptrScope_ == nullptr);

  // root scope object
  calleePromise.ptrScope_ = ptrScope_;

  // set executor_type
  switch (exeType_) {
  case executor_type::thread_pool:
    calleePromise.ptrExecutor_ = std::make_shared<thread_pool_executor>();
    break;
  case executor_type::single_thread:
    calleePromise.ptrExecutor_ = std::make_shared<single_thread_executor>();
    break;
  default:
    throw std::logic_error("invalid executor_type type");
  }
  auto curCoroHandle =
      coroutine::coroutine_handle<launch_promise>::from_promise(*this);
  calleePromise.continue_with([=] {
    auto &promise = curCoroHandle.promise();
    promise.resume();
  });
  calleePromise.resume();
  return std::move(task);
}
} // namespace detail

class coro_scope {
  executor_type exeType_ = executor_type::thread_pool;
  detail::coroutine::coroutine_handle<detail::launch_promise> handle_;
  template <typename Fn>
  detail::launch_task launch_bridge(executor_type exeType, Fn &&fn);

public:
  coro_scope() = default;
  ~coro_scope();
  coro_scope &with(executor_type exeType);
  template <typename Fn>
    requires detail::is_task_v<std::invoke_result_t<Fn>>
  void launch(Fn &&fn);
};
template <typename Fn>
  requires detail::is_task_v<std::invoke_result_t<Fn>>
void coro_scope::launch(Fn &&fn) {
  auto launchTask = launch_bridge(exeType_, std::forward<Fn>(fn));
  handle_ = launchTask.handle_;
  exeType_ = executor_type::thread_pool;
  return;
}
template <typename Fn>
detail::launch_task coro_scope::launch_bridge(executor_type exeType, Fn &&fn) {
  co_await std::invoke(fn);
}
} // namespace kv_async

#endif // KV_ASYNC_CORO_SCOPE_H
