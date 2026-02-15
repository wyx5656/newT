#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
//链表节点
typedef struct node{
    int data;
    struct node *next;
}Node;

//栈节点
typedef struct stack{
    Node *top;
}Stack;

void InitStack(Stack *stack)
{
    stack->top = NULL;
}

bool IsEmpty(Stack *stack)
{
    return NULL == stack->top;
}

void StackPush(Stack *stack, int data)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = stack->top;
    stack->top = newNode;
}

void StackPop(Stack *stack, int *data)
{
    if(IsEmpty(stack))
    {
        printf("栈为空\n");
        return;
    }
    Node *node = stack->top;
    *data = node->data;
    stack->top = node->next;
    free(node);
}

void StackPeek(Stack *stack, int *data)
{
    if(IsEmpty(stack))
    {
        printf("栈为空\n");
        return;
    }

    *data = stack->top->data;
}

void StackDestroy(Stack *stack)
{
    Node *node = stack->top;
    while(!IsEmpty(stack))
    {
        int dummy;
        StackPop(stack, &dummy);
    }
}

void printStack(Stack *stack)
{ 
    if(IsEmpty(stack))
    {
        printf("栈为空\n");
        return;
    }
    
    printf("栈元素为：");
    Node* currrnode = stack->top;
    while(currrnode != NULL)
    {
        printf("%d ", currrnode->data);
        currrnode = currrnode->next;
    }
    printf("\n");
}

int main() {
    Stack s;
    InitStack(&s);

    StackPush(&s, 10);
    StackPush(&s, 20);
    StackPush(&s, 30);

    printStack(&s);     // 30 20 10

    int top_val;
    StackPeek(&s, &top_val);
    printf("栈顶：%d\n", top_val);  // 30

    int popped;
    StackPop(&s, &popped);
    printf("出栈：%d\n", popped);   // 30

    printStack(&s);     // 20 10

    StackDestroy(&s);   // 清理内存
    return 0;
}