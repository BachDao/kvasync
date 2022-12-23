//
// Created by Bach Dao.
//

#ifndef KV_ASYNC_CORO_PROMISE_H
#define KV_ASYNC_CORO_PROMISE_H
// #include "kv_async/coroutine/coro_defs.h"
#include "kv_async/coroutine/coro_executor.h"
#include "kv_async/coroutine/coro_scope.h"
#include "kv_async/coroutine/task.h"
#include <experimental/coroutine>
namespace kv_async {
namespace detail {
class promise_base {
  coro_scope *ptrScope_ = nullptr;
  coro_executor *ptrExecutor_ = nullptr;
  coroutine::coroutine_handle<void> continuation_ = nullptr;

public:
  promise_base(coro_scope *ptrScope, coro_executor *ptrExe);
  coroutine::suspend_always initial_suspend();
  coroutine::suspend_always final_suspend() noexcept;
  void unhandled_exception();
  void resume();
  void continue_with(coroutine::coroutine_handle<void> handle);
};
template <typename T> class coro_promise {

public:
  task<T> get_return_object();
};

template <> class coro_promise<void> {
public:
  task<void> get_return_object();
};

template <typename T> task<T> coro_promise<T>::get_return_object() {
  return {*this};
}

} // namespace detail
} // namespace kv_async

#endif // KV_ASYNC_CORO_PROMISE_H
