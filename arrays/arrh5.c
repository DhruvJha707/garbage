// sum of array 


#include<stdio.h>

int main(){
    int n;
    printf("Enter the size of array : ");
    scanf("%d",&n);

    int arr[n];
    int sum = 0;

    for(int i = 0 ; i<=n-1 ; i++){
        printf("The %d element :",i);
        scanf("%d",&arr[i]);
        sum = sum + arr[i];
    }

    printf("the sum of all the elements in the array is : %d",sum);

    return 0;

}