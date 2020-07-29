/*
Group:33
2016A7PS0036P Megh Thakkar
2016A7PS0103P Sahil Singla
2016A7PS0110P Sankalp Sangle
2016A7PS0150P Patel Parth
*/

#include "stack.h"
#include "parser.h"

Stack* initializeStack() {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    stack -> bottom = NULL;
    stack -> top = NULL;
    stack -> size = 0;
    return stack;
}

StackNode* createStackNode(stackData* stack_data) {
    StackNode* stack_node = (StackNode*)malloc(sizeof(StackNode));
    stack_node -> stack_data = stack_data;
    stack_node -> next = NULL;
    return stack_node;
}

StackNode* pop(Stack* stack) {
    if (stack -> size == 0) {
        return NULL;
    }
    StackNode* temp = stack -> top;
    stack -> top = stack -> top -> next;
    stack -> size --;
    return temp;
}

StackNode* top(Stack* stack) {
    return stack -> top;
}

void push(Stack* stack, stackData* stack_data) {
    StackNode* new_node = createStackNode(stack_data);
    if (stack -> size == 0) {
        stack -> top = new_node;
        stack -> bottom = new_node;
        stack -> size++;
    } else {
        new_node -> next = stack -> top;
        stack -> top = new_node;
        stack -> size++;
    }
}

void pushAllChildrenOnStack(Stack* stack, stackData* stack_data) {
    if (stack_data->nextSibling!=NULL)
    {
        pushAllChildrenOnStack(stack,stack_data->nextSibling);
    }
    push(stack,stack_data);
}


void printStack(Stack* stack) {
    StackNode* temp = stack->top;
    while(temp!=NULL)
    {
        printf("%d\n",temp->stack_data->isTerminal);
        temp = temp->next;
    }
}