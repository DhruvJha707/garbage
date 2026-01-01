// check the minimum value 



#include<stdio.h>

int main(){
    int n ; 
    printf("Enter the size of array : ");
    scanf("%d",&n);

    int arr[n];

    for(int i = 0 ; i<=n-1 ; i++){
        scanf("%d",&arr[i]);
    }

    int min  ;

    for (int i = 0 ; i<=n-1 ; i++){
        if(arr[i]<min){
            min=arr[i];
        }
    }

    printf("The min is : %d",min);

    return 0;
}