#include "copyright.h"
#include "system.h"
#include "dllist.h"
#include <stdio.h>

int testnum = 0;
int threadnum = 2;  //线程数目
int oprnum = 5;
bool yield_flag = false;
int S = 0;      // 全局

DLList *dllist = new DLList();

//----------------------------------------------------------------------
// 原版测试
//----------------------------------------------------------------------
void SimpleThread(int which)
{
    int num;
    for (num = 0; num < 5; num++) {
        printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

void ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");
    Thread *t = new Thread("forked thread");
    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

//----------------------------------------------------------------------
// q=1 单线程
//----------------------------------------------------------------------
void DLlist_test1()
{
    printf("===== 单线程：插入 %d 个随机数 =====\n", oprnum);
    for (int i = 0; i < oprnum; i++) {
        int key = Random() % 100;
        printf("插入 key=%d\n", key);
        dllist->SortedInsert(NULL, key);
        dllist->ShowList(PRI);
    }

    printf("===== 开始删除 =====\n");
    for (int i = 0; i < oprnum; i++) {
        int key;
        dllist->Remove(&key);
        printf("删除 key=%d\n", key);
        dllist->ShowList(PRI);
    }
}

//----------------------------------------------------------------------
// q=2 基础乱序
//----------------------------------------------------------------------
void DLlist_test2(int which)
{
    printf("线程 %d 开始\n", which);
    for (int i = 0; i < oprnum; i++) {
        int key = which * 10 + i;
        printf("线程 %d 准备插入 key=%d\n", which, key);

        if (yield_flag) currentThread->Yield();

        dllist->SortedInsert(NULL, key);
        printf("线程 %d 插入完成：", which);
        dllist->ShowList(PRI);
    }
}

void ThreadTest2()
{
    printf("===== 基础乱序测试（预期 10 个元素）=====\n");
    dllist->ShowList(PRI);
    for (int i = 0; i < threadnum; i++) {
        char buf[20];
        sprintf(buf, "t%d", i);
        Thread *t = new Thread(buf);
        t->Fork(DLlist_test2, i);
    }
}


//  并发错误测试 S=1~4（）
// ---------------------- S=1 节点覆盖（实现：每一次完成插入后都showlist，因为后者可以覆盖前者，导致链表中的已经插入的值在链接之前被覆盖（终端出现同位置数值替换效果），最终总个数-1 才对，所以采用随机数效果会好一点。）----------------------
void Test_Cover(int which)
{
    for (int i = 0; i < oprnum; i++) {
        int key = Random() % 50;   // 改为30%以下, 2线程内,小范围随机 → 高概率覆盖
        printf("线程%d 插入 key=%d\n", which, key);

        if (yield_flag)
            currentThread->Yield();

        dllist->SortedInsert(NULL, key);
        printf("线程%d 插入后：", which);
        dllist->ShowList(PRI);
    }
}

// ---------------------- S=2 节点丢失（头插竞争，必然丢失，与上面的区别，是中间过程showlist直接不会出现某些值，list长度每次递增，但最终小于n*t个）----------------------
void Test_Lost(int which)
{
    for (int i = 0; i < oprnum; i++) {
        int key = Random() % 3;   //数据波动更小，极高概率冲突

        printf("线程%d 插入 key=%d\n", which, key);

        if (yield_flag)
            currentThread->Yield();

        dllist->SortedInsert(NULL, key);
        printf("线程%d 插入后：", which);
        dllist->ShowList(PRI);
    }
}

// ---------------------- S=3 环形链表（指针破坏，遍历卡死）----------------------
void Test_Loop(int which)
{
    if (which == 0) {
        dllist->SortedInsert(NULL, 1);
        printf("线程0 插入 1：");
        dllist->ShowList(PRI);

        if (yield_flag)
            currentThread->Yield();

        dllist->SortedInsert(NULL, 3);
        printf("线程0 插入 3：");
        dllist->ShowList(PRI);
    } else {
        dllist->SortedInsert(NULL, 2);
        printf("线程1 插入 2：");
        dllist->ShowList(PRI);
    }
}

// ---------------------- S=4 删除空表（直接崩溃）----------------------
void Test_DeleteEmpty(int which)
{
    if (which == 0) {
        dllist->Append(NULL);
        printf("线程0 添加节点：");
        dllist->ShowList(PRI);

        if (yield_flag)
            currentThread->Yield();

        int key;
        dllist->Remove(&key);
        printf("线程0 删除完成：");
        dllist->ShowList(PRI);
    } else {
        printf("线程%d 尝试删除空表...\n", which);
        int key;
        dllist->Remove(&key);
        printf("线程%d 错误删除！\n", which);
    }
}

// ---------------------- 统一错误入口 ----------------------
void Test_Error(int which)
{
    switch (S) {
        case 1: Test_Cover(which); break;
        case 2: Test_Lost(which); break;
        case 3: Test_Loop(which); break;
        case 4: Test_DeleteEmpty(which); break;
        default: break;
    }
}

void ThreadTest3()
{
    printf("===== 限定并发错误测试 S=%d =====\n", S);
    printf("线程数=%d, 每线程操作次数=%d, 切换开关 y=%d\n", threadnum, oprnum, yield_flag);

    dllist->ShowList(PRI);

    for (int i = 0; i < threadnum; i++) {
        char buf[20];
        sprintf(buf, "err_thread%d", i);
        Thread *t = new Thread(buf);
        t->Fork(Test_Error, i);
    }
}


//----------------------------------------------------------------------
// 主测试入口
//----------------------------------------------------------------------
void ThreadTest()
{
    printf("---- testnum = %d , threadnum = %d , oprnum = %d ----\n", testnum,threadnum,oprnum);

    switch (testnum) {
        case 0: ThreadTest1();      break;
        case 1: DLlist_test1();     break;
        case 2: ThreadTest2();      break;
        case 3: ThreadTest3();      break; // 
        default: printf("No test\n");
    }
}


