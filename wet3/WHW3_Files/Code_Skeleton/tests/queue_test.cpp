#include "PCQueue.hpp"
struct check{
    int index;
    int inserted;
    int num[100];
    PCQueue<int>* q;
    pthread_mutex_t m;
};
void* get(void* i){
    struct check* c = ((struct check*)i);
    pthread_mutex_lock(&c->m);
    c->num[c->index]=c->q->pop();
    c->index++;
    pthread_mutex_unlock(&c->m);
    return NULL;
}
void* get1(void* i){
    sleep(1);
    struct check* c = ((struct check*)i);
    pthread_mutex_lock(&c->m);
    c->num[c->index]=c->q->pop();
    c->index++;
    pthread_mutex_unlock(&c->m);
    return NULL;
}
void* add1(void* i){
    struct check* c = ((struct check*)i);
    for(int j=1;j<=11;j++){
        sleep(1);
         c->q->push(j);
    }
        return NULL;
}
void* add2(void* i){
    struct check* c = ((struct check*)i);
    for(int j=1;j<=15;j++){
        sleep(1);
         c->q->push(j);
    }
        return NULL;
}
void* add4(void* i){
    sleep(5);
    struct check* c = ((struct check*)i);
    for(int j=1;j<=11;j++){
        sleep(1);
         c->q->push(j);
    }
        return NULL;
}
void same(){
    PCQueue<int>* q = new PCQueue<int>;
    pthread_t t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13;
    struct check* c1 = new struct check;
    c1->q=q;
    c1->index=0;
    c1->inserted=0;
    pthread_mutex_init(&c1->m,NULL);
    pthread_create(&t2,NULL,get,(void*)c1);
    pthread_create(&t3,NULL,get,(void*)c1);
    pthread_create(&t8,NULL,get,(void*)c1);
     pthread_create(&t1,NULL,add1,(void*)c1);
    pthread_create(&t9,NULL,get,(void*)c1);
    pthread_create(&t10,NULL,get,(void*)c1);
    pthread_create(&t11,NULL,get,(void*)c1);
    pthread_create(&t4,NULL,get,(void*)c1);
    pthread_create(&t5,NULL,get,(void*)c1);
    pthread_create(&t6,NULL,get,(void*)c1);
    pthread_create(&t12,NULL,get,(void*)c1);
    pthread_create(&t13,NULL,get,(void*)c1);
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
    pthread_join(t13,NULL);
    FILE* f = fopen("queue_test.txt","w");
    for(int i=0;i<11;i++)
        fprintf(f,"%d\n",c1->num[i]);
    fclose(f);
    delete q;
    pthread_mutex_destroy(&c1->m);
    delete c1;
}
void more_inputs(){
    PCQueue<int>* q = new PCQueue<int>;
    pthread_t t14,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13;
    struct check* c1 = new struct check;
    c1->q=q;
    c1->index=0;
    c1->inserted=0;
    pthread_mutex_init(&c1->m,NULL);
    pthread_create(&t2,NULL,get1,(void*)c1);
    pthread_create(&t3,NULL,get1,(void*)c1);
    pthread_create(&t8,NULL,get1,(void*)c1);
    pthread_create(&t9,NULL,get1,(void*)c1);
    pthread_create(&t10,NULL,get1,(void*)c1);
    pthread_create(&t11,NULL,get1,(void*)c1);
    pthread_create(&t4,NULL,get1,(void*)c1);
    pthread_create(&t5,NULL,get1,(void*)c1);
    pthread_create(&t6,NULL,get1,(void*)c1);
    pthread_create(&t7,NULL,add2,(void*)c1);
    pthread_create(&t12,NULL,get1,(void*)c1);
    pthread_create(&t13,NULL,get1,(void*)c1);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    pthread_join(t4,NULL);
    pthread_join(t5,NULL);
    pthread_join(t6,NULL);
    pthread_join(t7,NULL);
    pthread_join(t8,NULL);
    pthread_join(t9,NULL);
    pthread_join(t10,NULL);
    pthread_join(t11,NULL);
    pthread_join(t12,NULL);
    pthread_join(t13,NULL);
    FILE* f = fopen("queue_test1.txt","w");
    for(int i=0;i<11;i++)
        fprintf(f,"%d\n",c1->num[i]);
    fclose(f);
    delete q;
    pthread_mutex_destroy(&c1->m);
    delete c1;
}
void less_inputs(){
    PCQueue<int>* q = new PCQueue<int>;
    pthread_t t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13;
    struct check* c1 = new struct check;
    c1->q=q;
    c1->index=0;
    c1->inserted=0;
    pthread_mutex_init(&c1->m,NULL);
    pthread_create(&t2,NULL,get,(void*)c1);
    pthread_create(&t3,NULL,get,(void*)c1);
    pthread_create(&t8,NULL,get,(void*)c1);
    pthread_create(&t9,NULL,get,(void*)c1);
    pthread_create(&t10,NULL,get,(void*)c1);
    pthread_create(&t11,NULL,get,(void*)c1);
    pthread_create(&t4,NULL,get,(void*)c1);
    pthread_create(&t5,NULL,get,(void*)c1);
    pthread_create(&t6,NULL,get,(void*)c1);
    pthread_create(&t12,NULL,get,(void*)c1);
    pthread_create(&t13,NULL,get,(void*)c1);
    pthread_create(&t7,NULL,add4,(void*)c1);//cause a sleep
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    pthread_join(t4,NULL);
    pthread_join(t5,NULL);
    pthread_join(t6,NULL);
    pthread_join(t7,NULL);
    pthread_join(t8,NULL);
    pthread_join(t9,NULL);
    pthread_join(t10,NULL);
    pthread_join(t11,NULL);
    pthread_join(t12,NULL);
    pthread_join(t13,NULL);
    FILE* f = fopen("queue_test2.txt","w");
    for(int i=0;i<11;i++)
        fprintf(f,"%d\n",c1->num[i]);
    fclose(f);
    delete q;
    pthread_mutex_destroy(&c1->m);
    delete c1;
}
int main(){
    /*
        Parallel queue test, should print diffrent nubmers
    */
    //same input as outputs
    same();
    //more inputs than outputs
    more_inputs();
    //more outputs than inputs
    less_inputs();
    return 0;
}
