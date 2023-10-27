// File:	thread-worker.h

// List all group member's name: Abhinav Acharya, Sai Manukonda
// username of iLab: aa2372, skm70 
// iLab Server: ilab1.cs.rutgers.edu

#ifndef WORKER_T_H
#define WORKER_T_H
#define _GNU_SOURCE

/* To use Linux pthread Library in Benchmark, you have to comment the USE_WORKERS macro */
#define USE_WORKERS 1

/* include lib header files that you need here: */
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

typedef uint worker_t;

typedef struct TCB {
	worker_t thread_id;   // Unique thread ID
    int thread_status;    // Thread status (READY, SCHEDULED, BLOCKED, etc.)
    ucontext_t thread_context; // Thread's execution context
    void* thread_stack;   // Pointer to the thread's stack
    int thread_priority;
	// YOUR CODE HERE
} tcb; 

/* mutex struct definition */
typedef struct worker_mutex_t {
	int is_locked;         // Mutex status (locked or unlocked)
    worker_t owner_thread;
} worker_mutex_t;

/* define your data structures here: */
// Feel free to add your own auxiliary data structures (linked list or queue etc...)

// YOUR CODE HERE
typedef struct PriorityQueueElement {
    tcb* thread;        // Pointer to the thread associated with this element
    int priority;       // Priority of the thread
    // Add more fields as needed...
    struct PriorityQueueElement* next; // Pointer to the next element in the queue
} PriorityQueueElement;

// Define the Priority Queue structure.
typedef struct PriorityQueue {
    PriorityQueueElement* front; // Pointer to the front (highest priority) element
    // Add more fields as needed...
} PriorityQueue;

// Function to initialize an empty priority queue.
void priority_queue_init(PriorityQueue* queue) {
    queue->front = NULL;
    // Initialize other fields as needed...
}

// Function to insert a thread into the priority queue.
void priority_queue_insert(PriorityQueue* queue, tcb* thread, int priority) {
    // Create a new queue element for the thread.
    PriorityQueueElement* newElement = (PriorityQueueElement*)malloc(sizeof(PriorityQueueElement));
    if (newElement == NULL) {
        // Handle memory allocation failure.
        exit(1); // For simplicity, exit on failure.
    }
    newElement->thread = thread;
    newElement->priority = priority;
    newElement->next = NULL;

    // Insert the new element based on priority (higher priority first).
    PriorityQueueElement* current = queue->front;
    if (current == NULL || priority < current->priority) {
        newElement->next = current;
        queue->front = newElement;
    } else {
        while (current->next != NULL && priority >= current->next->priority) {
            current = current->next;
        }
        newElement->next = current->next;
        current->next = newElement;
    }
    // Update other fields if needed...
}

// Function to remove and return the highest-priority thread from the queue.
tcb* priority_queue_pop(PriorityQueue* queue) {
    if (queue->front == NULL) {
        return NULL; // Queue is empty.
    }
    tcb* thread = queue->front->thread;
    PriorityQueueElement* temp = queue->front;
    queue->front = queue->front->next;
    free(temp);
    return thread;
}
/* Function Declarations: */

/* create a new thread */
int worker_create(worker_t * thread, pthread_attr_t * attr, void
    *(*function)(void*), void * arg);

/* give CPU pocession to other user level worker threads voluntarily */
int worker_yield();

/* terminate a thread */
void worker_exit(void *value_ptr);

/* wait for thread termination */
int worker_join(worker_t thread, void **value_ptr);

/* initial the mutex lock */
int worker_mutex_init(worker_mutex_t *mutex, const pthread_mutexattr_t
    *mutexattr);

/* aquire the mutex lock */
int worker_mutex_lock(worker_mutex_t *mutex);

/* release the mutex lock */
int worker_mutex_unlock(worker_mutex_t *mutex);

/* destroy the mutex */
int worker_mutex_destroy(worker_mutex_t *mutex);


/* Function to print global statistics. Do not modify this function.*/
void print_app_stats(void);

#ifdef USE_WORKERS
#define pthread_t worker_t
#define pthread_mutex_t worker_mutex_t
#define pthread_create worker_create
#define pthread_exit worker_exit
#define pthread_join worker_join
#define pthread_mutex_init worker_mutex_init
#define pthread_mutex_lock worker_mutex_lock
#define pthread_mutex_unlock worker_mutex_unlock
#define pthread_mutex_destroy worker_mutex_destroy
#endif

#endif