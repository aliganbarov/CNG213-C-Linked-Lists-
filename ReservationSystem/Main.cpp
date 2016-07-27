#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//STRUCTURE DEFINITIONS
struct passengerNode { 
	char *name;  
	struct passengerNode *next; 
};
struct flightRecord { 
	int flightNo;  
	int capacity;   
	int reservationCount;  
	struct passengerNode *passengerListHead;  
	struct passengerNode *waitingListHead; 
	struct flightRecord *next;
};
struct flightList {
	int numberOfFlights;
	struct flightRecord *flightListHead;
};
typedef struct passengerNode* Passenger;
typedef struct flightRecord* Flight;
typedef struct flightList* List;

//PRIMARY FUNCTION DEFINITIONS
List initialize();
void Status(List);
void processCommands(List);
void reservation(List);
void cancel(List);
void inquire(List);

//SECONDARY FUNCTION DEFINITIONS
int isFlightFull(Flight);
void memoryError();

int main() {
	List list = initialize();
	processCommands(list);

	system("PAUSE");
	return 0;
}

//PRIMARY FUNCTIONS
List initialize() {
	char fileName[50] = "data.txt"; //change to user input
	FILE *f = fopen(fileName, "r");
	if (f == NULL) {
		printf("Failed to open file\n");
		exit(1);
	}
	List list = (List)malloc(sizeof(struct flightList));
	if (list == NULL) {
		printf("Failed to allocate memory");
		exit(1);
	}
	list->flightListHead = (Flight)malloc(sizeof(struct flightRecord));
	list->flightListHead->next = NULL;
	Flight temp = list->flightListHead;
	fscanf(f, "%d", &list->numberOfFlights);

	//generating each flight node:
	for (int i = 0; i < list->numberOfFlights; i++) {
		temp->next = (Flight)malloc(sizeof(struct flightRecord));	//first node skipped as dummy node
		if (temp->next == NULL) {
			memoryError();
		}
		temp = temp->next;
		temp->next = NULL;
		fscanf(f, "%d %d", &temp->flightNo, &temp->capacity);
		temp->reservationCount = 0;

		temp->passengerListHead = (Passenger)malloc(sizeof(struct passengerNode));
		if (temp->passengerListHead == NULL) {
			memoryError();
		}
		temp->passengerListHead->next = NULL;
		temp->passengerListHead->name = "Dummy";

		temp->waitingListHead = (Passenger)malloc(sizeof(struct passengerNode));
		if (temp->waitingListHead == NULL) {
			memoryError();
		}
		temp->waitingListHead->next = NULL;
		temp->waitingListHead->name = "Dummy";
	}
	
}

void Status(List list) {
	Flight flight = list->flightListHead->next;
	Passenger passenger;
	Passenger waiting;
	printf("\nPRINTING STATUS\n\n");
	while (flight != NULL) {
		printf("Flight number %d:\n", flight->flightNo);
		printf("Number of reservations: %d\n", flight->reservationCount);
		
		printf("Names of customers: ");
		passenger = flight->passengerListHead->next;	//skipping dummy node
		while (passenger != NULL) {
			printf("%s, ", passenger->name);
			passenger = passenger->next;
		}
		
		printf("\nPeople on waiting list: ");
		waiting = flight->waitingListHead->next;	//skipping dummy node
		while (waiting != NULL) {
			printf("%s, ", waiting->name);
			waiting = waiting->next;
		}
		printf("\n\n");
		flight = flight->next;
	}
}

void reservation(List list) {
	char name[50];
	int flightNo, flightFound = 0;
	Flight flight = list->flightListHead;
	Passenger passenger;
	Passenger waiter;
	Passenger newPassenger = (Passenger)malloc(sizeof(struct passengerNode));;
	printf("\nEnter name and flight #: ");
	scanf("%s %d", &name, &flightNo);
	newPassenger->name = (char*)malloc(sizeof(char) * 50);
	strcpy(newPassenger->name, name);
	newPassenger->next = NULL;
	if (newPassenger == NULL) {
		memoryError();
	}
	while (flight->next != NULL) {
		if (flight->flightNo == flightNo) {
			flightFound = 1;
			break;
		}
		flight = flight->next;
	}
	if (!flightFound) { 
		printf("Requested flight was not found\n"); 
		return; 
	}
	if (!isFlightFull(flight)) {
		//case flight is not full, reserving place
		passenger = flight->passengerListHead;
		while (passenger->next != NULL) {
			passenger = passenger->next;	//traversing all existing passengers
		}
		passenger->next = newPassenger;
		flight->reservationCount++;
	}
	else {
		//case flight is full, put in waiting list
		waiter = flight->waitingListHead;
		while (waiter->next != NULL) {
			waiter = waiter->next;	//traversing all waiters
		}
		waiter->next = newPassenger;
	}
}

void cancel(List list) {
	char name[50];
	int found = 0, flightNo;
	printf("Enter name and flight #: ");
	scanf("%s %d", &name, &flightNo);
	
	//finding required flight no
	Flight flight = list->flightListHead->next;	//skipped dummy
	while (flight != NULL) {
		if (flight->flightNo == flightNo) {
			found = 1;
			break;
		}
		flight = flight->next;
	}
	if (!found) {
		printf("Required flight No is not in list\n");
		return;
	}

	//checking for passenger in passenger list
	Passenger passenger = flight->passengerListHead; //no skipped dummy
	while (passenger->next != NULL) {
		if (!strcmp(passenger->next->name, name)) {
			passenger->next = passenger->next->next; //deleting node
			flight->reservationCount--;
			return;
		}
		passenger = passenger->next;
	}
	
	//checking for passenger in waiting list
	passenger = flight->waitingListHead; //no skip dummy
	while (passenger->next != NULL) {
		if (!strcmp(passenger->next->name, name)) {
			passenger->next = passenger->next->next; //deleting node
			return;
		}
		passenger = passenger->next;
	}
	printf("Passenger with that name was not found!\n");
	return;
}

void inquire(List list) {
	char name[50];
	printf("Enter name: ");
	scanf("%s", &name);
	Flight flight = list->flightListHead->next;	//skipped dummy
	Passenger passenger;
	Passenger waiter;
	printf("Passenger's flight numbers: ");
	while (flight != NULL) {
		//first among passenger list
		passenger = flight->passengerListHead->next; //skipped dummy
		while (passenger != NULL) {
			if (!strcmp(passenger->name, name)) {
				printf("%d ", flight->flightNo);
			}
			passenger = passenger->next;
		}
		waiter = flight->waitingListHead->next;		//skipped dummy
		while (waiter != NULL) {
			if (!strcmp(waiter->name, name)) {
				printf("%d ", flight->flightNo);
			}
			waiter = waiter->next;
		}
		flight = flight->next;
	}
}

void processCommands(List list) {
	int opt;
	while (1) {
		printf("\n----------MENU----------\n");
		printf("1)Show status\n2)Reserve\n3)Cancel\n4)Inquire\n5)Exit\nOption: ");
		scanf("%d", &opt);
		switch (opt) {
		case 1:
			Status(list);
			break;
		case 2:
			reservation(list);
			break;
		case 3:
			cancel(list);
			break;
		case 4:
			inquire(list);
			break;
		case 5:
			return;
		default:
			printf("\nChoose correct option please\n");
		}
	}
}



//SECONDARY FUNCTIONS
int isFlightFull(Flight flight) {
	return flight->capacity == flight->reservationCount ? 1 : 0;
}

void memoryError() {
	printf("Failed to allocate memory!");
	exit(1);
}