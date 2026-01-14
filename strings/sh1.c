#include <stdio.h>

int main() {
    char str[] = "Dhruv Jha";
    int i;

    for (i = 0; str[i] != '\0'; i++);

    printf("Length of string = %d\n", i);
    return 0;
}
