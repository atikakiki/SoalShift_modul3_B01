#include <stdio.h>
#include <pthread.h>

void *factorial(void *a){
        long long int fact=1,j;
        long long int x= (long long int) a;
                for(j=1;j<=x;j++){
                fact=fact*j;
                }
        printf("%lld! = %lld\n",x, fact);
}

int main() {
        int c=0,i, j, a, num[100];
                char n;
        pthread_t T[100];
        for(i=0;n!='\n';i++){
                scanf ("%d%c", &num[i], &n);
                c++;
        }
                //sorting
                for(i=0;i<c;i++){
                        for(j=i+1;j<c;j++){
                        if(num[i]>num[j]){
                        a=num[i];
                        num[i]=num[j];
                        num[j]=a;
                        }
                }
        }

        for(i=0;i<c;i++){
        long long int m=num[i];
        pthread_create(&T[i], NULL, factorial, (void*) m);
        pthread_join(T[i], NULL);
        }
}
