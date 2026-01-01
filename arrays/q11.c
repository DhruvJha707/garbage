#include<stdio.h>
int main(){
    int n ; 
    printf("Enter the size of array :");
    scanf("%d",&n);

    int arr[n];
   
    for(int i = 0 ; i<=n-1 ; i++){
        scanf("%d",&arr[i]);
    }

     int x;
    printf("Enter the through which the divisiblity is checked : ");
    scanf("%d",&x);

    int count = 0;

    for(int i = 0 ; i<=n-1 ; i++){
        if(arr[i]>x){
            count++;
        }
    }
    
    printf("the number divisible by %d are : %d",x,count);

    return 0;
}