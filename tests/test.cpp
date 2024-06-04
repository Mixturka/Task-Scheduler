#include "../src/Scheduler.hpp"
#include <cmath>
#include <gtest/gtest.h>

TEST(SchedulerTest, BasicTest) {
  TTaskScheduler scheduler;
  auto id = scheduler.add([]() { return 23; });
  scheduler.executeAll();

  EXPECT_EQ(23, scheduler.getResult<int>(id));
}

TEST(SchedulerTest, SeveralTasksTest) {
  TTaskScheduler scheduler;
  auto id1 = scheduler.add([]() { return 1 + 1; });
  auto id2 = scheduler.add([]() { return 2 + 2; });
  scheduler.executeAll();

  EXPECT_EQ(2, scheduler.getResult<int>(id1));
  EXPECT_EQ(4, scheduler.getResult<int>(id2));
}

TEST(SchedulerTest, DependencyCorrectnessTest) {
  TTaskScheduler scheduler;
  auto id1 = scheduler.add([]() { return 10; });
  auto id2 = scheduler.add([&scheduler, id1]() { return scheduler.getResult<int>(id1) * 2; });
  scheduler.executeAll();

  EXPECT_EQ(20, scheduler.getResult<int>(id2));
}

TEST(SchedulerTest, FutureResultTest) {
  TTaskScheduler scheduler;
  auto id1 = scheduler.add([]() { return 2; });
  auto id2 = scheduler.add([](int x) { return x * x; }, scheduler.getFutureResult<int>(id1));
  scheduler.executeAll();

  EXPECT_EQ(4, scheduler.getResult<int>(id2));
}

TEST(SchedulerTest, SquareEquation) {
  Function<float(float, float)> f([](float a, float b) { return a * b; });

  TTaskScheduler scheduler;
  float a = 1.f;
  float b = -2.f;
  float c = 0;
  
  auto id1 = scheduler.add([](float a, float c){return -4 * a * c;}, a, c);

  auto id2 = scheduler.add([](float b, float v){return b * b + v;}, b, scheduler.getFutureResult<float>(id1));

  auto id3 = scheduler.add([](float b, float d){return -b + std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

  auto id4 = scheduler.add([](float b, float d){return -b - std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

  auto id5 = scheduler.add([](float a, float v){return v/(2*a);}, a, scheduler.getFutureResult<float>(id3));

  auto id6 = scheduler.add([](float a, float v){return v/(2*a);},a, scheduler.getFutureResult<float>(id4));

  scheduler.executeAll();

  EXPECT_EQ(2, scheduler.getResult<float>(id5));
  EXPECT_EQ(0, scheduler.getResult<float>(id6));
}

TEST(SchedulerTest, SquareEquation2) {
  Function<float(float, float)> f([](float a, float b) { return a * b; });

  TTaskScheduler scheduler;
  float a = 1.f;
  float b = -2.f;
  float c = 0;
  
  auto id1 = scheduler.add([](float a, float c){return -4 * a * c;}, a, c);

  EXPECT_EQ(scheduler.getResult<float>(id1), 0);

  auto id2 = scheduler.add([](float b, float v){return b * b + v;}, b, scheduler.getFutureResult<float>(id1));

  auto id3 = scheduler.add([](float b, float d){return -b + std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

  auto id4 = scheduler.add([](float b, float d){return -b - std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

  auto id5 = scheduler.add([](float a, float v){return v/(2*a);}, a, scheduler.getFutureResult<float>(id3));

  auto id6 = scheduler.add([](float a, float v){return v/(2*a);},a, scheduler.getFutureResult<float>(id4));

  scheduler.executeAll();

  EXPECT_EQ(2, scheduler.getResult<float>(id5));
  EXPECT_EQ(0, scheduler.getResult<float>(id6));
}
