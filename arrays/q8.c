// check max size of element in array ;

#include<stdio.h>

int main(){
    int n ; 
    printf("Enter the size of array : ");
    scanf("%d",&n);

    int arr[n];

    for(int i = 0 ; i<=n-1 ; i++){
        scanf("%d",&arr[i]);
    }

    int max = 0 ;

    for (int i = 0 ; i<=n-1 ; i++){
        if(arr[i]>max){
            max=arr[i];
        }
    }

    printf("The max is : %d",max);

    return 0;
}