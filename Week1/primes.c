#include <wire.h>
#include <wire_fd.h>
#include <wire_stack.h>
#include <macros.h>
#include <stdio.h>
#include <wire_lock.h>

//declare two tasks we start off with
static wire_t task_source;
static wire_t task_multiple2;

//global flags and variables
static int flag = 0;
//know that first prime is 2
static int j=2;
static int last = 2;

void source(void *args){

	for(int i = 3; i<200; i++){
		// printf("source control at %d\n", i);
		j = i;
		//start off prime check 
		wire_resume(&task_multiple2);
		wire_suspend();
	}
	//flag that we are finished
	flag = 1;

	//clean up
	wire_cancel(&task_multiple2);
	wire_cancel(&task_source);
return;
}

void multiple(void *n){

	int fst = 1;
	wire_t next;

	while(flag == 0){
		int k = * (int *)n; //Current prime

		//wire_t cur = *wire_get_current();
		// printf("\ncurrent thread is %s\n", cur.name);

		// printf("checking j %d, against prime k %d\n", j, k);
		if (j==k){
			// printf("this is the very first number in thread");
			//in queue when we initialized it but don't need to run it yet, first need to update to next number to check
			wire_suspend();
			}

		if (j%k == 0){
			// printf("not prime, moving on %d\n\n", j);
			//going onto next number to check
			wire_resume(&task_source);
			wire_suspend();
		}else {
			if (fst ==1){
				printf("prime found creating new factor %d\n", j);
				int newPrime = j;
				/* this is the first one that passes through so it is prime - becomes next factor */
				wire_init(&next, "next", multiple, &newPrime, WIRE_STACK_ALLOC(4096));
				fst = 0;
				wire_resume(&task_source);
				wire_suspend();
			} else {
			//go onto next prime filter
			wire_resume(&next);
			// printf("resuming %s\n", next.name);
			wire_suspend();
			}
		}
	}
}

void main(){
	printf("main control\n");
	//initialize
	wire_thread_init();
	wire_init(&task_source, "source", source, NULL, WIRE_STACK_ALLOC(4096));
	wire_init(&task_multiple2, "multiple2", multiple, &last, WIRE_STACK_ALLOC(4096));
	wire_thread_run();


	printf("finish\n");

return;
}