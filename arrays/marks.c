#include<stdio.h>
int main(){
    
    int marks[10]  = {95,46,58,33,45,15,67,8,4,2};

    for(int i = 0 ; i<=10 ; i++ ){
    
        if (marks[i]<35){
               printf("%d \n",i);
        }
    }

    return 0;

}