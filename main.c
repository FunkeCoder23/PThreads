//"They said something about being hungry as they left."

//I don't think that's tobacco in their cigarettes...

//Compile the code by linking with -lpthread -lrt


#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define true 1
#define false 0
#define debug true

//semaphores for agents
sem_t agentSem, tobacco, paper, match;
//Semaphores for Pushers
sem_t tobaccoSem, paperSem, matchSem, mutex; 
//Boolean vars for ingredients
int isMatch, isPaper, isTobacco;

void* agentTP(void* arg)
{
	//each agent provides two different ingredients

	for (int i=0; i<6; ++i) //each agent loops six times
	{
		sleep(.001*(rand()%200)); //wait from 0-200 ms
		sem_wait(&agentSem);// P(agentSem)
		if(debug) printf("Providing T & P\r\n");
		sem_post(&tobacco);// V(tobacco)
		sem_post(&paper);// V(paper)
	}
	return 0;	
}


void* agentPM(void* arg)
{
	//each agent provides two different ingredients

	for (int i=0; i<6; ++i) //each agent loops six times
	{
		sleep(.001*(rand()%200)); //wait from 0-200 ms
		sem_wait(&agentSem);// P(agentSem)
		if(debug) printf("Providing P & M\r\n");
		sem_post(&paper);// V(paper)
		sem_post(&match);// V(match)
	}
	return 0;	
}

void* agentMT(void* arg)
{
	//each agent provides two different ingredients

	for (int i=0; i<6; ++i) //each agent loops six times
	{
		sleep(.001*(rand()%200)); //wait from 0-200 ms
		sem_wait(&agentSem);// P(agentSem)
		if(debug) printf("Providing M & T\r\n");
		sem_post(&match);// V(match)
		sem_post(&tobacco);// V(tobacco)
	}
	return 0;	
}

void* pusherT(void* arg)
{
//each pusher waits for different ingredient

//each pusher loops 12 times
	for (int i=0; i<12; ++i)
	{
		sem_wait(&tobacco);//P(tobacco)
		sem_wait(&mutex);//P(mutex)
		if (isPaper) 
		{
			isPaper = false;
			if (debug) printf("Alerting smoker with match\r\n");
			sem_post(&matchSem);//V(matchSem)
		}
		else if (isMatch) 
		{
			isMatch = false;
			if (debug) printf("Alerting smoker with paper\r\n");	
			sem_post(&paperSem);//V(paperSem)
		}
		else isTobacco = true;
		sem_post(&mutex);//V(mutex)
		}
	return 0;		
}

void* pusherP(void* arg)
{
//each pusher waits for different ingredient

//each pusher loops 12 times
	for (int i=0; i<12; ++i)
	{
		sem_wait(&paper);//P(paper)
		sem_wait(&mutex);//P(mutex)
		if (isTobacco) 
		{
			isTobacco = false;
			if (debug) printf("Alerting smoker with match\r\n");			
			sem_post(&matchSem);//V(matchSem)
		}
		else if (isMatch) 
		{
			isMatch = false;
			if (debug) printf("Alerting smoker with Tobacco\r\n");			
			sem_post(&tobaccoSem);//V(tobaccoSem)
		}
		else isPaper = true;
		sem_post(&mutex);//V(mutex)
		}
	return 0;		
}

void* pusherM(void* arg)
{
//each pusher waits for different ingredient

//each pusher loops 12 times
	for (int i=0; i<12; ++i)
	{
		sem_wait(&match);//P(match)
		sem_wait(&mutex);//P(mutex)
		if (isPaper) 
		{
			isPaper = false;
			if (debug) printf("Alerting smoker with Tobacco\r\n");	
			sem_post(&tobaccoSem);//V(tobaccoSem)
		}
		else if (isTobacco) 
		{
			isTobacco = false;
			if (debug) printf("Alerting smoker with paper\r\n");	
			sem_post(&paperSem);//V(paperSem)
		}
		else isMatch = true;
		sem_post(&mutex);//V(mutex)
		}
	return 0;		
}

void* smokerT(void* arg)
{
	//Each smoker finishes 3 cigarettes before exiting
	for (int i=0; i<3; ++i)
	{
		sleep(.001*(rand()%50)); //wait from 0-50 ms
		sem_wait(&tobaccoSem);// P(tobaccoSem)
		// Make a "cigarette"
		if (debug) printf("making cigarette with Tobacco\r\n");
		sleep(.001*(rand()%50)); //wait from 0-50 ms
		sem_post(&agentSem);// V(agentSem)
		// Smoke the "cigarette"
		if (debug) printf("smoking cigarette with tobacco\r\n");
	}
	return 0;	
}

void* smokerP(void* arg)
{
	//Each smoker finishes 3 cigarettes before exiting
	for (int i=0; i<3; ++i)
	{
		sleep(.001*(rand()%50)); //wait from 0-50 ms
		sem_wait(&paperSem);// P(paperSem)
		// Make a "cigarette"
		if (debug) printf("making cigarette with paper\r\n");
		sleep(.001*(rand()%50)); //wait from 0-50 ms
		sem_post(&agentSem);// V(agentSem)
		// Smoke the "cigarette"
		if (debug) printf("smoking cigarette with paper\r\n");
	}
	return 0;	
}

void* smokerM(void* arg)
{
	//Each smoker finishes 3 cigarettes before exiting
	for (int i=0; i<3; ++i)
	{
		sleep(.001*(rand()%50)); //wait from 0-50 ms
		sem_wait(&matchSem);// P(matchSem)
		// Make a "cigarette"
		if (debug) printf("making cigarette with match\r\n");		
		sleep(.001*(rand()%50)); //wait from 0-50 ms
		sem_post(&agentSem);// V(agentSem)
		// Smoke the "cigarette"
		if (debug) printf("smoking cigarette with match\r\n");
	}
	return 0;
}



int main() 
{ 
	srand(time(0)); //set random seed

	//create 3 agents, 3 pushers, six smokers (2 holding tobacco, 2 paper, 2 matches)
	//JOIN ALL THREADS BEFORE PROG TERMINATES

	//init bools
	isTobacco = false;
	isPaper = false;
	isMatch = false;
  //init semaphores
	sem_init(&tobaccoSem, 0, 0); //tobaccoSem = 0
	sem_init(&paperSem, 0, 0); //paperSem = 0
	sem_init(&matchSem, 0, 0); //matchSem = 0
	sem_init(&mutex, 0, 1); //mutex = 1

	sem_init(&agentSem, 0, 1);//agentSem = 1
	sem_init(&tobacco, 0, 0);//tobacco = 0
	sem_init(&paper, 0, 0);//paper = 0
	sem_init(&match, 0, 0);//match = 0
	
	pthread_t smokers[6],pushers[3],agents[3];
	for(int i=0; i<6; i+=3)
	{
		pthread_create(&smokers[i],NULL,smokerM,NULL);
		pthread_create(&smokers[i+1],NULL,smokerT,NULL);
		pthread_create(&smokers[i+2],NULL,smokerP,NULL);
	}
	pthread_create(&agents[0],NULL,agentTP,NULL);
	pthread_create(&agents[1],NULL,agentMT,NULL);
	pthread_create(&agents[2],NULL,agentPM,NULL);

	pthread_create(&pushers[0],NULL,pusherM,NULL);
	pthread_create(&pushers[1],NULL,pusherT,NULL);
	pthread_create(&pushers[2],NULL,pusherP,NULL);
	
	for(int i=0; i<3; i++)
	{
	pthread_join(smokers[i],NULL); //smokers 0,1,2
	pthread_join(smokers[i+3],NULL); //smokers 3-6
	pthread_join(agents[i],NULL);  //agents 0-2
	pthread_join(pushers[i],NULL); //pusher 0-2 
	}

	//dealloc semaphores
	sem_destroy(&mutex);
	sem_destroy(&agentSem); 
	sem_destroy(&matchSem); 
	sem_destroy(&paperSem); 
	sem_destroy(&tobaccoSem); 
	sem_destroy(&match);
	sem_destroy(&paper); 
	sem_destroy(&tobacco);  

	return 0; 
} 


//SEMAPHORE GUIDE
/*
int sem_wait(sem_t *sem);
To release or signal a semaphore, we use the sem_post function:

int sem_post(sem_t *sem);
A semaphore is initialised by using sem_init(for processes or threads) or sem_open (for IPC).

sem_init(sem_t *sem, int pshared, unsigned int value);
Where,

sem : Specifies the semaphore to be initialized.
pshared : This argument specifies whether or not the newly initialized semaphore is shared between processes or between threads. A non-zero value means the semaphore is shared between processes and a value of zero means it is shared between threads.
value : Specifies the value to assign to the newly initialized semaphore.
*/