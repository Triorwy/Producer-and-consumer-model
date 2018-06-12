#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<stdlib.h>

#define NUM 128
#define PRODUCTSUM 3
#define CONSUMESUM 3

sem_t datasem;//数据信号量
sem_t blanksem;//空闲空间信号量
pthread_mutex_t mutex;//互斥锁
int ring[NUM];//循环队列

int index1 = 0;//生产者index
int index2 = 0;//消费者index

void *product(void *arg)
{
    srand((unsigned int)time(NULL));
    int n = *(int*)arg;
    while(1)
    {
        sem_wait(&blanksem);//P操作，减空闲资源
        pthread_mutex_lock(&mutex);
        int data = rand()%512 + 1;
        ring[index1] = data;
        printf("%d 生产者生产出产品：%d\n",n,data);
        ++index1;
        index1 %= NUM;
        sem_post(&datasem);//V操作，加数据资源
        pthread_mutex_unlock(&mutex);
       // sleep(1);
    }
}

void *consume(void *arg)
{

    int n = *(int*)arg;
    while(1)
    {
        sleep(1);
        sem_wait(&datasem);//P操作，减数据资源
        pthread_mutex_lock(&mutex);
        int data = ring[index2];
        printf("%d 消费者已消费产品：%d\n",n,data);
        ++index2;
        index2 %= NUM;
        sem_post(&blanksem);//V操作，加空闲资源
        pthread_mutex_unlock(&mutex);
    }
}
int main()
{
    pthread_mutex_init(&mutex,NULL);
    pthread_t p[PRODUCTSUM];//生产者
    pthread_t c[CONSUMESUM];//消费者
    sem_init(&blanksem,0,NUM);//初始化生产者空闲数量为NUM
    sem_init(&datasem,0,0);//消费者为0
    int  i = 1;
    for(i = 1; i < PRODUCTSUM ; ++i)
    {
        int *d = (int*)malloc(sizeof(int));//i是局部变量，如果可能当你将i值传进去时
        //时间片切出去将会把i更改，这有就会影响i的值
        *d = i;
        pthread_create(&p[i],NULL,product,(void*)d);
    }
    for(i = 1;i < CONSUMESUM; ++i)
    {
        int *d = (int*)malloc(sizeof(int));
        *d = i;
        pthread_create(&c[i],NULL,consume,(void*)d);
    }
    for(i = 1; i < PRODUCTSUM ;++i)
    {
        pthread_join(p[i],NULL);
    }
    for(i = 1; i < CONSUMESUM ;++i)
    {
        pthread_join(c[i],NULL);
    }
    sem_destroy(&blanksem);
    sem_destroy(&datasem);
    pthread_mutex_destroy(&mutex);
    return 0;
}
