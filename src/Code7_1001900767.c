//Diwas Sapkota  1001900767   Coding Assignment 7
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "MovieTheaterLib.h"
#include "QueueLib.h"
#include "BSTLib.h"
#include "ListLib.h"
#include "StackLib.h"

void PrintReceipts(SNODE **StackTop)
{	
	char Ticket[20];
	if(*StackTop==NULL)
	{
		printf("\nNo recepits\n");
	}
	else
	{	printf("Today's receipts\n");

		while(*StackTop!= NULL)
		{
			printf("Receipt #%d\n", (*StackTop)->ReceiptNumber);
			printf("\t%s\n\n", (*StackTop)->MovieTheaterName);
			while((*StackTop)->TicketList != NULL)
			{
				ReturnAndFreeLinkedListNode(&((*StackTop)->TicketList), Ticket);
				printf("\t%s", Ticket);
			}

			pop(StackTop);
		}
		
	}

	
}

BNODE *PickAndDisplayTheater(BNODE *BSTRoot, char MovieTheaterMap[][MAXCOLS], int *MapRow, int *MapCol)
{
	BNODE *MyTheater = NULL;
	char zip[6] = {};
	char MyDims[6] = {};
	
	printf("\n\nPick a theater by entering the zipcode\n\n");

	InOrder(BSTRoot);
	printf("\nEnter the 6-digit zip code: ");
	scanf("%s", zip);
	MyTheater=SearchForBNODE(BSTRoot, zip);

	if(MyTheater==NULL)
	{
		printf("The theater was not found\n");
	}

	if(MyTheater!=NULL)
	{
		char *token;
		int row, column;
		char DIM[6]={};

		strcpy(DIM, MyTheater->Dimensions);

		token= strtok(DIM,"x");
		row=atoi(token);
		token= strtok(NULL,"x");
		column=atoi(token);
		
		*MapRow= row;
		*MapCol = column;

		if(ReadMovieTheaterFile(MovieTheaterMap, MyTheater->FileName, *MapRow, *MapCol)==0)
		{
			printf("Unable to print that seat map at this time. Check back later\n");
		}
		else
		{
			PrintSeatMap(MovieTheaterMap, *MapRow, *MapCol);
		}

	}
	return MyTheater;
}	

void ReadFileIntoQueue(FILE *QueueFile, QNODE **QH, QNODE **QT)
{
	char CostumerName[30];

	while(fgets(CostumerName,sizeof(CostumerName)-1, QueueFile))
	{	
		if(CostumerName[strlen(CostumerName)-1]=='\n')
		{
			CostumerName[strlen(CostumerName)-1]='\0';
		}
		enQueue(CostumerName, QH, QT);
	}//May have to pass in address of QH and QT
	//	read the passed in file and calls enQueue for each name in the file
}

void ReadFileIntoBST(FILE *BSTFile, BNODE **BSTnode)
{	
	char FileLine[300];
	char TN[40], ZC[6], FN[100], DM[8];
	char *token;
	
	while(fgets(FileLine, sizeof(FileLine)-1, BSTFile))
	{	
		if(FileLine[strlen(FileLine)-1]=='\n')
			FileLine[strlen(FileLine)-1]='\0';

		token=strtok(FileLine, "|");
		strcpy(TN, token);
		token=strtok(NULL, "|");
		strcpy(ZC, token);
		token=strtok(NULL, "|");
		strcpy(FN, token);
		token=strtok(NULL, "\0");
		strcpy(DM, token);

		AddBSTNode(BSTnode, TN, ZC, FN, DM);
	}
	//	read the passed in file and tokenize each line and pass the information to AddBSTNode
}

void get_command_line_parameter(char *argv[], char ParamName[], char ParamValue[])
{
	int i = 0; 
	
	while (argv[++i] != NULL)
	{
		if (!strncmp(argv[i], ParamName, strlen(ParamName)))
		{
			strcpy(ParamValue, strchr(argv[i], '=') + 1);
		}
	}
}

int PrintMenu()
{
	int choice = 0;
	
	printf("\n\n1.	Sell tickets to next customer\n\n");
	printf("2.	See who's in line\n\n");
	printf("3.	See the seat map for a given theater\n\n");
	printf("4.	Print today's receipts\n\n");
	printf("Choice : ");
	scanf("%d", &choice);
	
	while (choice < 1 || choice > 4)
	{
		printf("Invalid choice.  Please reenter. ");
		scanf("%d", &choice);
	}
	
	return choice;
}

int main(int argc, char *argv[])
{
	int i, j, k;
	FILE *queueFile = NULL;
	FILE *zipFile = NULL;
	char arg_value[20];
	char queuefilename[20];
	char zipfilename[20];
	int ReceiptNumber = 0;
	int choice = 0;
	int SeatNumber;
	char Row;
	char Ticket[5];
	int ArrayRow, ArrayCol;
	int MapRow, MapCol;
	char MovieTheaterMap[MAXROWS][MAXCOLS] = {};
	LNODE *TicketLinkedListHead = NULL;
	QNODE *QueueHead = NULL;
	QNODE *QueueTail = NULL;
	BNODE *BSTRoot = NULL;
	BNODE *MyTheater = NULL;
	SNODE *StackTop = NULL;
	int NumberOfTickets = 0;
	
	if (argc != 4)
	{
		printf("%s QUEUE=xxxxxx ZIPFILE=xxxxx RECEIPTNUMBER=xxxxx\n", argv[0]);
		exit(0);
	}
	
	get_command_line_parameter(argv, "QUEUE=", queuefilename);
	get_command_line_parameter(argv, "ZIPFILE=", zipfilename);
	get_command_line_parameter(argv, "RECEIPTNUMBER=", arg_value);
	ReceiptNumber = atoi(arg_value);
		
	/* call function to open queuefilename - if it does not open, print message and exit */	
	queueFile=fopen(queuefilename, "r");
	if(queueFile==NULL)
	{
		printf("File doesn't exist\n");
		exit(0);
	}
	/* calll function to open zipfilename - if it does not open, print message and exit */
	zipFile=fopen(zipfilename, "r");
	if(zipFile==NULL)
	{
		printf("file doesn't exist\n"); 
		exit(0);
	}
	ReadFileIntoQueue(queueFile, &QueueHead, &QueueTail);
	ReadFileIntoBST(zipFile, &BSTRoot);
	
	while (QueueHead != NULL)
	{
		choice = PrintMenu();
	
		switch (choice)
		{
			case 1 :
				printf("\n\nHello %s\n", QueueHead->name);				
				MyTheater = PickAndDisplayTheater(BSTRoot, MovieTheaterMap, &MapRow, &MapCol);
				if (MyTheater != NULL)
				{
					printf("\n\nHow many movie tickets do you want to buy? ");
					scanf("%d", &NumberOfTickets);
					for (i = 0; i < NumberOfTickets; i++)
					{
						do
						{
							printf("\nPick a seat (Row Seat) ");
							scanf(" %c%d", &Row, &SeatNumber);
							Row = toupper(Row);
							ArrayRow = (int)Row - 65;
							ArrayCol = SeatNumber - 1;
						
							if ((ArrayRow < 0 || ArrayRow >= MapRow) ||
								(ArrayCol < 0 || ArrayCol >= MapCol))
							{
								printf("\nThat is not a valid seat.  Please choose again\n\n");
							}		
						}
						while ((ArrayRow < 0 || ArrayRow >= MapRow) ||
							   (ArrayCol < 0 || ArrayCol >= MapCol));
						
						if (MovieTheaterMap[ArrayRow][ArrayCol] == 'O')
						{	
							MovieTheaterMap[ArrayRow][ArrayCol] = 'X';
							sprintf(Ticket, "%c%d", Row, SeatNumber); 
							InsertNode(&TicketLinkedListHead, Ticket);
						}
						else
						{
							printf("\nSeat %c%d is not available.\n\n", Row, SeatNumber);
							i--;
						}
						PrintSeatMap(MovieTheaterMap, MapRow, MapCol);
					}
					
					WriteSeatMap(MyTheater, MovieTheaterMap, MapRow, MapCol);
					push(&StackTop, TicketLinkedListHead, ReceiptNumber, MyTheater->MovieTheaterName);
					TicketLinkedListHead = NULL;
					ReceiptNumber++;
					printf("\nThank you %s - enjoy your movie!\n", QueueHead->name);
					deQueue(&QueueHead);
				}
				break;
			case 2 : 
				printf("\n\nCustomer Queue\n\n");
				DisplayQueue(QueueHead);
				printf("\n\n");
				break;
			case 3 :
				PickAndDisplayTheater(BSTRoot, MovieTheaterMap, &MapRow, &MapCol);
				break;
			case 4 : 
				PrintReceipts(&StackTop);
				break;
			default :
				printf("Bad menu choice");
		}
	}
	
	printf("Good job - you sold tickets to all of the customers in line.\n");
	PrintReceipts(&StackTop);
	
	return 0;
}



