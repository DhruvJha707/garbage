#include<stdio.h>

int main(){
    int n ; 
    printf("enter the size of array : ");
    scanf("%d",&n);

    int totaltriplets = 0;
    int arr[n];
    int x = 12;


    for(int i = 0 ; i<=n-1 ; i++){
        scanf("%d",&arr[i]);
    }

    for(int i = 0 ; i<=n-1 ; i++){
        for(int j = i+1 ; j<=n-1 ;j++ ){
            for(int k = j+1 ; k<=n-1 ; k++){
            if(arr[i] + arr[j] + arr[k ]== x){
                totaltriplets++;
                printf("(%d,%d,%d)\n",arr[i],arr[j],arr[k]);
            }
        }
    }
}

    printf("total pairs are : %d",totaltriplets);

    return 0;
}