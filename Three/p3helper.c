/* p3helper.c
 * Bernardo Cervantes 
 * Program 3
 * CS570
 * John Carroll 
 * SDSU
 * 11/2/2018 
 *
 * This is the only file you may change. (In fact, the other files should
 * be symbolic links to:
 *   ~cs570/Three/p3main.c
 *   ~cs570/Three/p3.h
 *   ~cs570/Three/Makefile
 *   ~cs570/Three/CHK.h    )
 *
 */
#include "p3.h"

/* You may put any semaphore (or other global) declarations/definitions here: */

static int jcount, scount; //Counter for joggers and shooters
static sem_t critical_region;//Critical region is the gym
static sem_t slock, jlock;

/* General documentation for the following functions is in p3.h
 * Here you supply the code:
 */
void initstudentstuff(void) {
	/* We initilize to one and not MAXCUSTOMERS because our slock and jlock cover
	 * the amount of customers coming in and out. So we only need a binary value to check 
	 * off if there is one type of customer in the gym/critical region
	 */  
	pCHK(sem_init(&critical_region, 0, 1));

	/* We use the lock to keep track
	 * of incoming shooters/ joggers
	 * not incrementing when they are not
	 * suppose to.
	 */ 	
	pCHK(sem_init(&slock, 0 , 1));
	pCHK(sem_init(&jlock, 0 , 1));
}

void prolog(int kind) {
	if(kind == SHOOTER){
		pCHK(sem_wait(&slock));
		if(++scount == 1){
			/* First shooter to enter so we wait for 
			 * the remaining joggers to finish.
			 */ 
			pCHK(sem_wait(&critical_region));		
		}
		pCHK(sem_post(&slock));
	}
	else if(kind == JOGGER){
		pCHK(sem_wait(&jlock));
		if(++jcount == 1){
			/*First jogger to come along,
			 * so we wait for the shooter to
			 * exit.
			 */
			pCHK(sem_wait(&critical_region));
		}
		pCHK(sem_post(&jlock));
	}
}

void epilog(int kind) {
	if(kind == SHOOTER){
		pCHK(sem_wait(&slock));
		if(--scount == 0){
			/* Last shooter to leave, so
			 * we can accept shooters again 
			 * or joggers 
			 * in the gym.
			 */
			pCHK(sem_post(&critical_region));
		}
		pCHK(sem_post(&slock));
	}

	else if(kind == JOGGER){
		pCHK(sem_wait(&jlock));
		if(--jcount == 0){
			/*
			 *Last jogger to leave, 
			 * so we can accept 
			 * customers which can be 
			 * shooters or joggers again
			 * in the gym
			 */
			pCHK(sem_post(&critical_region));

		}
		pCHK(sem_post(&jlock));

	}
}  
