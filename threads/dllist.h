#ifndef __DLLIST_H
#define __DLLIST_H

#define INS 0
#define DEL 1
#define PRI 2

class DLLElement {
public:
    DLLElement(void *itemPtr, int sortKey);
    DLLElement *next;    // 后继指针
    DLLElement *prev;    // 前驱指针
    int key;             // 排序关键字
    void *item;          // 数据指针
};

class DLList {
public:
    DLList();                         // 构造函数
    ~DLList();                        // 析构函数
    void Append(void *item);          // 尾插
    void *Remove(int *keyPtr);        // 头删
    void *SortedRemove(int sortKey);   //有序链表的按值删除
    bool IsEmpty();                   // 判断空表
    void SortedInsert(void *item, int sortKey); // 有序插入
    void ShowList(int type);          // 打印链表

private:
    DLLElement *first; // 链表头
    DLLElement *last;  // 链表尾
};

// 工具函数
void InsertItem(int which, DLList *dllist, int key);
void RemoveItem(int which, DLList *dllist, int key);
void PrintList(int which, DLList *dllist);

#endif

