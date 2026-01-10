#include<stdio.h>
int main(){
    int n;
    printf("Enter the size of array : ");
    scanf("%d",&n);

     int arr[n];
     int max = 0;
     for(int i = 0; i<=n-1 ; i++){
        printf("Enter %d element : ",i);
        scanf("%d",&arr[i]);
     }

     for(int i = 0 ; i<=n-1 ; i++){
        if(arr[i]>max){
            max = arr[i];
        }
     }

     printf("The max element in the array is : %d",max);

     return 0;
}