#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *dine();
int chopsticks[5];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


int main(int argc, char *argv[])
{
	int i, rc;
	pthread_t philsophers[5];

	srand(time(NULL));   

	/* Initialize chopsticks */
	for (i=0; i<5; ++i)
	{
		chopsticks[i]=0;
	}
	
	/* Spawn threads */
	for (i=0; i<5; ++i)
	{
		if( (rc=pthread_create( &philsophers[i], NULL, &dine, &i)) )
			printf("Thread creation failed: %d\n", rc);
		sleep(1);
	}

	/* Wait for thread completion */
	for (i=0; i<5; ++i)
		pthread_join( philsophers[i], NULL);
	exit(0);
}

//Philosopher will sleep for 1-3 seconds
think(int pnum)
{
	int r = rand() % 3 + 1;
	printf("Philosopher %d is thinking for %d seconds\n", pnum, r);
	sleep(r);
}

//Philosopher will acquire chopsticks, sleep for 1-3 seconds, then return the chopsticks
eat(int pnum)
{
	printf("Philosopher %d is hungry\n", pnum);
	pickup_chopsticks(pnum);
	int r = rand() % 3 + 1;	
	printf("Philosopher %d is eating for %d seconds\n", pnum, r);
	sleep(r);
	return_chopsticks(pnum);
	//printf("Philosopher %d put down chopsticks\n", pnum);
}

/*Philosopher will wait until both chopsticks are freed before picking them up. This is
done to prevent deadlock, as there will not be a situation where philosophers are waiting
on one chopstick.*/
pickup_chopsticks(int pnum)
{
	pthread_mutex_lock(&mutex);
	while(chopsticks[pnum] == 1 || chopsticks[(pnum + 1) % 5] == 1)
		pthread_cond_wait(&cond, &mutex);
	chopsticks[pnum] = 1;
	chopsticks[(pnum + 1) % 5] = 1;
	pthread_mutex_unlock(&mutex);
	//display_table(chopsticks);
}

//Philosopher will return chopsticks and signal the condition that chopsticks are free
return_chopsticks(int pnum)
{
	pthread_mutex_lock(&mutex);
	chopsticks[pnum] = 0;
	chopsticks[(pnum + 1) % 5] = 0;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
	//display_table(chopsticks);
}

//Loops infinitely having philosophers alternate between thinking and dining.
void *dine(void *pnum_ptr)
{
	int pnum = *(int*)pnum_ptr;

	//printf("Created philosopher thread %d\n", pnum);

	while (1)
	{
		think(pnum);
		eat(pnum);
	}
}

//Used for debugging purposes to ensure the proper chopsticks are used
display_table(int chops[])
{
	int i;
	for(i = 0; i < 5; ++i)
		printf("C%d: %d ", i, chops[i]);
	printf("\n");
}


