#include<stdio.h>

int main(){
    int n;
    printf("Enter the size of array : ");
    scanf("%d",&n);

    int arr[n];
    int min;

    for(int i = 0; i<=n-1 ; i++){
        printf("Enter %d element : ",i);
        scanf("%d",&arr[i]);
    }

    for(int i = 0; i<=n-1 ; i++){
        if(arr[i]<min)
        min=arr[i];
    }


    printf("The minimum element in the array : %d",min);

    return 0;

}