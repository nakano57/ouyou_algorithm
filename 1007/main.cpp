#include <stdio.h>
#include <iostream>
#include <vector>

#define BUFSIZE 100

typedef struct node
{
    struct node *s;
    struct node *t;
    int id;
    int visited;
} node_t, *node_tp;

void printNode(node_tp node, std::vector<int> v)
{
    std::cout << "(" << node->id << "," << node->visited << ") #";
    std::cout << "stack";

    for (int x : v)
    {
        std::cout << ":" << x;
    }
    std::cout << std::endl;

    //printf("(%d, %d)\n", node->id, node->visited);
}

node_t nodes[BUFSIZE]; /* 大域変数（配列） */

void initNodes(int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        nodes[i].id = i;
        nodes[i].visited = 0;
        nodes[i].s = nodes[i].t = NULL;
    }
}

void link(node_tp node, node_tp s, node_tp t)
{
    node->s = s;
    node->t = t;
}

void dfs(node_tp node, std::vector<int> v)
{
    node->visited++;
    v.push_back(node->id);
    printNode(node, v);

    if (node->visited == 1)
    {
        node_tp s = node->s;
        node_tp t = node->t;

        if (s != NULL)
        {
            dfs(s, v);
        }

        if (t != NULL)
        {
            dfs(t, v);
        }
    }
}

void test1(void)
{
    std::vector<int> v;
    initNodes(4);
    link(&nodes[0], &nodes[3], &nodes[1]);
    link(&nodes[1], &nodes[3], &nodes[2]);
    link(&nodes[2], &nodes[0], NULL);
    link(&nodes[3], NULL, &nodes[2]);
    dfs(&nodes[0], v); /* ノード 0 から探索 */
}

void test2(void)
{
    int i;
    std::vector<int> v;

    initNodes(12);
    for (i = 0; i < 10; i++)
    {
        link(&nodes[i], &nodes[i + 1], &nodes[i + 2]);
    }

    dfs(&nodes[0], v); /* ノード 0 から探索 */
}

int main(void)
{
    test1();
    /* test2(); */
    return 0;
}
