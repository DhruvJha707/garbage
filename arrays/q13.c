#include<stdio.h>

int main(){
    int n ; 
    printf("enter the size of array : ");
    scanf("%d",&n);

    int totalpairs = 0;
    int arr[n];
    int x = 12;


    for(int i = 0 ; i<=n-1 ; i++){
        scanf("%d",&arr[i]);
    }

    for(int i = 0 ; i<=n-1 ; i++){
        for(int j = i+1 ; j<=n-1 ;j++ ){
            if(arr[i] + arr[j] == x){
                totalpairs++;
                printf("(%d,%d)\n",arr[i],arr[j]);
            }
        }
    }

    printf("total pairs are : %d",totalpairs);

    return 0;
}