#include "Semaphore.hpp"
struct s_semaphore{
    Semaphore* s;
    pthread_mutex_t m;
    int entered;
    FILE* f;
};
void* critical_area(void* x){
    struct s_semaphore* ss = (struct s_semaphore*)x;
    ss->s->down();
    fprintf(ss->f,"Entered critical area\n");
    pthread_mutex_lock(&ss->m);
    ss->entered++;
    fprintf(ss->f,"Currently %d in the area\n",ss->entered);
    pthread_mutex_unlock(&ss->m);
    sleep(3);
    pthread_mutex_lock(&ss->m);
    ss->entered--;
    fprintf(ss->f,"Currently %d in the area\n",ss->entered);
    pthread_mutex_unlock(&ss->m);
    fprintf(ss->f,"exited critical area\n");
    ss->s->up();
    return NULL;
}
void semaphore_as_mutex(){
    struct s_semaphore* ss = new struct s_semaphore;
    pthread_mutex_init(&ss->m,NULL);
    ss->s = new Semaphore(1);
    ss->entered=0;
    ss->f=fopen("sam.txt","w");
    pthread_t t1,t2,t3,t4;
    pthread_create(&t1,NULL,critical_area,(void*)ss);
    pthread_create(&t2,NULL,critical_area,(void*)ss);
    pthread_create(&t3,NULL,critical_area,(void*)ss);
    pthread_create(&t4,NULL,critical_area,(void*)ss);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    pthread_join(t4,NULL);
    fclose(ss->f);
    pthread_mutex_destroy(&ss->m);
    delete ss->s;
    delete ss;
}
void semaphore_5(){
    struct s_semaphore* ss = new struct s_semaphore;
    pthread_mutex_init(&ss->m,NULL);
    ss->s = new Semaphore(5);
    ss->entered=0;
    ss->f=fopen("s5.txt","w");
    pthread_t t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12;
    pthread_create(&t1,NULL,critical_area,(void*)ss);
    pthread_create(&t2,NULL,critical_area,(void*)ss);
    pthread_create(&t3,NULL,critical_area,(void*)ss);
    pthread_create(&t4,NULL,critical_area,(void*)ss);
    pthread_create(&t5,NULL,critical_area,(void*)ss);
    pthread_create(&t6,NULL,critical_area,(void*)ss);
    pthread_create(&t7,NULL,critical_area,(void*)ss);
    pthread_create(&t8,NULL,critical_area,(void*)ss);
    pthread_create(&t9,NULL,critical_area,(void*)ss);
    pthread_create(&t10,NULL,critical_area,(void*)ss);
    pthread_create(&t11,NULL,critical_area,(void*)ss);
    pthread_create(&t12,NULL,critical_area,(void*)ss);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    pthread_join(t4,NULL);
    pthread_join(t5,NULL);
    pthread_join(t6,NULL);
    pthread_join(t8,NULL);
    pthread_join(t9,NULL);
    pthread_join(t10,NULL);
    pthread_join(t11,NULL);
    pthread_join(t12,NULL);
    pthread_join(t7,NULL);
    fclose(ss->f);
    pthread_mutex_destroy(&ss->m);
    delete ss->s;
    delete ss;
}
int main(){
    /*
        Semaphore test
    */
    semaphore_as_mutex();

    semaphore_5();
    return 0;
}
