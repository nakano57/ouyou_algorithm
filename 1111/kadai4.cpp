#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cmath>
#include <iostream>

uint64_t pow2(uint64_t x)
{
    //念のため
    x = (x == 0) ? 1 : x;
    return x * x;
}

inline float sqrtF(const float &x)
{
    float xHalf = 0.5f * x;
    int tmp = 0x5F3759DF - (*(int *)&x >> 1);
    float xRes = *(float *)&tmp;
    xRes *= (1.5f - (xHalf * xRes * xRes));
    return xRes * x;
}

void setupBoard()
{
    /* 未実装 */
}
int solve(int amounts)
{
    //配列の動的確保が必要そうなのでC++
    std::vector<std::vector<uint64_t>> vec;
    //n creditに対し、1-credit coinだけで支払う方法は必ず1通りなので、配列を1で初期化
    vec.assign(ceil(sqrt(amounts)), std::vector<uint64_t>(amounts + 1, 1));
    //計算回数を減らすため
    int p2;

    //priceは支払わなければいけない金額
    for (int price = 0; price < amounts + 1; price++)
    {
        //1-credit coinだけで支払う方法についてはすでに全てわかっているため、coin = 1からスタート
        for (int coin = 1; coin < sqrt(amounts); coin++)
        {
            //配列を1で初期化したので0を代入
            vec[coin][price] = 0;

            p2 = pow2(coin + 1);

            //pow2(i)はi^2
            //price / pow2(coin + 1)で、pow2(coin + 1)-credit coinの使える枚数の最大が求められる
            //問題文の例なら、289 creditで4-credit coinが使える最大枚数は72枚 (4*72=288 < 289 < 4*73=292)
            //1枚使うとき~k枚使うときnの全ての場合について何通りあるか調べ、その合計を求める
            for (int k = 0; k < (price / p2 + 1); k++)
            {
                //pow2(coin + 1) - credit coinをk枚使ったとき、残り支払わなければいけない金額は
                //price - k * pow2(coin + 1) で求められ、使えるコインの金額はpow2(coin + 1)未満
                //このとき何通りあるかは既知
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

int main(int argc, char *argv[])
{
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

        setupBoard(); /* setupBoard() の呼び出し */
        while (1)
        {
            int amounts, result, ans;
            fscanf(in, "%d", &amounts);
            if (amounts == 0)
                break;
            result = solve(amounts); /* solve() の呼び出し */
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
