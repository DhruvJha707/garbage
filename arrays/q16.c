// to find the second largest element in an array

#include<stdio.h>


int main(){

    int arr[8] = {1,2,3,4,5,6,7,8};

    int max = arr[0];

    for(int i = 1 ; i<=7 ; i++){
        if(max<arr[i]){
            max = arr[i];
        }
    }

    int smax = 0;

    for(int i = 0 ; i<=7; i++){
        if(smax<arr[i] && arr[i]!=max){
            smax = arr[i];
        }
    }



    printf("%d\n",smax);

    return 0;

}
