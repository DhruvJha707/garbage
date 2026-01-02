// find the difference between the sum of even indices and sum of odd indices

#include<stdio.h>

int main(){

    int n;
    printf("Enter the size of array : ");
    scanf("%d",&n);

    int arr[n];

    for(int i = 0 ; i<=n-1 ; i++){
        scanf("%d",&arr[i]);
    }

    int oddsum = 0;
    int evensum = 0;

    for(int i = 0 ; i<=n-1 ; i++){
        if(i%2!=0){
            oddsum = oddsum + arr[i];
        }
        else{
            evensum = evensum + arr[i];
        }
    }

    printf("sum of odd indices is : %d\n",oddsum);
    printf("sum of even indices is : %d\n",evensum);

    int diff = evensum - oddsum;
    printf("The diffrence is : %d",diff);

    return 0;
}