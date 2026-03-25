/*1.	Using array and functions implement Stack and its operations like insert, delete, and display.*/

#include <stdio.h>
#define MAX 100
int stack[MAX];
int top = -1;

void push(int item) {
    if (top >= MAX - 1) {
        printf("Stack overflow\n");
    } else {
        stack[++top] = item;
        printf("Inserted: %d\n", item);
    }
}

void pop() {
    if (top < 0) {
        printf("Stack underflow\n");
    } else {
        printf("Deleted: %d\n", stack[top--]);
    }
}

void display() {
    if (top < 0) {
        printf("Stack is empty\n");
    } else {
        printf("Stack elements: ");
        for (int i = 0; i <= top; i++) {
            printf("%d ", stack[i]);
        }
        printf("\n");
    }
}
int main() {
    int choice, item;
    while (1) {
        printf("1. Push\n2. Pop\n3. Display\n4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                printf("Enter the item to push: ");
                scanf("%d", &item);
                push(item);
                break;
            case 2:
                pop();
                break;
            case 3:
                display();
                break;
            case 4:
                return 0;
            default:
                printf("Invalid choice\n");
        }
    }
}
