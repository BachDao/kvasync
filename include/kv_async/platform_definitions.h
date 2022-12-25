//
// Created by Bach Dao.
//

#ifndef KV_ASYNC_PLATFORM_DEFINITIONS_H
#define KV_ASYNC_PLATFORM_DEFINITIONS_H
namespace kv_async {
#if defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_CPU_ARM64
#define KV_CACHE_LINE_SIZE 128
#else
#define KV_CACHE_LINE_SIZE 64
#endif
#else
#define KV_CACHE_LINE_SIZE 64
#endif

#if defined(__clang__) && !defined(__apple_build_version__)
#define KV_CLANG
#elif defined(__clang__) && defined(__apple_build_version__)
#define KV_CLANG_MACOS
#elif defined(__GNUC__) || defined(__GNUG__)
#define KV_GCC
#elif defined(_MSC_VER)
#define KV_MSVC
#endif

#if defined(KV_GCC) || defined(KV_CLANG) || defined KV_CLANG_MACOS
#define KV_FORCE_INLINE inline __attribute__((always_inline))
#elif defined(KV_MSVC)
#define KV_FORCE_INLINE __forceinline
#endif
} // namespace kv_async
#endif // KV_ASYNC_PLATFORM_DEFINITIONS_H
