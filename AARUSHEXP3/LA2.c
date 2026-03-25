/*Reverse a steing using stack*/

#include <stdio.h>
#include <string.h>
#define MAX 100
char stack[MAX];
int top = -1;
void push(char item) {
    if (top >= MAX - 1) {
        printf("Stack overflow\n");
    } else {
        stack[++top] = item;
    }
}
void pop() {
    if (top < 0) {
        printf("Stack underflow\n");
    } else {
        printf("%c", stack[top--]);
    }
}
int main() {
    char str[MAX];
    printf("Enter a string: ");
    fgets(str, MAX, stdin);
    int len = strlen(str);
    for (int i = 0; i < len - 1; i++) { // Exclude the newline character
        push(str[i]);
    }
    printf("Reversed string: ");
    while (top >= 0) {
        pop();
    }
    printf("\n");
    return 0;
}
