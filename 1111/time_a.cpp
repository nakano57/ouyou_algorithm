#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cmath>
#include <chrono>
#include <iostream>

inline uint_fast32_t pow2(uint_fast32_t x)
{
  //念のため
  x = (x == 0) ? 1 : x;
  return x * x;
}

inline float sqrtF(const float &x)
{
  float xHalf = 0.5f * x;
  int tmp = 0x5F3759DF - (*(int *)&x >> 1); //initial guess
  float xRes = *(float *)&tmp;

  xRes *= (1.5f - (xHalf * xRes * xRes));
  return xRes * x;
}

int solve(int amounts)
{
  std::vector<std::vector<uint_fast32_t>> vec;
  vec.assign(ceil(sqrtF(amounts)), std::vector<uint_fast32_t>(amounts + 1, 1));
  uint_fast16_t p2;

  for (int price = 0; price < amounts + 1; price++)
  {
    for (int coin = 1; coin < sqrtF(amounts); coin++)
    {
      p2 = pow2(coin + 1);
      vec[coin][price] = 0;

      for (int k = 0; k < (price / p2 + 1); k++)
      {
        vec[coin][price] += vec[coin - 1][price - k * p2];
      }
    }
  }

  //For debug
  // for (auto i = vec.begin(); i < vec.end(); i++)
  // {
  //     for (auto j = i->begin(); j < i->end(); j++)
  //     {
  //         std::cout << *j << ',';
  //     }
  //     std::cout << std::endl;
  // }

  //一番最後の要素を返す
  return *((vec.end() - 1)->end() - 1);
}

int main()
{
  auto start = std::chrono::high_resolution_clock::now();
  auto end = std::chrono::high_resolution_clock::now();
  auto msec = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  std::cout << "n,time" << std::endl;

  for (int i = 1; i < 10000; i++)
  {
    start = std::chrono::high_resolution_clock::now();
    solve(i);
    end = std::chrono::high_resolution_clock::now();
    msec = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << i << "," << msec << std::endl;
  }
}
