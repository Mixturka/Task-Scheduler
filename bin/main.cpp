#include <iostream>
#include "../src/Scheduler.hpp"
#include "../src/Function.hpp"
#include "../src/Any.hpp"
#include <cmath>

int main() {
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
 auto r = scheduler.getResult<float>(id5);
  std::cout << r << '\n';
  std::cout << scheduler.getResult<float>(id6);
  return 0;
}