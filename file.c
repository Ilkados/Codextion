#include <stdio.h>
#include <pthread.h>

int x = 0;

void*increment(void*arg) {
for (int i = 0; i < 100000 ; i++) {
x++;// Increment shared variable
    }
return NULL;
}

int main() {
pthread_t t1 ,t2;

// Create two threads that will both increment the shared variable 'x'
pthread_create(&t1,NULL,increment,NULL);
pthread_create(&t2,NULL,increment,NULL);

// Wait for both threads to finish
pthread_join(t1,NULL);
pthread_join(t2,NULL);

// Print the final value of 'x'
printf("Final value of x: %d\n",x);

return 0;
}