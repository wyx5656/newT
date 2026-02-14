#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    int data;
    struct node* next;
}Node;

Node* addNode(Node* head, int data)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (NULL == newNode)
    {
        printf("malloc fail\n");
        exit(1);
    }

    newNode->data = data;
    newNode->next = head;

    return newNode;
}

void addNodePoint(Node** head, int data)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    if(NULL == newNode)
    {
        printf("malloc fail");
        exit(1);
    }

    newNode->data = data;
    newNode->next = *head;
    *head = newNode;
}

int main(void)
{
    Node* head = NULL;
    Node* p = NULL;
    head = addNode(head, 3);
    head = addNode(head, 2);
    head = addNode(head, 1);
    addNodePoint(&head, 4);
    p = head;
    while(p != NULL)
    {
        printf("point: %d\n", p->data);
        p = p->next;
    }

    return 0;
}