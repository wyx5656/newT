#include <stdio.h>
#include <stdlib.h>
typedef struct node {
    int data;
    struct node *next;
} Node;

Node* createNode(int data)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    
    return newNode;
}

void addBeforeHead(Node** head, int data)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = *head;
    *head = newNode;
}

void addAfterTail(Node** head, int data)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    Node* cur = *head;
    while(cur->next != NULL)
    {
        cur = cur->next;
    }
    cur->next = newNode;
    newNode->next = NULL;
}


void printList(Node* head)
{
    Node* cur = head;
    while(cur != NULL)
    {
        printf("%d ", cur->data);
        cur = cur->next;
    }
    printf("\n");
}

void insertAfterNode(Node* node, int data)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = node->next;
    node->next = newNode;
}

void deleteNode(Node** head, Node* nodeToDelete)
{   
    Node* prev = *head;
    if (NULL == *head || NULL == nodeToDelete)
    {
        return;
    }

    if(*head == nodeToDelete)
    {
        *head = (*head)->next;
        free(nodeToDelete);
        return;
    }

    while(prev->next != nodeToDelete)
    {
        prev = prev->next;
    }

    if (prev != NULL)
    {
        prev->next = nodeToDelete->next;
        free(nodeToDelete);
    }
}

int main(void)
{
    //1、创建新链表
    Node* head = createNode(1);
    //2、头插法
    addBeforeHead(&head, 2);
    //3、尾插法
    addAfterTail(&head, 3);
    //4、打印链表  
    printf("打印链表：");
    printList(head);
    //5、在某个节点后插入节点
    insertAfterNode(head->next, 4);
    printf("打印链表：");
    printList(head);
    //6、删除某个节点
    deleteNode(&head, head->next->next);
    printf("打印链表：");
    printList(head);

}