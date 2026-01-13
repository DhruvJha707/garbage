// removing duplicate item from an array 

#include<stdio.h>

int main(){
    int n ;
    printf("Enter the size of array : ");
    scanf("%d",&n);

    int arr[n];
    int found = 0 ;

    for(int i = 0 ; i<=n-1 ; i++){
        scanf("%d",&arr[i]);
    }

    // checking for duplicate 
    for(int i = 0 ; i < n ; i++){
        for(int j = i + 1; j < n ; j++){
            if(arr[i]==arr[j]){
                found = arr[i];
            }
        }
    }
    if(found){
            printf("Duplicate found %d",found);
        }
        if(!found){
            printf("No duplicate found ");
        }
    return 0;
}

