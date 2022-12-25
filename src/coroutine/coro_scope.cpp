//
// Created by Bach Dao.
//

#include "kv_async/coroutine/coro_scope.h"
#include <iostream>
namespace kv_async {
coro_scope &coro_scope::with(executor_type exeType) {
  exeType_ = exeType;
  return *this;
}
coro_scope::~coro_scope() {
  auto &promise = handle_.promise();
  promise.wait();
  handle_.destroy();
}
namespace detail {

coroutine::suspend_never launch_promise::initial_suspend() noexcept {
  return {};
}
coroutine::suspend_always launch_promise::final_suspend() noexcept {
  return {};
}
void launch_promise::unhandled_exception() {}
void launch_promise::return_void() {}
launch_task launch_promise::get_return_object() {
  return coroutine::coroutine_handle<launch_promise>::from_promise(*this);
}
launch_promise::~launch_promise() {
  std::cout << "~launch_promise()" << std::endl;
}
void launch_promise::resume() {
  std::lock_guard lck{mutex_};
  completed_ = true;
  cv_.notify_all();
}
void launch_promise::wait() {
  std::unique_lock lck{mutex_};
  cv_.wait(lck, [&] { return completed_; });
}

} // namespace detail
} // namespace kv_async