//
// Created by Bach Dao.
//

#ifndef KV_ASYNC_CORO_PROMISE_H
#define KV_ASYNC_CORO_PROMISE_H
#include "kv_async/coroutine/coro_executor.h"
#include "kv_async/coroutine/coro_platform.h"
#include "kv_async/forward_declarations.h"
namespace kv_async {
template <typename T> class task {
  using coro_handle_t =
      detail::coroutine::coroutine_handle<detail::coro_promise<T>>;
  friend class detail::launch_promise;
  friend class detail::promise_base;
  coro_handle_t handle_;

public:
  explicit task(task::coro_handle_t handle);
  task(const task &) = delete;
  task(task &&) = delete;
  ~task();
  bool await_ready();
  void await_suspend(detail::coroutine::coroutine_handle<>);
  T await_resume();
};
template <typename T> task<T>::~task() { handle_.destroy(); }
template <typename T> T task<T>::await_resume() {
  return handle_.promise().get_value();
}
template <typename T>
void task<T>::await_suspend(detail::coroutine::coroutine_handle<>) {}
template <typename T> bool task<T>::await_ready() {
  return handle_.promise().is_completed();
}
template <typename T>
task<T>::task(task::coro_handle_t handle) : handle_(handle) {}

class coro_final_suspend : public detail::coroutine::suspend_always {
public:
  template <typename Promise>
  void
  await_suspend(detail::coroutine::coroutine_handle<Promise> handle) noexcept {
    auto &curPromise = handle.promise();
    if (curPromise.continuation_) {
      curPromise.continuation_();
    }
  }
};

namespace detail {
class promise_base {
  friend class launch_promise;
  friend class kv_async::coro_final_suspend;
  std::function<void()> continuation_ = nullptr;

protected:
  coro_scope *ptrScope_ = nullptr;
  std::shared_ptr<coro_executor> ptrExecutor_ = nullptr;
  coroutine::coroutine_handle<promise_base> callerHandle_ = nullptr;
  promise_base *ptrCallerPromise = nullptr;
  bool isCompleted_ = false;

public:
  promise_base() = default;
  explicit promise_base(coro_scope *ptrScope);
  coroutine::suspend_always initial_suspend();
  coro_final_suspend final_suspend() noexcept;
  template <typename T> task<T> &&await_transform(task<T> &&task);
  void unhandled_exception();
  void resume();
  template <typename Fn>
    requires std::is_invocable_v<Fn>
  void continue_with(Fn &&fn);
  bool is_completed();
};

template <typename T> class coro_promise : public promise_base {
  friend class task<T>;
  std::unique_ptr<std::byte[]> endResult_ = nullptr;

public:
  coro_promise();
  task<T> get_return_object();
  template <typename U>
    requires std::is_convertible_v<U, T>
  void return_value(U &&val);
  const T &get_value();
  // Reference:
  // https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
  size_t next_power_of_two(size_t val) {
    val--;
    val |= val >> 1;
    val |= val >> 2;
    val |= val >> 4;
    val |= val >> 8;
    val |= val >> 16;
    val++;
    return val;
  }
};
template <typename T>
template <typename U>
  requires std::is_convertible_v<U, T>
void coro_promise<T>::return_value(U &&val) {
  new (endResult_.get()) T(std::forward<U>(val));
  isCompleted_ = true;
}
template <typename T> const T &coro_promise<T>::get_value() {
  assert(isCompleted_);
  return *reinterpret_cast<T *>(endResult_.get());
}
template <typename T> coro_promise<T>::coro_promise() {
  auto requestSize = next_power_of_two(sizeof(T));
  auto ptrStorage = new (std::align_val_t(requestSize)) std::byte[requestSize];
  endResult_.reset(ptrStorage);
}

template <> class coro_promise<void> : public promise_base {
  friend class task<void>;

public:
  task<void> get_return_object();
  void return_void();
};

template <typename T> task<T> coro_promise<T>::get_return_object() {
  return task<T>{
      coroutine::coroutine_handle<coro_promise<T>>::from_promise(*this)};
}
template <typename T> task<T> &&promise_base::await_transform(task<T> &&task) {
  auto &calleePromise = task.handle_.promise();
  assert(calleePromise.ptrExecutor_ == nullptr);
  assert(calleePromise.ptrScope_ == nullptr);

  calleePromise.ptrExecutor_ = this->ptrExecutor_;
  calleePromise.ptrScope_ = this->ptrScope_;

  // resume caller when callee return
  auto curCoroHandle =
      coroutine::coroutine_handle<promise_base>::from_promise(*this);
  calleePromise.continue_with([=] {
    auto &promise = curCoroHandle.promise();
    promise.resume();
  });
  calleePromise.resume();
  return std::move(task);
}
template <typename Fn>
  requires std::is_invocable_v<Fn>
void promise_base::continue_with(Fn &&fn) {
  continuation_ = std::forward<Fn>(fn);
}

} // namespace detail
} // namespace kv_async

#endif // KV_ASYNC_CORO_PROMISE_H
