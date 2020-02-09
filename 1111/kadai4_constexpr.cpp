/*****
 Apple clang version 11.0.0 (clang-1100.0.33.8)
Target: x86_64-apple-darwin19.2.0
Thread model: posix
 *****/
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cmath>
#include <iostream>
#include <array>

//全体を構造体で囲うのがコツ
struct setupBoard
{
    //constexprにより配列をコンパイル時に計算
    constexpr setupBoard() : arr()
    {
        for (auto &&a : arr)
        {
            //fillのconstexprはまだコンパイラが非対応だった
            //a.fill(1);

            for (auto &&b : a)
            {
                b = 1;
            }
        }
        int p2 = 0;

        for (int price = 0; price < 301; price++)
        {
            for (int coin = 1; coin < 18; coin++)
            {
                arr.at(coin).at(price) = 0;
                p2 = (coin + 1) * (coin + 1);
                for (int k = 0; k < (price / p2 + 1); k++)
                {
                    arr.at(coin).at(price) += arr.at(coin - 1).at(price - k * p2);
                }
            }
        }
    }
    std::array<std::array<int, 301>, 18> arr;
};

int solve(setupBoard b, int amounts)
{
    //オーダーはO(1)
    return b.arr[floor(sqrt(amounts))][amounts];
}

int main(int argc, char *argv[])
{
    //setupBoard(); /* setupBoard() の呼び出し */
    constexpr auto board = setupBoard();

    struct
    {
        char *in, *ans;
    } filepairs[] = {{"./sample.in", "./sample.ans"}};

    int i;
    for (i = 0; i < 1; i++)
    {
        char *inFile = filepairs[i].in;
        char *ansFile = filepairs[i].ans;
        FILE *in = fopen(inFile, "r");
        FILE *ansIn = fopen(ansFile, "r");
        if (in == NULL)
        {
            printf("Can't open file: %s.\n", inFile);
            return 0;
        }
        if (ansIn == NULL)
        {
            printf("Can't open file: %s.\n", ansFile);
            return 0;
        }
        int failCount = 0;
        int totalCount = 0;
        printf("Processing input: %s\n", inFile);

        while (1)
        {
            int amounts, result, ans;
            fscanf(in, "%d", &amounts);
            if (amounts == 0)
                break;
            result = solve(board, amounts); /* solve() の呼び出し */
            fscanf(ansIn, "%d", &ans);
            if (ans != result)
            {
                failCount++;
                printf("! You failed data No. %d (result: %d, ans: %d)\n",
                       totalCount, result, ans);
            }
            totalCount++;
        }
        if (failCount == 0)
        {
            printf("!! Congratulation! You passed all data (%d) in %s!\n",
                   totalCount, inFile);
        }
        else
        {
            printf("! Im sorry you missed %d/%d in %s!\n", failCount,
                   totalCount, inFile);
            return 0;
        }
    }
    return 0;
}
