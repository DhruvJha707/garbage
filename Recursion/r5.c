// both increasing and decreasing in single variable recusrive call

#include<stdio.h>

void both(int n){
    if(n==0) return; // base case
    printf("%d\n",n);
    both(n-1);
    printf("%d\n",n);
    return ; 
}

int main(){
    int n ;
    printf("Enter a number : ");
    scanf("%d",&n);
    both(n);
    return 0;
}