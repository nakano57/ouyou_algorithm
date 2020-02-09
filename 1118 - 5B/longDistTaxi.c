//Suprene
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>

/* For time
#include <chrono>
#include <iostream>
*/

/**********************
 * map の情報
 */
#define MAX_N_ROADS 3000
#define MAX_N_CITIES 2 * MAX_N_ROADS
#define MAX_LENGTH_CITYNAME 15
#define MAX_N_GAS 300

char cityNames[MAX_N_CITIES][MAX_LENGTH_CITYNAME + 1];

typedef struct roadInfo
{
  int num;
  struct
  {
    int dest;
    int dist;
  } roads[MAX_N_ROADS];
} roadinfo_t, *roadinfo_tp;
roadinfo_t roadinfo[MAX_N_CITIES];

bool withGas[MAX_N_CITIES];
int cityNum;

void resetRoadInfo(int target) { roadinfo[target].num = 0; }
void registerRoad(int src, int dest, int dist)
{
  int num = roadinfo[src].num++;
  roadinfo[src].roads[num].dest = dest;
  roadinfo[src].roads[num].dist = dist;
}
int searchOrRegisterCity(char *name)
{
  int i;
  for (i = 0; i < cityNum; i++)
  {
    if (strcmp(cityNames[i], name) == 0)
      return i;
  }
  strcpy(cityNames[cityNum], name);
  resetRoadInfo(cityNum);
  return cityNum++;
}

void setup(FILE *in, int n, int m)
{
  int i;
  fscanf(in, "%s %s", cityNames[0], cityNames[1]);
  cityNum = 2;
  withGas[0] = withGas[1] = true;
  resetRoadInfo(0);
  resetRoadInfo(1);
  for (i = 0; i < n; i++)
  {
    char srcname[MAX_LENGTH_CITYNAME + 1], destname[MAX_LENGTH_CITYNAME + 1];
    int src, dest, dist;
    fscanf(in, "%s %s %d", srcname, destname, &dist);
    src = searchOrRegisterCity(srcname);
    dest = searchOrRegisterCity(destname);
    registerRoad(src, dest, dist);
    registerRoad(dest, src, dist);
  }
  for (i = 2; i < cityNum; i++)
    withGas[i] = false;
  for (i = 0; i < m; i++)
  {
    char cityname[MAX_LENGTH_CITYNAME + 1];
    fscanf(in, "%s", cityname);
    withGas[searchOrRegisterCity(cityname)] = true;
  }
}

typedef struct searchNode
{
  int pathLen;
  int cityID;
} searchNode_t, *searchNode_tp;

void printItem(searchNode_tp elem)
{
  int id = elem->cityID;
  printf("([%d:%s], %d)\n", id, cityNames[id], elem->pathLen);
}

/***********************************************
 * priority queue (汎用のつもり)
 */

#define BUFSIZE 10000
/* ELEM は要素の型を表す。利用用途に合わせて型名を変えてください
 * ちなみに、今回は item_t (= struct myitem) を要素にする */
#define ELEM searchNode_t

/***
 * 比較機(comparator)のプロトタイプ宣言
 * 比較演算はデータ構造に合わせて定義すること
 * a,b: 要素へのポインタ
 * 返り値： aとb が同じ大きさなら0,
 *          [a, b] の順で並べるべきなら負の値、
 *          [b, a] の順で並べるべきなら正の値を返す
 * 例：要素が整数値なら、 return *a - *b; で OK.
 */
int compare(ELEM *a, ELEM *b);

typedef struct priorityQ
{
  int size;
  ELEM buf[BUFSIZE];
} priorityQ_t, *priorityQ_tp;

void reset(priorityQ_tp q)
{
  assert(q != NULL);
  q->size = 0;
}

int qSize(priorityQ_tp q)
{
  assert(q != NULL);
  return q->size;
}

void enqueue(priorityQ_tp q, ELEM data)
{
  assert(q != NULL);
  assert(q->size < BUFSIZE);
  { /* upheap */
    int target = q->size++;
    while (target != 0)
    {
      int parent = (target - 1) / 2;
      ELEM *pref = &q->buf[parent];
      if (compare(pref, &data) > 0)
      {
        q->buf[target] = *pref;
        target = parent;
      }
      else
      {
        break;
      }
    }
    q->buf[target] = data;
  }
}

ELEM dequeue(priorityQ_tp q)
{
  assert(q != NULL);
  assert(q->size > 0);
  {
    ELEM result = q->buf[0];
    q->size--;
    ELEM *moved = &q->buf[q->size];
    { /* downheap */
      int target = 0;
      while (1)
      {
        int child = target * 2 + 1;
        if (child >= q->size)
          break;
        if (child + 1 != q->size &&
            compare(&q->buf[child], &q->buf[child + 1]) > 0)
        {
          child = child + 1;
        }
        if (compare(moved, &q->buf[child]) > 0)
        {
          q->buf[target] = q->buf[child];
          target = child;
        }
        else
        {
          break;
        }
      }
      q->buf[target] = *moved;
      return result;
    }
  }
}
void printQueueInside(priorityQ_tp p)
{
  int i;
  printf("[priorityQ, size: %d, body:{", p->size);
  for (i = 0; i < p->size; i++)
  {
    if (i != 0)
      printf(", ");
    printItem(&p->buf[i]);
  }
  printf("}]\n");
}

/*
 * ここまで、優先度キューのライブラリ
 *************************************/

int compare(searchNode_tp a, searchNode_tp b)
{
  /* まずは、優先度の比較 */
  int ap = a->pathLen;
  int bp = b->pathLen;
  if (ap != bp)
    return ap - bp;
  /* 勝負がつかなかったときは、idの小さい順にしましょう。
   * idが一致することはないって前提 */
  return a->cityID - b->cityID;
}

/*********************************************
 * TODO
 */
priorityQ_t Q;

int solve(int n, int m, int cap)
{

  reset(&Q);

  searchNode_t start = {0, 0};
  searchNode_t next = {0, 0};
  searchNode_t here = {0, 0};

  uint32_t length[MAX_N_CITIES];
  for (int i = 0; i < MAX_N_CITIES; i++)
  {
    length[i] = -1; //経路長が無限の代わりにunsigned intの最大値を利用
  }
  length[0] = 0;        //最初だけ0に
  uint32_t result = -1; //全走査のために用意

  enqueue(&Q, start);

  while (qSize(&Q) > 0)
  {
    here = dequeue(&Q);

    if (here.cityID == 1) //ゴールについたときの処理
    {
      if (result > length[here.cityID])
      {
        result = length[here.cityID];
      }
    }

    for (int i = 0; i < roadinfo[here.cityID].num; i++)
    //全ての道を順に調べていく
    {
      next.cityID = roadinfo[here.cityID].roads[i].dest;
      next.pathLen = roadinfo[here.cityID].roads[i].dist;

      if (length[next.cityID] > (length[here.cityID] + next.pathLen))
      //経路を短くできるなら更新し、enqueueする
      {
        length[next.cityID] = length[here.cityID] + next.pathLen;
        enqueue(&Q, next);
      }
    }
  }

  //経路が見つかった場合、その経路が返される。
  //見つからなかった場合、最初に設定したunsigned intの最大値はintにキャストすると-1なのでそれが返される
  return (int)result;
}

/*******
 * こちらで用意したmain 関数。
 * 問題準備してから、solve() をよび、正解比較もおこなう。
 */
int main(int argc, char *argv[])
{
  /* For time
  auto start = std::chrono::system_clock::now();
  */

  int optionF = 0;
  struct
  {
    char *in, *ans0, *ans;
  } filesets[] = {
      {"./sample.in", "./sample0.ans", "./sample.ans"},
      {"./D.in", "./D0.ans", "./D.ans"}};

  int i;
  for (i = 0; i < 2; i++)
  {
    char *inFile = filesets[i].in;
    char *ansFile = (optionF == 0) ? filesets[i].ans0 : filesets[i].ans;
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
      int n, m, cap, result, ans;
      fscanf(in, "%d %d %d", &n, &m, &cap);
      if (n == 0)
        break;
      setup(in, n, m);
      result = solve(n, m, cap);
      fscanf(ansIn, "%d", &ans);
      /*printf("%d\n", result);*/
      if (result != ans)
      {
        failCount++;
        printf(
            "You failed problem No. %d (result: %d, ans: %d) header %d %d %d\n",
            totalCount, result, ans, n, m, cap);
      }
      totalCount++;
    }
    if (failCount == 0)
    {
      printf("Congratulation! You passed all problems (%d) in %s!\n",
             totalCount, inFile);
    }
    else
    {
      printf("Im sorry you missed %d/%d in %s!\n", failCount, totalCount,
             inFile);
      return 0;
    }
  }

  /* For time
  auto end = std::chrono::system_clock::now();
  auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  std::cout << msec << " ms" << std::endl;
  */

  return 0;
}
