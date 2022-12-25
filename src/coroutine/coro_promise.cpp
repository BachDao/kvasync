//
// Created by Bach Dao.
//

#include "kv_async/coroutine/coro_promise.h"
#include "kv_async/coroutine/coro_executor.h"
#include <cassert>
namespace kv_async {
namespace detail {
void promise_base::resume() {
  auto handle = coroutine::coroutine_handle<promise_base>::from_promise(*this);
  assert(handle);
  assert(ptrExecutor_);
  ptrExecutor_->submit([=] { handle.resume(); });
}
bool promise_base::is_completed() { return isCompleted_; }
coro_final_suspend promise_base::final_suspend() noexcept { return {}; }
coroutine::suspend_always promise_base::initial_suspend() { return {}; }
promise_base::promise_base(coro_scope *ptrScope) : ptrScope_(ptrScope) {}
void promise_base::unhandled_exception() {}

} // namespace detail
} // namespace kv_async