// product of size of array 

#include<stdio.h>

int main(){
    int n;
    printf("enter the size of array : ");
    scanf("%d",&n);

    int arr[n];

    for(int i = 0 ; i<=n-1 ; i++){
        scanf("%d",&arr[i]);
    }

    int product = 1;

    for(int i = 0 ; i<=n-1 ; i++){
        product = product*arr[i];
    }

    printf("the product of the size of array is %d",product);

    return 0;
}