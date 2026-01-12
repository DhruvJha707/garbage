// Reverse array

#include<stdio.h>

int main(){
    int n,j,i;
    printf("Enter the size of array : ");
    scanf("%d",&n);

    int arr[n];

    for( i = 0 ; i<=n-1 ; i++){
        printf("Enter %d element : ",i);
        scanf("%d",&arr[i]);
    }
    
    for( i = n-1 ; i>=0 ; i--){
            printf("%d ",arr[i]);
    }

    return 0;
}