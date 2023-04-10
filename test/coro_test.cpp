//
// Created by Bach Dao.
//
#include "thread"
#include <gtest/gtest.h>
#include <kv_async/coroutine/coroutine.h>
using namespace kv_async;
void log_thread_id() {
  std::cout << "thread id: " << std::this_thread::get_id() << std::endl;
}

using namespace std::chrono_literals;
task<std::string> get_string() {
  std::this_thread::sleep_for(1000ms);
  log_thread_id();
  co_return "hello world";
}
task<int> get_int() {
  log_thread_id();
  auto msg = co_await get_string();
  std::cout << msg << std::endl;
  co_return 1;
}
using namespace kv_async;

TEST(TEST_SUITE, TEST_NAME) {
  coro_scope scope;

  scope.with(executor_type::single_thread).launch([]() -> task<int> {
    auto result = co_await get_int();
    std::cout << "result:   " << result << std::endl;
    co_return 1;
  });
}
