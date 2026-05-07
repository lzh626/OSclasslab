#include "dllist.h"
#include <stdio.h>
#include "system.h"

extern bool yield_flag;
extern int testnum;

// 节点构造函数
DLLElement::DLLElement(void *itemPtr, int sortKey) {
    prev = NULL;
    next = NULL;
    item = itemPtr;
    key = sortKey;
}

// 链表构造函数
DLList::DLList() {
    first = NULL;
    last = NULL;
}

// 链表析构函数
DLList::~DLList() {
    while (first != NULL) {
        DLLElement *p = first;
        first = first->next;
        delete p;
    }
}

// 尾插
void DLList::Append(void *item) {
    DLLElement *newnode = new DLLElement(item, 1);
    if (IsEmpty()) {
        first = newnode;
        last = newnode;
    } else {
        last->next = newnode;
        newnode->prev = last;
        last = newnode;
    }
}

// 头部删除
void *DLList::Remove(int *keyPtr) {
    DLLElement *element = first;
    void *thing = first->item;

    if (first == last) { // 只有一个节点
        first = NULL;
        last = NULL;
    } else {
        first = element->next;
    }

    if (keyPtr != NULL)
        *keyPtr = element->key;

    delete element;
    return thing;
}

// 判断空表
bool DLList::IsEmpty() {
    return (first == NULL && last == NULL);
}

// 有序插入（核心：支持 Yield 触发并发错误）
void DLList::SortedInsert(void *item, int sortKey) {
    DLLElement *newnode = new DLLElement(item, sortKey);
    DLLElement *ptr;

    if (IsEmpty()) { // 空表直接插入
        first = newnode;
        last = newnode;
    } 
    else if (sortKey < first->key) { // 头部插入
        newnode->next = first;
        // 【并发错误触发点】
        if (yield_flag && testnum == 3)       
            currentThread->Yield();
        
        first->prev = newnode;
        
        // 【并发错误触发点】
        if (yield_flag && testnum == 3)
            currentThread->Yield();

        first = newnode;
    } 
    else { // 中间查找位置插入
        for (ptr = first; ptr->next != NULL; ptr = ptr->next) {
            if (sortKey < ptr->next->key) {
                newnode->next = ptr->next;
                newnode->prev = ptr;
                
                // 【并发错误触发点】
                if (yield_flag && testnum == 3)
                    currentThread->Yield();
                
                ptr->next->prev = newnode;

                // 【并发错误触发点】
                if (yield_flag) currentThread->Yield();         //

                ptr->next = newnode;
                return;
            }
        }

    
        // 尾部插入
        newnode->prev = last;
        
        // 【并发错误触发点】
        if (yield_flag && testnum == 3)
            currentThread->Yield();
        
        last->next = newnode;

        // 【并发错误触发点】
        if (yield_flag && testnum == 3)
            currentThread->Yield();

        last = newnode;
    }
}

// 正确版 SortedRemove：按key删第一个匹配节点，安全处理所有边界
void *DLList::SortedRemove(int sortKey) {
    if (IsEmpty()) { // 空表直接返回NULL
        return NULL;
    }
    DLLElement *ptr = first;
    // 1. 遍历找第一个key匹配的节点
    while (ptr != NULL && ptr->key != sortKey) {
        ptr = ptr->next;
    }
    if (ptr == NULL) { // 没找到
        return NULL;
    }

    void *item = ptr->item; // 保存要返回的数据

    // 2. 摘除节点：分3种位置处理
    if (ptr == first && ptr == last) { // 唯一节点
        first = NULL;
        last = NULL;
    } 
    else if (ptr == first) { // 删表头
        first = ptr->next;
        first->prev = NULL;
    } 
    else if (ptr == last) { // 删表尾
        last = ptr->prev;
        last->next = NULL;
    } 
    else { // 删中间节点
        ptr->prev->next = ptr->next;
        ptr->next->prev = ptr->prev;
    }

    delete ptr; // 释放节点内存
    return item;
}


// 打印链表
void DLList::ShowList(int type) {
    DLLElement *p = first;
    if (IsEmpty()) {
        fprintf(stdout, "NULL\n");
        return;
    }

    if (first == last) {
        if (type == PRI) fprintf(stdout, "# %d #\n", p->key);
        return;
    }

    if (type == PRI) fprintf(stdout, "# %d ", p->key);
    while (p->next != NULL) {
        p = p->next;
        fprintf(stdout, "%d ", p->key);
    }
    if (type == PRI) printf("#\n");
}


