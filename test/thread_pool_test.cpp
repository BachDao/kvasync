#include <gtest/gtest.h>
#include "kvasync/threadpool/thread_pool.h"
using namespace kvasync;
using namespace std::chrono_literals;
TEST(TestSuite, TestName){
  impl::thread_pool tp;
  int counter = 1;
  tp.submit([&]{
    std::cout << "hello world" << std::endl;
    counter++;
  });
  std::this_thread::sleep_for(100ms);
  EXPECT_EQ(counter, 2);
}