//
// Created by Bach Dao.
//
#include "kv_async/threadpool/hash_table.h"
#include "kv_async/threadpool/thread_pool.h"
#include <gtest/gtest.h>

using namespace kv_async;
TEST(hashtable, put_then_get) {
  std::atomic<int> outVal = 0;
  {
    thread_pool tp(4);
    tp.drain_work_queue_on_exit(false);
    tp.submit([&] { outVal.fetch_add(1); });
    tp.submit([&] { outVal.fetch_add(1); });
    tp.submit([&] { outVal.fetch_add(1); });
    tp.submit([&] { outVal.fetch_add(1); });
    tp.submit([&] { outVal.fetch_add(1); });
  }
  EXPECT_EQ(outVal, 5);
}