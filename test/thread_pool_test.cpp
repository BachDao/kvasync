//
// Created by Bach Dao.
//
#include "kv_async/threadpool/thread_pool.h"
#include <gtest/gtest.h>
#include <iostream>
using namespace kv_async;
TEST(ThreadPool, submitJob) {
  int outVal = 1;
  {
    thread_pool tp(4);
    tp.drain_work_queue_on_exit(true);
    tp.submit([&] {
      outVal = 2;
      std::cout << "done" << std::endl;
    });
  }
  EXPECT_EQ(outVal, 2);
}