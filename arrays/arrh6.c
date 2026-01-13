// second largest element in the array 

#include<stdio.h>

int main(){
    int n;
    printf("enter the size of array : ");
    scanf("%d",&n);

    int arr[n];
    int max = 0;
    int smax = 0;
    for(int i = 0 ; i<=n-1 ; i++){
        printf("Enter %d element : ",i);
        scanf("%d",&arr[i]);
    }

    for(int i = 0 ; i<=n-1 ; i++){
        if(arr[i]>max){
            max = arr[i];
        }
    }


    for(int i = 0 ; i<=n-1 ; i++){
        if(smax!=max && arr[i]<max){
            smax = arr[i];
        }
    }
    printf("The maximum value and the second largest value in the array is : %d,%d",max,smax);

    return 0;

}