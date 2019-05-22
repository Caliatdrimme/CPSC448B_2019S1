#include "aco.h"
#include <stdio.h>
#include <stdlib.h>
#include "uv.h"

/* this is a program for "proof of concept" 
to get the basics of libuv setup and working
- does not do anything necessary
- probably an example of how not to write programs

- tried to play around with the structure of how the sieve coroutines work
to change it up from the first project with libwire*/


//declare global variables
static int prime = 0;
static int number = 0;
static int flag =0;
static int move =0;

//define structure for linked list
struct Node  
{ 
  int data; 
  struct Node *next; 
}; 

struct Node * cur = NULL;
struct Node * last = NULL;

//declare global uv loop and handle
uv_loop_t *loop;
uv_idle_t idler;

//callback function of idle handle
void print_progress(uv_idle_t *handle){
	//printf("received messege\n");
	if (move == 0){
        move = 1;
    } else {move = 0;}
    uv_idle_stop(handle);
}

//function for the non-main coroutine
void co_fp0() {
	//printf("current prime is %d\n", prime);
	

	while(flag == 0){

		if (number%prime== 0){
			//printf("not prime, moving on %d\n\n", number);
			/* move onto next number in main */
			uv_idle_start(&idler, print_progress);
    		uv_run(uv_default_loop(), UV_RUN_DEFAULT);
			aco_yield();
		}else {
			if (cur->data == last->data){
				printf("prime found creating new factor %d\n", number);
			
				/* smallest number to pass through this filter - this is the new prime*/
				struct Node * new = (struct Node*)malloc(sizeof(struct Node));  
				new->data = number;
				new->next = NULL;
				//printf("created new node \n");
				cur->next = new;
				last = new;

				uv_idle_start(&idler, print_progress);
    			uv_run(uv_default_loop(), UV_RUN_DEFAULT);

				aco_yield();
			} else {
				//printf("checking...\n");
				/* send event that we can move onto next prime - this is automatic when yielding to main*/
				aco_yield();
			}
		}
	}
    aco_exit();
}

int main(){

	struct Node* head = NULL;
	head = (struct Node*)malloc(sizeof(struct Node));  
	head->data = 2;
	head->next = NULL;

	last = head;

    //initialize coroutines
	aco_thread_init(NULL);
	aco_t* main_co = aco_create(NULL, NULL, 0, NULL, NULL);
	aco_share_stack_t* sstk = aco_share_stack_new(0);

	int co_ct_arg_point_to_me = 0;

	printf("main control\n");

	aco_t* co = aco_create(main_co, sstk, 0, co_fp0, &co_ct_arg_point_to_me);

    //initialize event loops
	loop = uv_default_loop();
    uv_idle_init(uv_default_loop(), &idler);

	/* start loop on numbers to check */
	for(int i = 3; i<200; i++){
		move = 0;
		//printf("source control at %d\n", i);
		number = i;
		cur = head;

		while (cur != NULL) { 
			//printf("checking against prime %d\n", cur->data);
			prime = cur->data;
			aco_resume(co);
			if (move == 1){
				break;

			}
			cur = cur->next;
		}
	}
	
	printf("shutting down\n");

    //clean up
	uv_loop_close(loop);

 	aco_destroy(co);

    co = NULL;
    // Destroy the share stack sstk.
    aco_share_stack_destroy(sstk);
    sstk = NULL;
    // Destroy the main_co.
    aco_destroy(main_co);
    main_co = NULL;

	return 0;

}