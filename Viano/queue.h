#ifndef __QUEUE__
#define __QUEUE__

#include "process.h"
#include "bool.h"

typedef struct Node {
    Process* process;
    struct Node* next;
} Node;

typedef struct Queue {
    Node* front;
    Node* rear;
    int size;
} Queue;

// Forward Declaration
void initializeQueue(Queue* queue);
bool isEmpty(Queue* queue);
void peekQueue(Queue* queue, Process* p);
void enqueue(Queue* queue, Process* p);
Process* dequeue(Queue* queue);
void debugQueue(Queue* queue);
void printQueue(Queue* queue);
void freeQueue(Queue* queue);

// INITIALIZE THE QUEUE
void initializeQueue(Queue* queue){
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
}

// NO NEED TO CHECK IF QUEUE IS FULL

// CHECK IF QUEUE IS EMPTY
bool isEmpty(Queue* queue){
    if(queue->size == 0)
        return true;
    return false;
}

// PEEKS THE FRONT OF THE QUEUE WITHOUT REMOVAL, PEEK IS GIVEN TO p
void peekQueue(Queue* queue, Process* p){
    if(isEmpty(queue)){
        fprintf(stderr, "ERROR: Queue is empty, cannot peek (YOU SHOULDN'T SEE THIS ERROR)\n");
        return;
    }
    p = queue->front->process;
}

// ADD ELEMENT TO THE END OF THE QUEUE
// possible error case: memory not allocated for Node* n may result in not adding to queue (calloc fail)
void enqueue(Queue* queue, Process* p){
    Node* n = calloc(1, sizeof(Node));
    n->process = p;
    n->next = NULL;
    // add to queue
    if(queue->rear != NULL){ // if a rear exists, append to list
        queue->rear->next = n;
    }
    if(queue->front == NULL){ // if a front doesnt exist, front is node
        queue->front = n;
    }
    queue->rear = n;
    queue->size += 1;
}

// ADD ELEMENT TO THE QUEUE BASED ON estimated time left
void enqueueEstimated(Queue* queue, Process* p) {
    Node* n = calloc(1, sizeof(Node));
    n->process = p;
    n->next = NULL;

    if (queue->front == NULL){ // if a front doesnt exist, front and rear is node
        queue->front = n;
        queue->rear = n;
    } else {
        Node* temp = queue->front;;
        while (temp != NULL) { // Find where p should be placed
            if (p->estimated_wait_time < temp->process->estimated_wait_time || 
            (p->estimated_wait_time == temp->process->estimated_wait_time && p->name < temp->process->name)) {
                break;
            }
            temp = temp->next;
        }
        if(queue->size == 1){   // edge case when size of the queue is 1
            if((queue->front->process->estimated_wait_time == n->process->estimated_wait_time && 
            queue->front->process->name > n->process->name) || queue->front->process->estimated_wait_time > n->process->estimated_wait_time){   // add to front
                n->next = queue->front;
                queue->front = n;
            } else {    // add to rear
                queue->rear->next = n;
                queue->rear = n;
            }
        } else if (temp == queue->front) { // p has the lowest tau, append to front.
            queue->front = n;
            n->next = temp;
        } else if (temp == NULL) { // p has the greatest tau, append to the back
            queue->rear->next = n;
            queue->rear = n;
        } else { // p has a tau somewhere in the middle, add it to the queue in a middle spot
            Node* before_temp = queue->front;
            while(before_temp->next != temp) {
                before_temp = before_temp->next;
            }
            before_temp->next = n;
            n->next = temp;
        }
    }
    queue->size += 1;
}

// REMOVE ELEMENT POINTER FRONT THE FRONT OF THE QUEUE
// returns the element removed as a pointer
Process* dequeue(Queue* queue){
    Process* p = NULL;
    if(isEmpty(queue)){
        fprintf(stderr, "ERROR: Queue is empty, cannot delete (YOU SHOULDN'T SEE THIS ERROR)\n");
        return p;
    }
    Node* node = queue->front;
    p = queue->front->process;
    // remove from queue
    queue->front = node->next;
    if(queue->front == NULL){
        queue->rear = NULL;
    }
    queue->size -= 1;
    free(node);
    return p;
}

// prints out the entire queue for debugging purposes
// NOTE that this prints next regardless of the size count
void debugQueue(Queue* queue){
    printf("=========== QUEUE DEBUG ===========\n");
    printf("SIZE: %d\n", queue->size);
    if(isEmpty(queue)){
        printf("Q: [empty]\n");
    } else {
        printf("Q: [ ");
        Node* n = queue->front;
        while(n != NULL){
            printf("%c", n->process->name);
            if(n->next != NULL)
                printf(" -> ");
            n = n->next;
        }
        printf(" ]\n");
    }
}

void printQueue(Queue* queue){
    if(isEmpty(queue)){
        printf("[Q: empty]");
    } else {
        printf("[Q: ");
        Node* n = queue->front;
        while(n != NULL){
            printf("%c", n->process->name);
            if(n->next != NULL)
                printf(" ");
            n = n->next;
        }
        printf("]");
    }
}

void freeQueue(Queue* queue){
    while(!isEmpty(queue)){
        Node* n = queue->front;
        queue->front = n->next;
        if(queue->front == NULL){
            queue->rear = NULL;
        }
        queue->size -= 1;
        free(n);
    }
}

#endif