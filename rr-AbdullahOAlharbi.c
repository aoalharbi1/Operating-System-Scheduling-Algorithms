#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
* This program implements a Round Robin with a time quantum 
* of 12 units of time scheduling algorithm.
* It reads the jobs from a file and store it in an array, then the jobs are
* pushed to a linked list of structures. The information of the job that
* is processed is stored in the CPU, and deleted from the linked list.
* The output will ba written to a file.
*
* Abdullah O Alharbi
* CS-420 
* Programming Assignment 1
*/

const int NUMBER_OF_JOBS = 150;
int front = 0; 						// front of the array
int rear = 0; 						// rear of the array
int counter = 0; 				    // counter of jobs in the array
struct CPU *processor = NULL;       //pointer to the struct of CPU

typedef struct Process {
    char name[2];
    int priorityLevel;
    int serviceTime;
    int arrivalTime;
    int serviceTimeForPrint;
    struct Process* next;
} process;

struct CPU {
    char jobName[2];
    int jobPriority;
    int jobServiceTime;			 // the variable will be decremented
    int serviceTimeForPrint;	 // holds the value of the job service time
    int jobArrivalTime;
    int rrCounter;				// time quantum counter
    int clockCycle;					
    int cpuStatus; 				// 1 if free, and 0 if busy
};


/**
* inserts the job in the linked list, the job will be inserted
* in the end of the linked list  
*
* parameters: process** head, process newJob
*/
void insert(process** head, process newJob) {

    process* current = (process*) malloc(sizeof (process));
    process* prev = *head;



    current->name[0] = newJob.name[0];
    current->name[1] = newJob.name[1];
    current->arrivalTime = newJob.arrivalTime;
    current->priorityLevel = newJob.priorityLevel;
    current->serviceTime = newJob.serviceTime;
    current->serviceTimeForPrint = newJob.serviceTimeForPrint;

    front = front + 1;

    if (*head == NULL) {
        *head = current;
        current->next = NULL;
    } else {
        while (prev->next != NULL) {
            prev = prev->next;
        }

        prev->next = current;
        prev->next->next = NULL;
    }

}


/**
* asks the user to enter a name of a file to read jobs into array 
* of structs (process jobsFromFile[]). The filename up to
* 25 characters
*
* parameters: process jobsFromFile[]
*/
void readFile(process jobsFromFile[]) {


    char fileName[25];

    printf("Input the file name (up to 25 characters), the file must be in "
            "the same directory containing this program: ");
    scanf("%s", &fileName);

    FILE *file;
    file = fopen(fileName, "r");

    if (file) {

        while (!feof(file) && rear < NUMBER_OF_JOBS) {


            fscanf(file, "%s", jobsFromFile[rear].name);
            fscanf(file, "%i", &jobsFromFile[rear].arrivalTime);
            fscanf(file, "%i", &jobsFromFile[rear].serviceTime);
            fscanf(file, "%i", &jobsFromFile[rear].priorityLevel);
            counter = counter + 1;
            
            jobsFromFile[rear].serviceTimeForPrint = jobsFromFile[rear].serviceTime;


            rear = rear + 1;
        }
    } else {
        printf("Error, the file was not found\n\n exiting ...\n");
        exit(1);
    }

    fclose(file);


}


/**
* returns 1 if the queue is empty, and 0 otherwise
*
* parameters: process* head
*/
int isQueueEmpty(process* head) {

    if (head == NULL && front == rear) {
        return 1;
    } else {
        return 0;
    }
}


/**
* returns 1 if the CPU is free, and 0 if it is busy
*
*/
int isCPUFree() {

    return processor->cpuStatus;
}


/**
* dipatches the job from the head of linked list to the CPU
* to process it
*
* parameters: process **head
*/
void dispatch(process **head) {
    process *current = *head;

    if (*head == NULL) {
        return;
    } else if (current->next == NULL) {
        *head = NULL;
    } else {
        *head = current->next;
    }

    processor->jobName[0] = current->name[0];
    processor->jobName[1] = current->name[1];
    processor->jobArrivalTime = current->arrivalTime;
    processor->jobPriority = current->priorityLevel;
    processor->jobServiceTime = current->serviceTime;
    processor->serviceTimeForPrint = current->serviceTimeForPrint;
    processor->rrCounter = 12;

    processor->cpuStatus = 0;

    free(current);
    counter = counter - 1;

}


/**
* opens a file and writes the output of the program to it
*
*/
void print() {
    int waitingTime = processor->clockCycle - processor->jobArrivalTime - processor->serviceTimeForPrint;
	
	FILE *file;
	
	file = fopen("output_rr.txt", "a");
	
	if(!file)
	{
		printf("\nError, could not write to file output_rr.txt");
		exit(1);
	}
	
	fprintf(file, "\t%8c%c\t%12i\t%12i\t%12i\n", processor->jobName[0], processor->jobName[1], processor->jobArrivalTime, waitingTime, processor->clockCycle);
	
	fclose(file);
}


/**
* reinserts if it is not finished the job in the linked list, 
* the job will be inserted in the end of the linked list  
*
* parameters: process** head, process newJob
*/
void reinsert(process** head, process newJob) {


    process* current = (process*) malloc(sizeof (process));
    process* prev = *head;

    current->name[0] = newJob.name[0];
    current->name[1] = newJob.name[1];
    current->arrivalTime = newJob.arrivalTime;
    current->priorityLevel = newJob.priorityLevel;
    current->serviceTime = newJob.serviceTime;
    current->serviceTimeForPrint = newJob.serviceTimeForPrint;


    if (*head == NULL) {
        *head = current;
        current->next = NULL;
    } else {
        while (prev->next != NULL) {
            prev = prev->next;
        }

        prev->next = current;
        prev->next->next = NULL;
    }

}


int main(int argc, char** argv) {
    process *head = NULL;			     // pointes to the head of the linked list
    process jobsFromFile[NUMBER_OF_JOBS];
    process toInsert;
    processor = (struct CPU*) malloc(sizeof (struct CPU));


    readFile(jobsFromFile);
    

    processor->clockCycle = 0;
    processor->cpuStatus = 1;
    
    FILE *file;
    
    file = fopen("output_rr.txt", "w");
	
	if(!file)
	{
		printf("\nError, could not write to file output_rr.txt");
		exit(1);
	}

    fprintf(file, "\tJob name\tArrival time\tWaiting time\tCompletion time\n\n");
    
    fclose(file);


    while (isCPUFree() == 1 || isQueueEmpty(head) == 0 || processor->jobServiceTime != 0) {

        if (processor->jobServiceTime == 0 && isCPUFree() == 0) {
            print();
            processor->cpuStatus = 1;
        }
        
        else if(processor->rrCounter == 0 && processor->jobServiceTime > 0){
            toInsert.name[0] = processor->jobName[0];
            toInsert.name[1] = processor->jobName[1];
            toInsert.arrivalTime = processor->jobArrivalTime;
            toInsert.priorityLevel = processor->jobPriority;
            toInsert.serviceTime = processor->jobServiceTime;
            toInsert.serviceTimeForPrint = processor->serviceTimeForPrint;
            
            reinsert(&head, toInsert);
            processor->cpuStatus = 1;
        }

        if (front != rear) {
            while (processor->clockCycle == jobsFromFile[front].arrivalTime) {

                toInsert = jobsFromFile[front];
                insert(&head, toInsert);
            }
        }

        if (isCPUFree() == 1) {
            dispatch(&head);
        }
        if (processor->jobServiceTime > 0 && processor->rrCounter > 0) {
            processor->jobServiceTime = processor->jobServiceTime - 1;
            processor->rrCounter = processor->rrCounter - 1;

        }

        processor->clockCycle = processor->clockCycle + 1;
    }

    print();


    return (EXIT_SUCCESS);
}
