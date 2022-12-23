//
// Created by Bach Dao.
//
#include <gtest/gtest.h>
#include <kv_async/coroutine/coroutine.h>
using namespace kv_async;
task<int> get_int() { co_return 1; }

TEST(TEST_SUITE, TEST_NAME) {}
