#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <array>
#include <cmath>
#include <chrono>
#include <iostream>

inline uint_fast32_t pow2(uint_fast32_t x)
{
  //念のため
  //return std::pow(((x == 0) ? 1 : x), 2);
  x = (x == 0) ? 1 : x;
  return x * x;
}

int solve(int **arr, int amounts, int mcoin)
{
  int p2, km, pk, c1;

  for (int price = 0; price < amounts + 1; price++)
  {
    for (int coin = 1; coin < mcoin; coin++)
    {
      p2 = pow2(coin + 1);
      c1 = coin - 1;
      km = price / p2 + 1;

      for (int k = 0; k < km; k++)
      {
        pk = price - k * p2;
        arr[coin][price] += arr[c1][pk];
      }
    }
  }
  return arr[mcoin][100000];
}

int main()
{

  auto start = std::chrono::high_resolution_clock::now();
  auto end = std::chrono::high_resolution_clock::now();
  auto msec = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  std::cout << "n,time" << std::endl;

  //std::vector<std::vector<uint_fast32_t>> vec;

  static int mcoin = 300;
  static int amounts = mcoin * mcoin;
  //std::array<std::array<uint_fast32_t, 1000>, 1001> arr;
  int **arr;
  arr = new int *[mcoin];
  for (int i = 0; i < mcoin; i++)
  {
    arr[i] = new int[amounts + 1];
  }
  for (int i = 0; i < amounts; i++)
  {
    arr[0][i] = 1;
  }

  for (int i = 1; i < mcoin; i++)
  {
    for (int j = 0; j < amounts; j++)
    {
      arr[i][j] = 0;
    }
  }

  //vec.assign(ceil(sqrt(amounts)), std::vector<uint_fast32_t>(amounts + 1, 1));

  for (int i = 1; i < mcoin; i++)
  {
    start = std::chrono::high_resolution_clock::now();
    solve(arr, amounts, i);
    end = std::chrono::high_resolution_clock::now();
    msec = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << i << "," << msec << std::endl;
  }
}
