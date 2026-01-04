#include<stdio.h>

int main(){

    int arr[6] = {1,2,3,4,5,6};

    int max = arr[0];

    for(int i = 1 ; i<=6 ; i++){
        if(max<arr[i]){
            max = arr[i];
        }
    }


    printf("max %d",max);
    
    return 0;
}