//
// Created by Bach Dao.
//

#ifndef KV_ASYNC_CORO_PLATFORM_H
#define KV_ASYNC_CORO_PLATFORM_H
#include "kv_async/platform_definitions.h"

#ifdef KV_CLANG_MACOS
#include <experimental/coroutine>
namespace kv_coro = std::experimental;
#elif defined(KV_GCC)
#include <coroutine>
namespace coroutine = std;
#elif defined(KV_CLANG)
#include <coroutine>
namespace __coroutine = std;
#endif
namespace kv_async {
namespace detail {
namespace coroutine = __coroutine;
}
} // namespace kv_async
#endif // KV_ASYNC_CORO_PLATFORM_H
