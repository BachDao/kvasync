//
// Created by Bach Dao.
//

#include "kv_async/coroutine/coro_promise.h"

namespace kv_async {
namespace detail {
void promise_base::resume() {
  auto handle = coroutine::coroutine_handle<promise_base>::from_promise(*this);
  ptrExecutor_->submit([&] { handle.resume(); });
}
void promise_base::continue_with(coroutine::coroutine_handle<void> handle) {
  continuation_ = handle;
}
} // namespace detail
} // namespace kv_async